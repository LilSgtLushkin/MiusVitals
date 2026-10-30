#include "hooks.h"
#include "common.h"
#include <windows.h>
#include <stdint.h>
#include <string.h>

static bool safe_read(const void *p, size_t n)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!p || !VirtualQuery(p, &mbi, sizeof mbi) || mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD)) return false;
    DWORD protection = mbi.Protect & 0xff;
    if (protection != PAGE_READONLY && protection != PAGE_READWRITE && protection != PAGE_WRITECOPY &&
        protection != PAGE_EXECUTE_READ && protection != PAGE_EXECUTE_READWRITE && protection != PAGE_EXECUTE_WRITECOPY) return false;
    uintptr_t address = (uintptr_t)p, region = (uintptr_t)mbi.BaseAddress;
    return address >= region && address - region <= mbi.RegionSize && n <= mbi.RegionSize - (address - region);
}

// A codelib is a mapped PE image. All bounds and section flags are checked before looking at a
// fixed RVA or scanning it: an unknown/smaller build must not crash just because an RVA is stale.
struct Image {
    const char *base;
    unsigned size, count;
    const IMAGE_SECTION_HEADER *sections;
};

static bool image_info(const char *base, Image *out)
{
    if (!base || !safe_read(base, sizeof(IMAGE_DOS_HEADER))) return false;
    const IMAGE_DOS_HEADER *dos = (const IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < (long)sizeof(*dos) || dos->e_lfanew > 0x100000) return false;
    const IMAGE_NT_HEADERS *nt = (const IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    if (!safe_read(nt, sizeof(*nt)) || nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt->FileHeader.SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER) ||
        !nt->OptionalHeader.SizeOfImage || !nt->FileHeader.NumberOfSections || nt->FileHeader.NumberOfSections > 96)
        return false;
    if ((unsigned)dos->e_lfanew > nt->OptionalHeader.SizeOfImage ||
        sizeof(*nt) > nt->OptionalHeader.SizeOfImage - (unsigned)dos->e_lfanew) return false;
    const IMAGE_SECTION_HEADER *sections = IMAGE_FIRST_SECTION(nt);
    size_t size = (size_t)nt->FileHeader.NumberOfSections * sizeof(*sections);
    uintptr_t offset = (uintptr_t)sections - (uintptr_t)base;
    if (offset > nt->OptionalHeader.SizeOfImage || size > nt->OptionalHeader.SizeOfImage - offset ||
        !safe_read(sections, size)) return false;
    out->base = base;
    out->size = nt->OptionalHeader.SizeOfImage;
    out->count = nt->FileHeader.NumberOfSections;
    out->sections = sections;
    return true;
}

bool hook_image_identity(const char *module, unsigned timestamp, unsigned image_size)
{
    HMODULE m = GetModuleHandleA(module);
    Image image;
    if (!m || !image_info((const char *)m, &image) || image.size != image_size) return false;
    const IMAGE_DOS_HEADER *dos = (const IMAGE_DOS_HEADER *)m;
    const IMAGE_NT_HEADERS *nt = (const IMAGE_NT_HEADERS *)((const char *)m + dos->e_lfanew);
    return nt->FileHeader.TimeDateStamp == timestamp;
}

static bool executable_range(const Image &image, const IMAGE_SECTION_HEADER &section, unsigned *start, unsigned *size)
{
    if (!(section.Characteristics & IMAGE_SCN_MEM_EXECUTE) || section.VirtualAddress >= image.size) return false;
    unsigned length = section.Misc.VirtualSize > section.SizeOfRawData ? section.Misc.VirtualSize : section.SizeOfRawData;
    if (length > image.size - section.VirtualAddress) length = image.size - section.VirtualAddress;
    if (!length) return false;
    *start = section.VirtualAddress;
    *size = length;
    return true;
}

static bool fixed_match(const Image &image, const HookSig &h)
{
    if (!h.rva || !h.sig || !h.size) return false;
    for (unsigned i = 0; i < image.count; ++i) {
        unsigned start, size;
        if (!executable_range(image, image.sections[i], &start, &size) || h.rva < start) continue;
        unsigned delta = h.rva - start;
        if (delta > size || h.size > size - delta) continue;
        const char *p = image.base + h.rva;
        return safe_read(p, h.size) && !memcmp(p, h.sig, h.size);
    }
    return false;
}

// Search readable regions of executable sections. Never scan across inaccessible pages. A split
// signature at a protection boundary is conservatively missed rather than read outside a region.
static int scan_signature(const Image &image, const HookSig &h, const char **found)
{
    if (!h.sig || !h.size) return 0;
    int hits = 0;
    for (unsigned i = 0; i < image.count && hits < 2; ++i) {
        unsigned start, size;
        if (!executable_range(image, image.sections[i], &start, &size) || size < h.size) continue;
        const char *p = image.base + start, *end = p + size;
        while (p < end && hits < 2) {
            MEMORY_BASIC_INFORMATION mbi;
            if (!VirtualQuery(p, &mbi, sizeof mbi)) break;
            uintptr_t next_address = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
            if (next_address <= (uintptr_t)p) break;
            const char *next = next_address < (uintptr_t)end ? (const char *)next_address : end;
            size_t available = (size_t)(next - p);
            if (available >= h.size && safe_read(p, available)) {
                const char *last = next - h.size;
                for (const char *q = p; q <= last; ++q) {
                    if (!memcmp(q, h.sig, h.size)) {
                        *found = q;
                        if (++hits == 2) break;
                    }
                }
            }
            p = next;
        }
    }
    return hits;
}

// Result code: 0 found, 1 bad image, 2 absent, 3 ambiguous. Fixed RVAs for all known builds are
// checked before *any* fallback scan. Different unique signatures resolving to different addresses
// are ambiguous too; do not guess which version of the function is intended.
static char *resolve_image(const char *base, const HookSig *list, unsigned count, int *reason)
{
    Image image;
    if (!list || !count || !image_info(base, &image)) { *reason = 1; return 0; }
    for (unsigned i = 0; i < count; ++i)
        if (fixed_match(image, list[i])) { *reason = 0; return (char *)base + list[i].rva; }
    const char *answer = 0;
    for (unsigned i = 0; i < count; ++i) {
        const char *candidate = 0;
        int hits = scan_signature(image, list[i], &candidate);
        if (hits > 1 || (hits == 1 && answer && answer != candidate)) { *reason = 3; return 0; }
        if (hits == 1) answer = candidate;
    }
    *reason = answer ? 0 : 2;
    return (char *)answer;
}

static bool readonly_data_range(const Image &image, const IMAGE_SECTION_HEADER &section, unsigned *start, unsigned *size)
{
    static const char name[8] = {'.','r','d','a','t','a',0,0};
    if (memcmp(section.Name, name, sizeof name) || !(section.Characteristics & IMAGE_SCN_MEM_READ) ||
        (section.Characteristics & IMAGE_SCN_MEM_EXECUTE) || section.VirtualAddress >= image.size) return false;
    unsigned length = section.Misc.VirtualSize > section.SizeOfRawData ? section.Misc.VirtualSize : section.SizeOfRawData;
    if (length > image.size - section.VirtualAddress) length = image.size - section.VirtualAddress;
    *start = section.VirtualAddress; *size = length;
    return length != 0;
}

static bool vtable_matches(const char *base, const HookVtableMember *members, unsigned count, size_t span)
{
    if (!safe_read(base, span)) return false;
    for (unsigned i = 0; i < count; ++i) {
        const void *value = 0;
        memcpy(&value, base + (size_t)members[i].index * sizeof(void *), sizeof value);
        if (value != members[i].target) return false;
    }
    return true;
}

// reason: 0 found, 1 invalid image/specification, 2 absent, 3 ambiguous.
static void **resolve_vtable_image(const char *base, unsigned rva, const HookVtableMember *members,
                                   unsigned count, int *reason)
{
    Image image;
    if (!members || !count || count > 64 || !image_info(base, &image)) { *reason = 1; return 0; }
    unsigned last = 0;
    for (unsigned i = 0; i < count; ++i) {
        if (!members[i].target || members[i].index > 256) { *reason = 1; return 0; }
        if (members[i].index > last) last = members[i].index;
    }
    size_t span = ((size_t)last + 1) * sizeof(void *);
    for (unsigned i = 0; i < image.count; ++i) {
        unsigned start, size;
        if (!readonly_data_range(image, image.sections[i], &start, &size) || (rva & (sizeof(void *) - 1)) || rva < start) continue;
        unsigned delta = rva - start;
        if (delta <= size && span <= size - delta && vtable_matches(base + rva, members, count, span)) {
            *reason = 0; return (void **)(base + rva);
        }
    }
    void **found = 0;
    for (unsigned i = 0; i < image.count; ++i) {
        unsigned start, size;
        if (!readonly_data_range(image, image.sections[i], &start, &size) || size < span) continue;
        unsigned first = (start + (unsigned)sizeof(void *) - 1) & ~((unsigned)sizeof(void *) - 1);
        unsigned end = start + size - (unsigned)span;
        for (unsigned at = first; at <= end; at += (unsigned)sizeof(void *)) {
            const char *candidate = base + at;
            const void *value = 0;
            const char *first_member = candidate + (size_t)members[0].index * sizeof(void *);
            if (!safe_read(first_member, sizeof value)) continue;
            memcpy(&value, first_member, sizeof value);
            if (value != members[0].target || !vtable_matches(candidate, members, count, span)) continue;
            if (found) { *reason = 3; return 0; }
            found = (void **)candidate;
        }
    }
    *reason = found ? 0 : 2;
    return found;
}

void **hook_vtable_checked(const char *module, unsigned rva, const HookVtableMember *members,
                           unsigned count, const char *what)
{
    HMODULE m = GetModuleHandleA(module);
    int reason = 1;
    void **table = m ? resolve_vtable_image((const char *)m, rva, members, count, &reason) : 0;
    if (!table) {
        const char *why = reason == 1 ? "invalid or unreadable PE/specification" : reason == 3 ? "ambiguous tables" : "no matching table";
        mp_log("hooks: %s: %s in %s, the feature stays off", what, why, module);
    } else if ((char *)table != (char *)m + rva) {
        mp_log("hooks: %s table moved to +0x%lx; the unique data scan found it", what, (long)((char *)table - (char *)m));
    }
    return table;
}

bool hook_swap_vtable_slot(void **slot, void *expected, void *replacement)
{
    if (!slot || !expected || !replacement || ((uintptr_t)slot & (sizeof(void *) - 1)) || !safe_read(slot, sizeof *slot)) return false;
    DWORD old = 0;
    if (!VirtualProtect(slot, sizeof *slot, PAGE_READWRITE, &old)) return false;
    void *previous = InterlockedCompareExchangePointer((void *volatile *)slot, replacement, expected);
    DWORD ignored = 0;
    if (!VirtualProtect(slot, sizeof *slot, old, &ignored))
        mp_log("hooks: warning: could not restore vtable page protection (error %lu)", GetLastError());
    return previous == expected;
}

long hook_offset(const char *module, const void *address)
{
    HMODULE m = GetModuleHandleA(module);
    if (!m || !address) return -1;
    return (long)((const char *)address - (const char *)m);
}

char *hook_resolve_checked(const char *module, const HookSig *list, unsigned count, const char *what)
{
    HMODULE m = GetModuleHandleA(module);
    if (!m) {
        mp_log("hooks: %s: %s is not loaded, the feature stays off", what, module);
        return 0;
    }
    int reason = 0;
    char *p = resolve_image((const char *)m, list, count, &reason);
    if (!p) {
        const char *why = reason == 1 ? "invalid or unreadable PE image" : reason == 3 ? "ambiguous signatures" : "no matching signature";
        mp_log("hooks: %s: %s in %s, the feature stays off", what, why, module);
        return 0;
    }
    bool fixed = false;
    for (unsigned i = 0; i < count; ++i)
        if ((uintptr_t)p == (uintptr_t)m + list[i].rva) fixed = true;
    if (!fixed)
        mp_log("hooks: %s moved to +0x%lx; the unique code scan found it", what, (long)(p - (char *)m));
    return p;
}

#ifdef NETV21_TEST_HOOKS
char *hook_test_resolve(const void *base, const HookSig *list, unsigned count, int *reason)
{
    return resolve_image((const char *)base, list, count, reason);
}
void **hook_test_vtable(const void *base, unsigned rva, const HookVtableMember *members, unsigned count, int *reason)
{
    return resolve_vtable_image((const char *)base, rva, members, count, reason);
}
#endif

// The operation-map hooks use their existing resolver, kept as it was so this M3/M4 change does not
// alter M5. Tactical hooks above use the stricter, bounded resolver.
static const IMAGE_SECTION_HEADER *legacy_sections_of(const char *base, unsigned *count)
{
    const IMAGE_DOS_HEADER *dos = (const IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;
    const IMAGE_NT_HEADERS *nt = (const IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;
    *count = nt->FileHeader.NumberOfSections;
    return IMAGE_FIRST_SECTION(nt);
}

static char *legacy_find_one(const char *base, const HookSig &h)
{
    if (h.rva && !memcmp(base + h.rva, h.sig, h.size)) return (char *)base + h.rva;
    unsigned count = 0;
    const IMAGE_SECTION_HEADER *sections = legacy_sections_of(base, &count);
    if (!sections) return 0;
    const char *found = 0;
    int hits = 0;
    for (unsigned i = 0; i < count && hits <= 1; i++) {
        const IMAGE_SECTION_HEADER &s = sections[i];
        if (!(s.Characteristics & IMAGE_SCN_MEM_EXECUTE) || s.SizeOfRawData < h.size) continue;
        const char *p = base + s.VirtualAddress;
        const char *end = p + s.SizeOfRawData - h.size;
        for (const char *q = p; q <= end; q++)
            if (!memcmp(q, h.sig, h.size)) {
                if (++hits > 1) break;
                found = q;
            }
    }
    return hits == 1 ? (char *)found : 0;
}

char *hook_resolve(const char *module, const HookSig *list, unsigned count, const char *what)
{
    HMODULE m = GetModuleHandleA(module);
    if (!m) {
        mp_log("hooks: %s: %s is not loaded, the feature stays off", what, module);
        return 0;
    }
    char *base = (char *)m;
    for (unsigned i = 0; i < count; i++) {
        char *p = legacy_find_one(base, list[i]);
        if (!p) continue;
        long off = (long)(p - base);
        if (off != (long)list[i].rva)
            mp_log("hooks: %s is not where this build had it (now +0x%lx), the scan found it and it is used",
                   what, off);
        return p;
    }
    mp_log("hooks: %s: none of the %u known signatures is in %s, the feature stays off", what, count, module);
    return 0;
}

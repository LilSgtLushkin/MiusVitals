#include "hooks.h"
#include "common.h"
#include <windows.h>

// A codelib is a PE image mapped at some base; the offsets the recovery prints are RVAs from that base.
static const IMAGE_SECTION_HEADER *sections_of(const char *base, unsigned *count)
{
    const IMAGE_DOS_HEADER *dos = (const IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;
    const IMAGE_NT_HEADERS *nt = (const IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;
    *count = nt->FileHeader.NumberOfSections;
    return IMAGE_FIRST_SECTION(nt);
}

long hook_offset(const char *module, const void *address)
{
    HMODULE m = GetModuleHandleA(module);
    if (!m || !address) return -1;
    return (long)((const char *)address - (const char *)m);
}

// One signature in one module: at its own offset first, then anywhere in the code.  Returns the single
// address it matches, or null when it is absent or ambiguous.
static char *find_one(const char *base, const char *module, const HookSig &h)
{
    if (h.rva && !memcmp(base + h.rva, h.sig, h.size)) return (char *)base + h.rva;
    unsigned count = 0;
    const IMAGE_SECTION_HEADER *sections = sections_of(base, &count);
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
        char *p = find_one(base, module, list[i]);
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

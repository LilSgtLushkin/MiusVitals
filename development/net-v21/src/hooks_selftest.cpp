// In-memory PE fixture for the real, non-patching hook resolver. No game files or hooks are changed.
#include "hooks.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

void mp_log(const char *, ...) {}
static int checks, failures;
static void check(bool ok, const char *name)
{
    ++checks;
    if (!ok) { ++failures; printf("FAIL: %s\n", name); }
}

int main()
{
    char *image = (char *)VirtualAlloc(nullptr, 0x6000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!image) return 2;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)image;
    dos->e_magic = IMAGE_DOS_SIGNATURE;
    dos->e_lfanew = 0x100;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(image + 0x100);
    nt->Signature = IMAGE_NT_SIGNATURE;
    nt->FileHeader.NumberOfSections = 2;
    nt->FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER);
    nt->OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
    nt->OptionalHeader.SizeOfImage = 0x6000;
    IMAGE_SECTION_HEADER *sections = IMAGE_FIRST_SECTION(nt);
    sections[0].VirtualAddress = 0x1000;
    sections[0].Misc.VirtualSize = sections[0].SizeOfRawData = 0x1000;
    sections[0].Characteristics = IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
    sections[1].VirtualAddress = 0x3000;
    sections[1].Misc.VirtualSize = sections[1].SizeOfRawData = 0x1000;
    sections[1].Characteristics = IMAGE_SCN_MEM_READ;

    const unsigned char a[] = { 0x23, 0x91, 0x42, 0xa5, 0xe7, 0x34, 0x19, 0xc6 };
    const unsigned char b[] = { 0x72, 0x12, 0xb3, 0x46, 0xfa, 0x92, 0x04, 0x38 };
    HookSig first = { 0x1200, a, sizeof a };
    int reason = -1;
    memcpy(image + 0x1200, a, sizeof a);
    memcpy(image + 0x1300, a, sizeof a);
    check(hook_test_resolve(image, &first, 1, &reason) == image + 0x1200 && reason == 0,
          "known fixed RVA wins even if signature is duplicated elsewhere");
    memset(image + 0x1200, 0, sizeof a);
    check(hook_test_resolve(image, &first, 1, &reason) == image + 0x1300 && reason == 0,
          "unique executable fallback");
    memcpy(image + 0x1400, a, sizeof a);
    check(!hook_test_resolve(image, &first, 1, &reason) && reason == 3, "ambiguous fallback rejected");
    memset(image + 0x1300, 0, sizeof a);
    memset(image + 0x1400, 0, sizeof a);
    memcpy(image + 0x3200, a, sizeof a);
    first.rva = 0x3200;
    check(!hook_test_resolve(image, &first, 1, &reason) && reason == 2, "data section never counts as code");
    memcpy(image + 0x1300, a, sizeof a);
    first.rva = 0xfffffff0u;
    check(hook_test_resolve(image, &first, 1, &reason) == image + 0x1300 && reason == 0,
          "out-of-image fixed RVA is safe and still allows unique scan");

    HookSig two[] = { { 0x1200, a, sizeof a }, { 0x1500, b, sizeof b } };
    memcpy(image + 0x1500, b, sizeof b);
    check(hook_test_resolve(image, two, 2, &reason) == image + 0x1500 && reason == 0,
          "all fixed RVAs are checked before scanning another candidate");
    two[1].rva = 0;
    check(!hook_test_resolve(image, two, 2, &reason) && reason == 3,
          "different unique candidates for one hook are ambiguous");
    memset(image + 0x1500, 0, sizeof b);
    check(hook_test_resolve(image, two, 2, &reason) == image + 0x1300 && reason == 0,
          "absent alternative does not hide the unique candidate");
    two[1] = { 0, a, sizeof a };
    check(hook_test_resolve(image, two, 2, &reason) == image + 0x1300 && reason == 0,
          "two signatures resolving to the same address are allowed");

    DWORD old = 0, ignored = 0;
    check(VirtualProtect(image + 0x1000, 0x1000, PAGE_NOACCESS, &old) != 0, "fixture made inaccessible");
    check(!hook_test_resolve(image, &first, 1, &reason) && reason == 2, "inaccessible code page is skipped safely");
    check(VirtualProtect(image + 0x1000, 0x1000, old, &ignored) != 0, "fixture restored");
    dos->e_magic = 0;
    check(!hook_test_resolve(image, &first, 1, &reason) && reason == 1, "invalid PE rejected");
    dos->e_magic = IMAGE_DOS_SIGNATURE;
    nt->FileHeader.NumberOfSections = 97;
    check(!hook_test_resolve(image, &first, 1, &reason) && reason == 1, "implausible section count rejected");
    VirtualFree(image, 0, MEM_RELEASE);
    printf("hook resolver selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

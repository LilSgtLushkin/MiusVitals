// Patching a loaded module's imports, plus a small tracker for modules loaded after startup.
#pragma once
#include <windows.h>
#include <stddef.h>

namespace input_imports {
// Return the number of entries replaced, or -1 on malformed headers or a protection failure.
// This is intended only for trusted modules already loaded by the game process.
inline int replace(HMODULE module, void *from, void *to) {
    if (!module || !from || !to) return -1;
    BYTE *base = (BYTE *)module;
    const IMAGE_DOS_HEADER *dos = (const IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < (LONG)sizeof(*dos) || dos->e_lfanew > 0x100000) return -1;
    const IMAGE_NT_HEADERS64 *nt = (const IMAGE_NT_HEADERS64 *)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) return -1;
    const size_t image_size = nt->OptionalHeader.SizeOfImage;
    const IMAGE_DATA_DIRECTORY directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!directory.VirtualAddress) return 0;
    if (directory.VirtualAddress >= image_size || directory.Size > image_size - directory.VirtualAddress ||
        directory.Size < sizeof(IMAGE_IMPORT_DESCRIPTOR)) return -1;
    const auto *descriptors = (const IMAGE_IMPORT_DESCRIPTOR *)(base + directory.VirtualAddress);
    size_t count = directory.Size / sizeof(IMAGE_IMPORT_DESCRIPTOR);
    int replaced = 0;
    bool terminated = false;
    for (size_t d = 0; d < count; ++d) {
        if (!descriptors[d].Name) { terminated = true; break; }
        size_t rva = descriptors[d].FirstThunk;
        if (!rva || rva > image_size || sizeof(void *) > image_size - rva) return -1;
        void **slot = (void **)(base + rva);
        size_t maximum = (image_size - rva) / sizeof(void *);
        bool thunk_terminated = false;
        for (size_t i = 0; i < maximum; ++i) {
            if (!slot[i]) { thunk_terminated = true; break; }
            if (slot[i] != from) continue;
            DWORD old = 0, ignored = 0;
            if (!VirtualProtect(&slot[i], sizeof(slot[i]), PAGE_READWRITE, &old)) return -1;
            slot[i] = to;
            if (!VirtualProtect(&slot[i], sizeof(slot[i]), old, &ignored)) return -1;
            ++replaced;
        }
        if (!thunk_terminated) return -1;
    }
    return terminated ? replaced : -1;
}

template <size_t N> struct Tracker {
    HMODULE installed[N] = {};
    bool needs(size_t i, HMODULE current) {
        if (i >= N) return false;
        if (!current) { installed[i] = nullptr; return false; }
        return installed[i] != current;
    }
    void complete(size_t i, HMODULE current) { if (i < N) installed[i] = current; }
};
}

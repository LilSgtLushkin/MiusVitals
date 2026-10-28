// x64 fast path for the verified Steam 3598/1 basic_render matrix-cache write.
//
// This is reached after the engine's sentinel check and its matrix load. The displaced
// instructions and the two comparisons are emitted as code, so normal cache updates do not
// call C++ or VirtualQuery. Only a slot outside the allocator's live count/capacity goes to
// the engine's native allocation branch. The rare path atomically counts the repair.
//
// This is NOT equivalent to checking Windows allocation ownership: corrupted allocator
// metadata or a stale/reused pool pointer can make an unrelated writable address look valid.
// The exact-access-violation handler remains the last resort for inaccessible destinations.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

namespace render_cache_bounds {

enum { k_size = 74, k_continue_ptr = 37, k_counter_ptr = 48, k_allocate_ptr = 66 };
// RDI = renderer, EDX = original cache slot, XMM0 = loaded matrix; other registers are
// untouched except the original MOVs. RAX is saved/restored on the rare allocation branch.
static const unsigned char k_stub[] = {
    0x81,0xe2,0xff,0xff,0xff,0x00,                // and edx, 0x00ffffff
    0x3b,0x97,0x74,0x01,0x00,0x00,                // cmp edx, [rdi+0x174] (live count)
    0x73,0x1f,                                    // jae rare
    0x3b,0x97,0x78,0x01,0x00,0x00,                // cmp edx, [rdi+0x178] (capacity)
    0x73,0x17,                                    // jae rare
    0x48,0x8b,0x87,0x68,0x01,0x00,0x00,           // mov rax, [rdi+0x168]
    0x8b,0xca,                                    // mov ecx, edx
    0xff,0x25,0x00,0x00,0x00,0x00,                // jmp [rip+0], followed by continue address
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x50,                                          // rare: push rax
    0x48,0xb8,                                    // mov rax, counter address
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0xf0,0xff,0x00,                               // lock inc dword ptr [rax]
    0x58,                                          // pop rax
    0xff,0x25,0x00,0x00,0x00,0x00,                // jmp [rip+0], followed by allocator address
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
static_assert(sizeof k_stub == k_size, "cache bounds stub layout");

inline bool build(unsigned char *out, size_t size, uintptr_t next, uintptr_t allocate, uintptr_t counter)
{
    if (!out || size < k_size || !next || !allocate || !counter) return false;
    // This builder is intentionally x64 only: the instruction stream embeds 8-byte pointers.
    if (sizeof(uintptr_t) != 8) return false;
    memcpy(out, k_stub, k_size);
    const uint64_t next64 = next, allocate64 = allocate, counter64 = counter;
    memcpy(out + k_continue_ptr, &next64, sizeof next64);
    memcpy(out + k_counter_ptr, &counter64, sizeof counter64);
    memcpy(out + k_allocate_ptr, &allocate64, sizeof allocate64);
    return true;
}

} // namespace render_cache_bounds

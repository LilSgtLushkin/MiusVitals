// v20 RVA 0051f4..00540e  (538 bytes)  $_Emplace_reallocate@AEBUTagged@opsave
// decompiler failed (ClaripyZeroDivisionError: ): capstone listing, RIP-relative operands annotated

0051f4  mov     qword ptr [rsp + 0x18], rbx
0051f9  push    rbp
0051fa  push    rsi
0051fb  push    rdi
0051fc  push    r12
0051fe  push    r13
005200  push    r14
005202  push    r15
005204  mov     rbp, rsp
005207  sub     rsp, 0x80
00520e  mov     r14, r8
005211  mov     r15, rdx
005214  mov     r12, rcx
005217  sub     rdx, qword ptr [rcx]
00521a  movabs  r9, 0x2aaaaaaaaaaaaaab
005224  mov     rax, r9
005227  imul    rdx
00522a  mov     rbx, rdx
00522d  sar     rbx, 4
005231  mov     rax, rbx
005234  shr     rax, 0x3f
005238  add     rbx, rax
00523b  mov     rcx, qword ptr [rcx + 8]
00523f  sub     rcx, qword ptr [r12]
005243  mov     rax, r9
005246  imul    rcx
005249  sar     rdx, 4
00524d  mov     rax, rdx
005250  shr     rax, 0x3f
005254  add     rdx, rax
005257  movabs  r13, 0x2aaaaaaaaaaaaaa
005261  cmp     rdx, r13
005264  je      0x5408
00526a  lea     r8, [rdx + 1]
00526e  mov     qword ptr [rbp + 0x40], r8
005272  mov     rcx, qword ptr [r12 + 0x10]
005277  sub     rcx, qword ptr [r12]
00527b  mov     rax, r9
00527e  imul    rcx
005281  sar     rdx, 4
005285  mov     rax, rdx
005288  shr     rax, 0x3f
00528c  add     rdx, rax
00528f  mov     rcx, rdx
005292  shr     rcx, 1
005295  mov     rax, r13
005298  sub     rax, rcx
00529b  cmp     rdx, rax
00529e  jbe     0x52a9
0052a0  mov     rcx, 0xffffffffffffffc0
0052a7  jmp     0x52dd
0052a9  lea     rax, [rcx + rdx]
0052ad  mov     rdx, r8
0052b0  cmp     rax, r8
0052b3  cmovae  rdx, rax
0052b7  cmp     rdx, r13
0052ba  ja      0x5402
0052c0  lea     rcx, [rdx + rdx*2]
0052c4  shl     rcx, 5
0052c8  mov     r13, rdx
0052cb  test    rcx, rcx
0052ce  jne     0x52d4
0052d0  xor     esi, esi
0052d2  jmp     0x52ec
0052d4  cmp     rcx, 0x1000
0052db  jb      0x52e4
0052dd  call    0x48a0    ; $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std
0052e2  jmp     0x52e9
0052e4  call    0x402d0
0052e9  mov     rsi, rax
0052ec  lea     rbx, [rbx + rbx*2]
0052f0  shl     rbx, 5
0052f4  add     rbx, rsi
0052f7  mov     qword ptr [rbp + 0x48], rbx
0052fb  lea     rdi, [rbx + 0x60]
0052ff  mov     qword ptr [rbp - 0x30], r12
005303  mov     qword ptr [rbp - 0x28], rsi
005307  mov     qword ptr [rbp - 0x20], r13
00530b  mov     qword ptr [rbp - 0x18], rdi
00530f  mov     qword ptr [rbp - 0x10], rdi
005313  mov     rdx, r14
005316  mov     rcx, rbx
005319  call    0x8d38    ; 0Tagged@opsave
00531e  mov     qword ptr [rbp - 0x18], rbx
005322  mov     r14, qword ptr [r12 + 8]
005327  mov     rbx, qword ptr [r12]
00532b  mov     qword ptr [rbp - 0x60], rsi
00532f  mov     qword ptr [rbp - 0x58], rsi
005333  mov     qword ptr [rbp - 0x50], r12
005337  cmp     r15, r14
00533a  jne     0x5363
00533c  mov     rdi, rsi
00533f  jmp     0x5358
005341  mov     rdx, rbx
005344  mov     rcx, rdi
005347  call    0x8ca0    ; 0Tagged@opsave
00534c  add     rdi, 0x60
005350  mov     qword ptr [rbp - 0x58], rdi
005354  add     rbx, 0x60
005358  cmp     rbx, r14
00535b  jne     0x5341
00535d  mov     qword ptr [rbp - 0x60], rdi
005361  jmp     0x53bf
005363  mov     r14, rsi
005366  jmp     0x537f
005368  mov     rdx, rbx
00536b  mov     rcx, r14
00536e  call    0x8ca0    ; 0Tagged@opsave
005373  add     r14, 0x60
005377  mov     qword ptr [rbp - 0x58], r14
00537b  add     rbx, 0x60
00537f  cmp     rbx, r15
005382  jne     0x5368
005384  mov     qword ptr [rbp - 0x60], r14
005388  mov     qword ptr [rbp - 0x18], rsi
00538c  mov     rbx, qword ptr [r12 + 8]
005391  mov     qword ptr [rbp - 0x48], rdi
005395  mov     qword ptr [rbp - 0x40], rdi
005399  mov     qword ptr [rbp - 0x38], r12
00539d  jmp     0x53b6
00539f  mov     rdx, r15
0053a2  mov     rcx, rdi
0053a5  call    0x8ca0    ; 0Tagged@opsave
0053aa  add     rdi, 0x60
0053ae  mov     qword ptr [rbp - 0x40], rdi
0053b2  add     r15, 0x60
0053b6  cmp     r15, rbx
0053b9  jne     0x539f
0053bb  mov     qword ptr [rbp - 0x48], rdi
0053bf  mov     qword ptr [rbp - 0x28], 0
0053c7  mov     r9, r13
0053ca  mov     r8, qword ptr [rbp + 0x40]
0053ce  mov     rdx, rsi
0053d1  mov     rcx, r12
0053d4  call    0xaa5c    ; _Change_array@?$vector@UTagged@opsave
0053d9  nop     
0053da  lea     rcx, [rbp - 0x30]
0053de  call    0x9d50    ; 1_Reallocation_guard@?$vector@UTagged@opsave
0053e3  mov     rax, qword ptr [rbp + 0x48]
0053e7  mov     rbx, qword ptr [rsp + 0xd0]
0053ef  add     rsp, 0x80
0053f6  pop     r15
0053f8  pop     r14
0053fa  pop     r13
0053fc  pop     r12
0053fe  pop     rdi
0053ff  pop     rsi
005400  pop     rbp
005401  ret     
005402  call    0xbcd4    ; _Throw_bad_array_new_length@std
005407  int3    
005408  call    0xcdb8    ; _Xlength@?$vector@EV?$allocator@E@std
00540d  int3    

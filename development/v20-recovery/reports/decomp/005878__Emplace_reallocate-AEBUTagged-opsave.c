// v20 RVA 005878..005b30  (696 bytes)  $_Emplace_reallocate@AEBUTagged@opsave
// decompiler failed (ClaripyZeroDivisionError: ): capstone listing, RIP-relative operands annotated

005878  mov     qword ptr [rsp + 0x20], r9
00587d  mov     qword ptr [rsp + 0x18], r8
005882  push    rbp
005883  push    rbx
005884  push    rsi
005885  push    rdi
005886  push    r12
005888  push    r13
00588a  push    r14
00588c  push    r15
00588e  lea     rbp, [rsp - 0x1f]
005893  sub     rsp, 0x88
00589a  mov     r14, rdx
00589d  mov     rsi, rcx
0058a0  mov     r10, rdx
0058a3  sub     r10, qword ptr [rcx]
0058a6  movabs  r8, 0x2aaaaaaaaaaaaaab
0058b0  mov     rax, r8
0058b3  imul    r10
0058b6  mov     rdi, rdx
0058b9  sar     rdi, 2
0058bd  mov     rax, rdi
0058c0  shr     rax, 0x3f
0058c4  add     rdi, rax
0058c7  mov     rcx, qword ptr [rcx + 8]
0058cb  sub     rcx, qword ptr [rsi]
0058ce  mov     rax, r8
0058d1  imul    rcx
0058d4  sar     rdx, 2
0058d8  mov     rax, rdx
0058db  shr     rax, 0x3f
0058df  add     rdx, rax
0058e2  movabs  r15, 0xaaaaaaaaaaaaaaa
0058ec  cmp     rdx, r15
0058ef  je      0x5b2a
0058f5  lea     r13, [rdx + 1]
0058f9  mov     rcx, qword ptr [rsi + 0x10]
0058fd  sub     rcx, qword ptr [rsi]
005900  mov     rax, r8
005903  imul    rcx
005906  sar     rdx, 2
00590a  mov     rax, rdx
00590d  shr     rax, 0x3f
005911  add     rdx, rax
005914  mov     rcx, rdx
005917  shr     rcx, 1
00591a  mov     rax, r15
00591d  sub     rax, rcx
005920  cmp     rdx, rax
005923  jbe     0x592e
005925  mov     rcx, 0xfffffffffffffff0
00592c  jmp     0x5962
00592e  lea     rax, [rcx + rdx]
005932  mov     rdx, r13
005935  cmp     rax, r13
005938  cmovae  rdx, rax
00593c  cmp     rdx, r15
00593f  ja      0x5b24
005945  lea     rcx, [rdx + rdx*2]
005949  shl     rcx, 3
00594d  mov     r15, rdx
005950  test    rcx, rcx
005953  jne     0x5959
005955  xor     ebx, ebx
005957  jmp     0x5971
005959  cmp     rcx, 0x1000
005960  jb      0x5969
005962  call    0x48a0    ; $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std
005967  jmp     0x596e
005969  call    0x402d0
00596e  mov     rbx, rax
005971  lea     rax, [rdi + rdi*2]
005975  lea     r12, [rbx + rax*8]
005979  lea     rdi, [r12 + 0x18]
00597e  mov     qword ptr [rbp - 0x19], rsi
005982  mov     qword ptr [rbp - 0x11], rbx
005986  mov     qword ptr [rbp - 9], r15
00598a  mov     qword ptr [rbp - 1], rdi
00598e  mov     qword ptr [rbp + 7], rdi
005992  mov     rdx, qword ptr [rbp + 0x7f]
005996  mov     rdx, qword ptr [rdx]
005999  mov     rax, qword ptr [rbp + 0x77]
00599d  mov     rax, qword ptr [rax]
0059a0  mov     qword ptr [r12], 0
0059a8  mov     qword ptr [r12 + 8], 0
0059b1  mov     qword ptr [r12 + 0x10], 0
0059ba  mov     qword ptr [rbp + 0x6f], rax
0059be  mov     qword ptr [rbp + 0x67], rdx
0059c2  sub     rdx, rax
0059c5  lea     r9, [rbp + 0x67]
0059c9  lea     r8, [rbp + 0x6f]
0059cd  mov     rcx, r12
0059d0  call    0x4bd4    ; $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std
0059d5  mov     qword ptr [rbp - 1], r12
0059d9  mov     r10, qword ptr [rsi + 8]
0059dd  mov     r8, qword ptr [rsi]
0059e0  mov     qword ptr [rbp - 0x49], rbx
0059e4  mov     r9, rbx
0059e7  mov     qword ptr [rbp - 0x41], rbx
0059eb  mov     qword ptr [rbp - 0x39], rsi
0059ef  cmp     r14, r10
0059f2  jne     0x5a48
0059f4  cmp     r8, r10
0059f7  je      0x5a3f
0059f9  add     r8, 8
0059fd  mov     rdx, qword ptr [r8 + 8]
005a01  mov     qword ptr [r8 + 8], 0
005a09  mov     rcx, qword ptr [r8]
005a0c  mov     qword ptr [r8], 0
005a13  mov     rax, qword ptr [r8 - 8]
005a17  mov     qword ptr [r8 - 8], 0
005a1f  mov     qword ptr [r9], rax
005a22  mov     qword ptr [r9 + 8], rcx
005a26  mov     qword ptr [r9 + 0x10], rdx
005a2a  add     r9, 0x18
005a2e  mov     qword ptr [rbp - 0x41], r9
005a32  lea     r8, [r8 + 0x18]
005a36  lea     rax, [r8 - 8]
005a3a  cmp     rax, r10
005a3d  jne     0x59fd
005a3f  mov     qword ptr [rbp - 0x49], r9
005a43  jmp     0x5aea
005a48  cmp     r8, r14
005a4b  je      0x5a8c
005a4d  add     r8, 8
005a51  xor     r11d, r11d
005a54  mov     rdx, qword ptr [r8 + 8]
005a58  mov     qword ptr [r8 + 8], r11
005a5c  mov     rcx, qword ptr [r8]
005a5f  mov     qword ptr [r8], r11
005a62  mov     rax, qword ptr [r8 - 8]
005a66  mov     qword ptr [r8 - 8], r11
005a6a  mov     qword ptr [r9], rax
005a6d  mov     qword ptr [r9 + 8], rcx
005a71  mov     qword ptr [r9 + 0x10], rdx
005a75  add     r9, 0x18
005a79  mov     qword ptr [rbp - 0x41], r9
005a7d  lea     r8, [r8 + 0x18]
005a81  lea     rax, [r8 - 8]
005a85  cmp     rax, r14
005a88  jne     0x5a54
005a8a  jmp     0x5a8f
005a8c  xor     r11d, r11d
005a8f  mov     qword ptr [rbp - 0x49], r9
005a93  mov     qword ptr [rbp - 1], rbx
005a97  mov     r9, qword ptr [rsi + 8]
005a9b  mov     qword ptr [rbp - 0x31], rdi
005a9f  mov     qword ptr [rbp - 0x29], rdi
005aa3  mov     qword ptr [rbp - 0x21], rsi
005aa7  cmp     r14, r9
005aaa  je      0x5ae6
005aac  lea     r10, [r14 + 8]
005ab0  mov     rdx, qword ptr [r10 + 8]
005ab4  mov     qword ptr [r10 + 8], r11
005ab8  mov     rcx, qword ptr [r10]
005abb  mov     qword ptr [r10], r11
005abe  mov     rax, qword ptr [r10 - 8]
005ac2  mov     qword ptr [r10 - 8], r11
005ac6  mov     qword ptr [rdi], rax
005ac9  mov     qword ptr [rdi + 8], rcx
005acd  mov     qword ptr [rdi + 0x10], rdx
005ad1  add     rdi, 0x18
005ad5  mov     qword ptr [rbp - 0x29], rdi
005ad9  lea     r10, [r10 + 0x18]
005add  lea     rax, [r10 - 8]
005ae1  cmp     rax, r9
005ae4  jne     0x5ab0
005ae6  mov     qword ptr [rbp - 0x31], rdi
005aea  mov     qword ptr [rbp - 0x11], 0
005af2  mov     r9, r15
005af5  mov     r8, r13
005af8  mov     rdx, rbx
005afb  mov     rcx, rsi
005afe  call    0xac14    ; _Change_array@?$vector@UTagged@opsave
005b03  nop     
005b04  lea     rcx, [rbp - 0x19]
005b08  call    0x9e80    ; 1_Reallocation_guard@?$vector@UTagged@opsave
005b0d  mov     rax, r12
005b10  add     rsp, 0x88
005b17  pop     r15
005b19  pop     r14
005b1b  pop     r13
005b1d  pop     r12
005b1f  pop     rdi
005b20  pop     rsi
005b21  pop     rbx
005b22  pop     rbp
005b23  ret     
005b24  call    0xbcd4    ; _Throw_bad_array_new_length@std
005b29  int3    
005b2a  call    0xcdb8    ; _Xlength@?$vector@EV?$allocator@E@std
005b2f  int3    

// v20 RVA 00aa5c..00ab4e  (242 bytes)  _Change_array@?$vector@UTagged@opsave
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

00aa5c  mov     qword ptr [rsp + 0x10], rbx
00aa61  mov     qword ptr [rsp + 0x18], rbp
00aa66  mov     qword ptr [rsp + 0x20], rsi
00aa6b  push    rdi
00aa6c  push    r14
00aa6e  push    r15
00aa70  sub     rsp, 0x30
00aa74  mov     r14, r9
00aa77  mov     r15, r8
00aa7a  mov     rbp, rdx
00aa7d  mov     rdi, rcx
00aa80  mov     rbx, qword ptr [rcx]
00aa83  test    rbx, rbx
00aa86  je      0xaafa
00aa88  mov     rsi, qword ptr [rcx + 8]
00aa8c  jmp     0xaa9a
00aa8e  mov     rcx, rbx
00aa91  call    0xc2d8    ; _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std
00aa96  add     rbx, 0x60
00aa9a  cmp     rbx, rsi
00aa9d  mov     qword ptr [rsp + 0x50], rbx
00aaa2  jne     0xaa8e
00aaa4  mov     r8, qword ptr [rdi]
00aaa7  mov     rcx, qword ptr [rdi + 0x10]
00aaab  sub     rcx, r8
00aaae  movabs  rax, 0x2aaaaaaaaaaaaaab
00aab8  imul    rcx
00aabb  sar     rdx, 4
00aabf  mov     rax, rdx
00aac2  shr     rax, 0x3f
00aac6  add     rdx, rax
00aac9  lea     rdx, [rdx + rdx*2]
00aacd  shl     rdx, 5
00aad1  cmp     rdx, 0x1000
00aad8  jb      0xaaf2
00aada  add     rdx, 0x27
00aade  mov     rax, qword ptr [r8 - 8]
00aae2  sub     r8, rax
00aae5  sub     r8, 8
00aae9  cmp     r8, 0x1f
00aaed  ja      0xab34
00aaef  mov     r8, rax
00aaf2  mov     rcx, r8
00aaf5  call    0x4030c
00aafa  mov     qword ptr [rdi], rbp
00aafd  lea     rax, [r15 + r15*2]
00ab01  shl     rax, 5
00ab05  add     rax, rbp
00ab08  mov     qword ptr [rdi + 8], rax
00ab0c  lea     rax, [r14 + r14*2]
00ab10  shl     rax, 5
00ab14  add     rax, rbp
00ab17  mov     qword ptr [rdi + 0x10], rax
00ab1b  mov     rbx, qword ptr [rsp + 0x58]
00ab20  mov     rbp, qword ptr [rsp + 0x60]
00ab25  mov     rsi, qword ptr [rsp + 0x68]
00ab2a  add     rsp, 0x30
00ab2e  pop     r15
00ab30  pop     r14
00ab32  pop     rdi
00ab33  ret     
00ab34  mov     qword ptr [rsp + 0x20], 0
00ab3d  xor     r9d, r9d
00ab40  xor     r8d, r8d
00ab43  xor     edx, edx
00ab45  xor     ecx, ecx
00ab47  call    qword ptr [rip + 0x3999b]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
00ab4d  int3    

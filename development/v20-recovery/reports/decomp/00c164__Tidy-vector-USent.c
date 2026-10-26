// v20 RVA 00c164..00c20a  (166 bytes)  _Tidy@?$vector@USent
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

00c164  mov     qword ptr [rsp + 8], rbx
00c169  mov     qword ptr [rsp + 0x10], rsi
00c16e  push    rdi
00c16f  sub     rsp, 0x30
00c173  mov     rdi, rcx
00c176  mov     rbx, qword ptr [rcx]
00c179  test    rbx, rbx
00c17c  je      0xc1e0
00c17e  mov     rsi, qword ptr [rcx + 8]
00c182  jmp     0xc190
00c184  mov     rcx, rbx
00c187  call    0xc2d8    ; _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std
00c18c  add     rbx, 0x20
00c190  cmp     rbx, rsi
00c193  jne     0xc184
00c195  mov     rcx, qword ptr [rdi]
00c198  mov     rdx, qword ptr [rdi + 0x10]
00c19c  sub     rdx, rcx
00c19f  and     rdx, 0xffffffffffffffe0
00c1a3  cmp     rdx, 0x1000
00c1aa  jb      0xc1c4
00c1ac  add     rdx, 0x27
00c1b0  mov     rax, qword ptr [rcx - 8]
00c1b4  sub     rcx, rax
00c1b7  sub     rcx, 8
00c1bb  cmp     rcx, 0x1f
00c1bf  ja      0xc1f0
00c1c1  mov     rcx, rax
00c1c4  call    0x4030c
00c1c9  mov     qword ptr [rdi], 0
00c1d0  mov     qword ptr [rdi + 8], 0
00c1d8  mov     qword ptr [rdi + 0x10], 0
00c1e0  mov     rbx, qword ptr [rsp + 0x40]
00c1e5  mov     rsi, qword ptr [rsp + 0x48]
00c1ea  add     rsp, 0x30
00c1ee  pop     rdi
00c1ef  ret     
00c1f0  mov     qword ptr [rsp + 0x20], 0
00c1f9  xor     r9d, r9d
00c1fc  xor     r8d, r8d
00c1ff  xor     edx, edx
00c201  xor     ecx, ecx
00c203  call    qword ptr [rip + 0x382df]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
00c209  int3    

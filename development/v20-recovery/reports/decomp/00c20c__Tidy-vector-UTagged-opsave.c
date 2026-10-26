// v20 RVA 00c20c..00c2d8  (204 bytes)  _Tidy@?$vector@UTagged@opsave
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

00c20c  mov     qword ptr [rsp + 8], rbx
00c211  mov     qword ptr [rsp + 0x10], rsi
00c216  push    rdi
00c217  sub     rsp, 0x30
00c21b  mov     rdi, rcx
00c21e  mov     rbx, qword ptr [rcx]
00c221  test    rbx, rbx
00c224  je      0xc2ae
00c22a  mov     rsi, qword ptr [rcx + 8]
00c22e  jmp     0xc23c
00c230  mov     rcx, rbx
00c233  call    0xbe5c    ; _Tidy@?$vector@EV?$allocator@E@std
00c238  add     rbx, 0x18
00c23c  cmp     rbx, rsi
00c23f  jne     0xc230
00c241  mov     r8, qword ptr [rdi]
00c244  mov     rcx, qword ptr [rdi + 0x10]
00c248  sub     rcx, r8
00c24b  movabs  rax, 0x2aaaaaaaaaaaaaab
00c255  imul    rcx
00c258  sar     rdx, 2
00c25c  mov     rax, rdx
00c25f  shr     rax, 0x3f
00c263  add     rdx, rax
00c266  lea     rdx, [rdx + rdx*2]
00c26a  shl     rdx, 3
00c26e  cmp     rdx, 0x1000
00c275  jb      0xc28f
00c277  add     rdx, 0x27
00c27b  mov     rax, qword ptr [r8 - 8]
00c27f  sub     r8, rax
00c282  sub     r8, 8
00c286  cmp     r8, 0x1f
00c28a  ja      0xc2be
00c28c  mov     r8, rax
00c28f  mov     rcx, r8
00c292  call    0x4030c
00c297  mov     qword ptr [rdi], 0
00c29e  mov     qword ptr [rdi + 8], 0
00c2a6  mov     qword ptr [rdi + 0x10], 0
00c2ae  mov     rbx, qword ptr [rsp + 0x40]
00c2b3  mov     rsi, qword ptr [rsp + 0x48]
00c2b8  add     rsp, 0x30
00c2bc  pop     rdi
00c2bd  ret     
00c2be  mov     qword ptr [rsp + 0x20], 0
00c2c7  xor     r9d, r9d
00c2ca  xor     r8d, r8d
00c2cd  xor     edx, edx
00c2cf  xor     ecx, ecx
00c2d1  call    qword ptr [rip + 0x38211]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
00c2d7  int3    

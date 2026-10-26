// v20 RVA 00c090..00c161  (209 bytes)  _Tidy@?$vector@UTagged@opsave
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

00c090  mov     qword ptr [rsp + 0x10], rbx
00c095  mov     qword ptr [rsp + 0x18], rsi
00c09a  push    rdi
00c09b  sub     rsp, 0x30
00c09f  mov     rdi, rcx
00c0a2  mov     rbx, qword ptr [rcx]
00c0a5  test    rbx, rbx
00c0a8  je      0xc137
00c0ae  mov     rsi, qword ptr [rcx + 8]
00c0b2  jmp     0xc0c0
00c0b4  mov     rcx, rbx
00c0b7  call    0xc2d8    ; _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std
00c0bc  add     rbx, 0x60
00c0c0  cmp     rbx, rsi
00c0c3  mov     qword ptr [rsp + 0x40], rbx
00c0c8  jne     0xc0b4
00c0ca  mov     r8, qword ptr [rdi]
00c0cd  mov     rcx, qword ptr [rdi + 0x10]
00c0d1  sub     rcx, r8
00c0d4  movabs  rax, 0x2aaaaaaaaaaaaaab
00c0de  imul    rcx
00c0e1  sar     rdx, 4
00c0e5  mov     rax, rdx
00c0e8  shr     rax, 0x3f
00c0ec  add     rdx, rax
00c0ef  lea     rdx, [rdx + rdx*2]
00c0f3  shl     rdx, 5
00c0f7  cmp     rdx, 0x1000
00c0fe  jb      0xc118
00c100  add     rdx, 0x27
00c104  mov     rax, qword ptr [r8 - 8]
00c108  sub     r8, rax
00c10b  sub     r8, 8
00c10f  cmp     r8, 0x1f
00c113  ja      0xc147
00c115  mov     r8, rax
00c118  mov     rcx, r8
00c11b  call    0x4030c
00c120  mov     qword ptr [rdi], 0
00c127  mov     qword ptr [rdi + 8], 0
00c12f  mov     qword ptr [rdi + 0x10], 0
00c137  mov     rbx, qword ptr [rsp + 0x48]
00c13c  mov     rsi, qword ptr [rsp + 0x50]
00c141  add     rsp, 0x30
00c145  pop     rdi
00c146  ret     
00c147  mov     qword ptr [rsp + 0x20], 0
00c150  xor     r9d, r9d
00c153  xor     r8d, r8d
00c156  xor     edx, edx
00c158  xor     ecx, ecx
00c15a  call    qword ptr [rip + 0x38388]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
00c160  int3    

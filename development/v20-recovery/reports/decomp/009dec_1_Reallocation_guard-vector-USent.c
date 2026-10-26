// v20 RVA 009dec..009e7d  (145 bytes)  1_Reallocation_guard@?$vector@USent
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

009dec  mov     qword ptr [rsp + 8], rbx
009df1  mov     qword ptr [rsp + 0x10], rsi
009df6  push    rdi
009df7  sub     rsp, 0x30
009dfb  mov     rdi, rcx
009dfe  cmp     qword ptr [rcx + 8], 0
009e03  je      0x9e53
009e05  mov     rsi, qword ptr [rcx + 0x20]
009e09  mov     rbx, qword ptr [rcx + 0x18]
009e0d  jmp     0x9e1b
009e0f  mov     rcx, rbx
009e12  call    0xc2d8    ; _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std
009e17  add     rbx, 0x20
009e1b  cmp     rbx, rsi
009e1e  jne     0x9e0f
009e20  mov     rdx, qword ptr [rdi + 0x10]
009e24  shl     rdx, 5
009e28  mov     rcx, qword ptr [rdi + 8]
009e2c  cmp     rdx, 0x1000
009e33  jb      0x9e4d
009e35  add     rdx, 0x27
009e39  mov     rax, qword ptr [rcx - 8]
009e3d  sub     rcx, rax
009e40  sub     rcx, 8
009e44  cmp     rcx, 0x1f
009e48  ja      0x9e63
009e4a  mov     rcx, rax
009e4d  call    0x4030c
009e52  nop     
009e53  mov     rbx, qword ptr [rsp + 0x40]
009e58  mov     rsi, qword ptr [rsp + 0x48]
009e5d  add     rsp, 0x30
009e61  pop     rdi
009e62  ret     
009e63  mov     qword ptr [rsp + 0x20], 0
009e6c  xor     r9d, r9d
009e6f  xor     r8d, r8d
009e72  xor     edx, edx
009e74  xor     ecx, ecx
009e76  call    qword ptr [rip + 0x3a66c]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
009e7c  int3    

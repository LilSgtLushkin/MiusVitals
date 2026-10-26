// v20 RVA 00ac14..00acfd  (233 bytes)  _Change_array@?$vector@UTagged@opsave
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

00ac14  mov     qword ptr [rsp + 8], rbx
00ac19  mov     qword ptr [rsp + 0x10], rsi
00ac1e  mov     qword ptr [rsp + 0x18], rdi
00ac23  push    r12
00ac25  push    r14
00ac27  push    r15
00ac29  sub     rsp, 0x30
00ac2d  mov     r15, r9
00ac30  mov     r12, r8
00ac33  mov     r14, rdx
00ac36  mov     rdi, rcx
00ac39  mov     rbx, qword ptr [rcx]
00ac3c  test    rbx, rbx
00ac3f  je      0xacae
00ac41  mov     rsi, qword ptr [rcx + 8]
00ac45  jmp     0xac53
00ac47  mov     rcx, rbx
00ac4a  call    0xbe5c    ; _Tidy@?$vector@EV?$allocator@E@std
00ac4f  add     rbx, 0x18
00ac53  cmp     rbx, rsi
00ac56  jne     0xac47
00ac58  mov     r8, qword ptr [rdi]
00ac5b  mov     rcx, qword ptr [rdi + 0x10]
00ac5f  sub     rcx, r8
00ac62  movabs  rax, 0x2aaaaaaaaaaaaaab
00ac6c  imul    rcx
00ac6f  sar     rdx, 2
00ac73  mov     rax, rdx
00ac76  shr     rax, 0x3f
00ac7a  add     rdx, rax
00ac7d  lea     rdx, [rdx + rdx*2]
00ac81  shl     rdx, 3
00ac85  cmp     rdx, 0x1000
00ac8c  jb      0xaca6
00ac8e  add     rdx, 0x27
00ac92  mov     rax, qword ptr [r8 - 8]
00ac96  sub     r8, rax
00ac99  sub     r8, 8
00ac9d  cmp     r8, 0x1f
00aca1  ja      0xace3
00aca3  mov     r8, rax
00aca6  mov     rcx, r8
00aca9  call    0x4030c
00acae  mov     qword ptr [rdi], r14
00acb1  lea     rax, [r12 + r12*2]
00acb5  lea     rcx, [r14 + rax*8]
00acb9  mov     qword ptr [rdi + 8], rcx
00acbd  lea     rax, [r15 + r15*2]
00acc1  lea     rcx, [r14 + rax*8]
00acc5  mov     qword ptr [rdi + 0x10], rcx
00acc9  mov     rbx, qword ptr [rsp + 0x50]
00acce  mov     rsi, qword ptr [rsp + 0x58]
00acd3  mov     rdi, qword ptr [rsp + 0x60]
00acd8  add     rsp, 0x30
00acdc  pop     r15
00acde  pop     r14
00ace0  pop     r12
00ace2  ret     
00ace3  mov     qword ptr [rsp + 0x20], 0
00acec  xor     r9d, r9d
00acef  xor     r8d, r8d
00acf2  xor     edx, edx
00acf4  xor     ecx, ecx
00acf6  call    qword ptr [rip + 0x397ec]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
00acfc  int3    

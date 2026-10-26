// v20 RVA 009e80..009f15  (149 bytes)  1_Reallocation_guard@?$vector@UTagged@opsave
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

009e80  mov     qword ptr [rsp + 8], rbx
009e85  mov     qword ptr [rsp + 0x10], rsi
009e8a  push    rdi
009e8b  sub     rsp, 0x30
009e8f  mov     rdi, rcx
009e92  cmp     qword ptr [rcx + 8], 0
009e97  je      0x9eeb
009e99  mov     rsi, qword ptr [rcx + 0x20]
009e9d  mov     rbx, qword ptr [rcx + 0x18]
009ea1  jmp     0x9eaf
009ea3  mov     rcx, rbx
009ea6  call    0xbe5c    ; _Tidy@?$vector@EV?$allocator@E@std
009eab  add     rbx, 0x18
009eaf  cmp     rbx, rsi
009eb2  jne     0x9ea3
009eb4  mov     rax, qword ptr [rdi + 0x10]
009eb8  lea     rdx, [rax + rax*2]
009ebc  shl     rdx, 3
009ec0  mov     rcx, qword ptr [rdi + 8]
009ec4  cmp     rdx, 0x1000
009ecb  jb      0x9ee5
009ecd  add     rdx, 0x27
009ed1  mov     rax, qword ptr [rcx - 8]
009ed5  sub     rcx, rax
009ed8  sub     rcx, 8
009edc  cmp     rcx, 0x1f
009ee0  ja      0x9efb
009ee2  mov     rcx, rax
009ee5  call    0x4030c
009eea  nop     
009eeb  mov     rbx, qword ptr [rsp + 0x40]
009ef0  mov     rsi, qword ptr [rsp + 0x48]
009ef5  add     rsp, 0x30
009ef9  pop     rdi
009efa  ret     
009efb  mov     qword ptr [rsp + 0x20], 0
009f04  xor     r9d, r9d
009f07  xor     r8d, r8d
009f0a  xor     edx, edx
009f0c  xor     ecx, ecx
009f0e  call    qword ptr [rip + 0x3a5d4]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
009f14  int3    

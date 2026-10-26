// v20 RVA 00ab50..00ac13  (195 bytes)  _Change_array@?$vector@USent
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

00ab50  mov     qword ptr [rsp + 8], rbx
00ab55  mov     qword ptr [rsp + 0x10], rbp
00ab5a  mov     qword ptr [rsp + 0x18], rsi
00ab5f  push    rdi
00ab60  push    r14
00ab62  push    r15
00ab64  sub     rsp, 0x30
00ab68  mov     rsi, r9
00ab6b  mov     rbp, r8
00ab6e  mov     r15, rdx
00ab71  mov     rdi, rcx
00ab74  mov     rbx, qword ptr [rcx]
00ab77  test    rbx, rbx
00ab7a  je      0xabc7
00ab7c  mov     r14, qword ptr [rcx + 8]
00ab80  jmp     0xab8e
00ab82  mov     rcx, rbx
00ab85  call    0xc2d8    ; _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std
00ab8a  add     rbx, 0x20
00ab8e  cmp     rbx, r14
00ab91  jne     0xab82
00ab93  mov     rcx, qword ptr [rdi]
00ab96  mov     rdx, qword ptr [rdi + 0x10]
00ab9a  sub     rdx, rcx
00ab9d  and     rdx, 0xffffffffffffffe0
00aba1  cmp     rdx, 0x1000
00aba8  jb      0xabc2
00abaa  add     rdx, 0x27
00abae  mov     rax, qword ptr [rcx - 8]
00abb2  sub     rcx, rax
00abb5  sub     rcx, 8
00abb9  cmp     rcx, 0x1f
00abbd  ja      0xabf9
00abbf  mov     rcx, rax
00abc2  call    0x4030c
00abc7  mov     qword ptr [rdi], r15
00abca  shl     rbp, 5
00abce  add     rbp, r15
00abd1  mov     qword ptr [rdi + 8], rbp
00abd5  shl     rsi, 5
00abd9  add     rsi, r15
00abdc  mov     qword ptr [rdi + 0x10], rsi
00abe0  mov     rbx, qword ptr [rsp + 0x50]
00abe5  mov     rbp, qword ptr [rsp + 0x58]
00abea  mov     rsi, qword ptr [rsp + 0x60]
00abef  add     rsp, 0x30
00abf3  pop     r15
00abf5  pop     r14
00abf7  pop     rdi
00abf8  ret     
00abf9  mov     qword ptr [rsp + 0x20], 0
00ac02  xor     r9d, r9d
00ac05  xor     r8d, r8d
00ac08  xor     edx, edx
00ac0a  xor     ecx, ecx
00ac0c  call    qword ptr [rip + 0x398d6]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
00ac12  int3    

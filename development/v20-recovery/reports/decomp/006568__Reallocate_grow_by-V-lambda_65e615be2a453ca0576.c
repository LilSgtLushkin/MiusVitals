// v20 RVA 006568..0066bb  (339 bytes)  $_Reallocate_grow_by@V<lambda_65e615be2a453ca0576c979606f46740>
// decompiler failed (ClaripyOperationError: testing Expressions for truthiness does not do what you want, as these expressions can be symbolic): capstone listing, RIP-relative operands annotated

006568  mov     rax, rsp
00656b  mov     qword ptr [rax + 8], rbx
00656f  mov     qword ptr [rax + 0x10], rbp
006573  mov     qword ptr [rax + 0x18], rsi
006577  mov     qword ptr [rax + 0x20], rdi
00657b  push    r12
00657d  push    r14
00657f  push    r15
006581  sub     rsp, 0x30
006585  mov     r14, qword ptr [rcx + 0x10]
006589  movabs  rbx, 0x7fffffffffffffff
006593  mov     rax, rbx
006596  mov     bpl, r9b
006599  sub     rax, r14
00659c  mov     rsi, rcx
00659f  cmp     rax, rdx
0065a2  jb      0x66b5
0065a8  mov     r15, qword ptr [rcx + 0x18]
0065ac  lea     r12, [r14 + rdx]
0065b0  mov     rdx, r12
0065b3  or      rdx, 0xf
0065b7  cmp     rdx, rbx
0065ba  ja      0x65cd
0065bc  mov     rcx, r15
0065bf  mov     rax, rbx
0065c2  shr     rcx, 1
0065c5  sub     rax, rcx
0065c8  cmp     r15, rax
0065cb  jbe     0x65d9
0065cd  movabs  rcx, 0x8000000000000000
0065d7  jmp     0x65fd
0065d9  lea     rax, [r15 + rcx]
0065dd  mov     rbx, rdx
0065e0  cmp     rdx, rax
0065e3  cmovb   rbx, rax
0065e7  lea     rcx, [rbx + 1]
0065eb  test    rcx, rcx
0065ee  jne     0x65f4
0065f0  xor     edi, edi
0065f2  jmp     0x660c
0065f4  cmp     rcx, 0x1000
0065fb  jb      0x6604
0065fd  call    0x48a0    ; $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std
006602  jmp     0x6609
006604  call    0x402d0
006609  mov     rdi, rax
00660c  mov     qword ptr [rsi + 0x10], r12
006610  mov     r8, r14
006613  mov     qword ptr [rsi + 0x18], rbx
006617  mov     rcx, rdi
00661a  cmp     r15, 0xf
00661e  jbe     0x667e
006620  mov     rbx, qword ptr [rsi]
006623  mov     rdx, rbx
006626  call    0x414c0
00662b  lea     rdx, [r15 + 1]
00662f  mov     byte ptr [rdi + r14], bpl
006633  mov     byte ptr [rdi + r14 + 1], 0
006639  cmp     rdx, 0x1000
006640  jb      0x665a
006642  mov     rax, qword ptr [rbx - 8]
006646  add     rdx, 0x27
00664a  sub     rbx, rax
00664d  sub     rbx, 8
006651  cmp     rbx, 0x1f
006655  ja      0x6664
006657  mov     rbx, rax
00665a  mov     rcx, rbx
00665d  call    0x4030c
006662  jmp     0x6690
006664  xor     r9d, r9d
006667  mov     qword ptr [rsp + 0x20], 0
006670  xor     r8d, r8d
006673  xor     edx, edx
006675  xor     ecx, ecx
006677  call    qword ptr [rip + 0x3de6b]    ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
00667d  int3    
00667e  mov     rdx, rsi
006681  call    0x414c0
006686  mov     byte ptr [rdi + r14], bpl
00668a  mov     byte ptr [rdi + r14 + 1], 0
006690  mov     qword ptr [rsi], rdi
006693  mov     rax, rsi
006696  mov     rbx, qword ptr [rsp + 0x50]
00669b  mov     rbp, qword ptr [rsp + 0x58]
0066a0  mov     rsi, qword ptr [rsp + 0x60]
0066a5  mov     rdi, qword ptr [rsp + 0x68]
0066aa  add     rsp, 0x30
0066ae  pop     r15
0066b0  pop     r14
0066b2  pop     r12
0066b4  ret     
0066b5  call    0xcda4    ; _Xran@?$_String_val@U?$_Simple_types@D@std
0066ba  int3    

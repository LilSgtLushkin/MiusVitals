// v20 RVA 022c84..022d76  (242 bytes)  find_local_ip
// decompiler failed (TypeError: unsupported operand type(s) for //: 'NoneType' and 'int'): capstone listing, RIP-relative operands annotated

022c84  mov     qword ptr [rsp + 0x18], rbx
022c89  push    rdi
022c8a  sub     rsp, 0x30
022c8e  mov     rdi, rcx
022c91  xor     edx, edx
022c93  xor     ecx, ecx
022c95  call    qword ptr [rip + 0x219ad]    ; ole32.dll!CoInitializeEx
022c9b  test    eax, eax
022c9d  js      0x22d69
022ca3  xor     edx, edx
022ca5  mov     qword ptr [rsp + 0x48], 0
022cae  lea     rax, [rsp + 0x48]
022cb3  mov     qword ptr [rsp + 0x40], 0
022cbc  lea     r9, [rip + 0x24d55]    ; 0x47a18
022cc3  mov     qword ptr [rsp + 0x20], rax
022cc8  lea     rcx, [rip + 0x24d39]    ; 0x47a08
022ccf  lea     r8d, [rdx + 0x17]
022cd3  call    qword ptr [rip + 0x21967]    ; ole32.dll!CoCreateInstance
022cd9  test    eax, eax
022cdb  js      0x22d53
022cdd  mov     rcx, qword ptr [rsp + 0x48]
022ce2  lea     rdx, [rsp + 0x40]
022ce7  mov     rax, qword ptr [rcx]
022cea  call    qword ptr [rax + 0x38]
022ced  test    eax, eax
022cef  js      0x22d53
022cf1  cmp     qword ptr [rsp + 0x40], 0
022cf7  je      0x22d53
022cf9  lea     rcx, [rip + 0x24d90]    ; 0x47a90
022d00  call    qword ptr [rip + 0x21592]    ; 0x44298
022d06  mov     rcx, qword ptr [rsp + 0x40]
022d0b  mov     r8, rax
022d0e  mov     rbx, rax
022d11  mov     rdx, qword ptr [rcx]
022d14  mov     r9, qword ptr [rdx + 0x50]
022d18  mov     edx, edi
022d1a  call    r9
022d1d  test    eax, eax
022d1f  lea     rcx, [rip + 0x24e3a]    ; "is removed"
022d26  lea     r8, [rip + 0x24e43]    ; "could not be removed"
022d2d  mov     edx, edi
022d2f  cmovns  r8, rcx
022d33  lea     rcx, [rip + 0x24e4e]    ; "net: UPnP: the router's forward of UDP %ld %s"
022d3a  call    0x27dc    ; mp_log
022d3f  mov     rcx, rbx
022d42  call    qword ptr [rip + 0x21548]    ; 0x44290
022d48  mov     rcx, qword ptr [rsp + 0x40]
022d4d  mov     rax, qword ptr [rcx]
022d50  call    qword ptr [rax + 0x10]
022d53  mov     rcx, qword ptr [rsp + 0x48]
022d58  test    rcx, rcx
022d5b  je      0x22d63
022d5d  mov     rax, qword ptr [rcx]
022d60  call    qword ptr [rax + 0x10]
022d63  call    qword ptr [rip + 0x218e7]    ; ole32.dll!CoUninitialize
022d69  mov     rbx, qword ptr [rsp + 0x50]
022d6e  xor     eax, eax
022d70  add     rsp, 0x30
022d74  pop     rdi
022d75  ret     

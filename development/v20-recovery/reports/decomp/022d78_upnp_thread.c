// v20 RVA 022d78..022ff9  (641 bytes)  upnp_thread
// decompiler failed (TypeError: unsupported operand type(s) for //: 'NoneType' and 'int'): capstone listing, RIP-relative operands annotated

022d78  mov     rax, rsp
022d7b  mov     qword ptr [rax + 8], rbx
022d7f  mov     qword ptr [rax + 0x10], rsi
022d83  mov     qword ptr [rax + 0x18], rdi
022d87  push    rbp
022d88  push    r12
022d8a  push    r15
022d8c  lea     rbp, [rax - 0x5f]
022d90  sub     rsp, 0xf0
022d97  mov     rax, qword ptr [rip + 0x375e2]    ; 0x5a380
022d9e  xor     rax, rsp
022da1  mov     qword ptr [rbp + 0x37], rax
022da5  xor     edx, edx
022da7  xor     ecx, ecx
022da9  call    qword ptr [rip + 0x21899]    ; ole32.dll!CoInitializeEx
022daf  test    eax, eax
022db1  js      0x22fce
022db7  xor     edx, edx
022db9  mov     qword ptr [rbp - 0x51], 0
022dc1  lea     rax, [rbp - 0x51]
022dc5  mov     qword ptr [rbp - 0x69], 0
022dcd  lea     r9, [rip + 0x24c44]    ; 0x47a18
022dd4  mov     qword ptr [rsp + 0x20], rax
022dd9  lea     rcx, [rip + 0x24c28]    ; 0x47a08
022de0  lea     r8d, [rdx + 0x17]
022de4  call    qword ptr [rip + 0x21856]    ; ole32.dll!CoCreateInstance
022dea  test    eax, eax
022dec  js      0x22f99
022df2  mov     rcx, qword ptr [rbp - 0x51]
022df6  lea     rdx, [rbp - 0x69]
022dfa  mov     rax, qword ptr [rcx]
022dfd  call    qword ptr [rax + 0x38]
022e00  test    eax, eax
022e02  js      0x22f99
022e08  cmp     qword ptr [rbp - 0x69], 0
022e0d  je      0x22f99
022e13  lea     rax, [rbp - 0x49]
022e17  mov     dword ptr [rsp + 0x28], 0x40
022e1f  or      r12d, 0xffffffff
022e23  mov     qword ptr [rsp + 0x20], rax
022e28  mov     r9d, r12d
022e2b  lea     r8, [rip + 0x3733e]    ; 0x5a170
022e32  xor     edx, edx
022e34  xor     ecx, ecx
022e36  call    qword ptr [rip + 0x21294]    ; KERNEL32.dll!MultiByteToWideChar
022e3c  lea     rcx, [rip + 0x24c4d]    ; 0x47a90
022e43  call    qword ptr [rip + 0x2144f]    ; 0x44298
022e49  lea     rcx, [rbp - 0x49]
022e4d  mov     rbx, rax
022e50  call    qword ptr [rip + 0x21442]    ; 0x44298
022e56  lea     rcx, [rip + 0x24c3b]    ; 0x47a98
022e5d  mov     rdi, rax
022e60  call    qword ptr [rip + 0x21432]    ; 0x44298
022e66  mov     rcx, qword ptr [rbp - 0x69]
022e6a  mov     rsi, rax
022e6d  movzx   edx, word ptr [rip + 0x372f4]    ; 0x5a168
022e74  lea     rax, [rbp - 0x59]
022e78  mov     qword ptr [rsp + 0x38], rax
022e7d  mov     r9d, edx
022e80  mov     qword ptr [rbp - 0x59], 0
022e88  mov     r8, qword ptr [rcx]
022e8b  mov     qword ptr [rsp + 0x30], rsi
022e90  mov     word ptr [rsp + 0x28], r12w
022e96  mov     qword ptr [rsp + 0x20], rdi
022e9b  mov     r10, qword ptr [r8 + 0x58]
022e9f  mov     r8, rbx
022ea2  call    r10
022ea5  test    eax, eax
022ea7  js      0x22f43
022ead  mov     rcx, qword ptr [rbp - 0x59]
022eb1  lea     r15, [rip + 0x38b78]    ; 0x5ba30
022eb8  mov     qword ptr [rbp - 0x61], 0
022ec0  test    rcx, rcx
022ec3  je      0x22f12
022ec5  mov     rax, qword ptr [rcx]
022ec8  lea     rdx, [rbp - 0x61]
022ecc  call    qword ptr [rax + 0x38]
022ecf  test    eax, eax
022ed1  js      0x22f12
022ed3  mov     r8, qword ptr [rbp - 0x61]
022ed7  test    r8, r8
022eda  je      0x22f12
022edc  mov     qword ptr [rsp + 0x38], 0
022ee5  mov     r9d, r12d
022ee8  mov     qword ptr [rsp + 0x30], 0
022ef1  xor     edx, edx
022ef3  mov     dword ptr [rsp + 0x28], 0x40
022efb  xor     ecx, ecx
022efd  mov     qword ptr [rsp + 0x20], r15
022f02  call    qword ptr [rip + 0x21280]    ; KERNEL32.dll!WideCharToMultiByte
022f08  mov     rcx, qword ptr [rbp - 0x61]
022f0c  call    qword ptr [rip + 0x2137e]    ; 0x44290
022f12  cmp     byte ptr [rip + 0x38b17], 0
022f19  lea     r8, [rip + 0x24bb8]    ; "unknown"
022f20  movzx   edx, word ptr [rip + 0x37241]    ; 0x5a168
022f27  lea     rcx, [rip + 0x24bb2]    ; "net: UPnP mapped UDP %u (external address %s)"
022f2e  cmovne  r8, r15
022f32  call    0x27dc    ; mp_log
022f37  mov     dword ptr [rip + 0x389c3], 2
022f41  jmp     0x22f63
022f43  movzx   r8d, word ptr [rip + 0x3721d]    ; 0x5a168
022f4b  lea     rcx, [rip + 0x24bbe]    ; "net: UPnP mapping refused (0x%08lx); relying on a manual port forward of UDP %u"
022f52  mov     edx, eax
022f54  call    0x27dc    ; mp_log
022f59  mov     dword ptr [rip + 0x389a1], 3
022f63  mov     rcx, qword ptr [rbp - 0x59]
022f67  test    rcx, rcx
022f6a  je      0x22f72
022f6c  mov     rax, qword ptr [rcx]
022f6f  call    qword ptr [rax + 0x10]
022f72  mov     rcx, rbx
022f75  call    qword ptr [rip + 0x21315]    ; 0x44290
022f7b  mov     rcx, rdi
022f7e  call    qword ptr [rip + 0x2130c]    ; 0x44290
022f84  mov     rcx, rsi
022f87  call    qword ptr [rip + 0x21303]    ; 0x44290
022f8d  mov     rcx, qword ptr [rbp - 0x69]
022f91  mov     rax, qword ptr [rcx]
022f94  call    qword ptr [rax + 0x10]
022f97  jmp     0x22fb9
022f99  movzx   r8d, word ptr [rip + 0x371c7]    ; 0x5a168
022fa1  lea     rcx, [rip + 0x24a88]    ; "net: UPnP not available on this network (0x%08lx); internet play needs UDP %u forwarded by hand"
022fa8  mov     edx, eax
022faa  call    0x27dc    ; mp_log
022faf  mov     dword ptr [rip + 0x3894b], 3
022fb9  mov     rcx, qword ptr [rbp - 0x51]
022fbd  test    rcx, rcx
022fc0  je      0x22fc8
022fc2  mov     rax, qword ptr [rcx]
022fc5  call    qword ptr [rax + 0x10]
022fc8  call    qword ptr [rip + 0x21682]    ; ole32.dll!CoUninitialize
022fce  xor     eax, eax
022fd0  mov     rcx, qword ptr [rbp + 0x37]
022fd4  xor     rcx, rsp
022fd7  call    0x402b0    ; security_cookie_check
022fdc  lea     r11, [rsp + 0xf0]
022fe4  mov     rbx, qword ptr [r11 + 0x20]
022fe8  mov     rsi, qword ptr [r11 + 0x28]
022fec  mov     rdi, qword ptr [r11 + 0x30]
022ff0  mov     rsp, r11
022ff3  pop     r15
022ff5  pop     r12
022ff7  pop     rbp
022ff8  ret     

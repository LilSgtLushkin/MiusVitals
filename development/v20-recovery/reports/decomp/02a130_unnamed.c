// v20 RVA 02a130..02a47a  (842 bytes)  start@Joiner
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char field_0;
    char paddin1[7];
    unsigned long long field_8;
    unsigned int field_10;
    char paddin20[365];
    char field_181;
    char paddin386[78];
    unsigned long long field_1d0;
    unsigned long long field_1d8;
    char paddin480[8];
    unsigned long long field_1e8;
    unsigned long long field_1f0;
    char paddin504[112];
    unsigned long long field_268;
    unsigned short field_270;
    char paddin626[14];
    unsigned long long field_280;
    unsigned long long field_288;
    char paddin656[8];
    unsigned long long field_298;
    unsigned long long field_2a0;
    char paddin680[36];
    unsigned long long field_2cc;
    unsigned int field_2d4;
} struct_1;

typedef struct struct_0 {
    char field_0;
    char paddin1[7];
    unsigned long long field_8;
    unsigned int field_10;
    char paddin20[365];
    char field_181;
    char paddin386[78];
    unsigned long long field_1d0;
    unsigned long long field_1d8;
    char paddin480[8];
    unsigned long long field_1e8;
    unsigned long long field_1f0;
    char paddin504[112];
    unsigned int *field_268;
    unsigned short field_270;
    char paddin626[14];
    unsigned long long field_280;
    unsigned long long field_288;
    char paddin656[8];
    unsigned long long field_298;
    unsigned long long field_2a0;
    char paddin680[36];
    unsigned long long field_2cc;
    unsigned int field_2d4;
} struct_0;

typedef struct WSADATA {
    unsigned short wVersion;
    unsigned short wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char paddin394[6];
    char *lpVendorInfo;
} WSADATA;

typedef struct ADDRINFOA {
    unsigned int ai_flags;
    unsigned int ai_family;
    unsigned int ai_socktype;
    unsigned int ai_protocol;
    unsigned int *ai_addrlen;
    char *ai_canonname;
    struct SOCKADDR *ai_addr;
    void* ai_next;
} ADDRINFOA;

typedef struct SOCKADDR {
    unsigned int sa_family;
    char sa_data[14];
} SOCKADDR;


long long start@Joiner(struct_1 *a0, unsigned long long a1)
{
    char v0;  // [bp-0x2f8]
    unsigned int v1;  // [bp-0x2a8]
    unsigned int v2;  // [bp-0x2a4]
    unsigned int v3;  // [bp-0x2a0]
    void* v4;  // [bp-0x298]
    ADDRINFOA v5;  // [bp-0x290]
    WSADATA v9;  // [bp-0x258]
    char v10;  // [bp-0xb8]
    unsigned long long v11;  // [bp-0x38]
    unsigned long v12;  // [bp-0x8]
    char v13;  // [bp+0x0]
    struct_0 *v15;  // rbx
    unsigned int *v16;  // rcx
    char *v17;  // rsi
    unsigned long long v18;  // rax
    void* v19;  // rax, Other Possible Types: unsigned long
    unsigned long long v20;  // rdi, Other Possible Types: unsigned long
    unsigned short v21;  // di
    char *v22;  // rax
    ADDRINFOA *v24;  // rcx
    unsigned int *v25;  // rax
    unsigned int *v26;  // rcx
    unsigned long long v27;  // rax
    unsigned long long v28;  // rax
    unsigned long long v29;  // rax
    unsigned long long v30;  // rcx

    v12 = &v13;
    v11 = g_5a380 ^ &v0;
    v15 = a0;
    v16 = *((long long *)(a0 + 616));
    v20 = a1;
    if (v16 != -1)
        closesocket(v16);
    *((long long *)(v15 + 616)) = -1;
    *((char **)(v15 + 16)) = 0;
    v17 = v15 + 20;
    *(&v17) = 0;
    strcpy_s(v15 + 180, 128, a1);
    (unsigned int)v18 = _strnicmp(a1, "steam:", 6);
    v15->field_0 = (unsigned int)v18 == 0;
    if ((unsigned int)v18)
        v19 = 0;
    else
        (unsigned int)v19 = _strtoui64(&(6)[a1], 0, 10);
    *((unsigned long *)(v15 + 8)) = v19;
    if (v15->field_0 && (!(v19) || !(g_5c118) || !(g_5c120)))
    {
        strcpy_s(v17, 160, "Joining through Steam is done from the game's menu, with Steam running.");
        v26 = *((long long *)(v15 + 616));
        if (v26 != -1)
            closesocket(v26);
        *((long long *)(v15 + 616)) = -1;
        *((char **)(v15 + 16)) = 5;
        return security_cookie_check(v11 ^ &v0);
    }
    if (v15->field_0)
        v20 = "127.0.0.";
    strcpy_s(&v10, 128, v20);
    v21 = 1943;
    (unsigned int)v22 = strchr(&v10, 58);
    if (v22)
    {
        *(&v22) = 0;
        v21 = atoi(v22 + 1);
    }
    WSAStartup(514, &v9);
    *((char **)(v15 + 624)) = 0;
    *((char **)(v15 + 624)) = 2;
    *((unsigned short *)(v15 + 626)) = htons(v21);
    if (!v10)
        strcpy_s(v17, 160, "Enter the host's address first.");
    if (inet_pton(2, &v10, v15 + 628) != 1)
    {
        v4 = 0;
        *((char **)&v5.ai_flags) = 0;
        *((char **)&v5.ai_family) = 2;
        v5.ai_addrlen = 0;
        v5.ai_addr = 0;
        if (!(!getaddrinfo(&v10, 0, &v5, &v4)) || !((v24 = v4, v24)))
            strcpy_s(v17, 160, "That address could not be found.");
        *((int *)(v15 + 628)) = *((int *)&(4)[*((long long *)(v24 + 32))]);
        freeaddrinfo(v24);
    }
    v25 = socket(2, 2, 17);
    *((char **)&v1) = 1;
    *((unsigned int **)(v15 + 616)) = v25;
    ioctlsocket(v25, 2147772030, &v1);
    *((char **)&v3) = 0;
    *((char **)&v2) = 0;
    WSAIoctl(*((long long *)(v15 + 616)), 2550136844, &v3, 4, 0, 0, &v2, 0, 0);
    v27 = *((long long *)(v15 + 640));
    if (v27 != *((long long *)(v15 + 648)))
        *((unsigned long long *)(v15 + 648)) = v27;
    v28 = *((long long *)(v15 + 664));
    if (v28 != *((long long *)(v15 + 672)))
        *((unsigned long long *)(v15 + 672)) = v28;
    v29 = *((long long *)(v15 + 464));
    if (v29 != *((long long *)(v15 + 472)))
        *((unsigned long long *)(v15 + 472)) = v29;
    v30 = *((long long *)(v15 + 488));
    if (v30 != *((long long *)(v15 + 496)))
        *((unsigned long long *)(v15 + 496)) = v30;
    *((char **)(v15 + 385)) = 0;
    *((char **)(v15 + 716)) = 0;
    *((unsigned int *)(v15 + 724)) = GetTickCount();
    *((char **)(v15 + 16)) = 1;
    return security_cookie_check(v11 ^ &v0);
}


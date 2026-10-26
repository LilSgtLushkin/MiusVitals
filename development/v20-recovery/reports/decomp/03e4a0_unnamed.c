// v20 RVA 03e4a0..03e539  (153 bytes)  operation_exists@Joiner
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long operation_exists@Joiner(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3, unsigned int a4, unsigned int a5)
{
    char v0;  // [bp-0x138]
    char v1;  // [bp-0x118]
    char v2;  // [bp-0x114]
    char v3;  // [bp-0x113]
    unsigned int v4;  // [bp-0x110]
    unsigned int v5;  // [bp-0x10c]
    unsigned int v6;  // [bp-0x104]
    unsigned int v7;  // [bp-0x100]
    unsigned int v8;  // [bp-0xfc]
    unsigned long long v9;  // [bp-0x38]
    int v11;  // xmm3

    v9 = g_5a380 ^ &v0;
    memset(&v1, 0, 220);
    v6 = a4;
    v7 = a5;
    v8 = (unsigned int)v11;
    *((char **)&v2) = 8;
    v3 = a0;
    v4 = a1;
    v5 = a2;
    shots_queue(&v1);
    return security_cookie_check(v9 ^ &v0);
}


// v20 RVA 03387c..033940  (196 bytes)  explode_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long explode_hook(unsigned long long a0, unsigned long long a1, unsigned long a2)
{
    char v0;  // [bp-0xe8]
    unsigned long v1;  // [bp-0xc0]
    char v2;  // [bp-0xb8]
    char v3;  // [bp-0x80]
    char v4;  // [bp-0x78]
    unsigned long v5;  // [bp-0x70]
    unsigned long long v6;  // [bp-0x28]
    char v8;  // dl

    v6 = g_5a380 ^ &v0;
    if (!g_5d028)
        return security_cookie_check(v6 ^ &v0);
    memset(&v2, 0, 144);
    memcpy(&v2, a1, 48);
    memcpy(&v3, &(72)[a1], 8);
    memcpy(&v4, &(64)[a1], 8);
    v1 = a2;
    v8 = 88;
    v5 = g_5d018;
    log_launch(a0, rdx<8>, &v2, 0, 0);
    return security_cookie_check(v6 ^ &v0);
}


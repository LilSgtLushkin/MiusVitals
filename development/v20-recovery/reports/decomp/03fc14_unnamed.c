// v20 RVA 03fc14..03fd16  (258 bytes)  mp_log
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long mp_log(unsigned long long a0, void* a1, unsigned long a2)
{
    char v0;  // [bp-0x68]
    int v1;  // [bp-0x38], Other Possible Types: unsigned short
    unsigned short v2;  // [bp-0x36]
    unsigned long long v3;  // [bp-0x28]

    v3 = g_5a380 ^ &v0;
    if ((char)steam_init() && a0 && (unsigned int)(a2 - 1) <= 1199)
    {
        g_5df20 = &(1)[g_5df20];
        if (!g_5df68)
        {
            g_5de38(g_5de78, a0, a1, a2, 0);
            return security_cookie_check(v3 ^ &v0);
        }
        *((char **)&v1) = 0;
        *((char **)&v1) = 2;
        v2 = htons(a0 - &g_63c0);
        inet_pton(2, "127.0.0.", &v1);
        sendto(g_5a300, a1, a2, 0, &v1, 16);
        return security_cookie_check(v3 ^ &v0);
    }
    return security_cookie_check(v3 ^ &v0);
}


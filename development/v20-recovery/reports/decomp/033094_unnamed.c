// v20 RVA 033094..03327e  (490 bytes)  restore_slot
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long restore_slot(unsigned int a0[18], struct struct_2 **a1)
{
    char v0;  // [bp-0xe8]
    unsigned long v1;  // [bp-0xb0]
    char v2;  // [bp-0xa8]
    char v3;  // [bp-0xa4]
    char v4;  // [bp-0xa0]
    void* v5;  // [bp-0x98]
    unsigned long v6;  // [bp-0x90]
    unsigned long v7;  // [bp-0x88]
    struct_0 *v8;  // [bp-0x80]
    void* v9;  // [bp-0x78]
    unsigned int v10;  // [bp-0x68]
    unsigned int v11;  // [bp-0x64]
    unsigned int v12;  // [bp-0x60]
    unsigned int v13;  // [bp-0x5c]
    char v14;  // [bp-0x58]
    unsigned long long v15;  // [bp-0x48]
    struct struct_1 **v17;  // r8

    v15 = g_5a380 ^ &v0;
    if (!g_5cff8)
        return security_cookie_check(v15 ^ &v0);
    if (!g_5d010)
    {
        v5 = 0;
        v6 = "i_divisions";
        v9 = 0;
        v7 = "divisions";
        v8 = &v5;
        g_5d010 = v17(a1, &v6, *((long long *)(*(a1) + 72)));
        if (!g_5d010)
        {
            g_5d070 = &(1)[g_5d070];
            return security_cookie_check(v15 ^ &v0);
        }
    }
    v10 = *((int *)(a0 + 60));
    v12 = *((int *)(a0 + 68));
    v11 = *((int *)(a0 + 64));
    *((char **)&v13) = 0;
    memcpy(&v14, a0 + 76, 16);
    memcpy(&v3, a0 + 92, 4);
    memcpy(&v2, a0 + 96, 4);
    memcpy(&v4, a0 + 100, 4);
    if (g_5d028)
    {
        v1 = v12;
        fprintf(g_5d028, "F %llu received side=%08x kind=%u pos=%.2f,%.2f,%.2f\n", sub_19f10(), *((int *)&v2), *((int *)&v3), v10, v11);
        fflush(g_5d028);
    }
    *((char **)&g_5d024) = 1;
    g_5cff8(g_5d010, &v10, *((int *)&v3), &v14, *((int *)&v2));
    g_5d0c0 = &(1)[g_5d0c0];
    *((char **)&g_5d024) = 0;
    return security_cookie_check(v15 ^ &v0);
}


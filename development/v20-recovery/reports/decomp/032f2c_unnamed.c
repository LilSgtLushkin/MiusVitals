// v20 RVA 032f2c..033091  (357 bytes)  trench_sim_step
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long trench_sim_step(unsigned int a0[7])
{
    char v0;  // [bp-0xa8]
    char v1;  // [bp-0x88]
    char v2;  // [bp-0x80]
    char v3;  // [bp-0x78]
    unsigned long v4;  // [bp-0x38]
    unsigned long v5;  // [bp-0x30]
    unsigned long long v6;  // [bp-0x18]
    unsigned int v8[7];  // rbx
    unsigned long v10;  // rsi
    unsigned long long v11;  // rdi

    v6 = g_5a380 ^ &v0;
    v8 = a0;
    if (!g_5cfe8)
        return security_cookie_check(v6 ^ &v0);
    v10 = (xlate_desc(*((int *)(a0 + 12))) == -1 ? *((long long *)(g_5d000->field_0 + 456))(g_5d000) : 0);
    v11 = (xlate_desc(*((int *)(v8 + 8))) == -1 ? *((long long *)(g_5d000->field_0 + 456))(g_5d000) : 0);
    ammo_check(v11, &v2, &v1);
    if ((*((int *)(v8 + 12)) == -1 || v10) && (*((int *)(v8 + 8)) == -1 || v11) && !memcmp(&v2, v8 + 28, 8) && *((int *)&v1) == *((int *)(v8 + 24)))
    {
        memcpy(&v3, v8 + 76, 88);
        v4 = v10;
        v5 = v11;
        explode_hook("received", &v3, 0);
        *((char **)&g_5d024) = 1;
        g_5cfe8(g_5d008, &v3);
        g_5d0a8 = &(1)[g_5d0a8];
        *((char **)&g_5d024) = 0;
        return security_cookie_check(v6 ^ &v0);
    }
    g_5d070 = &(1)[g_5d070];
    return security_cookie_check(v6 ^ &v0);
}


// v20 RVA 03847c..0385bb  (319 bytes)  trench_resend
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    uint128_t field_0;
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    unsigned long long field_50;
    unsigned int field_58;
} struct_0;


long long trench_resend()
{
    char v0;  // [bp-0x1a8]
    char v1;  // [bp-0x188]
    int v2;  // [bp-0xf8]
    int v3;  // [bp-0xe8]
    int v4;  // [bp-0xd8]
    int v5;  // [bp-0xc8]
    int v6;  // [bp-0xb8]
    int v7;  // [bp-0xa8]
    int v8;  // [bp-0x98]
    int v9;  // [bp-0x88]
    int v10;  // [bp-0x78]
    int v11;  // [bp-0x68]
    int v12;  // [bp-0x58]
    int v13;  // [bp-0x48]
    int v14;  // [bp-0x38]
    unsigned long v15;  // [bp-0x28]
    unsigned int v16;  // [bp-0x20]
    unsigned long long v17;  // [bp-0x18]
    uint128_t v19[8];  // rbx, Other Possible Types: unsigned long
    struct_0 *v20;  // rcx

    v17 = g_5a380 ^ &v0;
    *((char **)&g_5a2ee) = 1;
    if (!g_5a2ed)
    {
        return security_cookie_check(v17 ^ &v0);
    }
    else if (!g_5d218)
    {
        return security_cookie_check(v17 ^ &v0);
    }
    else
    {
        for (v19 = g_5d190; v19 != g_5d198; v19 = &(220)[v19])
        {
            v20 = &(128)[v19];
            *((int128_t *)&v2) = *((int128_t *)v19);
            *((int128_t *)&v3) = *((int128_t *)&(16)[v19]);
            *((int128_t *)&v4) = *((int128_t *)&(32)[v19]);
            *((int128_t *)&v5) = *((int128_t *)&(48)[v19]);
            *((int128_t *)&v6) = *((int128_t *)&(64)[v19]);
            *((int128_t *)&v7) = *((int128_t *)&(80)[v19]);
            *((int128_t *)&v8) = *((int128_t *)&(96)[v19]);
            *((int128_t *)&v9) = *((int128_t *)&(112)[v19]);
            *((uint128_t *)&v10) = v20->field_0;
            *((int128_t *)&v11) = *((int128_t *)(v20 + 16));
            *((int128_t *)&v12) = *((int128_t *)(v20 + 32));
            *((int128_t *)&v13) = *((int128_t *)(v20 + 48));
            *((int128_t *)&v14) = *((int128_t *)(v20 + 64));
            v15 = *((long long *)(v20 + 80));
            v16 = *((int *)(v20 + 88));
            memcpy(&v1, &v6, 140);
            if ((char)sub_1bb14(*((int *)&v1)))
                shots_queue(&v2);
        }
        return security_cookie_check(v17 ^ &v0);
    }
}


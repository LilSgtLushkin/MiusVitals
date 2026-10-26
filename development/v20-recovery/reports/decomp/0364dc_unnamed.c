// v20 RVA 0364dc..03688e  (946 bytes)  dig_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[720];
    unsigned int field_2d0;
} struct_0;


long long dig_hook(struct_0 *a0, unsigned long long a1, unsigned long long a2, unsigned long long a3, unsigned long a4, unsigned int a5, unsigned int a6)
{
    char v0;  // [bp-0x208]
    unsigned long v1;  // [bp-0x1d8], Other Possible Types: unsigned int
    unsigned long v2;  // [bp-0x1d0]
    unsigned long v3;  // [bp-0x1c8]
    unsigned long v4;  // [bp-0x1c0]
    unsigned long v5;  // [bp-0x1b8]
    unsigned long v6;  // [bp-0x1b0]
    unsigned long long v7;  // [bp-0x1a8]
    int v8;  // [bp-0x1a0]
    int v9;  // [bp-0x190], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x18c]
    unsigned int v11;  // [bp-0x188]
    unsigned int v12;  // [bp-0x184]
    int v13;  // [bp-0x180]
    char v14;  // [bp-0x168]
    char v15;  // [bp-0x164]
    char v16;  // [bp-0x11c]
    char v17;  // [bp-0x88]
    void* v18;  // [bp-0x87]
    unsigned int v19;  // [bp-0x7f]
    unsigned short v20;  // [bp-0x7b]
    char v21;  // [bp-0x79]
    unsigned long long v22;  // [bp-0x78]
    unsigned long long v24;  // rsi
    struct_0 *v25;  // r15
    unsigned long long v26;  // rax, Other Possible Types: unsigned long
    unsigned int v27;  // ebx
    char v28;  // cl
    char v29;  // dil
    int v30;  // xmm6
    unsigned long long v31;  // rcx
    unsigned long long v32;  // rdx
    unsigned long v33;  // rax
    unsigned long long v34;  // rcx
    unsigned long long v35;  // r9
    unsigned long long v36;  // r8

    v22 = g_5a380 ^ &v0;
    v24 = a4;
    v7 = a1;
    g_5d270 = a0;
    v25 = a0;
    (unsigned int)v26 = sub_1c334();
    v27 = v26;
    if (!(unsigned int)v26)
    {
        v17 = v26;
        v18 = 0;
        *((char **)&v19) = 0;
        *((char **)&v20) = 0;
        *((char **)&v21) = 0;
        GetEnvironmentVariableA("MP_ROLE", &v17, 16);
        (unsigned int)v26 = strcmp(&v17, "host");
        if ((unsigned int)v26)
            v27 = -(0 < v17) & 2;
        else
            v27 = &(1)[v26];
    }
    v28 = g_5d0ed;
    *((char *)&v26) = g_5a2ef;
    if (!(!g_5d0ed) || !(g_5a2ef))
    {
        if (!(v27 == 0))
            goto LABEL_3681f;
    }
    else
    {
        if (v27)
        {
            if (GetCurrentThreadId() != g_5d278)
            {
                v28 = g_5d0ed;
                *((char *)&v26) = g_5a2ef;
            }
            else
            {
                if (!(char)sub_1d2d4() || !(char)sub_19f18() || !(char)sub_1e518())
                    v29 = 0;
                else
                    v29 = 1;
                if (v27 != 1 && v29)
                {
                    g_5d2d0 = &(1)[g_5d2d0];
                    return security_cookie_check(v22 ^ &v0);
                }
                memset(&v14, 0, 220);
                *((char **)&v8) = 0;
                *((char **)&v9) = 0;
                *((char **)&v13) = 0;
                if ((char)readable(a2, 16))
                    memcpy(&v8, a2, 16);
                if ((char)readable(v24, 16))
                    memcpy(&v13, v24, 16);
                v30 = 0;
                *((unsigned int *)&v30) = a6;
                v1 = (unsigned int)v30;
                v11 = (unsigned int)v30;
                v9 = a3;
                v10 = a5;
                v12 = *((int *)(v25 + 720));
                g_5d2c0(v25, v7, a2, a3, v24, v10);
                *((char **)&v15) = 5;
                memcpy(&v16, &v8, 48);
                if ((5366325548715505925 * (g_5d1b0 - g_5d1a8) >> 64 >> 6) + (5366325548715505925 * (g_5d1b0 - g_5d1a8) >> 64 >> 6 >> 63) < 60000)
                    sub_3277c(&g_5d1a8, &v14);
                if (v27 != 1)
                    return security_cookie_check(v22 ^ &v0);
                g_5d2c8 = &(1)[g_5d2c8];
                if (g_5d0eb && (char)sub_1d2d4())
                {
                    v31 = g_5d2f0;
                    g_5d2f0 = &(1)[g_5d2f0];
                    if (v31 < 300)
                    {
                        v32 = a3;
                        v33 = (v29 ? &g_45ce6 : " - sent");
                        v34 = "crater: kind %d at %.1f %.1f %.1f, flags %x, size %.2f, vector %.2f %.2f %.2f %.2f%s";
                        v6 = v33;
                        v5 = (int)(&v13)[12];
                        v4 = (int)(&v13)[8];
                        v3 = (int)(&v13)[4];
                        v2 = (int)v13;
                        v1 = (unsigned long long)v30;
                        v35 = (int)(&v8)[4];
                        v36 = (int)v8;
                        mp_log();
                    }
                }
                if (!v29)
                    return security_cookie_check(v22 ^ &v0);
                shots_queue(&v14);
                return security_cookie_check(v22 ^ &v0);
            }
LABEL_3681f:
            if (!v28 && (char)v26)
                g_5d2e0 = &(1)[g_5d2e0];
        }
    }
    v1 = a6;
    g_5d2c0(v25, a1, a2, a3, v24, a5);
    return security_cookie_check(v22 ^ &v0);
}


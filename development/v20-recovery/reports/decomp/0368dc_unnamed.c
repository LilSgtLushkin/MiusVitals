// v20 RVA 0368dc..036d37  (1115 bytes)  sub_368dc
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    struct struct_2 *field_0;
    char paddin8[712];
    unsigned int field_2d0;
} struct_3;

typedef struct struct_5 {
    char field_0;
} struct_5;

typedef struct struct_2 {
    char paddin0[40];
    struct struct_1 *field_28;
} struct_2;

typedef struct struct_0 {
    char paddin0[24];
    unsigned int field_18;
} struct_0;

typedef struct struct_1 {
    struct struct_1 *field_0;
} struct_1;


long long sub_368dc(struct struct_4 **a0)
{
    char v0;  // [bp-0x138]
    unsigned int v1;  // [bp-0x110]
    char v2;  // [bp-0x108]
    unsigned int v3;  // [bp-0x104]
    void* v4;  // [bp-0x100], Other Possible Types: unsigned long
    unsigned long v5;  // [bp-0xf8]
    char v6;  // [bp-0xf0]
    unsigned long v7;  // [bp-0xe8]
    unsigned long v8;  // [bp-0xe0]
    struct_5 *v9;  // [bp-0xd8]
    void* v10;  // [bp-0xd0]
    char v11;  // [bp-0xc8]
    char v12;  // [bp-0xb8]
    char v13;  // [bp-0xb0]
    char v14;  // [bp-0xa8]
    char v15;  // [bp-0xa4]
    char v16;  // [bp-0xa0]
    char v17;  // [bp-0x9c]
    char v18;  // [bp-0x98]
    char v19;  // [bp-0x88]
    char v20;  // [bp-0x80]
    char v21;  // [bp-0x78]
    char v22;  // [bp-0x58]
    char v23;  // [bp-0x48]
    unsigned long long v24;  // [bp-0x38]
    unsigned long long v26;  // rdi, Other Possible Types: unsigned long
    unsigned long long v27;  // rsi, Other Possible Types: unsigned long
    struct_3 *v28;  // rax, Other Possible Types: unsigned long
    struct_2 *v29;  // r14
    unsigned long long v30;  // r9, Other Possible Types: unsigned long
    unsigned long long v31;  // r8, Other Possible Types: unsigned long
    unsigned long long *v32;  // gs
    unsigned int *v33;  // rbx
    unsigned int v34;  // ymm0
    struct_0 *v35;  // rbx
    struct_0 *v36;  // rax

    v24 = g_5a380 ^ &v0;
    v26 = g_5d148;
    v27 = g_5d150;
    if (g_5d148 == g_5d150)
        return security_cookie_check(v24 ^ &v0);
    v28 = g_5d270;
    if (!g_5d270)
    {
        v4 = 0;
        v7 = "i_trencher";
        v8 = "trencher";
        v9 = &v4;
        v10 = 0;
        (unsigned int)v28 = *((long long *)(*(a0) + 72))();
        g_5d270 = v28;
        v27 = g_5d150;
        v26 = g_5d148;
    }
    if (g_5d2c0 && v28)
    {
        v29 = *((long long *)v28);
        if (v26 == v27)
            return security_cookie_check(v24 ^ &v0);
        do
        {
            memcpy(&v12, &(76)[v26], 48);
            memcpy(&v23, &v12, 16);
            memcpy(&v22, &v18, 16);
            if (sub_1c334(a0, &(76)[v30 + v31 * 220], 16) != 1)
            {
                v33 = *((long long *)(*((long long *)(88 + v32)) + g_5e590 * 8));
                if (g_5d350 > *((int *)(4 + v33)))
                {
                    sub_409cc(&g_5d350);
                    if (g_5d350 == -1)
                    {
                        *((char **)&v3) = 0;
                        0?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d310, &v3, &v2);
                        sub_4066c(__Fg_units);
                        sub_40960(&g_5d350);
                    }
                }
                if (g_5d3a0 > *((int *)(4 + v33)))
                {
                    sub_409cc(&g_5d3a0);
                    if (g_5d3a0 == -1)
                    {
                        *((char **)&v3) = 0;
                        0?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d360, &v3, &v2);
                        sub_4066c(__Fg_units);
                        sub_40960(&g_5d3a0);
                    }
                }
                v30 = g_5d1a8;
                v31 = g_5d3a8;
                if (g_5d3a8 < (5366325548715505925 * (g_5d1b0 - g_5d1a8) >> 64 >> 6) + (5366325548715505925 * (g_5d1b0 - g_5d1a8) >> 64 >> 6 >> 63))
                {
                    do
                    {
                        memcpy(&v19, &(76)[v30 + v31 * 220], 48);
                        floorf((int)((0 CONCAT *((int *)&v19)) * 0x41a00000 + 0x3f000000));
                        floorf((int)((0 CONCAT *((int *)&v20)) * 0x41a00000 + 0x3f000000));
                        v5 = *((int *)&v21) ^ (v34 ^ v34 * 0x10000000) * __Eg_my_units;
                        a0 = *((long long *)$_Try_emplace@_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject$_Try_emplace@_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d310, &v11, &v5));
                        *((char **)(a0 + 24)) = &(1)[*((int *)(a0 + 24))];
                        v31 = g_5d3a8 + 1;
                        g_5d3a8 = v31;
                        v30 = g_5d1a8;
                    } while (g_5d3a8 + 1 < (5366325548715505925 * (g_5d1b0 - g_5d1a8) >> 64 >> 6) + (5366325548715505925 * (g_5d1b0 - g_5d1a8) >> 64 >> 6 >> 63));
                }
                floorf((int)((0 CONCAT *((int *)&v12)) * 0x41a00000 + 0x3f000000));
                floorf((int)((0 CONCAT *((int *)&v13)) * 0x41a00000 + 0x3f000000));
                v4 = *((int *)&v14) ^ (v34 ^ v34 * 0x10000000) * __Eg_my_units;
                find@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d310, &v6, &v4);
                v35 = *((long long *)&v6);
                if (v35 != g_5d318 && (a0 = &g_5d360, v36 = *((long long *)(unsigned long long)$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d360, &v7, &v4)), *((char **)(v36 + 24)) = *((int *)(v36 + 24)) + 1, *((int *)(v36 + 24)) <= *((int *)(v35 + 24))))
                {
                    g_5d2e8 = &(1)[g_5d2e8];
                    continue;
                }
            }
            *((int *)(g_5d270 + 720)) = *((int *)&v17);
            *((char **)&g_5d0ed) = 1;
            v1 = *((int *)&v16);
            *((long long *)(v29 + 40))(g_5d270, &v23, *((int *)&v14), &v22, *((int *)&v15));
            *((char **)&g_5d0ed) = 0;
            g_5d2d8 = &(1)[g_5d2d8];
        } while ((v26 += 220, v26 != v27));
        v27 = g_5d150;
        v26 = g_5d148;
    }
    if (v26 == v27)
        return security_cookie_check(v24 ^ &v0);
    g_5d150 = v26;
}


// v20 RVA 0378e8..037d4d  (1125 bytes)  queue_shot
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_7 {
    uint128_t field_0;
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    unsigned long long field_50;
    unsigned int field_58;
} struct_7;

typedef struct struct_1 {
    char field_0;
} struct_1;

typedef struct struct_3 {
    char paddin0[48];
    struct struct_2 *field_30;
} struct_3;

typedef struct struct_5 {
    char paddin0[8];
    struct struct_5 *field_8;
    unsigned long long field_10;
} struct_5;

typedef struct struct_2 {
    struct struct_2 *field_0;
} struct_2;


long long queue_shot(struct struct_6 **a0)
{
    char v0;  // [bp-0x188]
    unsigned int v1;  // [bp-0x158]
    void* v2;  // [bp-0x148], Other Possible Types: unsigned long
    int v3;  // [bp-0x138]
    int v4;  // [bp-0x128]
    int v5;  // [bp-0x118]
    int v6;  // [bp-0x108]
    int v7;  // [bp-0xf8]
    int v8;  // [bp-0xe8]
    int v9;  // [bp-0xd8]
    int v10;  // [bp-0xc8]
    int v11;  // [bp-0xb8]
    int v12;  // [bp-0xa8]
    int v13;  // [bp-0x98]
    int v14;  // [bp-0x88]
    int v15;  // [bp-0x78]
    unsigned long v16;  // [bp-0x68]
    unsigned int v17;  // [bp-0x60]
    unsigned long v18;  // [bp-0x58]
    unsigned long v19;  // [bp-0x50]
    struct_1 *v20;  // [bp-0x48]
    void* v21;  // [bp-0x40]
    unsigned long long v22;  // [bp-0x38]
    unsigned long v23;  // [bp-0x8]
    char v24;  // [bp+0x0]
    struct struct_6 **v26;  // rsi
    uint128_t v27[8];  // rbx, Other Possible Types: unsigned long
    struct_7 *v28;  // rcx
    struct_7 *v29;  // rcx
    uint128_t v30[8];  // rbx, Other Possible Types: unsigned long
    unsigned long v31;  // r8
    unsigned long long v32;  // rcx
    unsigned long long v33;  // rdx
    unsigned long long v34;  // rbx, Other Possible Types: unsigned long
    unsigned long long v35;  // rdi, Other Possible Types: unsigned long
    struct struct_3 **v36;  // rax, Other Possible Types: unsigned long
    struct struct_2 **v37;  // r8
    struct_3 *v38;  // r14
    unsigned int v39;  // r15d
    unsigned long long v40;  // r13
    unsigned int v41;  // r12d
    unsigned long long v43;  // rax
    struct_5 *v44;  // rcx, Other Possible Types: unsigned long
    struct struct_2 **v45;  // rax

    v23 = &v24;
    v22 = g_5a380 ^ &v0;
    v26 = a0;
    if (g_5a2ee && g_5a2ed && g_5d220 && (char)sub_19f18() && (char)sub_1d2d4())
    {
        v27 = g_5d178;
        for (*((char **)&g_5a2ee) = 0; v27 != g_5d180; v27 = &(220)[v27])
        {
            v28 = &(128)[v27];
            *((int128_t *)&v3) = *((int128_t *)v27);
            *((int128_t *)&v4) = *((int128_t *)&(16)[v27]);
            *((int128_t *)&v5) = *((int128_t *)&(32)[v27]);
            *((int128_t *)&v6) = *((int128_t *)&(48)[v27]);
            *((int128_t *)&v7) = *((int128_t *)&(64)[v27]);
            *((int128_t *)&v8) = *((int128_t *)&(80)[v27]);
            *((int128_t *)&v9) = *((int128_t *)&(96)[v27]);
            *((int128_t *)&v10) = *((int128_t *)&(112)[v27]);
            *((uint128_t *)&v11) = v28->field_0;
            *((int128_t *)&v12) = *((int128_t *)(v28 + 16));
            *((int128_t *)&v13) = *((int128_t *)(v28 + 32));
            *((int128_t *)&v14) = *((int128_t *)(v28 + 48));
            *((int128_t *)&v15) = *((int128_t *)(v28 + 64));
            v16 = *((long long *)(v28 + 80));
            v17 = *((int *)(v28 + 88));
            shots_queue(&v3, &v11);
            g_5d2a0 = &(1)[g_5d2a0];
        }
        if (sub_1c334() == 1 && g_5a2ef)
        {
            for (v30 = g_5d1a8; v30 != g_5d1b0; v30 = &(220)[v30])
            {
                v29 = &(128)[v30];
                *((int128_t *)&v3) = *((int128_t *)v30);
                *((int128_t *)&v4) = *((int128_t *)&(16)[v30]);
                *((int128_t *)&v5) = *((int128_t *)&(32)[v30]);
                *((int128_t *)&v6) = *((int128_t *)&(48)[v30]);
                *((int128_t *)&v7) = *((int128_t *)&(64)[v30]);
                *((int128_t *)&v8) = *((int128_t *)&(80)[v30]);
                *((int128_t *)&v9) = *((int128_t *)&(96)[v30]);
                *((int128_t *)&v10) = *((int128_t *)&(112)[v30]);
                *((uint128_t *)&v11) = v29->field_0;
                *((int128_t *)&v12) = *((int128_t *)(v29 + 16));
                *((int128_t *)&v13) = *((int128_t *)(v29 + 32));
                *((int128_t *)&v14) = *((int128_t *)(v29 + 48));
                *((int128_t *)&v15) = *((int128_t *)(v29 + 64));
                v16 = *((long long *)(v29 + 80));
                v17 = *((int *)(v29 + 88));
                shots_queue(&v3, &v11);
            }
        }
        if (g_5d178 != g_5d180 || g_5d1a8 != g_5d1b0)
        {
            v31 = (sub_1c334() == 1 ? &g_45ce6 : " and of the craters made here so far");
            v32 = "trench: the other game is told of %zu trenches stamped here outside any dig%s";
            v33 = (5366325548715505925 * (g_5d180 - (char *)g_5d178) >> 64 >> 6) + (5366325548715505925 * (g_5d180 - (char *)g_5d178) >> 64 >> 6 >> 63);
            mp_log();
        }
    }
    v34 = g_5d160;
    v35 = g_5d168;
    if (g_5d160 == g_5d168)
        return security_cookie_check(v22 ^ &v0);
    v36 = g_5d270;
    if (!g_5d270)
    {
        v2 = 0;
        v18 = "i_trencher";
        v21 = 0;
        v19 = "trencher";
        v20 = &v2;
        (unsigned int)v36 = v37(v26, &v18, *((long long *)(*(v26) + 72)));
        v35 = g_5d168;
        v34 = g_5d160;
        g_5d270 = v36;
    }
    if (g_5d220 && v36 && g_5a2ed)
    {
        v38 = *((long long *)v36);
        if (v34 == v35)
            return security_cookie_check(v22 ^ &v0);
        do
        {
            memcpy(&v18, &(76)[v34], 24);
            v39 = *((int *)((char *)&v20 + 4));
            v40 = *((int *)((char *)&v19 + 4));
            v41 = (int)v20;
            v2 = sub_37844((unsigned int)v18, *((int *)((char *)&v18 + 4)), (unsigned int)v19, v40, v41);
            v43 = (sub_61cc(&v2) & g_5d120) * 2;
            if (v44 == g_5d0f8)
            {
                v44 = 0;
            }
            else
            {
                for (v44 = *((long long *)&(8)[v43 * 8 + g_5d108]); v2 != *((long long *)&(16)[v44]); v44 = *((long long *)&(8)[v44]))
                {
                    if (v44 == *((long long *)(g_5d108 + (v43 << 3))))
                        break;
                }
            }
            if (v44)
            {
                g_5d2b0 = &(1)[g_5d2b0];
            }
            else
            {
                v45 = *((long long *)(v38 + 48));
                v1 = v39;
                *((char **)&g_5d0ec) = 1;
                v45(g_5d270, (unsigned int)v18, *((int *)((char *)&v18 + 4)), (unsigned int)v19, v40, v41);
                g_5d2a8 = &(1)[g_5d2a8];
                *((char **)&g_5d0ec) = 0;
                if (g_5d0eb)
                {
                    v1 = *((int *)((char *)&v20 + 4));
                    mp_log("trench: stamped from the other game's list: tile %u cell %u kind %u direction %u variant %d flags %x", (unsigned int)v18, *((int *)((char *)&v18 + 4)), (unsigned int)v19, *((int *)((char *)&v19 + 4)), (int)v20);
                }
            }
            v34 = &(220)[v34];
        } while (v34 != v35);
        v35 = g_5d168;
        v34 = g_5d160;
    }
    if (v34 == v35)
        return security_cookie_check(v22 ^ &v0);
    g_5d168 = v34;
}


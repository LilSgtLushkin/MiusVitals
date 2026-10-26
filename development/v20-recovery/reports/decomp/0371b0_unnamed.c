// v20 RVA 0371b0..0377d5  (1573 bytes)  dig_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[24];
    unsigned long long field_18;
} struct_1;


long long dig_hook(unsigned long long a0, unsigned long long a1, unsigned long long a2, unsigned long long a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7, unsigned long long a8, char a9)
{
    char v0;  // [bp-0x1f8]
    int tmp_4;  // tmp #4
    char v1;  // [bp-0x1d8]
    unsigned long v2;  // [bp-0x1b0], Other Possible Types: char
    unsigned long v3;  // [bp-0x1a8]
    char v4;  // [bp-0x198]
    unsigned long long v5;  // [bp-0x190]
    void* v6;  // [bp-0x188], Other Possible Types: unsigned long
    unsigned long v7;  // [bp-0x178]
    unsigned long v8;  // [bp-0x170], Other Possible Types: unsigned long long
    unsigned long long v9;  // [bp-0x168]
    char v10;  // [bp-0x158]
    char v11;  // [bp-0x154]
    char v12;  // [bp-0x10c]
    unsigned int v13;  // [bp-0x78]
    unsigned int v14;  // [bp-0x74]
    unsigned int v15;  // [bp-0x70]
    unsigned int v16;  // [bp-0x6c]
    unsigned int v17;  // [bp-0x68]
    unsigned int v18;  // [bp-0x64]
    char v19;  // [bp-0x5e]
    unsigned long long v20;  // [bp-0x58]
    unsigned long v21;  // [bp-0x8]
    char v22;  // [bp+0x0]
    unsigned long long v25;  // rsi
    unsigned int v26;  // r15d
    unsigned int v27;  // r12d
    struct_1 *v28;  // rdx
    unsigned int v29;  // ecx
    unsigned long long v30;  // rdi, Other Possible Types: unsigned long
    unsigned long long v31;  // rbx, Other Possible Types: unsigned long
    unsigned long long v32;  // rcx, Other Possible Types: unsigned long
    unsigned long long v33;  // rbx, Other Possible Types: unsigned long
    char v34;  // al
    unsigned int v35;  // r13d, Other Possible Types: unsigned long, unsigned long long
    char v36;  // al
    unsigned long v37;  // rdi, Other Possible Types: unsigned long long
    unsigned long long v38;  // rbx
    unsigned long long v39;  // r8
    unsigned long long v40;  // rdx
    unsigned long long v41;  // r9
    unsigned long long v42;  // rcx
    unsigned long long v43;  // r9, Other Possible Types: unsigned long
    unsigned long long v44;  // r8, Other Possible Types: unsigned long

    v21 = &v22;
    v20 = g_5a380 ^ &v0;
    v25 = a2;
    v9 = a1;
    v8 = a0;
    v5 = a8;
    if (!g_5d0ea)
    {
        v26 = a7;
        v35 = a5;
        v37 = a4;
        v27 = a6;
        v7 = sub_37844(a2, a3, v37, v35, v27);
        if (g_5d0ec)
        {
            $emplace@_K@?$_Hash@V?$_Uset_traits@_KV?$_Uhash_compare@_KU?$hash@_K@std(&g_5d0f0, &v6, &v7);
            v2 = a9;
            *((unsigned long long *)&(24)[*((long long *)$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d1c0, &v6, &v7))]) = g_5d220(v8, v9, a2, a3, v37, v35, v27, v26, v5);
            return security_cookie_check(v20 ^ &v0);
        }
        if (!g_5d0e8 && !v5)
        {
            find@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d1c0, &v6, &v7);
            v28 = v6;
            if (v28 != g_5d1c8)
            {
                v6 = *((long long *)(v28 + 24));
                ammo_check(&g_5d1c0);
                g_5d2b8 = &(1)[g_5d2b8];
                memset(&v10, 0, 220);
                v13 = a2;
                v14 = a3;
                v15 = v37;
                v16 = v35;
                v17 = v27;
                v18 = v26;
                *((char **)&v11) = 6;
                memcpy(&v12, &v13, 24);
                v30 = g_5d180;
                *((char **)&v4) = 0;
                v31 = g_5d178;
                if (g_5d178 == g_5d180)
                {
LABEL_37487:
                    if ((5366325548715505925 * (v30 - v31) >> 64 >> 6) + (5366325548715505925 * (v30 - v31) >> 64 >> 6 >> 63) < 0x2000)
                        sub_3277c(&g_5d178, &v10);
                }
                else
                {
                    *((char *)&v35) = v4;
                    do
                    {
                        v34 = memcmp(&(76)[v31], &v13, 24) == 0;
                        v31 = &(220)[v31];
                        *((char *)&v35) = (char)v35 | v34;
                    } while (v31 != g_5d180);
                    tmp_4 = (char)v35;
                    v35 = a5;
                    if (!(char)tmp_4)
                    {
                        v30 = g_5d180;
                        v31 = g_5d178;
                        goto LABEL_37487;
                    }
                }
                if (!g_5d0eb)
                    return security_cookie_check(v20 ^ &v0);
                mp_log("trench: this game's own stamp left out, made on the other game's word already: tile %u cell %u kind %u direction %u variant %d flags %x", a2, a3, a4, (unsigned int)v35, v27);
            }
        }
        $emplace@_K@?$_Hash@V?$_Uset_traits@_KV?$_Uhash_compare@_KU?$hash@_K@std(&g_5d0f0, &v6, &v7);
        if (!g_5d0e8)
        {
            memset(&v10, 0, 220);
            v13 = a2;
            v14 = a3;
            v15 = v37;
            v16 = v35;
            v17 = v27;
            v18 = v26;
            *((char **)&v11) = 6;
            memcpy(&v12, &v13, 24);
            v32 = g_5d180;
            *((char **)&v4) = 0;
            v33 = g_5d178;
            if (g_5d178 == g_5d180)
            {
LABEL_3763e:
                if (!v5 && (5366325548715505925 * (v32 - v33) >> 64 >> 6) + (5366325548715505925 * (v32 - v33) >> 64 >> 6 >> 63) < 0x2000)
                {
                    sub_3277c(&g_5d178, &v10);
                    if ((char)sub_19f18(&g_5d178, 0) && (char)sub_1d2d4() && g_5a2ed)
                    {
                        shots_queue(&v10);
                        g_5d2a0 = &(1)[g_5d2a0];
                    }
                }
            }
            else
            {
                *((char *)&v35) = v4;
                do
                {
                    v36 = memcmp(&(76)[v33], &v13, 24) == 0;
                    v33 = &(220)[v33];
                    *((char *)&v35) = (char)v35 | v36;
                } while (v33 != g_5d180);
                v37 = a4;
                if (!(char)v35)
                {
                    v32 = g_5d180;
                    v33 = g_5d178;
                    goto LABEL_3763e;
                }
            }
            g_5d268 = &(1)[g_5d268];
            if (g_5d0eb && g_5d268 <= 400)
            {
                v6 = 0;
                RtlCaptureStackBackTrace(1, 2, &v6, 0);
                v38 = GetModuleHandleA("adv_game.codelib");
                v3 = (&v6)[1] - v38;
                v2 = v6 - v38;
                v39 = a3;
                v40 = v25;
                v41 = v37;
                v42 = "trench: stamped outside a dig: tile %u cell %u kind %u direction %u variant %d flags %x squad %s (%s), asked for by adv_game+0x%llx <- +0x%llx";
                mp_log();
            }
        }
        else
        {
            v29 = g_5d244;
            *((char **)&g_5d244) = &(1)[g_5d244];
            if (!v29)
            {
                g_5d228 = a2;
                g_5d22c = a3;
                g_5d240 = v5 != 0;
                g_5d241 = a9;
                g_5d242 = *((short *)&v19);
                g_5d230 = v37;
                g_5d234 = v35;
                g_5d238 = v27;
                g_5d23c = v26;
            }
        }
        v43 = a3;
        a1 = v9;
        v44 = v25;
        a0 = v8;
        v2 = a9;
    }
    else
    {
        if (!(g_5d248) || !(!g_5d0e9))
        {
            g_5d260 = &(1)[g_5d260];
            return security_cookie_check(v20 ^ &v0);
        }
        g_5d250 = &(1)[g_5d250];
        *((char **)&g_5d0e9) = 1;
        v8 = sub_37844(g_5d248->field_0, *((int *)(g_5d248 + 4)), *((int *)(g_5d248 + 8)), *((int *)(g_5d248 + 12)), *((long long *)&v1));
        $emplace@_K@?$_Hash@V?$_Uset_traits@_KV?$_Uhash_compare@_KU?$hash@_K@std(&g_5d0f0, &v6, &v8);
        v43 = *((int *)(g_5d248 + 4));
        v2 = *((char *)(g_5d248 + 25));
        v44 = g_5d248->field_0;
    }
    g_5d220(a0, a1, v44, v43, *((long long *)&a4), *((long long *)&a5), *((long long *)&a6), *((long long *)&a7), a8);
    return security_cookie_check(v20 ^ &v0);
}


// v20 RVA 036d38..0371b0  (1144 bytes)  dig_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char paddin0[720];
    unsigned int field_2d0;
} struct_2;

typedef struct struct_3 {
    char paddin0[64];
    unsigned long long field_40;
    char paddin72[736];
    unsigned int field_328;
    char paddin812[8];
    unsigned int field_334;
    unsigned int field_338;
    unsigned int field_33c;
    unsigned int field_340;
    char paddin836[124];
    unsigned int field_3c0;
    char paddin964[31];
    char field_3e3;
    char field_3e4;
    char paddin997[10];
    char field_3ef;
} struct_3;

typedef struct struct_0 {
    char field_0;
} struct_0;


long long dig_hook(struct_2 *a0, unsigned long long a1, unsigned long long a2)
{
    char v0;  // [bp-0x2b8]
    unsigned int v1;  // [bp-0x278]
    unsigned int v2;  // [bp-0x270]
    unsigned long v3;  // [bp-0x268]
    struct_0 *v4;  // [bp-0x260]
    unsigned long v5;  // [bp-0x258]
    unsigned int v6;  // [bp-0x248]
    unsigned int v7;  // [bp-0x244]
    unsigned long v8;  // [bp-0x240]
    unsigned int v9;  // [bp-0x238]
    unsigned int v10;  // [bp-0x234]
    char v11;  // [bp-0x230]
    unsigned int v12;  // [bp-0x220]
    unsigned int v13;  // [bp-0x21c]
    unsigned int v14;  // [bp-0x218]
    char v15;  // [bp-0x214]
    char v16;  // [bp-0x213]
    char v17;  // [bp-0x212]
    char v18;  // [bp-0x211]
    char v19;  // [bp-0x210]
    char v20;  // [bp-0x208]
    char v21;  // [bp-0x1fc]
    char v22;  // [bp-0x1f0]
    unsigned int v23;  // [bp-0x1d8]
    unsigned int v24;  // [bp-0x1d4]
    unsigned int v25;  // [bp-0x1d0]
    unsigned int v26;  // [bp-0x1cc]
    char v27;  // [bp-0x1c8]
    char v28;  // [bp-0x1c7]
    char v29;  // [bp-0x1c6]
    unsigned int v30;  // [bp-0x1c4]
    unsigned int v31;  // [bp-0x1c0]
    unsigned int v32;  // [bp-0x1bc]
    unsigned int v33;  // [bp-0x1b8]
    unsigned int v34;  // [bp-0x1b4]
    unsigned int v35;  // [bp-0x1b0]
    char v36;  // [bp-0x1a8]
    char v37;  // [bp-0x1a4]
    char v38;  // [bp-0x15c]
    char v39;  // [bp-0xc8]
    char v40;  // [bp-0xb8]
    unsigned long long v41;  // [bp-0x48]
    unsigned long v42;  // [bp-0x8]
    char v43;  // [bp+0x0]
    unsigned long long v45;  // r13, Other Possible Types: unsigned long
    char v46;  // r15b
    struct_3 *v47;  // rsi
    unsigned long long v48;  // r12
    unsigned long long v49;  // rsi, Other Possible Types: unsigned long
    unsigned long long v50;  // r12, Other Possible Types: unsigned long
    char v51;  // bl
    unsigned long long v52;  // rdi, Other Possible Types: unsigned long
    unsigned long long v53;  // rsi, Other Possible Types: unsigned long
    unsigned int v54;  // r12d
    unsigned int v55;  // r13d
    unsigned int v56;  // eax
    void* v57;  // rdx
    struct_0 *v58;  // rcx
    unsigned long v59;  // rcx
    unsigned long long v61;  // rcx
    unsigned long long v62;  // rdx
    unsigned long long v63;  // r8
    unsigned long long v64;  // r9

    v42 = &v43;
    v41 = g_5a380 ^ &v0;
    g_5d270 = a0;
    v45 = 0;
    if (!(char)sub_19f18() || !g_5a2ed || (v46 = 1, !(char)sub_1d2d4()))
        v46 = 0;
    if (!g_5d0ea && sub_1c334() && GetCurrentThreadId() == g_5d278)
    {
        v47 = squad_record(a0, a1);
        if (v46 && !(char)sub_1bb14(a1))
        {
            g_5d288 = &(1)[g_5d288];
            if (!g_5d0eb)
                return security_cookie_check(v41 ^ &v0);
            mp_log("trench: dig for squad %08x of the other player dropped", a1);
        }
        memset(&v36, 0, 220);
        memset(&v11, 0, 132);
        v9 = a1;
        v10 = a2 != 0;
        if (a2)
            memcpy(&v11, a2, 16);
        v14 = *((int *)(a0 + 720));
        if (v47)
        {
            v12 = *((int *)(v47 + 832));
            v13 = *((int *)(v47 + 808));
            v15 = *((char *)(v47 + 995));
            v16 = *((char *)(v47 + 996));
            v17 = *((char *)(v47 + 1007));
            v23 = *((int *)(v47 + 820));
            v24 = *((int *)(v47 + 824));
            v25 = *((int *)(v47 + 828));
            v26 = *((int *)(v47 + 960));
            v48 = *((long long *)(v47 + 64));
            if ((char)readable(v48, 704) && (char)sub_1e4e4(v48, &v19))
            {
                *((char **)&v18) = 1;
                memcpy(&v20, &(432)[v48], 12);
                v50 = &(240)[v48];
                v49 = &v21;
                v45 = 3;
                do
                {
                    memcpy(v49, v50, 12);
                    v50 = &(16)[v50];
                    v49 = &v22;
                    v45 -= 1;
                } while (true);
            }
        }
        *((char **)&g_5d0e8) = 1;
        g_5d244 = v45;
        v8 = g_5d218(a0, a1, a2);
        g_5d0e8 = v45;
        if (g_5d244 == (unsigned int)v45)
        {
            v55 = v33;
            v54 = v32;
            v53 = v31;
            v52 = v30;
            v51 = v27;
            v7 = v35;
            v56 = v34;
        }
        else
        {
            v51 = 1;
            v52 = g_5d228;
            v53 = g_5d22c;
            v54 = g_5d230;
            v55 = g_5d234;
            v56 = g_5d23c;
            v28 = g_5d240;
            v29 = g_5d241;
            v7 = g_5d238;
            v35 = g_5d238;
            *((char **)&v27) = 1;
            v30 = g_5d228;
            v31 = g_5d22c;
            v32 = g_5d230;
            v33 = g_5d234;
            v34 = g_5d23c;
        }
        g_5d280 = &(1)[g_5d280];
        v6 = v56;
        if (g_5d0eb)
        {
            v57 = 0;
            v58 = &v40;
            strncpy(&v39, "stamped nothing", 15);
            memset(&v40, 0, 104);
            if (v51)
            {
                v1 = g_5d244;
                $sprintf_s@$0BAE$sprintf_s@$0BAE(&v39, "stamped tile %u cell %u kind %u direction %u variant %d flags %x (%d calls)", v52, v53, v54, v55, v7, v6);
            }
            v59 = (v46 ? &g_45ce6 : ", sent");
            v5 = v59;
            v4 = &v39;
            v61 = "trench: squad %08x dug in (kind %u, settings %x %u %u %u, tile %u cell %u direction %u, %s): %s%s";
            *((int *)&v3) = (a2 != 0 ? "where it stands" : "at a given place");
            v2 = v23;
            v1 = v24;
            v62 = v9;
            v63 = v13;
            v64 = v12;
            mp_log();
        }
        *((char **)&v37) = 2;
        memcpy(&v38, &v9, 140);
        if (v10 && (5366325548715505925 * (g_5d198 - g_5d190) >> 64 >> 6) + (5366325548715505925 * (g_5d198 - g_5d190) >> 64 >> 6 >> 63) < __Eg_my_units)
            sub_3277c(&g_5d190, &v36);
        if ((5366325548715505925 * (g_5d208 - g_5d200) >> 64 >> 6) + (5366325548715505925 * (g_5d208 - g_5d200) >> 64 >> 6 >> 63) < __Eg_my_units)
            sub_3277c(&g_5d200, &v36);
        if (!v46)
            return security_cookie_check(v41 ^ &v0);
        shots_queue(&v36);
    }
    g_5d218(a0, a1, a2);
    return security_cookie_check(v41 ^ &v0);
}


// v20 RVA 02c8ac..02ce78  (1484 bytes)  ui_render
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_4 {
    struct struct_4 *field_0;
    char paddin8[40];
    struct struct_5 *field_30;
} struct_4;

typedef struct struct_7 {
    char paddin0[8];
    unsigned int field_8;
} struct_7;

typedef struct struct_5 {
    struct struct_3 *field_0;
} struct_5;

typedef struct struct_3 {
    char paddin0[16];
    struct struct_0 *field_10;
} struct_3;

typedef struct struct_0 {
    struct struct_0 *field_0;
} struct_0;


long long sub_2c8ac(unsigned long long a0)
{
    char v0;  // [bp-0xd8]
    int tmp_45;  // tmp #45
    int tmp_46;  // tmp #46
    int tmp_43;  // tmp #43
    unsigned int v1;  // [bp-0xb8]
    unsigned int v2;  // [bp-0xb0]
    char v3;  // [bp-0xa8]
    struct struct_8 **v4;  // [bp-0x98]
    char v5;  // [bp-0x90]
    char v6;  // [bp-0x88]
    char v7;  // [bp-0x84]
    char v8;  // [bp-0x78]
    int v9;  // [bp-0x77]
    int v10;  // [bp-0x67]
    int v11;  // [bp-0x57]
    void* v12;  // [bp-0x47]
    unsigned int v13;  // [bp-0x3f]
    unsigned short v14;  // [bp-0x3b]
    char v15;  // [bp-0x39]
    unsigned long long v16;  // [bp-0x38]
    unsigned long v17;  // [bp-0x8]
    char v18;  // [bp+0x0]
    char *v20;  // rdi
    char v21;  // r14b
    char v22;  // sil
    struct struct_1 **v23;  // rax
    struct struct_1 **v24;  // rbx
    struct_4 *v25;  // rbx, Other Possible Types: unsigned long
    struct struct_0 **v26;  // r8
    struct struct_3 **v27;  // rcx
    unsigned int v28;  // eax
    unsigned int v29;  // ecx
    unsigned int v30;  // ymm1
    struct_7 *v33;  // rax
    unsigned long long v34;  // rcx

    v17 = &v18;
    v16 = g_5a380 ^ &v0;
    v20 = session_status();
    if (!g_5c148 && !g_5c4b0 && g_5c14b)
    {
        v21 = 1;
    }
    else
    {
        v21 = 0;
        if (!(!g_5c148) || !(!g_5c4b0) || !(g_5c468) || !(!((v22 = 1, !g_5c46e))))
        {
            v22 = 0;
            goto LABEL_2c92b;
        }
    }
LABEL_2c92b:
    *((char **)&v8) = 0;
    v12 = 0;
    *((char **)&v13) = 0;
    *((char **)&v9) = 0;
    *((char **)&v14) = 0;
    *((char **)&v10) = 0;
    *((char **)&v15) = 0;
    *((char **)&v11) = 0;
    sub_2aa74(&v8, 64);
    if (!g_5c4b0 && !*(v20) && !v21 && !v8 && !g_5c14c && !v22)
    {
        if (!g_5c480)
        {
            return security_cookie_check(v16 ^ &v0);
        }
        else if (g_5c68c != g_5c68c / 600 * 600)
        {
            return security_cookie_check(v16 ^ &v0);
        }
        else
        {
            for (v25 = g_5c478->field_0; v25 != g_5c478; v25 = *((long long *)v25))
            {
                v27 = *((long long *)&(48)[v25]);
                if (v27)
                    *((long long *)(*(v27) + 16))(v27);
            }
            clear@?$_Hash@V?$_Umap_traits@_KV?$basic_string@DU?$char_traits@D@std@?$_Hash@V?$_Umap_traits@_KV?$basic_string@DU?$char_traits@D@std(&g_5c470);
            return security_cookie_check(v16 ^ &v0);
        }
    }
    (unsigned int)v23 = find_device(a0);
    v24 = v23;
    if (!v23)
    {
        return security_cookie_check(v16 ^ &v0);
    }
    else if (v26(v23, &v5, *((long long *)(*(v23) + 384))) < 0)
    {
        return security_cookie_check(v16 ^ &v0);
    }
    else
    {
        v28 = *((int *)&v6);
        if (!v28)
            return security_cookie_check(v16 ^ &v0);
        v29 = *((int *)&v7);
        if (!v30)
            return security_cookie_check(v16 ^ &v0);
        v4 = 0;
        g_5cbd0 = v24;
        g_5caac = v29;
        tmp_45 = DivV(tmp_46, tmp_43);
        g_5caa8 = v28;
        *((int128_t *)&g_5cbd8) = tmp_45;
        if (*((long long *)(*(v24) + 472))(v24, 1, &v4) < 0)
        {
            return security_cookie_check(v16 ^ &v0);
        }
        else if (v4)
        {
            *((long long *)(*(v24) + 736))(v24, 0, *((long long *)(*(v24) + 736)));
            *((long long *)(*(v24) + 856))(v24, 0, *((long long *)(*(v24) + 856)));
            *((long long *)(*(v24) + 712))(v24, 324, *((long long *)(*(v24) + 712)));
            *((long long *)(*(v24) + 456))(v24, 7, 0);
            *((long long *)(*(v24) + 456))(v24, 14, 0);
            *((long long *)(*(v24) + 456))(v24, 27, 1);
            *((long long *)(*(v24) + 456))(v24, 19, 5);
            *((long long *)(*(v24) + 456))(v24, 20, 6);
            *((long long *)(*(v24) + 456))(v24, 206, 0);
            *((long long *)(*(v24) + 456))(v24, 15, 0);
            *((long long *)(*(v24) + 456))(v24, 22, 1);
            *((long long *)(*(v24) + 456))(v24, 137, 0);
            *((long long *)(*(v24) + 456))(v24, 28, 0);
            *((long long *)(*(v24) + 456))(v24, 52, 0);
            *((long long *)(*(v24) + 456))(v24, 174, 0);
            *((long long *)(*(v24) + 456))(v24, 194, 0);
            *((long long *)(*(v24) + 456))(v24, 168, 15);
            *((long long *)(*(v24) + 456))(v24, 8, 3);
            *((long long *)(*(v24) + 536))(v24, 0, 1, 2);
            *((long long *)(*(v24) + 536))(v24, 0, 2, 0);
            *((long long *)(*(v24) + 536))(v24, 1, 1, 1);
            *((long long *)(*(v24) + 536))(v24, 1, 4, 1);
            *((long long *)(*(v24) + 552))(v24, 0, 6, 2);
            *((long long *)(*(v24) + 552))(v24, 0, 5, 2);
            *((long long *)(*(v24) + 552))(v24, 0, 1, 3);
            *((long long *)(*(v24) + 552))(v24, 0, 2, 3);
            *((long long *)(*(v24) + 552))(v24, 0, 11, 0);
            if (g_5c4b0)
            {
                sub_26470();
            }
            else
            {
                if (v21)
                    sub_26094();
                if (v22)
                    shots_install();
                if (*(v20))
                {
                    (unsigned int)v33 = text_tex(v24, v20, (0 CONCAT g_5cbd8) * 0x41880000 + 0x3f000000, 0);
                    v34 = (v33 == 0 ? *((int *)(v33 + 8)) : 0);
                    v1 = 3222803472;
                    quad(v34);
                    *((char **)&v3) = 0;
                    *((char **)&v2) = 1;
                    v1 = (-(0 < sub_19f18()) & -4847144) - 858424;
                    text(v20);
                }
                log_launch();
            }
            if (v8)
                save_frame(v24, &v8);
            if (!(char)(*((long long *)(*(v24) + 328))(v24) >> 31))
                *((long long *)(*(v24) + 336))(v24);
            *((long long *)(*(v4) + 40))(v4);
            *((long long *)(*(v4) + 16))(v4);
            *((char **)&g_5c46c) = 0;
            return security_cookie_check(v16 ^ &v0);
        }
        else
        {
            return security_cookie_check(v16 ^ &v0);
        }
    }
}


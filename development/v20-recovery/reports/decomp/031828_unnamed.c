// v20 RVA 031828..031b2f  (775 bytes)  sub_31828
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[24];
    struct struct_0 *field_18;
} struct_1;

typedef struct struct_4 {
    struct struct_4 *field_0;
    char paddin8[16];
    unsigned long long field_18;
    unsigned long long field_20;
    char paddin40[80];
    char field_78;
} struct_4;

typedef struct struct_2 {
    char paddin0[240];
    struct struct_3 *field_f0;
    char paddin248[4];
    unsigned int field_fc;
} struct_2;

typedef struct struct_0 {
    char paddin0[72];
    unsigned long long field_48;
} struct_0;

typedef struct struct_3 {
    unsigned long long field_0;
} struct_3;


long long sub_31828(struct_2 *a0)
{
    char v0;  // [bp-0x68]
    char v1;  // [bp-0x48]
    struct_1 *v2;  // [bp-0x40], Other Possible Types: unsigned long
    char v3;  // [bp-0x38]
    void* v4;  // [bp-0x37]
    unsigned int v5;  // [bp-0x2f]
    unsigned short v6;  // [bp-0x2b]
    char v7;  // [bp-0x29]
    unsigned long long v8;  // [bp-0x28]
    unsigned long v9;  // [bp-0x8]
    char v10;  // [bp+0x0]
    unsigned long long *v12;  // gs
    struct struct_4 **v13;  // rdi, Other Possible Types: unsigned long
    struct_4 *v14;  // rax, Other Possible Types: unsigned long
    unsigned long long v15;  // rbx
    void* v16;  // r14
    struct_1 *v17;  // rax
    unsigned long long *v18;  // rsi, Other Possible Types: unsigned long
    unsigned long v19;  // rdi, Other Possible Types: unsigned long long
    struct_4 *v20;  // rbx, Other Possible Types: unsigned long
    struct_1 *v21;  // rax
    struct_0 *v22;  // rsi
    unsigned int v23;  // r8d
    unsigned int v24;  // eax
    unsigned long v25;  // rdx
    unsigned long long v26;  // rcx
    unsigned long v27;  // rdx
    char v28;  // r15b
    unsigned long v29;  // rdx
    unsigned long v30;  // r8
    char v31;  // r9b
    unsigned long long v33;  // rax, Other Possible Types: unsigned long

    v9 = &v10;
    v8 = g_5a380 ^ &v0;
    if (g_5cf90 > *((int *)&(4)[*((long long *)(*((long long *)(88 + v12)) + (g_5e590 << 3)))]))
    {
        sub_409cc(&g_5cf90);
        if (g_5cf90 == -1)
        {
            v2 = 0;
            0?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5cf50, &v2, &v1);
            sub_4066c(__Fg_units);
            sub_40960(&g_5cf90);
        }
    }
    v13 = g_5ce18;
    v14 = g_5ce18->field_0;
    while (true)
    {
        if (v14 == g_5ce18)
            return security_cookie_check(v8 ^ &v0);
        if ((120)[v14])
            break;
        v14 = *((long long *)v14);
    }
    v15 = *((int *)(a0 + 252));
    v18 = *((long long *)(a0 + 240));
    if ((unsigned int)v15 != g_5cf94)
    {
        clear@?$_Hash@V?$_Umap_traits@_KPEAUObject@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5cf50);
        if ((unsigned int)v15)
        {
            v19 = v15;
            do
            {
                v16 = *((long long *)v18);
                if ((char)state_is_unit(v16) || (char)replicated_singleton(v16))
                {
                    (unsigned int)v17 = statesync_key(*((long long *)v18));
                    v2 = v17;
                    *((long long *)&(24)[*((long long *)$_Try_emplace@_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject$_Try_emplace@_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5cf50, &v3, &v2))]) = *((long long *)v18);
                }
            } while ((v18 += 8, v19 -= 1, v15 != 1));
        }
        g_5cf94 = v15;
        v13 = g_5ce18;
    }
    v20 = *((long long *)v13);
    while (v20 != v13)
    {
        if (!((120)[v20]) || !((*((char **)&(120)[v20]) = 0, find@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5cf50, &v2, v20 + 16), v21 = v2, v21 != g_5cf58)))
        {
LABEL_31aaa:
            v20 = *((long long *)v20);
        }
        else
        {
            v22 = *((long long *)(v21 + 24));
            if (g_5a2e8 < 0)
            {
                *((char **)&v3) = 0;
                v4 = 0;
                *((char **)&v5) = 0;
                *((char **)&v6) = 0;
                *((char **)&v7) = 0;
                g_5a2e8 = GetEnvironmentVariableA("MP_LOG_LOADS", &v3, 16) != 0;
                v23 = 16;
                if (GetEnvironmentVariableA("MP_LOAD_ONLY", &v3, 16))
                {
                    v24 = (unsigned int)&(1)[v3 != 116];
                    g_5cf98 = v24;
                    goto LABEL_319cf;
                }
            }
            v24 = g_5cf98;
LABEL_319cf:
            if (!v24 || (v25 = (v24 == 1 ? "i_human" : "i_techn"), v26 = *((long long *)(v22 + 72)), !strcmp()))
            {
                if (!(char)state_is_unit(v22) || (v27 = *((long long *)&(24)[v20]), (char)local_tables(v22, v27, *((long long *)&(32)[v20]) - v27)))
                    v28 = 1;
                else
                    v28 = 0;
                if ((char)session_state_wanted(v22))
                {
                    if (v28 || (char)replicated_singleton(v22))
                    {
                        if (g_5a2e8)
                            mp_log("load %s (%zu bytes)", v22 + 8, *((long long *)&(32)[v20]) - *((long long *)&(24)[v20]));
                        v29 = *((long long *)&(24)[v20]);
                        v30 = *((long long *)&(32)[v20]) - v29;
                        if (!(char)state_is_unit(v22))
                        {
                            (unsigned int)v33 = state_load(v22, v29, v30);
                        }
                        else
                        {
                            v31 = g_5ced0;
                            (unsigned int)v33 = state_load_keep_motion(v22, v29, v30, r9<8>);
                        }
                        g_5ceb0 = &(1)[g_5ceb0];
                        if ((unsigned int)v33)
                        {
                            g_5ceb8 = &(1)[g_5ceb8];
                            goto LABEL_31aaa;
                        }
                    }
                    else
                    {
                        g_5cec8 = &(1)[g_5cec8];
                        goto LABEL_31aaa;
                    }
                }
            }
        }
    }
    return security_cookie_check(v8 ^ &v0);
}


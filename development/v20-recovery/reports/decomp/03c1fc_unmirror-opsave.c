// v20 RVA 03c1fc..03c3ce  (466 bytes)  unmirror@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    char paddin0[240];
    struct struct_4 *field_f0;
    char paddin248[4];
    unsigned int field_fc;
} struct_3;

typedef struct struct_0 {
    char paddin0[72];
    unsigned long long field_48;
    char paddin80[7440];
    unsigned int field_1d60;
} struct_0;

typedef struct struct_4 {
    struct struct_0 *field_0;
} struct_4;


int unmirror@opsave()
{
    int tmp_5;  // tmp #5
    struct_0 *v0;  // [bp-0x58]
    unsigned int v1;  // [bp-0x50]
    unsigned int v2;  // [bp-0x4c]
    char v3;  // [bp-0x48]
    char v4;  // [bp-0x38]
    unsigned long v5;  // [bp-0x8]
    char v6;  // [bp+0x0]
    char v7;  // [bp+0x8], Other Possible Types: unsigned int
    char v8;  // [bp+0x10]
    struct_3 *v9;  // rcx
    unsigned long long v10;  // rbx
    unsigned long long v11;  // rax
    struct_0 *v12;  // rbx
    char *v13;  // rdi
    unsigned long v14;  // rcx
    struct struct_0 **v15;  // rsi, Other Possible Types: unsigned long
    unsigned long v16;  // r15, Other Possible Types: unsigned long long
    unsigned long long v17;  // rbp

    v5 = &v6;
    v10 = *((int *)(v9 + 252));
    v15 = *((long long *)(v9 + 240));
    (unsigned int)v11 = GetTickCount();
    if ((unsigned int)v10 == g_5d8a8 && (unsigned int)v11 - g_5dac0 < 1000)
    {
        v17 = v5;
        return;
    }
    g_5dac0 = v11;
    g_5d8a8 = v10;
    if (g_5d890 != g_5d898)
        g_5d898 = g_5d890;
    clear@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5d850);
    clear@?$_Hash@V?$_Umap_traits@_KPEAUObject@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5d8b0);
    memset(g_5d7b0, 0, g_5d7b8 - (char *)g_5d7b0);
    if (!(unsigned int)v10)
    {
        v17 = v5;
        return;
    }
    v16 = v10;
    do
    {
        v12 = *((long long *)v15);
        if (!strcmp(*((long long *)(v12 + 72)), "i_human") && (char)sub_1e4e4(v12, &v8))
        {
            v13 = *((int *)(v12 + &g_1d60));
            if ((unsigned int)v13 < 100000)
            {
                if (!(char)sub_1bacc(v12))
                {
                    v14 = g_5d7b0;
                    if (g_5d7b8 - (char *)g_5d7b0 <= v13)
                    {
                        *((char **)&v7) = 0;
                        $_Resize@U_Value_init_tag@std$_Resize@U_Value_init_tag@std(&g_5d7b0, v13 + 256, &v7);
                        v14 = g_5d7b0;
                    }
                    *((char **)&v13[v14]) = 1;
                    v7 = *((int *)&v8);
                    *((unsigned int *)&(20)[*((long long *)$_Try_emplace@I$$V@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std$_Try_emplace@I$$V@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5d850, &v3, &v7))]) = (unsigned int)v13;
                    v7 = *((int *)&v8);
                    *((struct_0 **)&(24)[*((long long *)$_Try_emplace@I$$V@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std$_Try_emplace@I$$V@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5d8b0, &v4, &v7))]) = v12;
                }
                else
                {
                    v0 = v12;
                    v1 = *((long long *)&v8);
                    v2 = (unsigned int)v13;
                    if (g_5d898 != g_5d8a0)
                    {
                        g_5d898->field_0 = v12;
                        *((unsigned int *)(g_5d898 + 8)) = v1;
                        *((unsigned int *)(g_5d898 + 12)) = (unsigned int)v13;
                        g_5d898 = g_5d898 + 16;
                    }
                    else
                    {
                        $_Emplace_reallocate@U?$pair@HH@std$_Emplace_reallocate@U?$pair@HH@std(&g_5d890, g_5d898, &v0);
                    }
                }
            }
        }
    } while ((v15 += 8, tmp_5 = v16, v16 -= 1, tmp_5 != 1));
    v17 = v5;
    return;
}


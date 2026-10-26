// v20 RVA 03b74c..03b875  (297 bytes)  sub_3b74c
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_4 {
    char paddin0[20];
    unsigned int field_14;
} struct_4;

typedef struct struct_3 {
    char paddin0[24];
    struct struct_0 *field_18;
} struct_3;

typedef struct struct_0 {
    char paddin0[7520];
    unsigned int field_1d60;
} struct_0;


int sub_3b74c()
{
    unsigned long long v1;  // [bp+0x10]
    unsigned long long v2;  // [bp+0x18]
    unsigned long long v3;  // rcx
    struct struct_1 **v4;  // rbx, Other Possible Types: unsigned long
    struct_4 *v5;  // rsi
    unsigned long long v6;  // rcx
    unsigned long long v7;  // rdi
    struct_3 *v8;  // rdi
    struct_0 *v9;  // rcx

    if (!g_5d7c9)
        sub_3be00();
    if (!(char)is_object())
        return;
    unmirror@opsave@opsave(v3);
    for (v4 = g_5d818->field_0; v4 != g_5d818; v4 = *((long long *)v4))
    {
        find@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5d850, &v1, &(16)[v4]);
        v5 = v1;
        if (v5 != g_5d858)
        {
            v6 = *((int *)(v5 + 20));
            v7 = v6 * 320 + *((long long *)(g_5d8f0 + 320));
            if ((unsigned int)v6 < 100000 && (char)readable(v7, 320) && v7)
                unmirror@opsave@opsave(v7, *((int *)(v5 + 20)), &(20)[v4]);
            find@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5d8b0, &v2, &(16)[v4]);
            if (g_5a2f1)
            {
                v8 = v2;
                if (v8 != g_5d8b8 && (char)readable(*((long long *)(v8 + 24)), &g_1f00))
                {
                    v9 = *((long long *)(v8 + 24));
                    if (*((int *)(v9 + &g_1d60)) == *((int *)(v5 + 20)))
                        erase@?$vector@USent@?$vector@USent(v9, &(20)[v4]);
                }
            }
        }
    }
    return;
}


// v20 RVA 01bb14..01bcd2  (446 bytes)  sub_1bb14
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_4 {
    char paddin0[16];
    unsigned long long field_10;
} struct_4;

typedef struct struct_1 {
    char paddin0[8];
    struct struct_2 *field_8;
    unsigned long long field_10;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
    char paddin8[8];
    unsigned int field_10;
} struct_0;

typedef struct struct_3 {
    char paddin0[8];
    struct struct_3 *field_8;
    unsigned int field_10;
} struct_3;

typedef struct struct_2 {
    struct struct_0 *field_0;
} struct_2;


long long sub_1bb14(unsigned long a0)
{
    char v0;  // [bp-0x28]
    char v2;  // [bp+0x10], Other Possible Types: unsigned int
    unsigned int v3;  // [bp+0x18]
    char v4;  // al, Other Possible Types: unsigned long
    unsigned long long *v5;  // gs
    struct_4 *v6;  // rbp
    struct_1 *v7;  // rdi
    struct struct_0 **v8;  // r14
    struct_0 *v9;  // rbx, Other Possible Types: unsigned long
    unsigned int v10;  // ebx
    unsigned long long v11;  // rax
    struct_3 *v12;  // rcx, Other Possible Types: unsigned long
    unsigned long v13;  // rcx

    if (!g_5b619)
    {
        v4 = g_5b8e0 == 1;
        return v4;
    }
    if (g_5c050 > *((int *)&(4)[*((long long *)(*((long long *)(88 + v5)) + (g_5e590 << 3)))]))
    {
        sub_409cc(&g_5c050);
        if (g_5c050 == -1)
        {
            *((char **)&v3) = 0;
            0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5c010, &v3, &v2);
            sub_4066c(__Fg_my_units);
            sub_40960(&g_5c050);
        }
    }
    v10 = g_5b8e0;
    v6 = (g_5b8e0 == 1 ? &g_5b770 : &g_5b6d0);
    v7 = (*((long long *)(v6 + 16)) == 0 ? v6 : &g_5b8a0);
    if (g_5c058 != v7 || g_5c060 != *((long long *)(v7 + 16)))
    {
        clear@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5c010);
        v8 = *((long long *)(v7 + 8));
        for (v9 = *(v8); v9 != v8; v9 = *((long long *)v9))
        {
            v2 = *((int *)&(16)[v9]) & 2147483616;
            $emplace@I@?$_Hash@V?$_Uset_traits@IV?$_Uhash_compare@IU?$hash@I@std(&g_5c010, &v0, &v2);
        }
        g_5c058 = v7;
        g_5c060 = *((long long *)(v7 + 16));
        v10 = g_5b8e0;
    }
    v2 = (unsigned int)a0 & 2147483616;
    v11 = (sub_619c(&v2) & g_5c040) * 2;
    if (v12 == g_5c018)
    {
        v12 = 0;
    }
    else
    {
        for (v12 = *((long long *)&(8)[v11 * 8 + g_5c028]); v2 != *((int *)&(16)[v12]); v12 = *((long long *)&(8)[v12]))
        {
            if (v12 == *((long long *)(g_5c028 + (v11 << 3))))
                break;
        }
    }
    v4 = v10 == 1 ^ v12 != 0;
    v13 = v12 != 0;
    if (*((long long *)(v6 + 16)))
        return v4;
    return v13;
}


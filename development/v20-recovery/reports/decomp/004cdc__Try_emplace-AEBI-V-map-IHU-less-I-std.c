// v20 RVA 004cdc..004dd5  (249 bytes)  $_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    unsigned long long field_0;
    char field_8;
} struct_3;

typedef struct struct_2 {
    struct struct_1 *field_0;
    unsigned long long field_8;
} struct_2;

typedef struct struct_1 {
    char paddin0[8];
    struct struct_0 *field_8;
    char paddin16[9];
    char field_19;
    char paddin26[2];
    unsigned int field_1c;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
    char paddin8[17];
    char field_19;
    char paddin26[2];
    unsigned int field_1c;
} struct_0;

typedef struct struct_4 {
    char paddin0[8];
    unsigned long long field_8;
} struct_4;


long long $_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(struct_2 *a0, struct_3 *a1, unsigned int *a2)
{
    char v0;  // [bp-0x38]
    char v1;  // [bp-0x30]
    struct_0 *v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x20]
    char v4;  // [bp-0x1c], Other Possible Types: unsigned int
    struct_3 *v6;  // rbx
    struct_1 *v8;  // r11
    unsigned int v9;  // r10d
    struct_0 *v10;  // rdx, Other Possible Types: struct_1 *, unsigned long
    struct_0 *v11;  // rax, Other Possible Types: unsigned long
    struct_0 *v12;  // r9, Other Possible Types: unsigned long
    struct_4 *v13;  // rax
    unsigned long long v14;  // rcx

    v6 = a1;
    v8 = a0->field_0;
    v12 = *((long long *)(v8 + 8));
    v9 = 0;
    v10 = v8;
    if (!*((char *)(v12 + 25)))
    {
        v11 = v12;
        do
        {
            v12 = v11;
            if (*((int *)&(28)[v11]) < *(a2))
            {
                v9 = 0;
                v11 = &(16)[v11];
            }
            else
            {
                v9 = 1;
                v10 = v11;
            }
        } while ((v11 = *((long long *)v11), !(25)[v11]));
    }
    v2 = v12;
    v3 = v9;
    v4 = *((int *)&v4);
    if (!(25)[v10] && *(a2) >= *((int *)&(28)[v10]))
    {
        v6->field_0 = v10;
        *((char **)(v6 + 8)) = 0;
        return v6;
    }
    if (*((long long *)(a0 + 8)) == 576460752303423487)
        _Xlen_string@std(); /* do not return */
    v13 = $?0AEBUpiecewise_construct_t@std$?0AEBUpiecewise_construct_t@std(&v0, a0, v8, a2);
    *((char **)(v13 + 8)) = 0;
    v14 = *((long long *)&v1);
    if (v14)
        sub_4030c(v14, 32);
    *((int128_t *)&v2) = *((int128_t *)&v2);
    v6->field_0 = _Insert_node@?$_Tree_val@U?$_Tree_simple_types@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(a0, &v2, *((long long *)(v13 + 8)));
    *((char **)(v6 + 8)) = 1;
    return v6;
}


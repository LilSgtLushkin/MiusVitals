// v20 RVA 0184fc..0187fb  (767 bytes)  read_sidecar@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    struct struct_2 *field_0;
    unsigned long long field_8;
} struct_1;

typedef struct struct_4 {
    unsigned long long field_0;
    void* field_8;
    unsigned long long field_10;
} struct_4;

typedef struct struct_3 {
    char paddin0[25];
    char field_19;
    char paddin26[38];
    unsigned int field_40;
} struct_3;

typedef struct struct_6 {
    char paddin0[8];
    void* field_8;
} struct_6;

typedef struct struct_0 {
    struct struct_0 *field_-8;
} struct_0;

typedef struct struct_2 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
} struct_2;


long long read_sidecar@opsave(unsigned long long a0, unsigned long long a1[4], struct_1 *a2)
{
    char v0;  // [bp-0x1a8]
    unsigned long v1;  // [bp-0x180]
    char v2;  // [bp-0x178]
    unsigned int v3;  // [bp-0x174]
    void* v4;  // [bp-0x170]
    struct struct_5 **v5;  // [bp-0x168]
    char v6;  // [bp-0x158]
    char v7;  // [bp-0x150]
    unsigned long long v8;  // [bp-0x148]
    char v9;  // [bp-0x140]
    char v10;  // [bp-0x130]
    struct struct_5 **v11;  // [bp-0x128]
    struct struct_5 **v12;  // [bp-0x118]
    char v13;  // [bp-0x108]
    char v14;  // [bp-0x107]
    char v15;  // [bp-0xa8]
    unsigned long long v16;  // [bp-0x48]
    unsigned long v17;  // [bp-0x8]
    char v18;  // [bp+0x0]
    struct_1 *v20;  // rsi
    struct_4 *v21;  // rbx
    unsigned long long v22;  // rax
    unsigned long long v23;  // rbx
    unsigned long long v25;  // r8
    char v26;  // dil
    struct_3 *v27;  // rbx, Other Possible Types: unsigned long
    struct_6 *v28;  // rax
    unsigned long long v29;  // rcx
    unsigned long long v30;  // rcx
    unsigned long long v31;  // rdx
    unsigned long long v32;  // rdx, Other Possible Types: unsigned long
    struct_0 *v33;  // rcx, Other Possible Types: unsigned long
    struct_0 *v34;  // rax
    struct_4 *v35;  // rbx

    v17 = &v18;
    v16 = g_5a380 ^ &v0;
    v20 = a2;
    v21 = a2->field_0;
    $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(v20, v20, *((long long *)(v21 + 8)));
    *((struct_4 **)(v21 + 8)) = v21;
    v21->field_0 = v21;
    *((struct_4 **)(v21 + 16)) = v21;
    *((char **)(a2 + 8)) = 0;
    v4 = 0;
    (unsigned int)v22 = fopen_s(&v4, a0, "r");
    if ((unsigned int)v22)
        return security_cookie_check(v16 ^ &v0);
    v23 = v4;
    if (v23)
    {
        v13 = v22;
        memset(&v14, 0, &(95)[v22]);
        *((char **)&v3) = 0;
        v26 = 1;
        if (fscanf_s(v23, "%95s", &v13, 96) != 1)
        {
LABEL_18794:
            v26 = 0;
        }
        else
        {
            v25 = *((long long *)(a1 + 16));
            if (!(v25 == strlen(&v13)) || !(!v25) && !(!memcmp()))
                goto LABEL_18794;
            for (; fscanf_s(v4, "%95s %u", &v15, 96) == 2; v11 = 0)
            {
                v11 = 0;
                v12 = 0;
                $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v11, &v15, strlen(&v15));
                $_Find_lower_bound@V?$basic_string@DU?$char_traits@D@std(v20, &v9, &v11);
                v27 = *((long long *)&v10);
                if (*((char *)(v27 + 25)) || (char)$?MDU?$char_traits@D@std(&v11, v27 + 32))
                {
                    if (*((long long *)(v20 + 8)) == 256204778801521550)
                        _Xlen_string@std(); /* do not return */
                    v5 = &v11;
                    v1 = &v2;
                    v28 = $?0AEBUpiecewise_construct_t@std$?0AEBUpiecewise_construct_t@std(&v6, v20, v20->field_0, &g_465a4, &v5);
                    *((char **)(v28 + 8)) = 0;
                    v29 = *((long long *)&v7);
                    if (v29)
                    {
                        v8 = &(32)[v29];
                        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(v8);
                    }
                    v30 = *((long long *)&v7);
                    if (v30)
                        sub_4030c(v30, 72);
                    *((int128_t *)&v5) = *((int128_t *)&v9);
                    v27 = _Insert_node@?$_Tree_val@U?$_Tree_simple_types@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(v20, &v5, *((long long *)(v28 + 8)));
                }
                *((unsigned int *)&(64)[v27]) = v3;
                v31 = (&v12)[1];
                if (v31 > 15)
                {
                    v32 = &(1)[v31];
                    v33 = v11;
                    v34 = v33;
                    if (v32 >= __Eg_my_units)
                    {
                        v32 = &(39)[v32];
                        v33 = *((long long *)(v33 - 8));
                        if (-8 + (char *)v34 - v33 > 31)
                        {
                            _invoke_watson(0, 0, 0, 0);
                            [D] Unsupported jumpkind Ijk_SigTRAP at address 100243
                        }
                    }
                    sub_4030c(v33, v32);
                }
                *((int128_t *)&v12) = 0xf0000000000000000;
            }
        }
        fclose(v4);
        if (v26)
            return security_cookie_check(v16 ^ &v0);
        v35 = v20->field_0;
        $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(v20, v20, *((long long *)(v35 + 8)));
        *((struct_4 **)(v35 + 8)) = v35;
        v35->field_0 = v35;
        *((struct_4 **)(v35 + 16)) = v35;
        *((char **)(v20 + 8)) = 0;
    }
}


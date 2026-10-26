// v20 RVA 019240..0194f9  (697 bytes)  restore_tags@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_5 {
    char padding_0[8];
    unsigned long long field_8;
} struct_5;

typedef struct struct_2 {
    struct struct_1 *field_0;
    struct struct_0 *field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_2;

typedef struct struct_6 {
    char padding_0[64];
    unsigned long long field_40;
} struct_6;

typedef struct struct_3 {
    char padding_0[25];
    char field_19;
    char padding_1a[38];
    unsigned int field_40;
} struct_3;

typedef struct struct_4 {
    char padding_0[32];
    unsigned int field_20;
} struct_4;

typedef struct struct_1 {
    struct struct_1 *field_0;
    char padding_8[17];
    char field_19;
    char padding_1a[2];
    unsigned int field_1c;
    unsigned int field_20;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
    char padding_8[8];
    unsigned long long field_10;
    char padding_18[1];
    char field_19;
} struct_0;

extern char g_0;
extern char g_1;
extern char g_10;
extern char g_101;
extern char g_18;
extern char g_19;
extern char g_1c;
extern char g_20;
extern char g_28;
extern char g_3;
extern char g_4;
extern char g_40;
extern char g_60;
extern char g_8;

int restore_tags@opsave()
{
    unsigned int v0;  // [bp-0x98]
    struct_2 *v1;  // [bp-0x90]
    char v2;  // [bp-0x80]
    char v3;  // [bp-0x70]
    char v4;  // [bp-0x68]
    char v5;  // [bp-0x58]
    char v6;  // [bp-0x48]
    unsigned long v7;  // [bp-0x8]
    char v8;  // [bp+0x0]
    unsigned long long *v9;  // [bp+0x8]
    char v10;  // [bp+0x18]
    unsigned int v11;  // [bp+0x20]
    unsigned long long *v12;  // rcx
    struct_5 *v13;  // rdx
    struct_5 *v14;  // r15
    struct_2 *v16;  // rax
    struct_3 *v17;  // rdi
    unsigned long long v18;  // rcx
    struct_6 *v20;  // rbx, Other Possible Types: unsigned long
    unsigned long long v21;  // r12, Other Possible Types: unsigned long
    void* v22;  // r13, Other Possible Types: unsigned int, unsigned long
    void* v23;  // r8, Other Possible Types: unsigned long
    struct_1 *v24;  // rax, Other Possible Types: unsigned long
    unsigned long long v25;  // rcx, Other Possible Types: unsigned long
    unsigned int v26;  // cc_dep2
    struct_1 *v27;  // rdx
    struct_1 *v28;  // rcx, Other Possible Types: unsigned long
    struct_1 *v29;  // rdx, Other Possible Types: unsigned long
    unsigned int v30;  // esi
    unsigned long long v31[5];  // rbx, Other Possible Types: unsigned long
    struct_3 *v32;  // rdi
    unsigned long long *v33;  // r14
    unsigned long long v34;  // rcx
    unsigned int v35;  // cc_dep1
    unsigned int v36;  // cc_dep2
    struct_2 *v37;  // rcx, Other Possible Types: unsigned long
    struct_0 *v38;  // rbx, Other Possible Types: unsigned long
    struct_0 *v39;  // rcx
    unsigned long long v40;  // rbp

    v9 = v12;
    v7 = &v8;
    v14 = v13;
    tagged@opsave(&v3, v12);
    v1 = &g_0;
    (unsigned int)v16 = sub_402d0(&g_28);
    v16->field_0 = &v16;
    *((struct_2 **)(v16 + &g_8)) = v16;
    *((struct_2 **)(v16 + &g_10)) = v16;
    *((char **)(v16 + &g_18)) = &g_101;
    v1 = v16;
    v20 = *((long long *)&v3);
    v21 = *((long long *)&v4);
    if (v20 != v21)
    {
        do
        {
            $_Find_lower_bound@V?$basic_string@DU?$char_traits@D@std(v13 + &g_8, &v5, v20);
            v17 = *((long long *)&v6);
            if (!*((char *)(v17 + &g_19)) && !(char)$?MDU?$char_traits@D@std(v20, v17 + &g_20) && v17 != *((long long *)(v13 + &g_8)) && *((int *)(v17 + &g_40)) == 1380275029)
            {
                v18 = *((long long *)&(&g_40)[v20]);
                if (v18)
                {
                    memcpy(&v10, *(v12) + v18, &g_4);
                    v11 = *((int *)&v10);
                    *((char **)&(&g_20)[*((long long *)::0x69ac::$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v1, &v2, &v11))]) = &(&g_1)[*((int *)&(&g_20)[*((long long *)::0x69ac::$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v1, &v2, &v11))])];
                }
            }
        } while ((v20 += &g_60, v20 != v21));
        v21 = *((long long *)&v4);
        v20 = *((long long *)&v3);
    }
    v22 = &g_0;
    v23 = &g_0;
    v24 = v1->field_0;
    if (!*((char *)(v24 + &g_19)))
    {
        do
        {
            v25 = *((int *)&(&g_20)[v24]);
            v26 = v23;
            if ((unsigned int)v25 > (unsigned int)v23)
                v22 = *((int *)&(&g_1c)[v24]);
            if ((unsigned int)v25 <= v26)
                v25 = v23;
            v23 = v25;
            v27 = *((long long *)&(&g_10)[v24]);
            if (!*((char *)(v27 + &g_19)))
            {
                v24 = v27;
                v29 = v27->field_0;
                if (!*((char *)(v29 + &g_19)))
                {
                    do
                    {
                        v24 = v29;
                        v29 = *((long long *)v29);
                    } while (!(&g_19)[v29]);
                }
            }
            else
            {
                for (v28 = *((long long *)&(&g_8)[v24]); !(&g_19)[v28] && v24 == *((long long *)&(&g_10)[v28]); v28 = *((long long *)&(&g_8)[v28]))
                {
                    v24 = v28;
                }
                v24 = v28;
            }
        } while (!(&g_19)[v24]);
    }
    v30 = &g_0;
    if (v20 != v21)
    {
        v31 = &(&g_20)[v20];
        do
        {
            $_Find_lower_bound@V?$basic_string@DU?$char_traits@D@std(v14 + &g_8, &v5, v31 - &g_20);
            v32 = *((long long *)&v6);
            if (!*((char *)(v32 + &g_19)) && !(char)$?MDU?$char_traits@D@std(v31 - &g_20, v32 + &g_20) && v32 != *((long long *)(v14 + &g_8)))
            {
                v33 = v9;
                memcpy(&v10, *((long long *)v31) + *(v33), &g_4);
                if (*((int *)&v10) != *((int *)(v32 + &g_40)))
                    v30 = &(&g_1)[v30];
                memcpy(*((long long *)v31) + *(v33), v32 + &g_40, &g_4);
                if (*((long long *)&(&g_8)[v31]))
                {
                    v34 = *((long long *)&(&g_20)[v31]);
                    if (!v34 || !(unsigned int)v22)
                    {
                        v35 = *((int *)(v32 + &g_40));
                        v36 = 1380275029;
                    }
                    else
                    {
                        memcpy(&v11, *(v33) + v34, &g_4);
                        v35 = v11;
                        v36 = v22;
                    }
                    if (v35 == v26)
                    {
                        *((char **)&v0) = &g_3;
                        memcpy(*((long long *)&(&g_8)[v31]) + *(v33), &v0, &g_4);
                    }
                }
            }
        } while ((v20 += &g_60, v20 != v21));
    }
    v37 = v1;
    v38 = *((long long *)(v37 + &g_8));
    if (!*((char *)(v38 + &g_19)))
    {
        do
        {
            ::0x5d98::$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std(&v1, &v1, *((long long *)&(&g_10)[v38]));
            v39 = v38;
            v38 = *((long long *)v38);
            sub_4030c(v39, &g_28);
        } while (!(&g_19)[v38]);
        v37 = v1;
    }
    sub_4030c(v37, &g_28);
    ::0xc090::_Tidy@?$vector@UTagged@opsave(&v3);
    v40 = v7;
    return;
}


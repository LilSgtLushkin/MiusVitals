// v20 RVA 019240..0194f9  (697 bytes)  restore_tags@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_5 {
    char paddin0[8];
    unsigned long long field_8;
} struct_5;

typedef struct struct_2 {
    struct struct_1 *field_0;
    struct struct_0 *field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_2;

typedef struct struct_6 {
    char paddin0[64];
    unsigned long long field_40;
} struct_6;

typedef struct struct_3 {
    char paddin0[25];
    char field_19;
    char paddin26[38];
    unsigned int field_40;
} struct_3;

typedef struct struct_4 {
    char paddin0[32];
    unsigned int field_20;
} struct_4;

typedef struct struct_1 {
    struct struct_1 *field_0;
    char paddin8[17];
    char field_19;
    char paddin26[2];
    unsigned int field_1c;
    unsigned int field_20;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
    char paddin8[8];
    unsigned long long field_10;
    char paddin24[1];
    char field_19;
} struct_0;


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
    v1 = 0;
    (unsigned int)v16 = sub_402d0(40);
    v16->field_0 = &v16;
    *((struct_2 **)(v16 + 8)) = v16;
    *((struct_2 **)(v16 + 16)) = v16;
    *((char **)(v16 + 24)) = 257;
    v1 = v16;
    v20 = *((long long *)&v3);
    v21 = *((long long *)&v4);
    if (v20 != v21)
    {
        do
        {
            $_Find_lower_bound@V?$basic_string@DU?$char_traits@D@std(v13 + 8, &v5, v20);
            v17 = *((long long *)&v6);
            if (!*((char *)(v17 + 25)) && !(char)$?MDU?$char_traits@D@std(v20, v17 + 32) && v17 != *((long long *)(v13 + 8)) && *((int *)(v17 + 64)) == 1380275029)
            {
                v18 = *((long long *)&(64)[v20]);
                if (v18)
                {
                    memcpy(&v10, *(v12) + v18, 4);
                    v11 = *((int *)&v10);
                    *((char **)&(32)[*((long long *)$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v1, &v2, &v11))]) = &(1)[*((int *)&(32)[*((long long *)$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v1, &v2, &v11))])];
                }
            }
        } while ((v20 += 96, v20 != v21));
        v21 = *((long long *)&v4);
        v20 = *((long long *)&v3);
    }
    v22 = 0;
    v23 = 0;
    v24 = v1->field_0;
    if (!*((char *)(v24 + 25)))
    {
        do
        {
            v25 = *((int *)&(32)[v24]);
            v26 = v23;
            if ((unsigned int)v25 > (unsigned int)v23)
                v22 = *((int *)&(28)[v24]);
            if ((unsigned int)v25 <= v26)
                v25 = v23;
            v23 = v25;
            v27 = *((long long *)&(16)[v24]);
            if (!*((char *)(v27 + 25)))
            {
                v24 = v27;
                v29 = v27->field_0;
                if (!*((char *)(v29 + 25)))
                {
                    do
                    {
                        v24 = v29;
                        v29 = *((long long *)v29);
                    } while (!(25)[v29]);
                }
            }
            else
            {
                for (v28 = *((long long *)&(8)[v24]); !(25)[v28] && v24 == *((long long *)&(16)[v28]); v28 = *((long long *)&(8)[v28]))
                {
                    v24 = v28;
                }
                v24 = v28;
            }
        } while (!(25)[v24]);
    }
    v30 = 0;
    if (v20 != v21)
    {
        v31 = &(32)[v20];
        do
        {
            $_Find_lower_bound@V?$basic_string@DU?$char_traits@D@std(v14 + 8, &v5, v31 - 32);
            v32 = *((long long *)&v6);
            if (!*((char *)(v32 + 25)) && !(char)$?MDU?$char_traits@D@std(v31 - 32, v32 + 32) && v32 != *((long long *)(v14 + 8)))
            {
                v33 = v9;
                memcpy(&v10, *((long long *)v31) + *(v33), 4);
                if (*((int *)&v10) != *((int *)(v32 + 64)))
                    v30 = &(1)[v30];
                memcpy(*((long long *)v31) + *(v33), v32 + 64, 4);
                if (*((long long *)&(8)[v31]))
                {
                    v34 = *((long long *)&(32)[v31]);
                    if (!v34 || !(unsigned int)v22)
                    {
                        v35 = *((int *)(v32 + 64));
                        v36 = 1380275029;
                    }
                    else
                    {
                        memcpy(&v11, *(v33) + v34, 4);
                        v35 = v11;
                        v36 = v22;
                    }
                    if (v35 == v26)
                    {
                        *((char **)&v0) = 3;
                        memcpy(*((long long *)&(8)[v31]) + *(v33), &v0, 4);
                    }
                }
            }
        } while ((v20 += 96, v20 != v21));
    }
    v37 = v1;
    v38 = *((long long *)(v37 + 8));
    if (!*((char *)(v38 + 25)))
    {
        do
        {
            $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std(&v1, &v1, *((long long *)&(16)[v38]));
            v39 = v38;
            v38 = *((long long *)v38);
            sub_4030c(v39, 40);
        } while (!(25)[v38]);
        v37 = v1;
    }
    sub_4030c(v37, 40);
    _Tidy@?$vector@UTagged@opsave@?$vector@UTagged@opsave(&v3);
    v40 = v7;
    return;
}


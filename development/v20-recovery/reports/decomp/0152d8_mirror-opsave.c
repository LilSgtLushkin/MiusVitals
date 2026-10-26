// v20 RVA 0152d8..0159e5  (1805 bytes)  mirror@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    struct struct_1 *field_0;
    struct struct_0 *field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_2;

typedef struct struct_9 {
    unsigned long long field_0;
    char paddin8[24];
    unsigned long long field_20;
    char paddin40[16];
    char field_38;
} struct_9;

typedef struct struct_5 {
    struct struct_6 *field_0;
    unsigned long long field_8;
} struct_5;

typedef struct struct_3 {
    char paddin0[32];
    unsigned int field_20;
} struct_3;

typedef struct struct_1 {
    struct struct_1 *field_0;
    char paddin8[17];
    char field_19;
    char paddin26[2];
    unsigned int field_1c;
    unsigned int field_20;
} struct_1;

typedef struct struct_8 {
    unsigned long long field_0;
    void* field_8;
    unsigned long long field_10;
} struct_8;

typedef struct struct_10 {
    unsigned long long field_-10;
    unsigned long long field_-8;
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
    unsigned long long field_18;
    char paddin32[8];
    char field_28;
} struct_10;

typedef struct struct_7 {
    char paddin0[64];
    unsigned int field_40;
} struct_7;

typedef struct struct_4 {
    struct struct_4 *field_0;
    char paddin8[8];
    void* field_10;
    char paddin24[1];
    char field_19;
} struct_4;

typedef struct struct_0 {
    struct struct_0 *field_0;
    char paddin8[8];
    unsigned long long field_10;
    char paddin24[1];
    char field_19;
} struct_0;

typedef struct struct_6 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
} struct_6;


long long mirror@opsave(unsigned long long *a0, unsigned int *a1, unsigned int *a2, unsigned long long *a3, unsigned int *a4)
{
    unsigned int v0;  // [bp-0x138]
    char v1;  // [bp-0x134], Other Possible Types: unsigned int
    char v2;  // [bp-0x130]
    char v3;  // [bp-0x12c], Other Possible Types: unsigned int
    char v4;  // [bp-0x128]
    char v5;  // [bp-0x124]
    char v6;  // [bp-0x120]
    unsigned int v7;  // [bp-0x11c]
    unsigned int v8;  // [bp-0x118]
    struct_2 *v9;  // [bp-0x110]
    void* v10;  // [bp-0x108]
    struct_2 *v11;  // [bp-0x100]
    void* v12;  // [bp-0xf8]
    unsigned int v13;  // [bp-0xf0]
    char v14;  // [bp-0xec]
    char v15;  // [bp-0xe8]
    unsigned long long v16[3];  // [bp-0xe0]
    int v17;  // [bp-0xd8], Other Possible Types: unsigned long
    void* v18;  // [bp-0xd0]
    unsigned long v19;  // [bp-0xc8]
    char v20;  // [bp-0xc0]
    char v21;  // [bp-0xb8]
    struct_5 *v22;  // [bp-0xa8]
    struct_5 *v23;  // [bp-0x98]
    char v24;  // [bp-0x88]
    char v25;  // [bp-0x78]
    char v26;  // [bp-0x70]
    char v27;  // [bp-0x60]
    char v28;  // [bp-0x58]
    char v29;  // [bp-0x50]
    unsigned int *v31;  // [bp+0x18]
    unsigned long long *v32;  // [bp+0x20]
    unsigned long long v33;  // rbp
    unsigned int *v34;  // r13, Other Possible Types: unsigned long
    unsigned int *v35;  // r15
    unsigned long long *v36;  // rsi
    unsigned int v37;  // r12d
    struct_2 *v38;  // rax
    struct_2 *v39;  // rax
    unsigned long long v40;  // r14
    unsigned long long v41;  // rbx
    struct_9 *v42;  // rbx, Other Possible Types: unsigned long
    char v43;  // dil
    unsigned long long v44;  // rcx
    unsigned int v45;  // cc_dep1
    unsigned int v46;  // cc_dep2
    void* v49;  // r9, Other Possible Types: unsigned long
    void* v50;  // r8, Other Possible Types: unsigned long
    struct_2 *v51;  // rax, Other Possible Types: unsigned long
    struct_1 *v52;  // rax, Other Possible Types: unsigned long
    unsigned long long v53;  // rcx, Other Possible Types: unsigned long
    unsigned int v54;  // cc_dep2
    struct_1 *v55;  // rdx
    struct_1 *v56;  // rcx, Other Possible Types: unsigned long
    struct_1 *v57;  // rdx, Other Possible Types: unsigned long
    struct_8 *v58;  // rbx
    struct_8 *v59;  // rbx
    unsigned long long v60;  // rcx
    unsigned int v61;  // cc_dep1
    unsigned int v62;  // cc_dep2
    unsigned long long v63;  // rcx
    unsigned long long *v64;  // rdx
    unsigned long long v65[5];  // rbx, Other Possible Types: unsigned long
    unsigned long long v66;  // r14, Other Possible Types: unsigned long
    struct_10 *v67;  // rdi, Other Possible Types: char, unsigned long long, unsigned long
    unsigned int v68;  // r15d
    void* v69;  // r13, Other Possible Types: unsigned long
    unsigned int *v70;  // r12
    unsigned int v71;  // ebx
    struct struct_7 **v72;  // rax
    unsigned long long v73;  // rcx
    unsigned int v74;  // ebx
    unsigned int v75;  // cc_dep1
    unsigned int v76;  // cc_dep2
    unsigned int v77[2];  // rbx, Other Possible Types: unsigned long
    unsigned long long *v78;  // rbx
    unsigned long long v79;  // rcx
    struct_7 *v80;  // rcx
    char v81[65];  // rbx
    unsigned long long v83;  // rcx
    struct_2 *v84;  // rcx, Other Possible Types: unsigned long
    struct_4 *v85;  // rbx, Other Possible Types: unsigned long
    struct_0 *v86;  // rcx
    struct_2 *v87;  // rcx, Other Possible Types: unsigned long
    struct_4 *v88;  // rbx, Other Possible Types: unsigned long
    struct_0 *v89;  // rcx
    unsigned long long v90;  // rax

    v32 = a3;
    v31 = a2;
    v33 = &v28;
    v34 = a2;
    v35 = a1;
    v36 = a0;
    tagged@opsave(&v20, a0);
    v37 = user_side@opsave@opsave(a0, &v20);
    v13 = v37;
    v11 = 0;
    v12 = 0;
    (unsigned int)v38 = sub_402d0(40);
    v38->field_0 = &v38;
    *((struct_2 **)(v38 + 8)) = v38;
    *((struct_2 **)(v38 + 16)) = v38;
    *((char **)(v38 + 24)) = 257;
    v11 = v38;
    v9 = 0;
    v10 = 0;
    (unsigned int)v39 = sub_402d0(40);
    v39->field_0 = &v39;
    *((struct_2 **)(v39 + 8)) = v39;
    *((struct_2 **)(v39 + 16)) = v39;
    *((char **)(v39 + 24)) = 257;
    v9 = v39;
    v43 = 0;
    v40 = *((long long *)&v21);
    v41 = *((long long *)&v20);
    if (v41 == v40)
    {
        goto LABEL_1592e;
    }
    else
    {
        v42 = &(32)[v41];
        do
        {
            memcpy(&v1, *(v36) + *((long long *)v42), 4);
            v0 = *((int *)&v1);
            if (v0 == 1380275029)
            {
                v43 = 1;
            }
            else if (v0)
            {
                v44 = *((long long *)&(32)[v42]);
                if (!v44 || !v37)
                {
                    memcpy(&v2, *(v36) + *((long long *)v42), 4);
                    v45 = *((int *)&v2);
                    v46 = 1380275029;
                }
                else
                {
                    memcpy(&v3, *(v36) + v44, 4);
                    v45 = *((int *)&v3);
                    v46 = v37;
                }
                if (v45 != v46)
                {
                    *((char **)&(32)[*((long long *)$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v9, &v23, &v0, a3, v0))]) = &(1)[*((int *)&(32)[*((long long *)$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v9, &v23, &v0, a3, v0))])];
                    if ((56)[v42])
                        *((char **)&(32)[*((long long *)$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v11, &v22, &v0, a3, *((long long *)&v0)))]) = &(1)[*((int *)&(32)[*((long long *)$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v11, &v22, &v0, a3, *((long long *)&v0)))])];
                }
            }
        } while ((v42 += 96, v42 - 32 != v40));
        if (!(v43) || !(v10))
        {
LABEL_1592e:
            v67 = 0;
        }
        else
        {
            v49 = 0;
            *((char **)&v3) = 0;
            v50 = 0;
            v51 = v11;
            if (!v12)
                v51 = v9;
            v52 = *((long long *)v51);
            if (!*((char *)(v52 + 25)))
            {
                do
                {
                    v53 = *((int *)&(32)[v52]);
                    v54 = v50;
                    if ((unsigned int)v53 > (unsigned int)v50)
                        v49 = *((int *)&(28)[v52]);
                    if ((unsigned int)v53 <= v46)
                        v53 = v50;
                    v50 = v53;
                    v55 = *((long long *)&(16)[v52]);
                    if (*((char *)(v55 + 25)))
                    {
                        for (v56 = *((long long *)&(8)[v52]); !(25)[v56] && v52 == *((long long *)&(16)[v56]); v56 = *((long long *)&(8)[v56]))
                        {
                            v52 = v56;
                        }
                        v52 = v56;
                    }
                    else
                    {
                        v52 = v55;
                        v57 = v55->field_0;
                        if (!*((char *)(v57 + 25)))
                        {
                            do
                            {
                                v52 = v57;
                                v57 = *((long long *)v57);
                            } while (!(25)[v57]);
                        }
                    }
                } while (!(25)[v52]);
                v3 = v49;
            }
            *(v35) = v49;
            v22 = v35 + 8;
            v58 = v22->field_0;
            $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(v22, v22, *((long long *)(v58 + 8)));
            *((struct_8 **)(v58 + 8)) = v58;
            v58->field_0 = v58;
            *((struct_8 **)(v58 + 16)) = v58;
            *((char **)(v22 + 8)) = 0;
            v23 = v35 + 24;
            v59 = v23->field_0;
            $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(v23, v23, *((long long *)(v59 + 8)));
            *((struct_8 **)(v59 + 8)) = v59;
            v59->field_0 = v59;
            *((struct_8 **)(v59 + 16)) = v59;
            *((char **)(v23 + 8)) = 0;
            *((char **)v16) = 0;
            v18 = 0;
            v66 = *((long long *)&v21);
            v19 = v66;
            v67 = *((long long *)&v20);
            if (v67 != v66)
            {
                v65 = &(32)[v67];
                do
                {
                    memcpy(&v2, *((long long *)v65) + *(v36), 4);
                    if (*((int *)&v2))
                    {
                        v60 = *((long long *)&(32)[v65]);
                        if (!v60 || !v37)
                        {
                            memcpy(&v0, *((long long *)v65) + *(v36), 4);
                            v61 = v0;
                            v62 = 1380275029;
                        }
                        else
                        {
                            memcpy(&v1, *(v36) + v60, 4);
                            v61 = *((int *)&v1);
                            v62 = v37;
                        }
                        if (v45 != v46 && *((long long *)&(16)[v65]) && *((long long *)&(24)[v65]))
                        {
                            v63 = *((long long *)&(8)[v65]);
                            if (v63)
                            {
                                memcpy(&v4, *(v36) + v63, 4);
                                if (*((int *)&v4) >= 1)
                                {
                                    memcpy(&v5, *(v36) + *((long long *)&(16)[v65]), 4);
                                    memcpy(&v6, *((long long *)&(24)[v65]) + *(v36), 4);
                                    v7 = *((int *)&v5);
                                    v8 = *((int *)&v6);
                                    v64 = v16[1];
                                    if (v64 != v18)
                                    {
                                        *(v64) = *((long long *)&v7);
                                        v17 = &(8)[v16[1]];
                                    }
                                    else
                                    {
                                        $_Emplace_reallocate@U?$pair@HH@std$_Emplace_reallocate@U?$pair@HH@std(v16, v64, &v7);
                                    }
                                }
                            }
                        }
                    }
                } while ((v42 += 96, v42 - 32 != v40));
                v66 = *((long long *)&v21);
                v19 = v66;
                v67 = *((long long *)&v20);
            }
            v68 = 0;
            if (v67 != v66)
            {
                v67 = &(48)[v67];
                v69 = v67 - 48;
                v70 = a4;
                do
                {
                    memcpy(&v6, *(v36) + *((long long *)(v67 - 16)), 4);
                    v71 = *((int *)&v6);
                    if (v71)
                    {
                        *((unsigned int *)&(64)[*((long long *)$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(v22, &v26, v69))]) = v71;
                        if (*((long long *)(v67 - 8)))
                        {
                            v72 = $_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(v23, &v27, v67 - 48);
                            memcpy(&v5, *(v36) + *((long long *)(v67 - 8)), 4);
                            *((int *)(*(v72) + 64)) = *((int *)&v5);
                        }
                        v73 = *((long long *)&(16)[v67]);
                        if (!v73 || (v74 = v13, !v74))
                        {
                            memcpy(&v2, *(v36) + *((long long *)(v67 - 16)), 4);
                            v75 = *((int *)&v2);
                            v76 = 1380275029;
                        }
                        else
                        {
                            memcpy(&v4, *(v36) + v73, 4);
                            v75 = *((int *)&v4);
                            v76 = v74;
                        }
                        if (v45 != v46)
                        {
                            strncpy(&v1, "USER", 4);
                        }
                        else
                        {
                            v1 = v3;
                            if (*((long long *)v67) && *((long long *)&(8)[v67]))
                            {
                                for (v77 = v16[0]; v77 != (long long)v17; v77 = &(8)[v77])
                                {
                                    memcpy(&v14, *(v36) + v67->field_-10, 4);
                                    if (abs(*((int *)v77) - *((int *)&v14)) <= 1)
                                    {
                                        memcpy(&v15, *(v36) + *((long long *)(v67 + 8)), 4);
                                        if (abs(*((int *)&(4)[v77]) - *((int *)&v15)) <= 1)
                                            break;
                                    }
                                }
                            }
                            v78 = v32;
                            if (v78 && !(40)[v67])
                            {
                                v79 = *((long long *)&(24)[v67]);
                                if (v79)
                                {
                                    memcpy(&v7, *(v36) + v79, 4);
                                    if (((unsigned short)v7 & 8208))
                                    {
                                        v80 = *((long long *)$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(v78, &v29, v67 - 48));
                                        *((unsigned long *)(v80 + 64)) = *((int *)(v80 + 64)) | 2;
                                    }
                                }
                                $_Find_lower_bound@V?$basic_string@DU?$char_traits@D@std(v78, &v24, v67 - 48);
                                v81 = *((long long *)&v25);
                                if (!*((char *)(v81 + 25)) && !(char)$?MDU?$char_traits@D@std(v67 - 48, v81 + 32) && v81 != *(v32) && (*((char *)(v81 + 64)) & 2) && v70)
                                    *((char **)&v70) = &(1)[*(v70)];
                            }
                        }
                        memcpy(*(v36) + *((long long *)(v67 - 16)), &v1, 4);
                        v83 = *((long long *)(v67 - 8));
                        if (v83)
                            memcpy(v83 + *(v36), &v0, 4);
                        v68 = &(1)[v68];
                        v66 = v19;
                    }
                } while ((v42 += 96, v42 - 32 != v40));
                v34 = v31;
            }
            if (v34)
                *((unsigned int *)v34) = v68;
            *((char *)&v67) = v68 != 0;
            _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(v16);
        }
    }
    v84 = v9;
    v85 = *((long long *)(v84 + 8));
    if (!*((char *)(v85 + 25)))
    {
        do
        {
            $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std(&v9, &v9, *((long long *)&(16)[v85]));
            v86 = v85;
            v85 = *((long long *)v85);
            sub_4030c(v86, 40);
        } while (!(25)[v85]);
        v84 = v9;
    }
    sub_4030c(v84, 40);
    v87 = v11;
    v88 = *((long long *)(v87 + 8));
    if (!*((char *)(v88 + 25)))
    {
        do
        {
            $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std(&v11, &v11, *((long long *)&(16)[v88]));
            v89 = v88;
            v88 = *((long long *)v88);
            sub_4030c(v89, 40);
        } while (!(25)[v88]);
        v87 = v11;
    }
    sub_4030c(v87, 40);
    v90 = _Tidy@?$vector@UTagged@opsave@?$vector@UTagged@opsave(&v20);
    *((char *)&v90) = v67;
    return v90;
}


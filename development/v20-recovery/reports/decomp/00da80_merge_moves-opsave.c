// v20 RVA 00da80..00e709  (3209 bytes)  merge_moves@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_4 {
    char field_0;
    char paddin1[7];
    struct struct_5 *field_8;
    unsigned long long field_10;
} struct_4;

typedef struct struct_1 {
    uint128_t field_0;
    unsigned int field_10;
} struct_1;

typedef struct struct_3 {
    char field_0;
    char paddin1[15];
    unsigned long long field_10;
    unsigned long long field_18;
} struct_3;

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_0 {
    struct struct_0 *field_-8;
} struct_0;

typedef struct struct_6 {
    uint128_t field_0;
    char field_10;
} struct_6;

typedef struct struct_5 {
    char field_0;
} struct_5;


long long merge_moves@opsave(struct_4 *a0, unsigned long long a1, unsigned long long a2[2], unsigned long long a3, unsigned int a4, unsigned int *a5, unsigned int *a6)
{
    char v0;  // [bp-0x368]
    int tmp_5;  // tmp #5
    char v1;  // [bp-0x328]
    unsigned int v2;  // [bp-0x320]
    unsigned int v3;  // [bp-0x318]
    unsigned int v4;  // [bp-0x310]
    int v5;  // [bp-0x308]
    unsigned int v6;  // [bp-0x2f8]
    unsigned long long v7[2];  // [bp-0x2f0]
    unsigned long long v8[3];  // [bp-0x2e8]
    void* v9;  // [bp-0x2d8]
    unsigned long v10;  // [bp-0x2d0]
    int v11;  // [bp-0x2c8], Other Possible Types: unsigned long
    unsigned long v12;  // [bp-0x2c0]
    void* v13;  // [bp-0x2b8], Other Possible Types: unsigned long
    unsigned long long v14[2];  // [bp-0x2b0], Other Possible Types: unsigned int, unsigned long
    unsigned long long v15[2];  // [bp-0x2a0], Other Possible Types: unsigned long
    unsigned int *v16;  // [bp-0x290]
    unsigned int *v17;  // [bp-0x288]
    char v18;  // [bp-0x280]
    unsigned long long v19[2];  // [bp-0x268]
    char v20;  // [bp-0x258]
    char v21;  // [bp-0x250]
    char v22;  // [bp-0x240]
    char v23;  // [bp-0x218]
    char v24;  // [bp-0x210]
    char v25;  // [bp-0x208]
    char v26;  // [bp-0x200]
    char v27;  // [bp-0x1d8]
    char v28;  // [bp-0x1d0]
    char v29;  // [bp-0x1c8]
    char v30;  // [bp-0x1c0]
    char v31;  // [bp-0x198]
    char v32;  // [bp-0x190]
    char v33;  // [bp-0x180]
    char v34;  // [bp-0x158]
    char v35;  // [bp-0x150]
    char v36;  // [bp-0x140]
    char v37;  // [bp-0x118]
    char v38;  // [bp-0x110]
    char v39;  // [bp-0x100]
    int v40;  // [bp-0xd8], Other Possible Types: char
    void* v41;  // [bp-0xc8], Other Possible Types: int, char
    char v42;  // [bp-0xc0]
    int v43;  // [bp-0xb8], Other Possible Types: char
    int v44;  // [bp-0xa8], Other Possible Types: char
    char v45;  // [bp-0xa0]
    int v46;  // [bp-0x98], Other Possible Types: char
    int v47;  // [bp-0x88], Other Possible Types: unsigned int
    char v48;  // [bp-0x78]
    char v49;  // [bp-0x68]
    unsigned long long v50;  // [bp-0x58]
    char v53;  // edi
    struct_4 *v54;  // rsi
    struct_1 *v55;  // rax
    struct_1 *v56;  // rax
    char v57;  // r12b
    unsigned long long v58;  // rbx
    unsigned long v59;  // r15
    unsigned long long v60;  // r12
    unsigned long long v61;  // rbx, Other Possible Types: unsigned long
    unsigned long v62;  // r14, Other Possible Types: unsigned long long
    unsigned long long v63;  // rdx
    unsigned long long v64;  // rdx
    unsigned long long v65;  // rdx
    unsigned long long v66;  // r9
    unsigned long v67;  // r8
    unsigned long long v68;  // rdx
    unsigned long long v69;  // rdx
    void* v70;  // rdi, Other Possible Types: unsigned long
    unsigned long long v71;  // r13
    unsigned long long v72;  // r8
    struct_0 *v74;  // rcx, Other Possible Types: unsigned int *, struct_2 *, struct_3 *, unsigned long, unsigned long long
    unsigned long long v75[2];  // rdi, Other Possible Types: unsigned long
    unsigned long v76;  // r13, Other Possible Types: unsigned long long
    unsigned long long v77;  // rbx, Other Possible Types: unsigned long
    unsigned long long v78[2];  // rdi
    char *v79;  // rdx, Other Possible Types: unsigned long
    unsigned long long v80;  // rax
    unsigned long long v81;  // r8
    void* v82;  // rax, Other Possible Types: unsigned long
    unsigned long long v83;  // rax
    unsigned long long v84;  // rdx
    unsigned long v85;  // rdx
    struct_3 *v86;  // rdi, Other Possible Types: unsigned long
    unsigned long long v87;  // rbx, Other Possible Types: unsigned long
    unsigned long long v88;  // rdx
    unsigned long long v89;  // rdx, Other Possible Types: unsigned long
    struct_0 *v90;  // rax
    unsigned long long *v91;  // rbx
    unsigned long long v92;  // rcx
    unsigned long long v93;  // rcx
    unsigned long long v94;  // rcx
    unsigned long long v95;  // rdx
    unsigned long long v96;  // rdx, Other Possible Types: unsigned long
    struct_0 *v97;  // rcx, Other Possible Types: unsigned long
    struct_0 *v98;  // rax
    struct_6 *v99;  // rax
    unsigned int v100;  // r13d
    void* v101;  // rdi, Other Possible Types: unsigned long
    unsigned long long v102;  // r12
    unsigned long long v103;  // r15
    unsigned long v104;  // r14, Other Possible Types: unsigned long long
    unsigned long long v106;  // r14
    struct_2 *v108;  // rcx
    unsigned long long v109;  // rcx
    struct_2 *v110;  // rcx
    unsigned long long v111;  // rcx
    unsigned long long v112;  // rdx
    unsigned long long v113;  // rdx
    unsigned long long v114;  // rdx, Other Possible Types: unsigned long
    struct_0 *v115;  // rcx, Other Possible Types: unsigned long
    struct_0 *v116;  // r8, Other Possible Types: unsigned long

    v50 = g_5a380 ^ &v0;
    v7[0] = a2;
    v53 = a1;
    v54 = a0;
    v16 = a5;
    v74 = a6;
    v17 = v74;
    *((char **)&v4) = 0;
    *((char **)&v74) = 0;
    *((char **)&v16) = 0;
    (unsigned int)v55 = find@image(&v46, a2, "supports");
    *((uint128_t *)&v5) = v55->field_0;
    v6 = *((int *)(v55 + 16));
    (unsigned int)v56 = find@image(&v18, v54, "supports");
    *((uint128_t *)&v46) = v56->field_0;
    v47 = *((int *)(v56 + 16));
    state_load(&v37, a2, &v5);
    state_load(&v34, v54, &v46);
    *((char **)&v2) = 0;
    0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&v23, &v2);
    *((char **)&v2) = 0;
    0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&v27, &v2);
    *((char **)&v2) = 0;
    0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&v31, &v2);
    squad_owners@image@image(a2, a3, 0, &v37);
    squad_owners@image@image(a1, a4, 0, &v34);
    squad_owners@image@image(v54, a4, 1, &v34);
    if (!(*((long long *)&v25) || *((long long *)&v29)))
    {
        _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v33);
        _Tidy@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v32);
        _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v30);
        _Tidy@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v28);
        _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v26);
        _Tidy@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v24);
        _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v36);
        _Tidy@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v35);
        _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v39);
        _Tidy@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v38);
        return security_cookie_check(v50 ^ &v0);
    }
    v57 = v47;
    v58 = (int)(&v46)[4];
    if (!(char)v6 || (int)v5 != 2 || v57 && ((int)v46 != 2 || (unsigned int)v58 != (int)(&v5)[4]))
        v53 = 0;
    else
        v53 = 1;
    v59 = (int)(&v5)[4];
    if (v57)
        v59 = v58;
    v76 = v59 * 32;
    v10 = v76;
    *((char **)v8) = 0;
    v9 = 0;
    *((char **)&v2) = 0;
    0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&v20, &v2);
    if (v57 && (int)v46 == 2 && (unsigned int)a2)
    {
        v60 = v58 * 32;
        v61 = (int)(&v46)[12];
        v62 = (int)(&v46)[8];
        do
        {
            $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v40, v54, v61);
            v74 = &v40;
            if (*((long long *)&v42) > 15)
                v74 = *((long long *)&v40);
            v63 = *((long long *)&v41);
            if (*((long long *)&(8)[$_Find_last@V?$basic_string@DU?$char_traits@D@std(&v27, v15, &v40, sub_6168())]))
            {
                v74 = &v40;
                if (*((long long *)&v42) > 15)
                    v74 = *((long long *)&v40);
                v64 = *((long long *)&v41);
                if (!*((long long *)&(8)[$_Find_last@V?$basic_string@DU?$char_traits@D@std(&v31, v14, &v40, sub_6168())]))
                {
                    if (!v53)
                        continue;
                    v74 = &v40;
                    if (*((long long *)&v42) > 15)
                        v74 = *((long long *)&v40);
                    v65 = *((long long *)&v41);
                    if (*((long long *)&(8)[$_Find_last@V?$basic_string@DU?$char_traits@D@std(&v23, v19, &v40, sub_6168())]) == 0)
                        continue;
                }
            }
            v66 = *((long long *)&v54->field_0);
            v67 = v66 + v61;
            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v8, v8[1], v67, v66 - v67 + v61 + v76);
            $_Try_emplace@V?$basic_string@DU?$char_traits@D@std$_Try_emplace@V?$basic_string@DU?$char_traits@D@std(&v20, &v18, &v40);
        } while ((_Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v40), v61 += v60, tmp_5 = v62, v62 -= 1, tmp_5 != 1));
    }
    if (v53 && v53)
    {
        v77 = (int)(&v5)[12];
        v75 = (int)(&v5)[8];
        v7[0] = v75;
        do
        {
            $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v43, a2, v77);
            v74 = &v43;
            if (*((long long *)&v45) > 15)
                v74 = *((long long *)&v43);
            v68 = *((long long *)&v44);
            if (*((long long *)&(8)[$_Find_last@V?$basic_string@DU?$char_traits@D@std(&v23, &v18, &v43, sub_6168())]))
            {
                v74 = &v43;
                if (*((long long *)&v45) > 15)
                    v74 = *((long long *)&v43);
                v69 = *((long long *)&v44);
                if (!*((long long *)&(8)[$_Find_last@V?$basic_string@DU?$char_traits@D@std(&v20, v19, &v43, sub_6168())]))
                {
                    *((char **)&v40) = 0;
                    v41 = 0;
                    v14 = a2[0] + v77;
                    *((unsigned long *)v15) = v14 + v76;
                    $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std(v14, v69, v77, v15);
                    if ((unsigned int)v59)
                    {
                        v70 = 0;
                        v71 = v59;
                        do
                        {
                            v15 = (long long)v40 + v70;
                            memcpy(&v2, &(16)[v15], 4);
                            if (v2 == 2)
                            {
                                $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v48, a2, v77 + v70);
                                v14 = *((int *)&v49);
                                memcpy(v15, v14, 4);
                                v3 = put_varint(v54, &v48);
                                memcpy(&(4)[v15], &v3, 4);
                                _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v48);
                            }
                        } while ((_Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v40), v61 += v60, tmp_5 = v62, v62 -= 1, tmp_5 != 1));
                        v76 = v10;
                        v75 = v7;
                    }
                    v72 = (long long)v40;
                    $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v8, v8[1], v72, (long long)(&v40)[8] - v72);
                    $_Try_emplace@V?$basic_string@DU?$char_traits@D@std$_Try_emplace@V?$basic_string@DU?$char_traits@D@std(&v20, &v5, &v43);
                    *((char **)&v16) = &(1)[*(v16)];
                    _Tidy@?$vector@EV?$allocator@E@std(&v40);
                }
            }
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v43);
            v77 += (int)(&v5)[4] * 32;
            tmp_5 = v75;
            v78 = v75 - 1;
            v7[0] = v78;
        } while (tmp_5 != 1);
    }
    if ((char)v47)
    {
        v79 = *((long long *)(v54 + 8));
        if (((char)v79 - v54->field_0 & 15))
        {
            *((char **)&v1) = 0;
            do
            {
                if (v79 == *((long long *)(v54 + 16)))
                {
                    $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(v54, v79, &v1);
                }
                else
                {
                    *((char **)v79) = 0;
                    *((char **)(v54 + 8)) = &(1)[*((long long *)(v54 + 8))];
                }
            } while ((_Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v40), v61 += v60, tmp_5 = v62, v62 -= 1, tmp_5 != 1));
        }
        v81 = v8[0];
        $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v54, v79, v81, v8[1] - v81);
        (unsigned int)v83 = turns@opsave@opsave(v54, "supports");
        v3 = (0 CONCAT v8[1] - v8[0]) /m v76;
        memcpy(&(40)[*((long long *)&v54->field_0) + v83], &v3, 4);
        v3 = (unsigned int)v79 - *((int *)&v54->field_0);
        memcpy(&(48)[*((long long *)&v54->field_0) + v83], &v3, 4);
    }
    else if (v8[0] != v8[1])
    {
        (unsigned int)v80 = turns@opsave@opsave(a2, "supports");
        if (!v80)
        {
            v82 = 0;
        }
        else
        {
            memcpy(&v3, &(44)[v80 + a2[0]], 4);
            v82 = v3;
        }
        if (!(unsigned int)v82 || (v74 = a2[0], v84 = *((long long *)(a2 + 8)) - v74, v82 >= v84))
        {
            *((char **)&v40) = 0;
            *((char **)&v41) = 0;
            $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v40, &g_45ce6, strlen(&g_45ce6));
            v86 = &v40;
            v87 = 2;
        }
        else
        {
            v85 = v84 - v82;
            *((char **)&v46) = 0;
            *((char **)&v47) = 0;
            $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v46, a2[0] + v82, strnlen(v74 + v82, v85));
            v86 = &v46;
            v87 = 1;
        }
        v4 = v87;
        *((char **)&v43) = 0;
        *((char **)&v44) = 0;
        memcpy(&v43, v86, 32);
        *((char **)&(16)[v86]) = 0;
        *((char **)&(24)[v86]) = 15;
        *((char **)v86) = 0;
        if (((char)v87 & 2))
        {
            v87 = (unsigned int)v87 & -3;
            v4 = v87;
            v88 = (long long)(&v41)[8];
            if (v88 > 15)
            {
                v89 = &(1)[v88];
                v74 = (long long)v40;
                v90 = v74;
                if (v89 >= __Eg_my_units)
                {
                    v89 = &(39)[v89];
                    v74 = *((long long *)(v74 - 8));
                    if (-8 + (char *)v90 - v74 > 31)
                    {
                        _invoke_watson(0, 0, 0, 0);
                        [D] Unsupported jumpkind Ijk_SigTRAP at address 57970
                    }
                }
                sub_4030c(v74, v89);
            }
            *((int128_t *)&v41) = 0xf0000000000000000;
            *((char **)&v40) = 0;
        }
        if (((char)v87 & 1))
        {
            v4 = (unsigned int)v87 & -2;
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v46);
        }
        *((char **)&v11) = 0;
        v13 = 0;
        v91 = v54 + 8;
        $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std(v74, *(v91) - *((long long *)&v54->field_0), v54, v91);
        if ((long long)v44 != v59 || !(char)nodes@opsave@opsave(v54, "i_divisions:divisions", "supports", v59, &v43, v8, a2))
        {
            if (v54 != &v11)
            {
                v92 = *((long long *)&v54->field_0);
                *((long long *)&v54->field_0) = (long long)v11;
                v11 = v92;
                v93 = *(v91);
                *(v91) = (long long)(&v11)[8];
                v12 = v93;
                v94 = *((long long *)(v54 + 16));
                *((void* *)(v54 + 16)) = v13;
                v13 = v94;
            }
            clear@?$_Hash@V?$_Umap_traits@_KV?$basic_string@DU?$char_traits@D@std@?$_Hash@V?$_Umap_traits@_KV?$basic_string@DU?$char_traits@D@std(&v20);
            *((char **)&v16) = 0;
        }
        _Tidy@?$vector@EV?$allocator@E@std(&v11);
        v95 = (long long)(&v44)[8];
        if (v95 > 15)
        {
            v96 = &(1)[v95];
            v97 = (long long)v43;
            v98 = v97;
            if (v96 >= __Eg_my_units)
            {
                v96 = &(39)[v96];
                v97 = *((long long *)(v97 - 8));
                if (-8 + (char *)v98 - v97 > 31)
                {
                    _invoke_watson(0, 0, 0, 0);
                    [D] Unsupported jumpkind Ijk_SigTRAP at address 58276
                }
            }
            sub_4030c(v97, v96);
        }
        *((int128_t *)&v44) = 0xf0000000000000000;
        *((char **)&v43) = 0;
    }
    (unsigned int)v99 = find@image(&v5, v54, "squads");
    *((uint128_t *)&v46) = v99->field_0;
    v100 = 0;
    v101 = 0;
    if (*((char *)(v99 + 16)))
    {
        for (v102 = (int)(&v46)[4]; (unsigned int)v101 < (int)(&v46)[8]; v101 = &(1)[v101])
        {
            v103 = v101 * v102;
            v104 = (int)(&v46)[12];
            v10 = v104;
            memcpy(&v3, &(1)[v103] * 32 + v104 + *((long long *)&v54->field_0), 4);
            if ((unsigned int)(v3 >> 24) == a4 && (unsigned int)v102)
            {
                do
                {
                    v106 = v104 + &(0)[v103] * 32;
                    memcpy(v14, &(16)[*((long long *)&v54->field_0) + v106], 4);
                    if ((int)v14[0] == 2)
                    {
                        $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v43, v54, v106);
                        if ((long long)v44)
                        {
                            v108 = &v43;
                            if (*((long long *)&v45) > 15)
                                v109 = *((long long *)&v43);
                            if (*((long long *)&(8)[$_Find_last@V?$basic_string@DU?$char_traits@D@std(&v37, &v5, &v43, sub_6168())]))
                            {
                                v110 = &v43;
                                if (*((long long *)&v45) > 15)
                                    v111 = *((long long *)&v43);
                                v112 = (long long)v44;
                                if (!*((long long *)&(8)[$_Find_last@V?$basic_string@DU?$char_traits@D@std(&v20, &v18, &v43, sub_6168())]))
                                {
                                    if (!v100)
                                    {
                                        *((char **)&v40) = 0;
                                        *((char **)&v41) = 0;
                                        $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v40, &g_45ce6, strlen(&g_45ce6));
                                        v100 = put_varint(v54, &v40);
                                        v113 = (long long)(&v41)[8];
                                        if (v113 > 15)
                                        {
                                            v114 = &(1)[v113];
                                            v115 = (long long)v40;
                                            v116 = v115;
                                            if (v114 >= __Eg_my_units)
                                            {
                                                v114 = &(39)[v114];
                                                v115 = *((long long *)(v115 - 8));
                                                v116 = (char *)v116 - v115 - 8;
                                                if (v116 > 31)
                                                {
                                                    _invoke_watson(0, 0, 0, 0);
                                                    _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v22);
                                                }
                                            }
                                            sub_4030c(v115, v114, v116);
                                        }
                                        *((int128_t *)&v41) = 0xf0000000000000000;
                                        *((char **)&v40) = 0;
                                    }
                                    *((char **)&v2) = 0;
                                    memcpy(*((long long *)&v54->field_0) + v106, &v2, 4);
                                    v2 = v100;
                                    memcpy(&(4)[*((long long *)&v54->field_0) + v106], &v2, 4);
                                    *((char **)&v17) = &(1)[*(v17)];
                                }
                            }
                        }
                        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v43);
                    }
                } while ((_Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v40), v61 += v60, tmp_5 = v62, v62 -= 1, tmp_5 != 1));
            }
        }
    }
    _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v22);
    _Tidy@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v21);
    _Tidy@?$vector@EV?$allocator@E@std(v8);
}


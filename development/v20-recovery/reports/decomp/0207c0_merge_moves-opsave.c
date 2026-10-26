// v20 RVA 0207c0..021974  (4532 bytes)  merge_moves@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_0 {
    uint128_t field_0;
    unsigned int field_10;
} struct_0;

typedef struct struct_9 {
    char field_0;
    char paddin1[15];
    unsigned long long field_10;
    unsigned long long field_18;
} struct_9;

typedef struct struct_8 {
    unsigned long long field_-8;
} struct_8;

typedef struct struct_1 {
    char paddin0[8];
    struct struct_1 *field_8;
    unsigned int field_10;
} struct_1;

typedef struct struct_7 {
    struct struct_6 *field_0;
    char paddin8[8];
    char field_10;
} struct_7;

typedef struct struct_4 {
    struct struct_3 *field_0;
    struct struct_5 *field_8;
} struct_4;

typedef struct struct_6 {
    char field_0;
} struct_6;

typedef struct struct_3 {
    struct struct_3 *field_0;
} struct_3;

typedef struct struct_5 {
    unsigned long long field_0;
} struct_5;


long long merge_moves@opsave(unsigned long long a0[2], unsigned long long *a1, unsigned long long a2, unsigned long long a3, unsigned long long a4)
{
    char v0;  // [bp-0x3d8]
    char <0x207c0[is_45]|Stack bp-0x240, 1 B>;  // [bp-0x240]
    void* v1;  // [bp-0x3b8], Other Possible Types: unsigned int
    struct_2 *v2;  // [bp-0x3a8], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x3a0]
    unsigned int v4;  // [bp-0x398]
    unsigned int v5;  // [bp-0x390]
    unsigned int v6;  // [bp-0x388]
    unsigned long v7;  // [bp-0x380]
    char v8;  // [bp-0x378]
    char v9;  // [bp-0x370], Other Possible Types: unsigned int
    unsigned long v10;  // [bp-0x368], Other Possible Types: unsigned int
    unsigned int v11;  // [bp-0x360]
    unsigned int v12;  // [bp-0x35c]
    char *v13;  // [bp-0x358], Other Possible Types: unsigned long
    char *v14;  // [bp-0x350], Other Possible Types: unsigned long
    void* v15;  // [bp-0x348], Other Possible Types: unsigned long
    char v16;  // [bp-0x340], Other Possible Types: unsigned int
    unsigned int v17;  // [bp-0x338]
    unsigned int v18;  // [bp-0x330]
    char *v19;  // [bp-0x328]
    void* v20;  // [bp-0x318]
    char *v21;  // [bp-0x310]
    void* v22;  // [bp-0x300]
    unsigned long long *v23;  // [bp-0x2f8]
    int v24;  // [bp-0x2f0]
    unsigned int v25;  // [bp-0x2e0]
    unsigned long long v26[2];  // [bp-0x2d8]
    unsigned long v27;  // [bp-0x2d0]
    unsigned long v28;  // [bp-0x2c8], Other Possible Types: unsigned long long
    int v29;  // [bp-0x2c0], Other Possible Types: char
    unsigned int v30;  // [bp-0x2b0]
    char *v31;  // [bp-0x2a8]
    unsigned long v32;  // [bp-0x2a0]
    void* v33;  // [bp-0x298]
    char *v34;  // [bp-0x290]
    unsigned long v35;  // [bp-0x288]
    void* v36;  // [bp-0x280]
    char v37;  // [bp-0x278]
    char v38;  // [bp-0x274]
    unsigned long long v39;  // [bp-0x270]
    unsigned long v40;  // [bp-0x268]
    unsigned long v41;  // [bp-0x260]
    unsigned long v42;  // [bp-0x258]
    unsigned long v43;  // [bp-0x250]
    unsigned long v44;  // [bp-0x248]
    char *v45;  // [bp-0x240]
    void* v46;  // [bp-0x230]
    unsigned long v47;  // [bp-0x228]
    char v48;  // [bp-0x218]
    char v49;  // [bp-0x210]
    char v50;  // [bp-0x200]
    char v51;  // [bp-0x1e8]
    char v52;  // [bp-0x1d8]
    char v53;  // [bp-0x1c0]
    char v54;  // [bp-0x1b8]
    char v55;  // [bp-0x1a8]
    char v56;  // [bp-0x1a0]
    char v57;  // [bp-0x198]
    char v58;  // [bp-0x190]
    char v59;  // [bp-0x188]
    char v60;  // [bp-0x168]
    char v61;  // [bp-0x160]
    char v62;  // [bp-0x150]
    char *v63;  // [bp-0x140]
    char *v64;  // [bp-0x130]
    char *v65;  // [bp-0x120]
    char *v66;  // [bp-0x110]
    char v67;  // [bp-0xf8]
    char v68;  // [bp-0xf0]
    char v69;  // [bp-0xe0]
    char v70;  // [bp-0xc8]
    void* v71;  // [bp-0xb8]
    unsigned int v72;  // [bp-0xb0]
    void* v73;  // [bp-0xa8], Other Possible Types: char, unsigned int, unsigned long
    unsigned int v74;  // [bp-0xa0]
    char *v75;  // [bp-0x98]
    void* v76;  // [bp-0x88]
    unsigned long long v77;  // [bp-0x68]
    unsigned long long *v81;  // r12, Other Possible Types: unsigned long
    struct_2 *v83;  // rdx
    unsigned long long v84;  // rdx
    unsigned long long v85;  // r8
    struct struct_6 **v86;  // rcx
    struct_2 *v87;  // rdi, Other Possible Types: unsigned long
    unsigned long long v88;  // rsi
    struct_0 *v89;  // rax
    struct_0 *v90;  // rax
    struct_0 *v91;  // rax
    unsigned long long v92;  // rax
    struct_0 *v93;  // rax
    struct_0 *v94;  // rax
    void* v95;  // rdi, Other Possible Types: unsigned long
    unsigned long long v97;  // rdx
    unsigned long long v98;  // r8
    unsigned long long v99;  // rcx
    unsigned int *v100;  // rdx
    unsigned long long v101;  // rax
    unsigned long long v102;  // rdx
    unsigned long long v103;  // rdx
    unsigned long long v104;  // rcx
    struct_9 *v105;  // rax
    struct_8 *v106;  // rcx
    unsigned long long v107;  // rax
    unsigned long long *v108;  // rdx
    struct_1 *v109;  // rcx, Other Possible Types: unsigned long
    unsigned long long v110;  // rdi, Other Possible Types: unsigned long
    unsigned long v111;  // rsi
    unsigned long long v112[2];  // r13, Other Possible Types: unsigned long
    void* v113;  // rbx, Other Possible Types: unsigned int *, unsigned int, unsigned long long, unsigned long
    void* v114;  // r15, Other Possible Types: struct_2 *, unsigned long
    unsigned long v115;  // rsi, Other Possible Types: unsigned long long
    void* v116;  // r14, Other Possible Types: unsigned long
    unsigned long v117;  // r14, Other Possible Types: unsigned long long
    unsigned long long v118;  // rsi
    unsigned long long *v119;  // rdi
    unsigned long v120;  // rdx
    unsigned long v122;  // r14
    unsigned int v123;  // eax
    unsigned long long v124;  // rcx
    struct struct_6 **v125;  // r8
    unsigned long long v126;  // r8
    unsigned long long v127;  // rdx
    struct struct_6 **v128;  // rcx
    unsigned int *v129;  // rdx
    unsigned long long v130;  // r8
    unsigned long long *v131;  // rdi, Other Possible Types: unsigned long
    void* v132;  // r12, Other Possible Types: unsigned long
    char *v133;  // rbx, Other Possible Types: unsigned long
    unsigned long v134;  // rdi, Other Possible Types: unsigned long long
    unsigned long long v135;  // r8
    unsigned long long v136;  // rax
    unsigned int v137;  // eax
    unsigned int v138;  // r14d
    struct_7 *v139;  // rax
    char *v140;  // xmm6
    unsigned long v141;  // rax
    unsigned long long v142;  // rbx, Other Possible Types: unsigned long
    unsigned long long v143;  // rdi
    unsigned long long v144;  // r12
    void* v145;  // rsi, Other Possible Types: unsigned long
    unsigned int v146;  // r15d
    unsigned long long v148;  // rcx, Other Possible Types: unsigned long
    unsigned long long v149;  // rcx
    unsigned long long v150;  // rax
    unsigned long long v151;  // rax
    unsigned long long *v152;  // rdx
    struct_1 *v153;  // rcx, Other Possible Types: unsigned long
    unsigned int *v154;  // rcx, Other Possible Types: unsigned long
    unsigned int *v155;  // rdx
    unsigned long long v156;  // rax
    unsigned long v157;  // rdx
    unsigned long long v158;  // rdx
    unsigned long long v159;  // r8
    struct_8 *v160;  // rcx
    unsigned int *v161;  // rbx, Other Possible Types: unsigned long
    unsigned long v162;  // rcx
    unsigned long long v163;  // rdi
    char *v164;  // rdi, Other Possible Types: unsigned long
    unsigned long v165;  // rsi, Other Possible Types: unsigned long long
    unsigned long long v166;  // r8
    unsigned long long v167;  // rax
    struct_4 *v168;  // rcx
    struct struct_3 **v169;  // rcx, Other Possible Types: unsigned long
    unsigned long long v170;  // rcx
    unsigned long long v171;  // rcx
    unsigned long long v172;  // rcx
    unsigned long long v174;  // r9
    unsigned long long v175;  // r8
    unsigned long long v176;  // rdx
    unsigned long long v177;  // rcx
    struct_4 *v178;  // rcx
    struct struct_3 **v179;  // rcx, Other Possible Types: unsigned long

    v77 = g_5a380 ^ &v0;
    v39 = a3;
    v81 = a1;
    v23 = a1;
    v112 = a0;
    v26[0] = a0;
    counterpart@opsave@opsave(&v58);
    counterpart@opsave@opsave(&v53, a0, a2);
    v114 = 0;
    if (!v60)
        sprintf_s(a3, a4);
    if (v57)
    {
        *((char **)&v11) = 0;
        sides@image(a0, &v11, &v9);
        v17 = ((unsigned int)a2 == v11 ? 1296387653 : 1380275029);
        v63 = 0;
        v64 = 0;
        v83 = &v54;
        if (*((long long *)&v56) > 15)
            v84 = *((long long *)&v54);
        v85 = *((long long *)&v55);
        v86 = &v63;
        $_Construct@$01PEBD@?$basic_string@DU?$char_traits@D@std();
        *((char **)&v12) = 0;
        0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&v48, &v12, &v8);
        v42 = "plats";
        v43 = "squads";
        v44 = "units";
        v87 = &v42;
        do
        {
            v88 = *((long long *)v87);
            (unsigned int)v89 = find@image(&v29, v81, v88);
            *((uint128_t *)&v24) = v89->field_0;
            v25 = *((int *)(v89 + 16));
            (unsigned int)v90 = find@image(&v29, v112, v88);
            if (!((char)v25) || !((int)(int128_t)v24 == 2) || !((char)*((int *)(v90 + 16))) || !((unsigned int)v90->field_0 == 2) || !((unsigned int)((int128_t)v24 >> 64 >> 32 CONCAT (int128_t)v24 >> 32 | (int128_t)v24 >> 64 << 32) == (unsigned int)(v90->field_0 >> 32)))
            {
                sprintf_s(a3, a4, "table %s is not the same in both descriptions", v88);
LABEL_218d3:
                goto LABEL_218d3;
            }
            v113 = 0;
            if ((unsigned int)(0 CONCAT (int128_t)v24 >> 64))
            {
                do
                {
                    a3 = strcmp(v88, "units") == 0;
                    v12 = operation_name@opsave@opsave(v81, &v24, v113, r9<8>);
                    if ((unsigned int)(v12 >> 24) == *((int *)&v58))
                        $emplace@I@?$_Hash@V?$_Uset_traits@IV?$_Uhash_compare@IU?$hash@I@std(&v48, &v73, &v12);
                } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
            }
            v87 = &v43;
        } while (v87 != &<0x207c0[is_45]|Stack bp-0x240, 1 B>);
        (unsigned int)v91 = find@image(&v52, v81, "squads");
        *((uint128_t *)&v24) = v91->field_0;
        v25 = *((int *)(v91 + 16));
        (unsigned int)v92 = squad_columns@image(v81, &v24);
        v47 = v92;
        if ((unsigned int)(v92 >> 32) < 0)
        {
            sprintf_s(a3, a4, "the chosen force's squads have no command tag");
LABEL_218d3:
            _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v50);
            v178 = *((long long *)&v49);
            *((char **)*((long long *)(v178 + 8))) = 0;
            v179 = v178->field_0;
            if (v179)
            {
                do
                {
                    sub_4030c(v179, 24);
                    v179 = *((long long *)v179);
                } while (v179);
            }
        }
        else
        {
            v13 = 0;
            v15 = 0;
            $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std(&v13, *((long long *)(v112 + 8)) - v112[0], v112, v112 + 8);
            v71 = 0;
            *((char **)&v72) = 0;
            v73 = 0;
            *((char **)&v74) = 0;
            v45 = 0;
            v46 = 0;
            v34 = 0;
            v36 = 0;
            v31 = 0;
            v33 = 0;
            v137 = 0;
            *((char **)&v11) = 0;
            while (true)
            {
                v117 = *((long long *)((char *)&v42 + v114 * 8));
                v41 = v117;
                *((char *)&v113) = v137 == 2;
                v8 = v137 == 2;
                (unsigned int)v93 = find@image(&v75, v81, v117);
                *((uint128_t *)&v24) = v93->field_0;
                v25 = *((int *)(v93 + 16));
                (unsigned int)v94 = find@image(&v75, &v13, v117);
                *((uint128_t *)&v29) = v94->field_0;
                v30 = *((int *)(v94 + 16));
                v28 = ((int128_t)v29 >> 32) * 32;
                v21 = 0;
                v22 = 0;
                v95 = 0;
                if ((unsigned int)(0 CONCAT (int128_t)v29 >> 64))
                {
                    do
                    {
                        *((char *)&a3) = v113;
                        if ((unsigned int)(operation_name@opsave@opsave(&v13, &v29, v95, a3) >> 24) != *((int *)&v53))
                        {
                            v97 = (int)(&v29)[4] * v95 * 32 + (int)(&v29)[12];
                            a3 = v13;
                            v98 = a3 + v97;
                            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v21, (&v21)[1], v98, a3 - v98 + v97 + v28);
                        }
                        else
                        {
                            *((char **)&v73) = v73 + 1;
                            if (!v114)
                            {
                                v113 = (int)(&v29)[4] * v95 * 32 + (int)(&v29)[12];
                                v99 = &v13[v113];
                                v27 = v99 + v28;
                                v10 = v99;
                                __Fg_held(&<0x207c0[is_45]|Stack bp-0x240, 1 B>, &v10, &v27);
                                memcpy(&v12, v13 + 32 + v113, 4);
                                v10 = v12;
                                v100 = (&v34)[1];
                                if (v100 == v36)
                                {
                                    $_Emplace_reallocate@AEBI@?$vector@IV?$allocator@I@std(&v34);
                                }
                                else
                                {
                                    *(v100) = v10;
                                    v35 = (&v34)[1] + 4;
                                }
                                *((char *)&v113) = v8;
                            }
                        }
                    } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
                    v112 = v26;
                }
                v132 = 0;
                if ((unsigned int)(0 CONCAT (int128_t)v24 >> 64))
                {
                    v131 = v23;
                    do
                    {
                        *((char *)&a3) = v113;
                        (unsigned int)v101 = operation_name@opsave@opsave(v131, &v24, v132, a3);
                        if ((unsigned int)(v101 >> 24) == *((int *)&v58))
                        {
                            v102 = (int)(&v24)[12];
                            v113 = (int)(&v24)[4];
                            v115 = v132 * v113;
                            v27 = v115;
                            v103 = v102 + v115 * 32 + *((long long *)v131);
                            v19 = 0;
                            v20 = 0;
                            v40 = v103;
                            v104 = v28;
                            v10 = v40 + v104;
                            $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std(&v19, v104, &v40, &v10);
                            v116 = 0;
                            if ((unsigned int)v113)
                            {
                                do
                                {
                                    v110 = &v19[v116 * 32];
                                    memcpy(&v37, &(16)[v110], 4);
                                    a3 = *((int *)&v37);
                                    if (a3 == 2)
                                    {
                                        $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v61, v23, (v115 + v116) * 32 + (int)(&v24)[12]);
                                        (unsigned int)v105 = counterpart@opsave@opsave(&v67, &v61, &v48, *((int *)&v58));
                                        if (&v61 != v105)
                                        {
                                            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v61);
                                            memcpy(&v61, v105, 32);
                                            *((char **)(v105 + 16)) = 0;
                                            *((char **)(v105 + 24)) = 15;
                                            *((char **)&v105->field_0) = 0;
                                        }
                                        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v67);
                                        memcpy(&v38, &(4)[v110], 4);
                                        v113 = (*((int *)&v38) == 0 ? put_varint(&v13, &v61) : 0);
                                        v10 = *((int *)&v62);
                                        memcpy(v110, &v10, 4);
                                        v10 = v113;
                                        memcpy(&(4)[v110], &v10, 4);
                                        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v61);
                                    }
                                    else
                                    {
                                        if ((a3 - 4 & -3))
                                        {
                                            if (!((a3 & -6)) && !(a3 == 4))
                                                continue;
                                            sprintf_s(v39, a4, "an unknown kind of value (%u) in table %s");
                                            v106 = v19;
                                            v114 = 0;
                                            if (!v106)
                                            {
                                                _Tidy@?$vector@EV?$allocator@E@std(&v21);
                                            }
                                            else if (!(v20 - v106 >= __Eg_my_units) || !(-8 + (char *)v106 - *((long long *)(v106 - 8)) > 31))
                                            {
                                                sub_4030c();
                                                v19 = 0;
                                                v20 = 0;
                                                _Tidy@?$vector@EV?$allocator@E@std(&v21);
                                            }
                                            else
                                            {
                                                v1 = 0;
                                                _invoke_watson(0, 0, 0, 0);
                                            }
                                        }
                                        v111 = (a3 == 6 ? 1 : 4);
                                        do
                                        {
                                            memcpy(&v16, v110, 4);
                                            v10 = *((int *)&v16);
                                            if ((unsigned int)(v10 >> 24) == *((int *)&v58))
                                            {
                                                v107 = (sub_619c(&v10) & *((long long *)&v51)) * 2;
                                                v108 = *((long long *)&v50);
                                                if (v109 == *((long long *)&v49))
                                                {
                                                    v109 = 0;
                                                }
                                                else
                                                {
                                                    for (v109 = *((long long *)((char *)v108 + v107 * 8 + 8)); v10 != *((int *)&(16)[v109]); v109 = *((long long *)&(8)[v109]))
                                                    {
                                                        if (v109 == *((long long *)((char *)v108 + (v107 << 3))))
                                                            break;
                                                    }
                                                }
                                                if (v109)
                                                {
                                                    v10 = v10 & 16777215 | *((int *)&v53) * 0x1000000;
                                                    memcpy(v110, &v10, 4);
                                                }
                                            }
                                        } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
                                        v112 = v26;
                                    }
                                    v115 = v27;
                                } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
                            }
                            v117 = *((long long *)((char *)&v42 + v114 * 8));
                            if (!strcmp(v117, "squads"))
                            {
                                v10 = v17;
                                memcpy(&v19[*((int *)((char *)&v47 + 4)) * 32], &v10, 4);
                            }
                            if (!v114)
                            {
                                v118 = (int)v71;
                                v119 = v45;
                                if (v119 != (&v45)[1])
                                {
                                    v120 = ((&v45)[1] - v119) / 24 + ((&v45)[1] - v119) / 0xc0000000000000000;
                                    v122 = (v120 <= v118 ? v118 : v120 - 1) * 3;
                                    v113 = &g_46808;
                                    v123 = (int)(&v29)[4];
                                    do
                                    {
                                        if (*((int *)v113) < v123)
                                        {
                                            v124 = *((int *)v113) * 32;
                                            memcpy(&v19[v124], *((long long *)((char *)v119 + v122 * 8)) + v124, 32);
                                            v123 = (int)(&v29)[4];
                                        }
                                    } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
                                    if ((int)v71 < ((&v45)[1] - v45) / 24 + ((&v45)[1] - v45) / 0)
                                    {
                                        v117 = *((long long *)((char *)&v42 + v114 * 8));
                                    }
                                    else
                                    {
                                        v125 = &v63;
                                        if ((&v64)[1] > 15)
                                            v126 = v63;
                                        a3 = (int)v71;
                                        v127 = "pl_%s_%u";
                                        v128 = &v75;
                                        $sprintf_s@$0BAE$sprintf_s@$0BAE();
                                        v65 = 0;
                                        v66 = 0;
                                        $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v65, &v75, strlen(&v75));
                                        v113 = put_varint(&v13, &v65);
                                        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v65);
                                        v18 = strlen(&v75);
                                        memcpy(v19, &v18, 4);
                                        v10 = v113;
                                        memcpy(v19 + 4, &v10, 4);
                                        memcpy(&v9, v19 + 32, 4);
                                    }
                                }
                                memcpy(&v9, v19 + 32, 4);
                                v10 = *((int *)&v9);
                                v129 = (&v31)[1];
                                if (v129 != v33)
                                {
                                    *(v129) = v10;
                                    v32 = (&v31)[1] + 4;
                                    v130 = v19;
                                    $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v21, (&v21)[1], v130, &((&v19)[1])[-1 * v130]);
                                }
                                $_Emplace_reallocate@AEBI@?$vector@IV?$allocator@I@std(&v31);
                            }
                            v130 = v19;
                            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v21, (&v21)[1], v130, &((&v19)[1])[-1 * v130]);
                            *((char **)((char *)v26 + v114 * 4 + 544)) = &(1)[*((int *)((char *)v26 + v114 * 4 + 544))];
                            _Tidy@?$vector@EV?$allocator@E@std(&v19);
                            v131 = v23;
                            *((char *)&v113) = v8;
                        }
                    } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
                }
                v133 = (&v13)[1];
                v134 = v13;
                if (((char)v133 - (char)v134 & 15))
                {
                    *((char **)&v8) = 0;
                    do
                    {
                        if (v133 == v15)
                        {
                            $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(&v13, v133, &v8);
                            v133 = (&v13)[1];
                        }
                        else
                        {
                            *((char **)v133) = 0;
                            v133 = (&v13)[1] + 1;
                            v14 = v133;
                        }
                    } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
                }
                v135 = v21;
                $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v13, v133, v135, &((&v21)[1])[-1 * v135]);
                (unsigned int)v136 = turns@opsave@opsave(&v13, v117);
                if (!v136)
                {
                    sprintf_s(v39, a4, "table %s not found", v117);
                    _Tidy@?$vector@EV?$allocator@E@std(&v21);
                    break;
                }
                else
                {
                    v10 = (0 CONCAT (&v21)[1] - v21) /m v28;
                    memcpy(v13 + 40 + v136, &v10, 4);
                    v10 = v133 - v134;
                    memcpy(v13 + 48 + v136, &v10, 4);
                    _Tidy@?$vector@EV?$allocator@E@std(&v21);
                    v137 = &(1)[v11];
                    v11 = v137;
                    v114 = &(1)[v114];
                    if (v114 < 3)
                    {
                        v81 = v23;
                    }
                    else
                    {
                        v138 = 0;
                        (unsigned int)v139 = find@image(&v75, &v13, "brief");
                        v140 = *((int128_t *)&v139->field_0);
                        if (*((char *)(v139 + 16)) && (unsigned int)v140 == 2)
                        {
                            v141 = v140 / 0x100000000;
                            if ((unsigned int)v141 >= 1)
                            {
                                v142 = v34;
                                v143 = (&v34)[1];
                                if (v142 != v143)
                                {
                                    v41 = v141;
                                    v144 = v141 * 32;
                                    *((char **)&v9) = 0;
                                    0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&v67, &v9, &v8);
                                    do
                                    {
                                        $emplace@I@?$_Hash@V?$_Uset_traits@IV?$_Uhash_compare@IU?$hash@I@std(&v67, &v47, v142);
                                        v142 = &(4)[v142];
                                    } while (v142 != v143);
                                    v21 = 0;
                                    v22 = 0;
                                    v19 = 0;
                                    v20 = 0;
                                    v145 = 0;
                                    v146 = 0 CONCAT (unsigned long long)v140 / 0x10000000000000000;
                                    v112 = v26;
                                    if (v146)
                                    {
                                        v148 = 0 CONCAT (unsigned long long)v140 / 0x1000000000000000000000000;
                                        v28 = v148;
                                        while (true)
                                        {
                                            v149 = v148 + v41 * v145 * 32;
                                            v150 = v13;
                                            v75 = 0;
                                            v76 = 0;
                                            v40 = v150 + v149;
                                            v47 = v149 + v150 + v144;
                                            $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std(&v75, v47 - v40, &v40, &v47);
                                            memcpy(&v9, v75 + 4, 4);
                                            v17 = v9;
                                            memcpy(&v18, v75 + 16, 4);
                                            if (v18 != 6)
                                            {
                                                v159 = v75;
                                                $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v21, (&v21)[1], v159, &((&v75)[1])[-1 * v159]);
                                            }
                                            else
                                            {
                                                v151 = (sub_619c(&v17) & *((long long *)&v70)) * 2;
                                                v152 = *((long long *)&v69);
                                                if (v153 == *((long long *)&v68))
                                                {
                                                    v153 = 0;
                                                }
                                                else
                                                {
                                                    for (v153 = *((long long *)((char *)v152 + v151 * 8 + 8)); v17 != *((int *)&(16)[v153]); v153 = *((long long *)&(8)[v153]))
                                                    {
                                                        if (v153 == *((long long *)((char *)v152 + (v151 << 3))))
                                                            break;
                                                    }
                                                }
                                                if (!v153)
                                                {
                                                    v159 = v75;
                                                    $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v21, (&v21)[1], v159, &((&v75)[1])[-1 * v159]);
                                                }
                                                v154 = 0;
                                                v155 = v34;
                                                v156 = (&v34)[1] - v155 >> 2;
                                                if (v156)
                                                {
                                                    while (*((int *)((char *)v155 + (v154 << 2))) != v17 && (v154 += 1, v154 < v156))
                                                    ;
                                                }
                                                v157 = v31;
                                                if (v154 < &((&v31)[1])[-1 * v157] >> 2)
                                                {
                                                    v16 = *((int *)(v157 + v154 * 4));
                                                    memcpy(v75 + 4, &v16, 4);
                                                    v158 = v75;
                                                    $_Assign_counted_range@PEBE@?$vector@EV?$allocator@E@std(&v19, v158, &((&v75)[1])[-1 * v158]);
                                                    v159 = v75;
                                                    $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v21, (&v21)[1], v159, &((&v75)[1])[-1 * v159]);
                                                }
                                            }
                                            v160 = v75;
                                            if (v160)
                                            {
                                                if (v76 - v160 >= __Eg_my_units && -8 + (char *)v160 - *((long long *)(v160 - 8)) > 31)
                                                {
                                                    _invoke_watson(0, 0, 0, 0);
                                                    [D] Unsupported jumpkind Ijk_SigTRAP at address 136646
                                                }
                                                sub_4030c();
                                                v76 = 0;
                                                v75 = 0;
                                            }
                                            v145 = &(1)[v145];
                                            if ((unsigned int)v145 >= v146)
                                                break;
                                            v148 = v28;
                                        }
                                    }
                                    v161 = (&v34)[1] - v34 >> 2;
                                    v162 = v31;
                                    if ((&v34)[1] - v34 >> 2 < (&v31)[1] - v31 >> 2)
                                    {
                                        v163 = v19;
                                        while (v163 != (&v19)[1])
                                        {
                                            v9 = *((int *)(v162 + v161 * 4));
                                            memcpy(&(4)[v163], &v9, 4);
                                            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v21, (&v21)[1], v163, &((&v19)[1])[-1 * v163]);
                                            v138 = &(1)[v138];
                                            v161 = &(1)[v161];
                                            v162 = v31;
                                            if (v161 >= &((&v31)[1])[-1 * v162] >> 2)
                                                break;
                                        }
                                        v112 = v26;
                                    }
                                    v164 = (&v13)[1];
                                    v165 = v13;
                                    if (((char)v164 - (char)v165 & 15))
                                    {
                                        *((char **)&v8) = 0;
                                        do
                                        {
                                            if (v164 != v15)
                                            {
                                                *((char **)v164) = 0;
                                                v164 = (&v13)[1] + 1;
                                                v14 = v164;
                                            }
                                            else
                                            {
                                                $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(&v13, v164, &v8);
                                                v164 = (&v13)[1];
                                            }
                                        } while ((v113 = (unsigned long long)((unsigned int)v113 + 1), (unsigned int)v113 < (int)(&v24)[8]));
                                    }
                                    v166 = v21;
                                    $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v13, v164, v166, &((&v21)[1])[-1 * v166]);
                                    (unsigned int)v167 = turns@opsave@opsave(&v13, "brief");
                                    v9 = (0 CONCAT (&v21)[1] - v21) /m v144;
                                    memcpy(v13 + 40 + v167, &v9, 4);
                                    v9 = v164 - v165;
                                    memcpy(v13 + 48 + v167, &v9, 4);
                                    _Tidy@?$vector@EV?$allocator@E@std(&v19);
                                    _Tidy@?$vector@EV?$allocator@E@std(&v21);
                                    _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v69);
                                    v168 = *((long long *)&v68);
                                    *((char **)*((long long *)(v168 + 8))) = 0;
                                    v169 = v168->field_0;
                                    if (v169)
                                    {
                                        do
                                        {
                                            sub_4030c(v169, 24);
                                            v169 = *((long long *)v169);
                                        } while (v169);
                                    }
                                    sub_4030c(*((long long *)&v68), 24);
                                }
                            }
                        }
                        *((char **)&v12) = 0;
                        *((char **)&v11) = 0;
                        v2 = &v11;
                        v1 = *((int *)&v53);
                        merge_moves@opsave@opsave(&v13, v112, v23, *((int *)&v58), v1, &v12);
                        if (v112 != &v13)
                        {
                            v170 = *((long long *)v112);
                            *((char **)v112) = v13;
                            v13 = v170;
                            v171 = *((long long *)&(8)[v112]);
                            *((char **)&(8)[v112]) = (&v13)[1];
                            v14 = v171;
                            v172 = *((long long *)&(16)[v112]);
                            *((void* *)&(16)[v112]) = v15;
                            v15 = v172;
                        }
                        *((int *)&v7) = (v11 != 0 ? &g_45ce6 : " (a support could not be carried: its squad is an ordinary one)");
                        v6 = v12;
                        v5 = v138;
                        v4 = v74;
                        v3 = *((int *)((char *)&v73 + 4));
                        v2 = v73;
                        v174 = (int)v71;
                        v175 = "%u platoons, %u squads, %u units in place of %u, %u, %u; %u platoons placed where the battle's stood; %u supports%s";
                        v176 = a4;
                        v177 = v39;
                        sprintf_s();
                        break;
                    }
                }
            }
            _Tidy@?$vector@KV?$allocator@K@std(&v31);
            _Tidy@?$vector@KV?$allocator@K@std(&v34);
            _Tidy@?$vector@UTagged@opsave@?$vector@UTagged@opsave(&<0x207c0[is_45]|Stack bp-0x240, 1 B>);
            _Tidy@?$vector@EV?$allocator@E@std(&v13);
            _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v50);
        }
        sub_4030c(*((long long *)&v49), 24);
        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v63);
    }
    else
    {
        sprintf_s(a3, a4);
    }
    _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v54);
    _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v59);
    return security_cookie_check(v77 ^ &v0);
}


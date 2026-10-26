// v20 RVA 01c544..01d2d4  (3472 bytes)  session_sim_step
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_9 {
    char paddin0[240];
    struct struct_10 *field_f0;
    char paddin248[4];
    unsigned int field_fc;
} struct_9;

typedef struct struct_0 {
    unsigned int field_0;
    char paddin4[12];
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    unsigned long long field_50;
} struct_0;

typedef struct struct_3 {
    char paddin0[24];
    unsigned long long field_18;
} struct_3;

typedef struct struct_5 {
    uint128_t field_0;
    char paddin16[368];
    unsigned int field_180;
} struct_5;

typedef struct struct_11 {
    char paddin0[384];
    unsigned int field_180;
} struct_11;

typedef struct struct_8 {
    char paddin0[24];
    struct struct_4 *field_18;
} struct_8;

typedef struct struct_7 {
    char paddin0[4];
    char field_4;
    char paddin5[1];
    char field_6;
    char paddin7[49];
    unsigned int field_38;
    char paddin60[4];
    unsigned int field_40;
} struct_7;

typedef struct struct_1 {
    char field_0;
} struct_1;

typedef struct struct_4 {
    char paddin0[72];
    unsigned long long field_48;
    char paddin80[4576];
    unsigned int field_1230;
} struct_4;

typedef struct struct_2 {
    char paddin0[80];
    unsigned long long field_50;
} struct_2;

typedef struct struct_10 {
    unsigned long long field_0;
} struct_10;


long long session_sim_step(struct_9 *a0)
{
    char v0;  // [bp-0x188]
    int tmp_8;  // tmp #8
    int tmp_10;  // tmp #10
    int tmp_9;  // tmp #9
    int tmp_11;  // tmp #11
    int tmp_7;  // tmp #7
    int tmp_32;  // tmp #32
    int tmp_33;  // tmp #33
    int tmp_30;  // tmp #30
    int tmp_28;  // tmp #28
    int tmp_29;  // tmp #29
    int tmp_21;  // tmp #21
    int tmp_23;  // tmp #23
    int tmp_22;  // tmp #22
    int tmp_15;  // tmp #15
    int tmp_17;  // tmp #17
    int tmp_16;  // tmp #16
    int tmp_6;  // tmp #6
    int tmp_27;  // tmp #27
    int tmp_34;  // tmp #34
    int tmp_31;  // tmp #31
    int tmp_42;  // tmp #42
    int tmp_43;  // tmp #43
    int tmp_40;  // tmp #40
    int tmp_4;  // tmp #4
    int tmp_5;  // tmp #5
    int tmp_24;  // tmp #24
    int tmp_12;  // tmp #12
    int tmp_13;  // tmp #13
    int tmp_14;  // tmp #14
    int tmp_18;  // tmp #18
    int tmp_20;  // tmp #20
    int tmp_19;  // tmp #19
    int tmp_3;  // tmp #3
    int tmp_1;  // tmp #1
    int tmp_2;  // tmp #2
    unsigned long v1;  // [bp-0x150]
    char v2;  // [bp-0x148]
    char v3;  // [bp-0x147]
    char v4;  // [bp-0x140], Other Possible Types: unsigned long, unsigned int
    char v5;  // [bp-0x138]
    struct struct_6 **v6;  // [bp-0x130]
    unsigned int v7;  // [bp-0x128], Other Possible Types: unsigned long long
    char v8;  // [bp-0x124]
    char v9;  // [bp-0x120]
    char v10;  // [bp-0x11c]
    char v11;  // [bp-0x118]
    char v15;  // [bp-0x108]
    char v16;  // [bp-0x100]
    char v17;  // [bp-0xf8]
    int v18;  // [bp-0xf0], Other Possible Types: char
    char v19;  // [bp-0xec]
    char v20;  // [bp-0xe8]
    char v21;  // [bp-0xe0]
    unsigned long long v22;  // [bp-0xd8]
    unsigned long v23;  // [bp-0x8]
    char v24;  // [bp+0x0]
    struct_9 *v26;  // rbx
    unsigned int v27;  // ecx
    int v29;  // xmm0
    char v30;  // rsi
    unsigned long long *v31;  // rdi, Other Possible Types: unsigned long
    unsigned long v32;  // r14, Other Possible Types: unsigned long long
    struct_0 *v33;  // rbx, Other Possible Types: unsigned long
    int v34;  // xmm14
    struct_3 *v35;  // rcx
    int v36;  // xmm7
    int v37;  // xmm8
    int v38;  // xmm9
    struct_5 *v39;  // rdi
    int v40;  // xmm10
    int v41;  // xmm11
    struct_5 *v42;  // rdx, Other Possible Types: unsigned long
    unsigned long long v43;  // rax
    uint128_t v44[6];  // rax
    unsigned long v46;  // rcx
    int v47;  // xmm0
    unsigned long long *v49;  // r12
    struct_11 *v50;  // r13
    struct_8 *v51;  // rbx
    unsigned long long v52;  // rdx
    unsigned long long v53;  // rcx
    unsigned long long *v54;  // rax, Other Possible Types: unsigned long
    int v55;  // xmm7
    unsigned long long *v56;  // rax, Other Possible Types: unsigned long
    int v57;  // xmm1
    unsigned long long v58;  // rcx
    int v60;  // xmm0
    unsigned long long v61;  // rdx, Other Possible Types: unsigned long
    unsigned long long *v62;  // r8, Other Possible Types: unsigned long
    void* v63;  // r15, Other Possible Types: unsigned long
    int v64;  // xmm3
    int v65;  // xmm6
    int v66;  // xmm9
    unsigned long long v67;  // r11, Other Possible Types: unsigned long
    struct_7 *v68;  // r14
    int v70;  // xmm1
    int v71;  // xmm0
    int v73;  // xmm1
    unsigned int v74[13];  // rax, Other Possible Types: unsigned long
    unsigned long long v75;  // r8, Other Possible Types: unsigned long
    unsigned long long v76;  // rdx, Other Possible Types: unsigned long
    unsigned int v77[13];  // rax, Other Possible Types: unsigned long
    int v79;  // xmm0
    unsigned int v84;  // xmm0
    unsigned int v85;  // xmm1
    unsigned int v86;  // xmm2
    int v87;  // xmm0
    unsigned long v88;  // rax
    struct_1 *v90;  // rax, Other Possible Types: unsigned long
    unsigned long long v91;  // rcx, Other Possible Types: unsigned long
    struct_4 *v103;  // r15
    unsigned int v104;  // cc_dep1
    unsigned long long v105;  // rax, Other Possible Types: unsigned long
    int v106;  // xmm0
    int v107;  // xmm1
    int v108;  // xmm1
    unsigned long long v109;  // rcx, Other Possible Types: unsigned long
    unsigned long long v110;  // rcx
    int v111;  // xmm0
    unsigned long long v112;  // r8
    struct_2 *v113;  // r9
    int v114;  // xmm1
    int v115;  // xmm3
    int v116;  // xmm0
    unsigned long long v118;  // rcx, Other Possible Types: unsigned long
    struct_1 *v120;  // rax, Other Possible Types: unsigned long
    unsigned long long v121;  // r8, Other Possible Types: unsigned long
    unsigned long v122;  // rax
    int v123;  // xmm1
    unsigned int v124[13];  // rcx, Other Possible Types: unsigned long
    int v126;  // xmm0
    int v127;  // xmm0
    unsigned int v128;  // xmm0
    unsigned int v129;  // xmm3
    unsigned int v130;  // xmm4
    unsigned int *v132;  // rbx, Other Possible Types: unsigned long
    int v133;  // xmm0
    int v134;  // xmm1
    int v135;  // xmm0
    int v136;  // xmm1
    struct_1 *v137;  // rcx, Other Possible Types: unsigned long
    unsigned int *v139;  // rdx, Other Possible Types: unsigned long
    unsigned long long v140;  // r8, Other Possible Types: unsigned long
    unsigned int v141;  // eax
    struct struct_6 **v149;  // rdi, Other Possible Types: unsigned long

    v23 = &v24;
    v22 = g_5a380 ^ &v0;
    v26 = a0;
    if (!g_5b61b)
        return security_cookie_check(v22 ^ &v0);
    if (!g_5b618)
        return security_cookie_check(v22 ^ &v0);
    if (g_5a264 < 0)
    {
        if (!GetEnvironmentVariableA("MP_APPLY_DELAY", &v18, 16))
            v27 = &g_2ee0;
        else
            v27 = atoi(&v18) * 1000;
        g_5a264 = v27;
        g_5c080 = GetTickCount();
    }
    seconds_now();
    if ((char)sync_state_enabled() && (char)sub_39a38() && GetTickCount() - g_5c080 >= g_5a264)
        sub_31828(v26);
    seconds_now();
    tmp_8 = SubV(tmp_10, tmp_9);
    v29 = tmp_8;
    v30 = *((int *)(v26 + 252));
    g_5bf20 = (unsigned long long)((0 CONCAT g_5bf20) + v29);
    if (v30 != g_5be40)
    {
        v31 = *((long long *)(v26 + 240));
        clear@?$_Hash@V?$_Umap_traits@_KPEAUObject@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5b730);
        if (v30)
        {
            v32 = v30;
            do
            {
                if ((char)unit_key(*((long long *)v31), &v4, 0))
                    *((long long *)&(24)[*((long long *)$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5b730, &v18, &v4))]) = *((long long *)v31);
            } while ((v31 += 8, v32 -= 1, v30 != 1));
        }
        g_5be40 = v30;
    }
    shots_sim_step(v26);
    unmirror@opsave@opsave(v26);
    trench_sim_step(v26);
    sub_3b74c(v26);
    seconds_now(v26);
    v33 = g_5b5e8;
    v34 = v29;
    if (g_5b5e8 != g_5b5f0)
    {
        do
        {
            v4 = (4)[v33] * 0x100000000 | *((int *)v33);
            find@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5b730, &v5, &v4);
            v35 = *((long long *)&v5);
            if (v35 != g_5b738 && !(char)owns_object(*((long long *)(v35 + 24))))
            {
                v36 = *((int128_t *)v33);
                v37 = *((int128_t *)&(16)[v33]);
                v38 = *((int128_t *)&(32)[v33]);
                v39 = &(24)[*((long long *)$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KUHeld(&g_5b840, &v18, &v4))];
                v40 = *((int128_t *)&(48)[v33]);
                v41 = *((int128_t *)&(64)[v33]);
                v42 = *((int *)(v39 + 384));
                v65 = 0;
                *((unsigned long long *)&v65) = *((long long *)&(80)[v33]);
                if (!(unsigned int)v42)
                {
                    v46 = v42 * 96;
                    *((void*)(v46 + (char *)&v39->field_0)) = v36;
                    *((void*)(v46 + (char *)v39 + 16)) = v37;
                    *((void*)(v46 + (char *)v39 + 32)) = v38;
                    *((void*)(v46 + (char *)v39 + 48)) = v40;
                    *((void*)(v46 + (char *)v39 + 64)) = v41;
                    *((unsigned long long *)(v46 + (char *)v39 + 80)) = (unsigned long long)v65 CONCAT (unsigned long long)v34;
                    *((char **)(v39 + 384)) = &(1)[*((int *)(v39 + 384))];
                    continue;
                }
                v43 = (v42 - 1) * 96;
                tmp_11 = v43;
                v44 = v43 + (char *)v39;
                if (tmp_11 + (char *)v39)
                {
                    if (!(CmpF((unsigned long long)v65, 0) & 1) && (*((unsigned long long *)&v29) = *((long long *)(v44 + 88)), !((char)(CmpF((unsigned long long)v29, 0)) & 1)))
                    {
                        if (!(((CmpF((unsigned long long)v29, (unsigned long long)v65) & 69 | (CmpF((unsigned long long)v29, (unsigned long long)v65) & 69) >> 6) & 1) == 1))
                            continue;
                        if (!((char)((CmpF((unsigned long long)v65, (unsigned long long)v29) & 69) >> 2) & 1) && !(CmpF((unsigned long long)v65, (unsigned long long)v29) & 64))
                            goto LABEL_1c8a8;
                    }
                    else
                    {
                        tmp_9 = SubV(tmp_10, tmp_7);
                        if (!(CmpF(4571261708172110332, tmp_9) & 1))
                        {
LABEL_1c8a8:
                            *((void*)&v44[0]) = v36;
                            *((void*)(v44 + 16)) = v37;
                            *((void*)(v44 + 32)) = v38;
                            *((void*)(v44 + 48)) = v40;
                            *((void*)(v44 + 64)) = v41;
                            *((unsigned long long *)(v44 + 80)) = (unsigned long long)v65 CONCAT (unsigned long long)v34;
                            continue;
                        }
                    }
                }
                if ((unsigned int)v42 == 4)
                {
                    memmove(v39, v39 + 96, 288);
                    v42 = *((int *)(v39 + 384)) - 1;
                    *((unsigned int *)(v39 + 384)) = v42;
                    v46 = v42 * 96;
                    *((void*)(v46 + (char *)&v39->field_0)) = v36;
                    *((void*)(v46 + (char *)v39 + 16)) = v37;
                    *((void*)(v46 + (char *)v39 + 32)) = v38;
                    *((void*)(v46 + (char *)v39 + 48)) = v40;
                    *((void*)(v46 + (char *)v39 + 64)) = v41;
                    *((unsigned long long *)(v46 + (char *)v39 + 80)) = (unsigned long long)v65 CONCAT (unsigned long long)v34;
                    *((char **)(v39 + 384)) = &(1)[*((int *)(v39 + 384))];
                }
            }
        } while ((v31 += 8, v32 -= 1, v30 != 1));
        if (g_5b5e8 != g_5b5f0)
            g_5b5f0 = g_5b5e8;
    }
    if (g_5beb4 != 2 || !g_5be48)
        v30 = 0;
    else
        v30 = 1;
    v3 = v30;
    tmp_32 = MaxV(tmp_33, tmp_30);
    v47 = tmp_32;
    tmp_28 = SubV(tmp_30, tmp_29);
    g_5c088 = g_5beb8;
    v6 = &g_5b848->field_0;
    v149 = g_5b848->field_0;
    if (v149 == g_5b848)
        return security_cookie_check(v22 ^ &v0);
    while (true)
    {
        v49 = &(16)[v149];
        v50 = v49 + 8;
        find@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5b730, &v5, v49);
        v51 = *((long long *)&v5);
        if (!(v51 == g_5b738) && !((char)owns_object(*((long long *)(v51 + 24)))) && !(!((v52 = (long long)*((int *)(v50 + 384)), (unsigned int)v52))))
            break;
        v149 = *((long long *)v149);
        v30 = v3;
        if (v149 == v6)
            return security_cookie_check(v22 ^ &v0);
    }
    v53 = v52 * 96;
    v67 = v52 - 1;
    v54 = v53 - 8 + v50;
    if (!v30 || (CmpF(*(v54), 0) & 1))
    {
        v30 = 0;
        v63 = 0;
        v54 = v50 - 16 + v53;
        tmp_21 = SubV(tmp_23, tmp_22);
        v55 = tmp_21;
    }
    else
    {
        v30 = 1;
        v63 = 0;
        v55 = tmp_28;
    }
    v61 = v52 - 1;
    *((char **)&v4) = 0;
    *((unsigned long long *)&v65) = *((long long *)v54);
    v64 = 0;
    if ((unsigned int)v61 >= 1)
    {
        v62 = v61 * 96 - 8 + v50;
        do
        {
            if (!v30)
            {
                v56 = v62 - 8;
            }
            else
            {
                if ((((char)(CmpF(0, *((long long *)v62)) & 69) | (char)((CmpF(0, *((long long *)v62)) & 69) >> 6)) & 1) != 1)
                    break;
                v56 = v62;
            }
            v57 = 0;
            *((unsigned long long *)&v57) = *((long long *)v56);
            v58 = v61 * 96;
            if (!v30)
            {
                v65 = 0;
                *((unsigned long long *)&v65) = *((long long *)(&v50->paddin0[v58] + 80));
            }
            else
            {
                v65 = 0;
                *((unsigned long long *)&v65) = *((long long *)(&v50->paddin0[v58] + 88));
                *((char **)&v4) = 0;
                tmp_15 = SubV(tmp_17, tmp_16);
                if (!(CmpF(tmp_15, 4603579539098121011) & 1))
                {
                    tmp_9 = MaxV(tmp_10, tmp_7);
                    v60 = tmp_9;
                    *((char **)&v4) = 0;
                    tmp_6 = SubV(tmp_8, tmp_7);
                    v57 = tmp_6;
                }
            }
            if (!(CmpF((unsigned long long)v55, (unsigned long long)v57) & 1) || (unsigned int)v61 == 1)
            {
                v63 = v61;
                v4 = v61;
                v64 = v57;
                v65 = v65;
                break;
            }
        } while ((v31 += 8, v32 -= 1, v30 != 1));
    }
    if ((unsigned int)v63)
        v67 = v63;
    *((char **)&v2) = 1;
    v68 = &v50->paddin0[v67 * 96];
    if (v3 && !v30)
    {
        memcpy(&v16, v68 + 8, 12);
        memcpy(&v7, v68 + 20, 36);
        v2 = v30;
        v84 = 0;
        v84 = v7;
        v85 = 0;
        v85 = *((int *)&v8);
        v86 = 0;
        v86 = *((int *)&v9);
        v87 = v84 * v84 + v85 * v85 + v86 * v86;
        sqrtf((unsigned int)v87);
        if ((((char)(CmpF((unsigned int)v87, 953267991) & 69) | (char)((CmpF((unsigned int)v87, 953267991) & 69) >> 6)) & 1) == 1)
        {
            memcpy(&v7, v68 + 20, 36);
            v103 = *((long long *)(v51 + 24));
            v104 = g_5a268;
            if (g_5a268 >= 0)
            {
LABEL_1ce8d:
                if (!v104)
                {
                    tmp_29 = SubV(tmp_30, tmp_27);
                    v133 = tmp_29;
                    tmp_33 = SubV(tmp_34, tmp_31);
                    v65 = tmp_33;
                    tmp_42 = SubV(tmp_43, tmp_40);
                    v134 = tmp_42;
                    v135 = v133 * v133 + v65 * v65 + v134 * v134;
                    sqrtf((unsigned int)v135);
                }
            }
            else if (!GetEnvironmentVariableA("MP_TEST_TRACE", &v21, 8) || atoi(&v21) <= 0)
            {
                *((char **)&g_5a268) = 0;
                v104 = 0;
                goto LABEL_1ce8d;
            }
            else
            {
                *((char **)&g_5a268) = 1;
            }
            if (v30 && *((int *)(v50 + 384)) >= 2 && !strcmp(*((long long *)&(72)[*((long long *)(v51 + 24))]), "i_human"))
            {
                v105 = g_5c090;
                if (g_5c090 || (v106 = 0, *((unsigned int *)&v106) = *((int *)(v68 + 64)), v107 = 0, *((unsigned int *)&v107) = *((int *)(v68 + 56)), v108 = v107 * v107 + v106 * v106, (((char)(unsigned long long)(CmpF((unsigned long long)(unsigned int)v108, 0x40000000) & 69) | (char)((unsigned long long)(CmpF((unsigned long long)(unsigned int)v108, 0x40000000) & 69) >> 6)) & 1) == 1))
                {
                    v109 = g_5c098;
                }
                else
                {
                    v105 = *(v49);
                    v109 = &(&g_1f40)[g_5beb8];
                    g_5c090 = v105;
                    g_5c098 = v109;
                }
                if (*(v49) == v105 && g_5beb8 < v109)
                {
                    v110 = *((int *)(v50 + 384));
                    tmp_30 = SubV(tmp_32, tmp_31);
                    v111 = tmp_30;
                    v112 = v110 * 96;
                    v113 = &v50->paddin0[(v110 - 2) * 96];
                    *((unsigned long long *)&v64) = *((long long *)(&v50->paddin0[v112] - 16));
                    if ((((char)(CmpF((unsigned long long)v64, (unsigned long long)v111) & 69) | (char)((CmpF((unsigned long long)v64, (unsigned long long)v111) & 69) >> 6)) & 1) == 1 || (tmp_4 = SubV(tmp_6, tmp_5), v114 = tmp_4, !((((char)(unsigned long long)(CmpF((unsigned long long)v114, 4562254508917369340) & 69) | (char)((unsigned long long)(CmpF((unsigned long long)v114, 4562254508917369340) & 69) >> 6)) & 1) != 1) || !((((char)(unsigned long long)(CmpF(4603579539098121011, (unsigned long long)v114) & 69) | (char)((unsigned long long)(CmpF(4603579539098121011, (unsigned long long)v114) & 69) >> 6)) & 1) != 1)))
                    {
                        tmp_5 = MaxV(tmp_7, tmp_6);
                        v116 = tmp_5;
                        if ((((char)(CmpF(0x3fd0000000000000, (unsigned long long)v116) & 69) | (char)((CmpF(0x3fd0000000000000, (unsigned long long)v116) & 69) >> 6)) & 1) == 1)
                            v116 = 0 CONCAT 0x3fd0000000000000;
                        v122 = 0;
                        v123 = 0;
                        v124 = v50 - 88 + v112;
                        *((unsigned int *)&v123) = (unsigned long long)v116;
                        do
                        {
                            xmm0<16> = ((xmm1<16> * Conv(32->128, Load(addr=(rcx<8> + 0x30<64>), size=4, endness=Iend_LE))) + Conv(32->128, Load(addr=rcx<8>, size=4, endness=Iend_LE)))
                            v124 = &(4)[v124];
                            *((unsigned int *)&(&v18)[v122 * 4]) = (unsigned int)v127;
                            v122 = &(1)[v122];
                        } while (v122 < 3);
                    }
                    else
                    {
                        v115 = 0;
                        if ((CmpF(*((long long *)(v113 + 80)), (unsigned long long)v111) & 1))
                        {
                            tmp_5 = DivV(tmp_7, tmp_6);
                            *((unsigned int *)&v115) = tmp_5;
                        }
                        v118 = v112 - 88 + v50;
                        v120 = &v18;
                        v121 = 3;
                        do
                        {
                            v118 = &(4)[v118];
                            tmp_23 = SubV(tmp_24, tmp_21);
                            xmm0<16> = ((t23 * xmm3<16>) + Conv(32->128, Load(addr=(((r9<8> - stack_base-240) + rax<8>) + 0x8<64>), size=4, endness=Iend_LE)))
                            *((unsigned int *)v120) = (unsigned int)v126;
                            v120 = &v19;
                            v121 -= 1;
                        } while (true);
                    }
                    v128 = 0;
                    v128 = *((int *)&v20);
                    v129 = 0;
                    v129 = *((int *)&v17);
                    v130 = 0;
                    v130 = *((int *)&v16);
                    tmp_11 = DivV(tmp_12, tmp_9);
                    v1 = *((unsigned long long *)&v128);
                    mp_log("trace: battle %.3f shown %.3f real %.4f  by battle time %.3f %.3f  by arrival %.3f %.3f", tmp_11, (unsigned long long)v55, (unsigned long long)v34, *((unsigned long long *)&v130), *((unsigned long long *)&v129), (int)v18);
                }
            }
            v132 = v103 + 304;
            tmp_29 = SubV(tmp_30, tmp_27);
            v133 = tmp_29;
            tmp_33 = SubV(tmp_34, tmp_31);
            v65 = tmp_33;
            tmp_42 = SubV(tmp_43, tmp_40);
            v134 = tmp_42;
            v135 = v133 * v133 + v65 * v65 + v134 * v134;
            sqrtf((unsigned int)v135);
            v136 = 0;
            *((unsigned long long *)&v136) = (unsigned int)v135;
            g_5bea8 = (unsigned long long)((0 CONCAT g_5bea8) + v136);
            if ((((char)(CmpF((unsigned int)v135, g_5beb0) & 69) | (char)((CmpF((unsigned int)v135, g_5beb0) & 69) >> 6)) & 1) != 1)
                g_5beb0 = (unsigned int)v135;
            if ((((char)(CmpF((unsigned int)v135, 0x40400000) & 69) | (char)((CmpF((unsigned int)v135, 0x40400000) & 69) >> 6)) & 1) != 1)
                *((char **)&g_5bea4) = &(1)[g_5bea4];
            *((char **)&g_5be98) = &(1)[g_5be98];
            memcpy(v103 + 432, &v16, 12);
            v137 = &v7;
            v139 = v132;
            v140 = 3;
            while (true)
            {
                *((int *)((unsigned long long *)(18446744073709551552 + -0x1 * &v7) + v132 + v137)) = *((int *)v137);
                v141 = *((int *)v137);
                v137 = &v8;
                *((unsigned int *)v139) = v141;
                v139 = &(16)[v139];
                v140 -= 1;
            }
        }
        else
        {
            v88 = 0;
            do
            {
                tmp_13 = DivV(tmp_15, tmp_14);
                *((int128_t *)((char *)&v7 + v88 * 4)) = tmp_13;
                v88 = &(1)[v88];
            } while (v88 < 3);
            v90 = &v10;
            v91 = 3;
            while (true)
            {
                tmp_18 = SubV(tmp_20, tmp_19);
                *((int128_t *)v90) = tmp_18;
                v90 = &v11;
                v91 -= 1;
            }
        }
    }
    if ((unsigned int)v63 && (((char)(CmpF((unsigned long long)v65, (unsigned long long)v55) & 69) | (char)((CmpF((unsigned long long)v65, (unsigned long long)v55) & 69) >> 6)) & 1) != 1)
    {
        tmp_4 = SubV(tmp_6, tmp_5);
        v70 = tmp_4;
        if ((((char)(CmpF((unsigned long long)v70, 4562254508917369340) & 69) | (char)((CmpF((unsigned long long)v70, 4562254508917369340) & 69) >> 6)) & 1) != 1 && (((char)(CmpF(4603579539098121011, (unsigned long long)v70) & 69) | (char)((CmpF(4603579539098121011, (unsigned long long)v70) & 69) >> 6)) & 1) != 1)
        {
            v66 = 0;
            if ((CmpF((unsigned long long)v64, (unsigned long long)v55) & 1))
            {
                tmp_6 = DivV(tmp_8, tmp_7);
                *((unsigned int *)&v66) = tmp_6;
            }
            v74 = v68 + 8;
            v75 = 3;
            while (true)
            {
                tmp_20 = SubV(tmp_21, tmp_18);
                xmm0<16> = ((t20 * xmm9<16>) + Conv(32->128, Load(addr=(((((r15<8> * 0x60<64>) + r13<8>) - r14<8>) + rax<8>) - 0x60<64>), size=4, endness=Iend_LE)))
                *((unsigned int *)(&v15 - v68 + v74)) = (unsigned int)v79;
                v74 = &(4)[v74];
                v75 -= 1;
            }
        }
    }
    tmp_3 = SubV(tmp_5, tmp_4);
    v71 = tmp_3;
    if ((((char)(CmpF(0, (unsigned long long)v71) & 69) | (char)((CmpF(0, (unsigned long long)v71) & 69) >> 6)) & 1) != 1)
    {
        v73 = 0;
    }
    else
    {
        tmp_1 = MinV(tmp_3, tmp_2);
        v73 = tmp_1;
    }
    v76 = 3;
    v77 = v68 + 8;
    v2 = ((CmpF(0x3fd0000000000000, (unsigned long long)v71) & 69 | (CmpF(0x3fd0000000000000, (unsigned long long)v71) & 69) >> 6) & 1) == 0;
    while (true)
    {
        STORE(addr=((stack_base-264 - r14<8>) + rax<8>), data=Conv(128->32, (((0x0<96> CONCAT Conv(64->32, xmm1lq<8>)) * Conv(32->128, Load(addr=(rax<8> + 0x30<64>), size=4, endness=Iend_LE))) + Conv(32->128, Load(addr=rax<8>, size=4, endness=Iend_LE)))), size=4, endness=Iend_LE, guard=None)
        v77 = &(4)[v77];
        v76 -= 1;
    }
}


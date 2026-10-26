// v20 RVA 03af00..03b5ba  (1722 bytes)  R<lambda_4850e5e4fe7cad2db8d8b8fa5a62506f>
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_4 {
    struct struct_1 *field_0;
    unsigned int field_8;
    unsigned int field_c;
} struct_4;

typedef struct struct_0 {
    char field_0;
} struct_0;

typedef struct struct_1 {
    char paddin0[7520];
    unsigned int field_1d60;
} struct_1;

typedef struct struct_5 {
    char paddin0[20];
    uint128_t field_14;
    unsigned int field_24;
    char paddin40[12];
    unsigned int field_34;
    char paddin56[12];
    char field_44;
    char paddin69[15];
    uint128_t field_54;
    unsigned int field_64;
    char paddin104[12];
    unsigned int field_74;
    char paddin120[4];
    unsigned int field_7c;
} struct_5;


long long R(struct struct_2 **a0)
{
    char v0;  // [bp-0x598]
    int tmp_11;  // tmp #11
    int tmp_13;  // tmp #13
    int tmp_12;  // tmp #12
    char <0x3af00[is_33]|Stack bp-0x4c0, 1 B>;  // [bp-0x4c0]
    char <0x3af00[is_41]|Stack bp-0x480, 1 B>;  // [bp-0x480]
    char <0x3af00[is_43]|Stack bp-0x470, 1 B>;  // [bp-0x470]
    char v1;  // [bp-0x578]
    unsigned short v2;  // [bp-0x572]
    struct struct_2 **v3;  // [bp-0x570]
    unsigned long long v4;  // [bp-0x568]
    char v5;  // [bp-0x560]
    unsigned int v6;  // [bp-0x548]
    char v7;  // [bp-0x544]
    char v8;  // [bp-0x540]
    char v9;  // [bp-0x53c]
    char v10;  // [bp-0x538]
    char v11;  // [bp-0x534]
    char v12;  // [bp-0x530]
    char v13;  // [bp-0x52c]
    char v14;  // [bp-0x528]
    char v15;  // [bp-0x524]
    char v16;  // [bp-0x520]
    char v17;  // [bp-0x51c]
    char v18;  // [bp-0x518]
    char v19;  // [bp-0x517]
    char v20;  // [bp-0x516]
    char v21;  // [bp-0x514]
    char v22;  // [bp-0x508]
    char v23;  // [bp-0x4f8]
    char v24;  // [bp-0x4f4]
    char v25;  // [bp-0x4f0]
    char v26;  // [bp-0x4ec]
    char v27;  // [bp-0x4e8]
    char v28;  // [bp-0x4e4]
    char v29;  // [bp-0x4d8]
    int v30;  // [bp-0x4d0]
    int v32;  // [bp-0x4b0]
    int v33;  // [bp-0x4a0]
    int v34;  // [bp-0x490]
    unsigned long long v37;  // [bp-0x58]
    struct struct_2 **v39;  // rbx, Other Possible Types: unsigned long
    struct_4 *v40;  // rsi, Other Possible Types: unsigned long
    unsigned int v41;  // r15d
    unsigned long long v42;  // rcx
    unsigned long long v43;  // rbx
    struct_1 *v44;  // rbx
    struct_5 *v45;  // rbx
    unsigned long long v46;  // rax
    unsigned long long v47;  // rdx, Other Possible Types: unsigned long
    char v48;  // r12b
    unsigned int v49;  // ymm1
    unsigned int v50;  // ymm2
    int v51;  // xmm0
    unsigned int v52;  // ecx
    unsigned int v53;  // eax
    unsigned int v54;  // ymm1
    unsigned int v55;  // ymm2
    int v56;  // xmm0
    unsigned int v57;  // ecx
    unsigned int v58;  // eax
    unsigned int v59;  // ymm1
    unsigned int v60;  // ymm2
    int v61;  // xmm0
    unsigned int v62;  // ecx
    unsigned int v63;  // eax
    unsigned int v64;  // ymm1
    unsigned int v65;  // ymm2
    int v66;  // xmm0
    unsigned int v67;  // ecx
    unsigned int v68;  // eax
    unsigned int v69;  // ymm1
    unsigned int v70;  // ymm2
    int v71;  // xmm0
    unsigned int v72;  // ecx
    unsigned int v73;  // eax
    unsigned int v74;  // ymm1
    unsigned int v75;  // ymm2
    int v76;  // xmm0
    unsigned int v77;  // ecx
    unsigned int v78;  // eax
    unsigned int v79;  // ymm1
    unsigned int v80;  // ymm2
    int v81;  // xmm0
    unsigned int v82;  // ecx
    unsigned int v83;  // eax
    unsigned int v84;  // ymm1
    unsigned int v85;  // ymm2
    int v86;  // xmm0
    unsigned int v87;  // ecx
    unsigned int v88;  // eax
    unsigned int v89;  // ymm1
    unsigned int v90;  // ymm2
    int v91;  // xmm0
    unsigned int v92;  // ecx
    unsigned int v93;  // eax
    unsigned int v94;  // ymm1
    unsigned int v95;  // ymm2
    int v96;  // xmm0
    unsigned int v97;  // ecx
    unsigned int v98;  // eax
    unsigned int v99;  // ymm1
    unsigned int v100;  // ymm2
    int v101;  // xmm0
    unsigned int v102;  // ecx
    unsigned int v103;  // eax
    unsigned long long v107;  // rax
    unsigned long long v109;  // r14
    struct_0 *v110;  // rcx
    unsigned long long v111;  // rdx

    v37 = g_5a380 ^ &v0;
    v39 = a0;
    v3 = a0;
    if (!(char)is_object())
        return security_cookie_check(v37 ^ &v0);
    v40 = g_5d890;
    v41 = GetTickCount();
    if (g_5d890 == g_5d898)
        return security_cookie_check(v37 ^ &v0);
    do
    {
        if ((char)readable(*((long long *)v40), &g_1d68))
        {
            v42 = *((int *)&(12)[v40]);
            if (*((int *)&(&g_1d60)[*((long long *)v40)]) == (unsigned int)v42)
            {
                v43 = v42 * 320 + *((long long *)(g_5d8f0 + 320));
                if ((unsigned int)v42 >= 100000 || !(char)readable(v43, 320) || !v43)
                {
                    v39 = v3;
                }
                else
                {
                    memset(&v7, 0, 100);
                    v6 = *((int *)&(8)[v40]);
                    sub_3c0d0(v43, &v6);
                    v44 = *((long long *)v40);
                    memcpy(&v23, v44 + &g_1e70, 4);
                    memcpy(&v24, v44 + __Eg_map_sidecar, 4);
                    memcpy(&v25, v44 + &g_1214, 4);
                    memcpy(&v26, v44 + &g_1218, 4);
                    memcpy(&v27, v44 + &g_1eac, 4);
                    memcpy(&v28, v44 + &g_1ef8, 4);
                    v45 = *((long long *)$_Try_emplace@AEBI$$V@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std$_Try_emplace@AEBI$$V@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5d7d0, &v5, &(8)[v40]));
                    if (*((int *)&v7) != *((int *)(v45 + 24)) || *((int *)&v8) != *((int *)(v45 + 28)) || *((int *)&v9) != *((int *)(v45 + 32)) || *((int *)&v10) != *((int *)(v45 + 36)) || *((int *)&v11) != *((int *)(v45 + 40)) || v18 != *((char *)(v45 + 68)) || v19 != *((char *)(v45 + 69)) || v20 != *((char *)(v45 + 70)) || *((int *)&v23) != *((int *)(v45 + 100)) || !(((unsigned int)v46 = memcmp(&v21, v45 + 72, 28), !(unsigned int)v46)))
                    {
                        v47 = 1;
                        v48 = 1;
                    }
                    else
                    {
                        v48 = 0;
                        v47 = &(1)[v46];
                    }
                    v49 = *((int *)&v27);
                    v50 = *((int *)(v45 + 116));
                    tmp_11 = SubV(tmp_13, tmp_12);
                    v51 = tmp_11;
                    if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                    {
                        if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                            v52 = 0;
                        else
                            v52 = v47;
                        if (((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                            v53 = v47;
                        else
                            v53 = 0;
                        if (v52 == v53)
                        {
                            v54 = *((int *)&v28);
                            v55 = *((int *)(v45 + 120));
                            tmp_11 = SubV(tmp_13, tmp_12);
                            v56 = tmp_11;
                            if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                            {
                                if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                    v57 = 0;
                                else
                                    v57 = v47;
                                if (!((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                    v58 = 0;
                                else
                                    v58 = v47;
                                if (v57 == v58)
                                {
                                    v59 = *((int *)&v24);
                                    v60 = *((int *)(v45 + 104));
                                    tmp_11 = SubV(tmp_13, tmp_12);
                                    v61 = tmp_11;
                                    if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                    {
                                        if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                            v62 = 0;
                                        else
                                            v62 = v47;
                                        if (!((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                            v63 = 0;
                                        else
                                            v63 = v47;
                                        if (v62 == v63)
                                        {
                                            v64 = *((int *)&v25);
                                            v65 = *((int *)(v45 + 108));
                                            tmp_11 = SubV(tmp_13, tmp_12);
                                            v66 = tmp_11;
                                            if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                            {
                                                if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                    v67 = 0;
                                                else
                                                    v67 = v47;
                                                if (!((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                    v68 = 0;
                                                else
                                                    v68 = v47;
                                                if (v67 == v68)
                                                {
                                                    v69 = *((int *)&v26);
                                                    v70 = *((int *)(v45 + 112));
                                                    tmp_11 = SubV(tmp_13, tmp_12);
                                                    v71 = tmp_11;
                                                    if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                                    {
                                                        if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                            v72 = 0;
                                                        else
                                                            v72 = v47;
                                                        if (!((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                            v73 = 0;
                                                        else
                                                            v73 = v47;
                                                        if (v72 == v73)
                                                        {
                                                            v74 = *((int *)&v12);
                                                            v75 = *((int *)(v45 + 44));
                                                            tmp_11 = SubV(tmp_13, tmp_12);
                                                            v76 = tmp_11;
                                                            if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                                            {
                                                                if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                                    v77 = 0;
                                                                else
                                                                    v77 = v47;
                                                                if (((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                                    v78 = v47;
                                                                else
                                                                    v78 = 0;
                                                                if (v77 == v78)
                                                                {
                                                                    v79 = *((int *)&v13);
                                                                    v80 = *((int *)(v45 + 48));
                                                                    tmp_11 = SubV(tmp_13, tmp_12);
                                                                    v81 = tmp_11;
                                                                    if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                                                    {
                                                                        if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                                            v82 = 0;
                                                                        else
                                                                            v82 = v47;
                                                                        if (!((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                                            v83 = 0;
                                                                        else
                                                                            v83 = v47;
                                                                        if (v82 == v83)
                                                                        {
                                                                            v84 = *((int *)&v14);
                                                                            v85 = *((int *)(v45 + 52));
                                                                            tmp_11 = SubV(tmp_13, tmp_12);
                                                                            v86 = tmp_11;
                                                                            if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                                                            {
                                                                                if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                                                    v87 = 0;
                                                                                else
                                                                                    v87 = v47;
                                                                                if (((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                                                    v88 = v47;
                                                                                else
                                                                                    v88 = 0;
                                                                                if (v87 == v88)
                                                                                {
                                                                                    v89 = *((int *)&v15);
                                                                                    v90 = *((int *)(v45 + 56));
                                                                                    tmp_11 = SubV(tmp_13, tmp_12);
                                                                                    v91 = tmp_11;
                                                                                    if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                                                                    {
                                                                                        if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                                                            v92 = 0;
                                                                                        else
                                                                                            v92 = v47;
                                                                                        if (((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                                                            v93 = v47;
                                                                                        else
                                                                                            v93 = 0;
                                                                                        if (v92 == v93)
                                                                                        {
                                                                                            v94 = *((int *)&v16);
                                                                                            v95 = *((int *)(v45 + 60));
                                                                                            tmp_11 = SubV(tmp_13, tmp_12);
                                                                                            v96 = tmp_11;
                                                                                            if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                                                                            {
                                                                                                if (((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                                                                    v97 = v47;
                                                                                                else
                                                                                                    v97 = 0;
                                                                                                if (((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                                                                    v98 = v47;
                                                                                                else
                                                                                                    v98 = 0;
                                                                                                if (v97 == v98)
                                                                                                {
                                                                                                    v99 = *((int *)&v17);
                                                                                                    v100 = *((int *)(v45 + 64));
                                                                                                    tmp_11 = SubV(tmp_13, tmp_12);
                                                                                                    v101 = tmp_11;
                                                                                                    if ((((char)(CmpF((unsigned int)v51, 1022739087) & 69) | (char)((CmpF((unsigned int)v51, 1022739087) & 69) >> 6)) & 1) == 1 && (((char)(CmpF(3170222735, (unsigned int)v51) & 69) | (char)((CmpF(3170222735, (unsigned int)v51) & 69) >> 6)) & 1) == 1)
                                                                                                    {
                                                                                                        if (!((char)((CmpF(v99, v99) & 69) >> 2) & 1))
                                                                                                            v102 = 0;
                                                                                                        else
                                                                                                            v102 = v47;
                                                                                                        if (!((char)((CmpF(v100, v100) & 69) >> 2) & 1))
                                                                                                            v103 = 0;
                                                                                                        else
                                                                                                            v103 = v47;
                                                                                                        if (v102 == v103)
                                                                                                            memcmp(v45 + 20, &v6, 104);
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (v48 || (char)v47 && v41 - *((int *)(v45 + 124)) >= 140 || memcmp(v45 + 20, &v6, 104) && v41 - *((int *)(v45 + 124)) >= 1000 || !((v107 = (unsigned long long)((*((int *)&(8)[v40]) & 31) + 75), (unsigned int)(unsigned long long)(v41 - *((int *)(v45 + 124))) < (unsigned int)(v107 + (v107 << 2) << 3))))
                    {
                        g_5daa8 = &(1)[g_5daa8];
                        *((int128_t *)(v45 + 20)) = *((int128_t *)&v6);
                        v109 = 0 + 1;
                        *((int128_t *)(v45 + 36)) = *((int128_t *)&v10);
                        *((int128_t *)(v45 + 52)) = *((int128_t *)&v14);
                        *((int128_t *)(v45 + 68)) = *((int128_t *)&v18);
                        *((int128_t *)(v45 + 84)) = *((int128_t *)&v22);
                        *((int128_t *)(v45 + 100)) = *((int128_t *)&v23);
                        *((long long *)(v45 + 116)) = *((long long *)&v27);
                        *((unsigned int *)(v45 + 124)) = v41;
                        v110 = 0 * 104;
                        *((int128_t *)(&v30 + v110)) = *((int128_t *)&v6);
                        *((int128_t *)(&<0x3af00[is_33]|Stack bp-0x4c0, 1 B> + v110)) = *((int128_t *)&v10);
                        *((int128_t *)(&v32 + v110)) = *((int128_t *)&v14);
                        *((int128_t *)(&v33 + v110)) = *((int128_t *)&v18);
                        *((int128_t *)(&v34 + v110)) = *((int128_t *)&v22);
                        *((int128_t *)(&<0x3af00[is_41]|Stack bp-0x480, 1 B> + v110)) = *((int128_t *)&v23);
                        *((long long *)(&<0x3af00[is_43]|Stack bp-0x470, 1 B> + v110)) = *((long long *)&v27);
                        if ((unsigned int)v109 == 11)
                        {
                            strncpy(&v1, "GTMP\x0c", 5);
                            v2 = v109;
                            v4 = *((long long *)&v1);
                            memcpy(&v29, &v4, v109 - 3);
                            v39 = v3;
                            v111 = 1152;
                            v39(&v29, 1152);
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        v39 = v3;
                    }
                }
            }
        }
    } while ((v40 += 16, v40 != g_5d898));
    if (!(unsigned int)0)
        return security_cookie_check(v37 ^ &v0);
    strncpy(&v1, "GTMP\x0c", 5);
    *((char **)&v2) = 0;
    v4 = *((long long *)&v1);
    memcpy(&v29, &v4, 8);
    v39(&v29, &(8)[(unsigned int)0 * 104]);
    return security_cookie_check(v37 ^ &v0);
}


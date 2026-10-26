// v20 RVA 0284d0..028ecf  (2559 bytes)  poll@Joiner
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char field_0;
    char paddin1[7];
    unsigned long long field_8;
    unsigned int field_10;
    char paddin20[288];
    char field_134;
    char paddin309[3];
    unsigned int field_138;
    unsigned int field_13c;
    unsigned int field_140;
    char field_144;
    char paddin325[24];
    char field_15d;
    char paddin350[22];
    char field_174;
    char field_175;
    char field_176;
    char field_177;
    char field_178;
    char paddin377[3];
    unsigned int field_17c;
    char field_180;
    char field_181;
    char paddin386[63];
    char field_1c1;
    char paddin450[2];
    unsigned int field_1c4;
    unsigned int field_1c8;
    char paddin460[147];
    char field_25f;
    unsigned int field_260;
    unsigned int field_264;
    unsigned long long field_268;
    char paddin624[4];
    unsigned int field_274;
    char paddin632[8];
    unsigned long long field_280;
    unsigned long long field_288;
    char paddin656[8];
    struct struct_2 *field_298;
    unsigned long long field_2a0;
    char paddin680[8];
    struct struct_3 *field_2b0;
    char paddin696[16];
    unsigned int field_2c8;
    unsigned int field_2cc;
    unsigned int field_2d0;
    unsigned int field_2d4;
    unsigned int field_2d8;
    char paddin732[4];
    unsigned int field_2e0;
    char paddin740[12];
    unsigned int field_2f0;
    unsigned int field_2f4;
    unsigned int field_2f8;
    unsigned int field_2fc;
    unsigned int field_300;
    unsigned int field_304;
    unsigned int field_308;
    unsigned int field_30c;
    unsigned int field_310;
} struct_1;

typedef struct sockaddr {
    unsigned short sin_family;
    char sa_data[14];
} sockaddr;

typedef struct struct_0 {
    char field_0;
} struct_0;

typedef struct struct_5 {
    struct struct_2 *field_0;
    unsigned long long field_8;
    unsigned long long field_10;
} struct_5;

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_3 {
    unsigned int field_0;
} struct_3;


long long poll@Joiner(struct_1 *a0)
{
    char v0;  // [bp-0xcf8]
    int tmp_5;  // tmp #5
    char v1;  // [bp-0xcc8]
    unsigned int v2;  // [bp-0xcc4]
    void* v3;  // [bp-0xcc0], Other Possible Types: char, unsigned int
    char v4;  // [bp-0xcbc]
    char v5;  // [bp-0xcbb]
    unsigned short v6;  // [bp-0xcba]
    unsigned int v7;  // [bp-0xcb8]
    char v8;  // [bp-0xca8]
    unsigned int v9;  // [bp-0xca4]
    char v10;  // [bp-0xca0]
    unsigned int v11;  // [bp-0xc94]
    unsigned int v12;  // [bp-0xc90]
    unsigned short v13;  // [bp-0xc8c]
    char v14;  // [bp-0xc8a]
    char v15;  // [bp-0xc89]
    char v16;  // [bp-0xc88]
    unsigned int v17;  // [bp-0xc60]
    unsigned int v18;  // [bp-0xc5c]
    unsigned int v19;  // [bp-0xc58]
    sockaddr v20;  // [bp-0xc48]
    char v21;  // [bp-0xc44]
    char v22;  // [bp-0xc38]
    unsigned int v23;  // [bp-0xc34]
    unsigned int v24;  // [bp-0xc30]
    unsigned int v25;  // [bp-0xc2c]
    char v26;  // [bp-0xc28]
    char v27;  // [bp-0x838]
    char v28;  // [bp-0x834]
    unsigned long v29;  // [bp-0x830]
    char v30;  // [bp-0x82c]
    char v31;  // [bp-0x824]
    char v32;  // [bp-0x820]
    char v33;  // [bp-0x81c]
    char v34;  // [bp-0x818]
    char v35;  // [bp-0x814]
    char v36;  // [bp-0x810]
    char v37;  // [bp-0x80f]
    char v40;  // [bp-0x80c]
    char v41;  // [bp-0x7f4]
    char v42;  // [bp-0x7ec]
    char v43;  // [bp-0x7e8]
    char v44;  // [bp-0x788]
    char v45;  // [bp-0x784]
    char v46;  // [bp-0x744]
    char v47;  // [bp-0x740]
    char v48;  // [bp-0x73c]
    char v49;  // [bp-0x738]
    char v50;  // [bp-0x734]
    char v51;  // [bp-0x730]
    char v52;  // [bp-0x72c]
    char v53;  // [bp-0x728]
    char v54;  // [bp-0x724]
    unsigned long long v55;  // [bp-0x38]
    unsigned int v57;  // eax
    struct_1 *v58;  // rdi
    unsigned long long v59;  // rax
    unsigned int v60;  // r15d
    void* v62;  // rdx
    unsigned long long v63;  // r8
    unsigned int v64;  // r9d
    unsigned int v65;  // r14d
    unsigned long v66;  // rbx
    char *v67;  // rsi
    unsigned long long v68;  // rcx
    unsigned long long v69;  // r8
    unsigned int v70;  // r9d
    char *v71;  // rdx, Other Possible Types: unsigned long
    unsigned int v72;  // r9d
    unsigned long long v73;  // rdx, Other Possible Types: unsigned long
    unsigned long long v74;  // cc_op
    unsigned long long v75;  // cc_dep1
    unsigned long long v76;  // cc_dep2
    unsigned long long v77;  // cc_op
    unsigned long long v78;  // cc_dep1
    unsigned long long v79;  // cc_dep2
    unsigned long long v80;  // r8
    char v81;  // al
    unsigned long long v82;  // rbx
    unsigned long long v83;  // rsi
    unsigned long long v84;  // rcx
    unsigned long long v85;  // rbx
    unsigned long long v86;  // r14
    char v87;  // al
    unsigned long long v88;  // rdx
    unsigned long long v89;  // rbx, Other Possible Types: unsigned long
    struct_0 *v90;  // rcx
    unsigned int v91;  // ecx
    unsigned int v92;  // esi
    unsigned long long v93;  // r8
    char *v94;  // r8
    unsigned long v95;  // rcx
    unsigned int v96;  // r9d
    unsigned long long v97;  // r9
    unsigned long long v98;  // r10
    char *v99;  // r11
    unsigned int v100;  // eax
    unsigned long long v101;  // rcx
    unsigned long long v102;  // r8
    struct_0 *v103;  // rdx
    unsigned long long v104;  // rbx
    unsigned int v105;  // eax
    unsigned long long v106[2];  // rbx
    unsigned long long v108[2];  // rbx
    struct_5 *v109;  // r9
    unsigned int v110;  // edx
    char *v111;  // r10
    unsigned long v112;  // rcx
    unsigned long long v113;  // r8
    unsigned int v115;  // eax
    unsigned int v116;  // eax
    unsigned int *v117;  // rcx
    unsigned long long v118[3];  // r8
    unsigned int *v119;  // rcx
    unsigned long long v120;  // rcx
    unsigned long long v121;  // rax
    unsigned long long v122;  // rdx
    unsigned long long v123;  // rax
    unsigned long long v124;  // rdx
    unsigned long long v125[3];  // rdx
    unsigned int v126;  // edx
    unsigned long long v127;  // rcx
    unsigned long long v128;  // rax
    unsigned long long v129;  // rcx
    unsigned long long v130;  // rax
    unsigned long long v131;  // rcx
    unsigned long long v132[3];  // rdx
    char *v133;  // rcx
    unsigned long long v134;  // rax
    unsigned long long v135;  // rcx
    unsigned long long v136;  // rax
    unsigned long long v137;  // rcx
    unsigned int *v138;  // rcx

    v55 = g_5a380 ^ &v0;
    v57 = *((int *)(a0 + 16));
    v58 = a0;
    if (v57 != 1 && v57 - 2 > 1)
        return security_cookie_check(v55 ^ &v0);
    (unsigned int)v59 = GetTickCount();
    v60 = v59;
    if (*((int *)(a0 + 16)) == 3)
    {
LABEL_28637:
        v71 = *((long long *)(v58 + 664));
        v65 = 0;
        v66 = 0;
        if (*((long long *)(v58 + 672)) != v71)
        {
            while (v65 < 24)
            {
                v67 = v66;
                if (!*((char *)(v71 + v66)))
                {
                    v68 = (unsigned int)v59 - *((int *)(*((long long *)(v58 + 688)) + v66 * 4));
                    if ((unsigned int)v68 >= 300)
                    {
                        strncpy(&v3, "GTMP", 4);
                        *((char **)&v4) = 4;
                        *((char **)&v6) = 0;
                        v7 = v66;
                        if (*((char *)(v58 + 384)))
                            *((char **)&v5) = 1;
                        else
                            *((unsigned long *)&v5) = -(0 < *((char *)(v58 + 385))) & 2;
                        v69 = 12;
                        if (v58->field_0)
                        {
                            g_5c118(*((long long *)(v58 + 8)));
                        }
                        else
                        {
                            v70 = 0;
                            sendto(*((long long *)(v58 + 616)), &v3, 12, 0, v58 + 624, 16);
                        }
                        v65 = &(1)[v65];
                        *((unsigned int *)(*((long long *)(v58 + 688)) + v67 * 4)) = v59;
                    }
                }
                v71 = *((long long *)(v58 + 664));
                v66 = &(1)[v66];
                if (v66 >= *((long long *)(v58 + 672)) - v71)
                    break;
            }
        }
    }
    else
    {
        if ((unsigned int)v59 - *((int *)(a0 + 720)) >= 500)
        {
            v62 = 0;
            strncpy(&v8, "GTMP", 4);
            *((char **)&v9) = 1;
            memset(&v10, 0, 76);
            memcpy(&v10, "GTMF_TA_A1", 12);
            v12 = *((int *)(v58 + 312));
            v14 = *((char *)(v58 + 373));
            v15 = *((char *)(v58 + 374));
            *((char **)&v11) = 20;
            *((char **)&v13) = &g_ff00;
            strncpy_s(&v16, 24, v58 + 325, 23);
            v63 = 84;
            v17 = *((int *)(v58 + 736)) - *((int *)(v58 + 728));
            v18 = *((int *)(v58 + 752));
            v19 = *((int *)(v58 + 772));
            if (!v58->field_0)
            {
                v64 = 0;
                sendto(*((long long *)(v58 + 616)), &v8, 84, 0, v58 + 624, 16);
            }
            else
            {
                g_5c118(*((long long *)(v58 + 8)));
            }
            *((unsigned int *)(v58 + 720)) = v59;
        }
        if (*((int *)(v58 + 16)) == 3)
            goto LABEL_28637;
    }
    do
    {
        *((char **)&v2) = 16;
        if (v58->field_0)
        {
            v3 = 0;
            v73 = g_5c120(&v3);
            if ((unsigned int)v73 <= 0)
                break;
            v74 = 8;
            v75 = v3;
            v76 = *((long long *)(v58 + 8));
        }
        else
        {
            v72 = 0;
            v73 = recvfrom(*((long long *)(v58 + 616)), &v27, 0x800, 0, &v20, &v2);
            if ((unsigned int)v73 <= 0)
                break;
            v77 = 7;
            v78 = *((int *)&v21);
            v79 = *((int *)(v58 + 628));
        }
        if ((char)[D] amd64g_calculate_condition(0x4<64>, cc_op<8>, cc_dep1<8>, cc_dep2<8>, cc_ndep<8>) && (unsigned int)v73 >= 8 && (v80 = 1347245127, *((int *)&v27) == 1347245127))
        {
            v81 = *((long long *)&v28);
            if (v81 != 2)
            {
                if (v81 != 14)
                {
                    if (v81 == 5 && *((int *)(v58 + 16)) == 3 && (unsigned int)v73 >= 16)
                    {
                        if (!*((char *)(v58 + 384)))
                            v91 = -(0 < *((char *)(v58 + 385))) & 2;
                        else
                            v91 = 1;
                        if ((&v28)[1] == v91)
                        {
                            v94 = *((long long *)(v58 + 664));
                            v95 = *((int *)&(&v28)[4]);
                            if (v95 < *((long long *)(v58 + 672)) - v94 && !v94[v95])
                            {
                                v97 = *((long long *)&v30);
                                if ((unsigned int)v97 <= 1000)
                                {
                                    v98 = *((long long *)(v58 + 640));
                                    v99 = v95 * 1000;
                                    if (&v99[v97] <= *((long long *)(v58 + 648)) - v98 && (unsigned int)v73 >= (unsigned int)&(16)[v97])
                                    {
                                        memcpy(&v99[v98], &v30);
                                        *((char **)(*((int *)&(&v28)[4]) + *((long long *)(v58 + 664)))) = 1;
                                        *((char **)(v58 + 716)) = &(1)[*((int *)(v58 + 716))];
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ((unsigned int)v73 >= 12)
                    {
                        v82 = *((long long *)(v58 + 736));
                        v83 = *((long long *)(v58 + 728));
                        if (v83 != v82)
                        {
                            v84 = v29;
                            v85 = v82 - v83;
                            v86 = v84 * 1000;
                            if (v86 < v85)
                            {
                                strncpy(&v22, "GTMP", 4);
                                v24 = v84;
                                *((char **)&v23) = 15;
                                memset(&v26, 0, 1000);
                                v88 = v86 + v83;
                                v89 = v85 - v86;
                                v90 = &v26;
                                if (v89 > 1000)
                                    v89 = 1000;
                                v93 = v89;
                                v25 = v89;
                                memcpy();
                                if (!v58->field_0)
                                {
                                    v96 = 0;
                                    sendto(*((long long *)(v58 + 616)), &v22, &(16)[v25], 0, v58 + 624, 16);
                                }
                                else
                                {
                                    g_5c118(*((long long *)(v58 + 8)));
                                }
                            }
                        }
                    }
                }
            }
            if (!(*((int *)&v31) == 20) || !((unsigned int)v73 >= 280))
            {
LABEL_28c86:
                strcpy_s(v58 + 20, 160, "The host runs a different version of the multiplayer files.");
            }
            *((unsigned int *)(v58 + 380)) = v60;
            *((char *)(v58 + 375)) = (&v36)[3] != 0;
            *((char *)(v58 + 376)) = v36 != 0;
            memcpy(v58 + 349, &v40, 24);
            *((char *)(v58 + 308)) = v37;
            *((int *)(v58 + 316)) = *((int *)&v34);
            *((int *)(v58 + 320)) = *((int *)&v35);
            *((char *)(v58 + 324)) = (&v36)[2] != 0;
            *((int *)(v58 + 756)) = *((int *)&v48);
            *((int *)(v58 + 760)) = *((int *)&v49);
            *((int *)(v58 + 764)) = *((int *)&v50);
            *((int *)(v58 + 768)) = *((int *)&v51);
            *((int *)(v58 + 776)) = *((int *)&v52);
            *((int *)(v58 + 780)) = *((int *)&v53);
            *((int *)(v58 + 784)) = *((int *)&v54);
            v87 = *((long long *)&v36);
            *((char **)(v58 + 372)) = 0;
            if (v87 || (v92 = (int)*((long long *)&v41), !v92))
            {
                v92 = *((int *)&v32);
                *((char *)&v65) = 0;
                if (!v87)
                    *((char **)(v58 + 16)) = 2;
            }
            else
            {
                *((char *)&v65) = 1;
            }
            if (!v92 || !*((char *)(v58 + 373)))
            {
                *((char **)(v58 + 16)) = 2;
            }
            else if (v92 > 0x4000000)
            {
                strcpy_s(v58 + 20, 160, "The host sent an invalid battle.");
            }
            else
            {
                v100 = *((int *)&v46);
                v101 = v58 + 386;
                *((char *)(v58 + 384)) = v65;
                v102 = 64;
                *((char **)(v58 + 385)) = 0;
                if ((char)v65)
                    v100 = 0;
                v103 = &v45;
                *((unsigned int *)(v58 + 452)) = v100;
                *((int *)(v58 + 456)) = *((int *)&v47);
                memcpy();
                *((char **)(v58 + 449)) = 0;
                if (*((int *)(v58 + 452)) > 0x4000000)
                {
                    goto LABEL_28c86;
                }
                else if (!(char)v65)
                {
                    *((char **)&v1) = 0;
                    assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(v58 + 640, v92, &v1);
                    v105 = *((int *)&v33);
                    *((char **)&v1) = 0;
                    v106 = v58 + 664;
                    if ((char)v65)
                        v105 = *((int *)&(&v41)[4]);
                    *((unsigned int *)(v58 + 712)) = v105;
                    assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(v106, &(999)[v92] / 1000, &v1);
                    *((char **)&v3) = 0;
                    assign@?$vector@KV?$allocator@K@std(v58 + 688, *((long long *)(v58 + 672)) - v106[0], &v3);
                    *((char **)(v58 + 716)) = 0;
                    *((char **)(v58 + 16)) = 3;
                }
                else
                {
                    v104 = v58 + 512;
                    memcpy(v104, &v43, 96);
                    *((int *)(v58 + 608)) = *((int *)&v42);
                    *((unsigned int *)(v58 + 612)) = *((int *)&v44) >> 8;
                    *((char **)(v58 + 607)) = 0;
                    if ((char)safe_file_name@opsave(v104))
                    {
                        *((char **)&v1) = 0;
                        assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(v58 + 640, v92, &v1);
                    }
                    strcpy_s(v58 + 20, 160, "The host sent an invalid operation save.");
                    goto LABEL_28c5f;
                }
            }
        }
    } while (true);
    if (*((int *)(v58 + 16)) == 2)
    {
        if (v60 - *((int *)(v58 + 380)) <= 6000)
            return security_cookie_check(v55 ^ &v0);
        *((char **)(v58 + 16)) = 1;
        *((char **)(v58 + 349)) = 0;
        *((unsigned int *)(v58 + 724)) = v60;
    }
    if (*((int *)(v58 + 16)) != 3)
        return security_cookie_check(v55 ^ &v0);
    v108 = v58 + 664;
    if (*((int *)(v58 + 716)) != *((long long *)(v108 + 8)) - v108[0])
        return security_cookie_check(v55 ^ &v0);
    v109 = v58 + 640;
    v110 = 2166136261;
    v111 = v109->field_0;
    v112 = 0;
    v113 = *((long long *)(v109 + 8));
    tmp_5 = v113;
    if (tmp_5 != v111)
    {
        do
        {
            v115 = v111[v112];
            v112 = &(1)[v112];
            v116 = v115 ^ v110;
            v110 = v116 * 16777619;
        } while (v112 < v113 - v111);
    }
    if (v110 != *((int *)(v58 + 712)))
    {
        v117 = *((long long *)(v58 + 616));
        if (v117 != -1)
            closesocket(v117);
        *((long long *)(v58 + 616)) = -1;
        strcpy_s(v58 + 20, 160, "The battle arrived damaged. Try again.");
LABEL_28c5f:
        v119 = *((long long *)(v58 + 616));
        if (v119 != -1)
            closesocket(v119);
        *((char **)(v58 + 16)) = 5;
        *((long long *)(v58 + 616)) = -1;
        return security_cookie_check(v55 ^ &v0);
    }
    if (!*((char *)(v58 + 384)))
    {
        if (!(*((char *)(v58 + 385)) == 0))
            goto LABEL_28e07;
        if (*((int *)(v58 + 452)))
        {
            v118 = v58 + 464;
            if (v118 != v109)
            {
                v120 = v118[0];
                v118[0] = v111;
                v121 = *((long long *)(v109 + 8));
                v109->field_0 = v120;
                v122 = *((long long *)(v118 + 8));
                *((unsigned long long *)(v118 + 8)) = v121;
                v123 = *((long long *)(v109 + 16));
                *((unsigned long long *)(v109 + 8)) = v122;
                v124 = *((long long *)(v118 + 16));
                *((unsigned long long *)(v118 + 16)) = v123;
                *((unsigned long long *)(v109 + 16)) = v124;
            }
            v126 = *((int *)(v58 + 452));
            *((char **)(v58 + 385)) = 1;
            *((char **)&v1) = 0;
            assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(v109, v126, &v1);
            *((int *)(v58 + 712)) = *((int *)(v58 + 456));
            *((char **)&v1) = 0;
            assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(v108, &(999)[*((int *)(v58 + 452))] / 1000, &v1);
            *((char **)&v3) = 0;
            assign@?$vector@KV?$allocator@K@std(v58 + 688, *((long long *)(v108 + 8)) - v108[0], &v3);
            *((char **)(v58 + 716)) = 0;
            return security_cookie_check(v55 ^ &v0);
        }
    }
    if (!*((char *)(v58 + 385)))
        goto LABEL_28e7b;
LABEL_28e07:
    v125 = v58 + 488;
    if (v125 != v109)
    {
        v127 = v125[0];
        v125[0] = v111;
        v128 = *((long long *)(v109 + 8));
        v109->field_0 = v127;
        v129 = *((long long *)(v125 + 8));
        *((unsigned long long *)(v125 + 8)) = v128;
        v130 = *((long long *)(v109 + 16));
        *((unsigned long long *)(v109 + 8)) = v129;
        v131 = *((long long *)(v125 + 16));
        *((unsigned long long *)(v125 + 16)) = v130;
        *((unsigned long long *)(v109 + 16)) = v131;
    }
    v132 = v58 + 464;
    if (v109 != v132)
    {
        v133 = v109->field_0;
        v109->field_0 = v132[0];
        v134 = *((long long *)(v132 + 8));
        v132[0] = v133;
        v135 = *((long long *)(v109 + 8));
        *((unsigned long long *)(v109 + 8)) = v134;
        v136 = *((long long *)(v132 + 16));
        *((unsigned long long *)(v132 + 8)) = v135;
        v137 = *((long long *)(v109 + 16));
        *((unsigned long long *)(v109 + 16)) = v136;
        *((unsigned long long *)(v132 + 16)) = v137;
    }
    *((char **)(v58 + 385)) = 0;
LABEL_28e7b:
    v138 = *((long long *)(v58 + 616));
    if (v138 != -1)
        closesocket(v138);
    *((char **)(v58 + 16)) = 4;
    *((long long *)(v58 + 616)) = -1;
    return security_cookie_check(v55 ^ &v0);
}


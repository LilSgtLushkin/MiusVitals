// v20 RVA 016230..01755a  (4906 bytes)  on_packet
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    unsigned int field_0;
    char field_4;
    char field_5;
    unsigned short field_6;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    char field_18;
    char paddin25[3];
    char field_1c;
    char paddin29[3];
    char field_20;
    char paddin33[7];
    char field_28;
    char field_29;
    char field_2a;
    char paddin43[13];
    unsigned int field_38;
    unsigned int field_3c;
    unsigned int field_40;
    unsigned int field_44;
    unsigned int field_48;
    unsigned int field_4c;
    unsigned int field_50;
    char paddin84[92];
    char field_b0;
} struct_3;

typedef struct struct_0 {
    unsigned int field_0;
    char paddin4[4];
    unsigned long long field_8;
} struct_0;

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_1 {
    uint128_t field_0;
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    unsigned long long field_50;
} struct_1;


int on_packet()
{
    char v0;  // [bp-0x578]
    int tmp_7;  // tmp #7
    int tmp_5;  // tmp #5
    int tmp_8;  // tmp #8
    int tmp_19;  // tmp #19
    int tmp_20;  // tmp #20
    int tmp_17;  // tmp #17
    int tmp_13;  // tmp #13
    int tmp_14;  // tmp #14
    int tmp_11;  // tmp #11
    int tmp_24;  // tmp #24
    int tmp_26;  // tmp #26
    int tmp_25;  // tmp #25
    int tmp_34;  // tmp #34
    int tmp_35;  // tmp #35
    int tmp_32;  // tmp #32
    int tmp_69;  // tmp #69
    int tmp_71;  // tmp #71
    int tmp_70;  // tmp #70
    int tmp_10;  // tmp #10
    int tmp_9;  // tmp #9
    int tmp_15;  // tmp #15
    int tmp_12;  // tmp #12
    int tmp_3;  // tmp #3
    int tmp_4;  // tmp #4
    int s_f8;  // [bp-0xf8]
    char v1;  // [bp-0x558]
    char v2;  // [bp-0x557]
    char v3;  // [bp-0x556]
    char v4;  // [bp-0x550], Other Possible Types: unsigned int
    char v5;  // [bp-0x548]
    char v6;  // [bp-0x544], Other Possible Types: unsigned int
    char v7;  // [bp-0x543]
    unsigned short v8;  // [bp-0x542]
    unsigned int v9;  // [bp-0x540], Other Possible Types: char
    unsigned int v10;  // [bp-0x53c]
    char v11;  // [bp-0x538]
    unsigned int v12;  // [bp-0x534]
    unsigned int v13;  // [bp-0x530]
    unsigned int v14;  // [bp-0x52c]
    unsigned int v15;  // [bp-0x528]
    unsigned int v16;  // [bp-0x524]
    char v17;  // [bp-0x520]
    char v18;  // [bp-0x51f]
    char v19;  // [bp-0x51e]
    char v20;  // [bp-0x51d]
    char v21;  // [bp-0x51c]
    unsigned int v22;  // [bp-0x504]
    unsigned int v23;  // [bp-0x500]
    unsigned int v24;  // [bp-0x4fc]
    char v25;  // [bp-0x4f8]
    unsigned int v26;  // [bp-0x498]
    char v27;  // [bp-0x494]
    unsigned int v28;  // [bp-0x454]
    unsigned int v29;  // [bp-0x450]
    unsigned int v30;  // [bp-0x44c]
    unsigned int v31;  // [bp-0x448]
    unsigned int v32;  // [bp-0x444]
    unsigned int v33;  // [bp-0x440]
    char v34;  // [bp-0x43c]
    char v35;  // [bp-0x438]
    unsigned int v36;  // [bp-0x434]
    char v37;  // [bp-0x148]
    char v38;  // [bp-0x147]
    char v39;  // [bp-0x131]
    char v40;  // [bp-0x128]
    unsigned long v42;  // [bp-0x120]
    int v43;  // [bp-0x118]
    int v44;  // [bp-0x108]
    int v45;  // [bp-0xe8]
    unsigned long v46;  // [bp-0xd8]
    unsigned long long v47;  // [bp-0xa8]
    int v48;  // [bp-0x98]
    int v49;  // [bp-0x88]
    int v50;  // [bp-0x78]
    int v51;  // [bp-0x68]
    int v52;  // [bp-0x58]
    int v53;  // [bp-0x48]
    unsigned long v54;  // [bp-0x38]
    unsigned long v55;  // [bp-0x30]
    unsigned long v56;  // [bp-0x28]
    unsigned long v57;  // [bp-0x20]
    unsigned long v58;  // [bp-0x18]
    unsigned long v59;  // [bp-0x10]
    unsigned long v60;  // [bp-0x8]
    unsigned long v62;  // [bp+0x20]
    unsigned long long v63;  // r8
    unsigned long v64;  // rbx
    unsigned long v65;  // rsi
    unsigned long v66;  // rdi
    unsigned long v67;  // r12
    unsigned long v68;  // r13
    unsigned long v69;  // r14
    unsigned long v70;  // r15
    int v71;  // xmm8
    int v72;  // xmm9
    int v73;  // xmm10
    int v74;  // xmm11
    unsigned long long v75;  // r9
    unsigned int v76;  // r13d
    struct_3 *v77;  // rdx
    struct_3 *v78;  // rsi
    unsigned short *v79;  // rcx
    unsigned short *v80;  // r15
    unsigned int v82;  // al, Other Possible Types: char
    unsigned long v83;  // rdx, Other Possible Types: unsigned long long, unsigned int
    unsigned long long v84;  // rcx
    unsigned long long v86;  // rcx
    unsigned long long v87;  // rcx
    unsigned long long v88;  // rcx
    unsigned int v90;  // eax
    unsigned long long v91;  // rcx
    unsigned long long v93;  // rcx
    char v95;  // r12b
    unsigned long long v96;  // rbx, Other Possible Types: unsigned long
    char v97;  // r9b
    int v98;  // ymm6, Other Possible Types: unsigned int
    unsigned int v99;  // xmm9
    struct_0 *v101;  // rbx, Other Possible Types: unsigned long
    char *v102;  // r9
    char v104;  // bl
    unsigned long long v105;  // r13
    unsigned int v106;  // eax
    unsigned long long v107;  // rbx
    unsigned long long v108;  // rsi
    unsigned long long v109;  // rbx
    unsigned long v110;  // rdx
    unsigned long long v111;  // rdx
    unsigned long long v112;  // rcx
    char *v113;  // r11
    unsigned long long v114;  // rbx, Other Possible Types: unsigned long
    unsigned long long v115;  // rdx
    struct_2 *v116;  // rcx
    unsigned int v117;  // ebx
    unsigned long long v118;  // r8
    char *v119;  // rcx, Other Possible Types: unsigned long
    unsigned int v120;  // eax
    unsigned int v121;  // eax
    unsigned long v122;  // rcx, Other Possible Types: unsigned long long
    unsigned int v123;  // edx
    int v124;  // xmm0
    unsigned int v125;  // ymm1
    unsigned int v126;  // eax, Other Possible Types: unsigned long, unsigned long long
    int v129;  // xmm7lq, Other Possible Types: unsigned long long, unsigned long
    char v130;  // rax, Other Possible Types: unsigned long
    int v131;  // xmm8
    char *v132;  // rcx, Other Possible Types: unsigned short *, unsigned int, unsigned long, unsigned long long
    int v134;  // xmm9
    struct_2 *v135;  // rbx, Other Possible Types: unsigned long
    int v137;  // xmm1
    unsigned long v138;  // xmm4hq
    unsigned long v139;  // xmm4lq
    int v140;  // xmm1
    int v142;  // xmm3
    unsigned long v143;  // xmm3hq
    unsigned long v144;  // xmm3lq
    unsigned int v145;  // ecx
    int v146;  // xmm2, Other Possible Types: unsigned long long
    unsigned long long v147;  // xmm2
    int v150;  // xmm0
    struct_2 *v151;  // rbx, Other Possible Types: unsigned long
    struct_2 *v153;  // r8
    struct_1 *v154;  // rdx, Other Possible Types: unsigned long
    struct_2 *v155;  // r8
    unsigned long long v156;  // rcx
    void* v157;  // rdi, Other Possible Types: unsigned long
    struct_2 *v158;  // rdx
    unsigned int v159;  // ecx
    unsigned long long v161;  // r8
    unsigned long v162;  // rdx
    int v165;  // xmm0
    unsigned long long v174;  // rbp

    if ((unsigned int)v63 < 8)
        return;
    v62 = v64;
    *((int *)&v60) = rbp<8>;
    v59 = v65;
    v58 = v66;
    v57 = v67;
    v56 = v68;
    v55 = v69;
    v54 = v70;
    *((unsigned int *)&v53) = v98;
    *((unsigned long long *)&v52) = v129;
    v51 = v71;
    v50 = v72;
    v49 = v73;
    v48 = v74;
    v47 = g_5a380 ^ &v0;
    v76 = v75;
    v78 = v77;
    v80 = v79;
    if (v77->field_0 == 1347245127)
    {
        v157 = 0;
        if (!g_5b61a || *(v79) != g_5b8f0 || *((int *)(v79 + 4)) != g_5b8f4 || *((short *)(v79 + 2)) != g_5b8f2 || (tmp_7 = (unsigned long long)memcmp(v79 + 8, &g_5b8f8, 8), v82 = 1, (int)tmp_7))
            v82 = 0;
        v83 = g_5b8e0;
        v1 = v82;
        if (!(g_5b8e0 != 1 || *((char *)(v78 + 4)) != 1))
        {
            if ((unsigned int)v63 < 24)
            {
                security_cookie_check(v47 ^ &v0);
                v174 = v60;
                return;
            }
            if (memcmp(v78 + 8, "GTMF_TA_A1", 12))
            {
                security_cookie_check(v47 ^ &v0);
                v174 = v60;
                return;
            }
            if (!((unsigned int)v63 >= 84) || !(*((int *)(v78 + 20)) == 20))
            {
                *((char **)&v2) = 1;
                goto LABEL_16378;
            }
            else
            {
                v90 = *((int *)(v78 + 24));
                g_5b89b = *((char *)(v78 + 29)) == 255;
                if (!(v90) || !(v90 == g_5be38))
                {
                    *((char **)&v2) = 1;
LABEL_16378:
                    strncpy(&v5, "GTMP", 4);
                    *((char **)&v6) = 2;
                    memset(&v9, 0, 272);
                }
                *((char **)&v2) = 0;
LABEL_16378:
                strncpy(&v5, "GTMP", 4);
                *((char **)&v6) = 2;
                memset(&v9, 0, 272);
                memcpy(&v9, "GTMF_TA_A1", 12);
                *((char **)&v12) = 20;
                if (!g_5b618 || *((long long *)&g_5b6a0) == *((long long *)&g_5b6a8) || g_5beb4 == 3 || (char)sub_3dbc4() || g_5b89c || (char)ui_blocks_input() && g_5b7c8 != g_5b7d0)
                {
                    *((char **)&v3) = 0;
                    *((char **)&v13) = 0;
                }
                else
                {
                    *((char **)&v3) = 1;
                    v13 = g_5b6a8 - g_5b6a0;
                    if (g_5b8e0 == 1 && g_5bf40)
                    {
                        strcpy_s(&v27, 64, &g_5bf40);
                        v28 = g_5b828 - g_5b820;
                        v29 = g_5bf2c;
                    }
                }
                v14 = g_5be34;
                v15 = g_5be38;
                v16 = g_5be3c;
                v17 = v3;
                v18 = g_5b8e4;
                v19 = v2;
                v20 = g_5b61f;
                strcpy_s(&v21, 24, "Player");
                *((char *)&v83) = g_5b618;
                if (!g_5b729 || g_5b618 || g_5bd40)
                {
                    v23 = g_5ba2c;
                }
                else
                {
                    v22 = g_5b5d8 - g_5b5d0.field_0;
                    v23 = g_5b91c;
                    v24 = g_5bbc8;
                    strcpy_s(&v25, 96, &g_5bce0);
                    *((char *)&v83) = g_5b618;
                }
                *((unsigned long *)&v26) = -(0 < g_5b72f) & 2 | g_5b72c != 0 | g_5bd44 * 256;
                if ((char)v83)
                {
                    if (*((int *)&g_5b8e4) != 1)
                        input_action_hook(v80, &v5, 280);
                    v30 = g_5bfb0;
                    v31 = g_5bfb4;
                    if ((char)ui_blocks_input())
                    {
                        v32 = g_5be3c;
                        v36 = g_5be38;
                    }
                }
                else
                {
                    if (*((int *)&g_5b8e4) != 1)
                        input_action_hook(v80, &v5, 280);
                    v30 = g_5bfb0;
                    v31 = g_5bfb4;
                    v32 = operation_id@opsave@opsave();
                    *((unsigned int *)&v130) = operation_id@opsave@opsave();
                    v36 = v130;
                    v33 = sync_state_enabled();
                    operation_id@opsave@opsave(&v34, &v35);
                }
                v132 = v80;
                input_action_hook(v80, &v5, 280);
                if (false)
                {
                    security_cookie_check(v47 ^ &v0);
                    v174 = v60;
                    return;
                }
                memcpy(&v37, v78 + 32, 24);
                *((char *)&v130) = v37;
                v135 = &v37;
                for (*((char **)&v39) = 0; (char)v130; *((char *)&v130) = *((char *)v135))
                {
                    v132 = (char)v130;
                    if (!isalnum(v132) && *((char *)v135) != 95 && *((char *)v135) != 45)
                        *((char **)v135) = 95;
                    v135 = &v38;
                }
                if (!v1)
                {
                    if (*(v80) == 21332)
                    {
                        memcpy(&v4, v80 + 8, 8);
                        $sprintf_s@$0BAE$sprintf_s@$0BAE(&v40, "%llu", *((long long *)&v4));
                        SetEnvironmentVariableA("MP_STEAM_PEER", &v40);
                        memcpy(&v4, v80 + 8, 8);
                        hook_GetAsyncKeyState(*((long long *)&v4));
                        v153 = &v37;
                        if (!v37)
                            v155 = "no name";
                        v158 = &v40;
                        v132 = "net: player connected through Steam (steam:%s, %s)";
                        mp_log();
                    }
                    else
                    {
                        inet_ntop(2, v80 + 4, &v40, 64);
                        SetEnvironmentVariableA("MP_STEAM_PEER", 0);
                        v151 = &v37;
                        if (!v37)
                            v151 = "no name";
                        mp_log("net: player connected from %s:%u (%s)", &v40, ntohs(*((short *)(v80 + 2))), v151);
                    }
                    clear@?$_Hash@V?$_Umap_traits@_KUSent(&g_5b660);
                    clear@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(&g_5b840);
                    *((char **)&g_5be48) = 0;
                    sub_32664(&g_5b840);
                    sub_34ee0();
                    __Eg_map_sidecar();
                    *((char **)&g_5bed0) = 0;
                    *((char **)&g_5b728) = 0;
                    if (!g_5b618)
                        explode_hook("another player");
                }
                if (v37)
                {
                    v132 = &g_5bbb0;
                    strcpy_s(&g_5bbb0, 24, &v37);
                }
                g_5b61e = *((char *)(v78 + 30)) != 0;
                if (!(*((char *)(v78 + 29)) == 254) || !(*((int *)(v78 + 68)) == g_5bd44))
                {
                    *((char **)&g_5b839) = 0;
                    goto LABEL_167ec;
                }
                else
                {
                    *((char **)&g_5b839) = 1;
                    if (*((int *)(v78 + 64)))
                        *((char *)&v130) = 1;
                    else
                        *((char *)&v130) = 0;
LABEL_167ec:
                    g_5b838 = v130;
                    if (!g_5b618)
                    {
                        if (g_5b728 && *((char *)(v78 + 31)) != g_5b83b && *((int *)&g_5b8e4) != 2)
                            session_toggle_swap();
                        g_5b83b = *((char *)(v78 + 31));
                        *((char **)&g_5b728) = 1;
                        SetEnvironmentVariableA("MP_PEER_NAME", -(0 < g_5bbb0) & &g_5bbb0);
                        if (!g_5b618 && *((int *)&g_5b8e4) == 1)
                            isolate_input(*((int *)(v78 + 72)), *((int *)(v78 + 76)), *((int *)(v78 + 80)));
                    }
                    if ((char)ui_blocks_input())
                    {
                        v132 = *((int *)(v78 + 72));
                        if ((unsigned int)v132)
                            isolate_input(v132, *((int *)(v78 + 76)), *((int *)(v78 + 80)));
                    }
                    if (g_5b839 && g_5b72c && !g_5bd40 && *((int *)(v78 + 56)) && *((int *)(v78 + 56)) <= 0x4000000 && *((int *)(v78 + 60)) != g_5ba24)
                    {
                        *((char **)&v1) = 0;
                        assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(&g_5b808, *((int *)(v78 + 56)), &v1);
                        v159 = *((int *)(v78 + 56));
                        g_5ba24 = *((int *)(v78 + 60));
                        *((char **)&v1) = 0;
                        assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(&g_5b880, &(999)[v159] / 1000, &v1);
                        *((char **)&v4) = 0;
                        assign@?$vector@KV?$allocator@K@std(&g_5b7b0, g_5b888 - g_5b880, &v4);
                        v161 = *((int *)(v78 + 56));
                        v132 = "map: %s is sending his moves (%u bytes)";
                        v162 = (g_5bbb0 != 0 ? "the other player" : &g_5bbb0);
                        *((char **)&g_5ba28) = 0;
                        mp_log();
                    }
                    v165 = *((int128_t *)&v80);
                    *((char **)&g_5b61b) = 1;
                    *((char **)&g_5b61a) = 1;
                    *((void*)&g_5b8f0) = v165;
                    g_5b908 = v76;
                    if (!g_5b618)
                    {
                        security_cookie_check(v47 ^ &v0);
                        v174 = v60;
                        return;
                    }
                    *((char *)&v132) = v2;
                    set_versus(v132);
                }
            }
        }
        else if (!v82)
        {
            security_cookie_check(v47 ^ &v0);
            v174 = v60;
            return;
        }
        else
        {
            v84 = *((char *)(v78 + 4));
            g_5b908 = v75;
            if ((unsigned int)v84 > 9)
            {
                tmp_5 = v84;
                if ((int)tmp_5 == 10 || (tmp_7 = (unsigned long long)((unsigned int)v84 - 10), v86 = (unsigned long long)((unsigned int)v84 - 10) - 1, (int)tmp_7 == 1))
                {
                    shots_on_packet(v78, v63, send_peer_packet);
                }
                else
                {
                    tmp_7 = v86;
                    v88 = v86 - 1;
                    if ((int)tmp_7 == 1)
                    {
                        send_table(v78, v63);
                        security_cookie_check(v47 ^ &v0);
                        v174 = v60;
                        return;
                    }
                    tmp_7 = v88;
                    if ((int)tmp_7 == 1)
                    {
                        obj_by_place_hook(v78, v63);
                        security_cookie_check(v47 ^ &v0);
                        v174 = v60;
                        return;
                    }
                    if (v88 - 1 == 2 && g_5b8e0 == 1 && (!g_5b618 || (char)ui_blocks_input()) && (unsigned int)v63 >= 16)
                    {
                        load_battle_description(v78, v63);
                        security_cookie_check(v47 ^ &v0);
                        v174 = v60;
                        return;
                    }
                }
            }
            else if ((unsigned int)v84 == 9)
            {
                v97 = g_5b619;
                sub_31bfc(v78, v63, send_peer_packet, r9<8>);
                security_cookie_check(v47 ^ &v0);
                v174 = v60;
                return;
            }
            else
            {
                tmp_8 = v84;
                v132 = v84 - 2;
                if ((int)tmp_8 == 2)
                {
                    if (g_5b8e0 != 2)
                    {
                        security_cookie_check(v47 ^ &v0);
                        v174 = v60;
                        return;
                    }
                    if ((unsigned int)v63 < 280)
                    {
                        security_cookie_check(v47 ^ &v0);
                        v174 = v60;
                        return;
                    }
                    if (!g_5b61b)
                    {
                        v82 = *((int *)(v78 + 32));
                        *((char **)&g_5b61b) = 1;
                        v4 = v82;
                        if (v4 == 5391687)
                        {
                            v96 = "Germans";
                        }
                        else if (v4 == 1381192533)
                        {
                            v96 = "Soviets";
                        }
                        else
                        {
                            v96 = &g_5b5c8;
                            memcpy(&g_5b5c8, &v4, 4);
                            *((char **)&g_5b5cc) = 0;
                        }
                        mp_log("net: connected to host (host commands %s)", v96);
                    }
                    memcpy(&g_5bbb0, v78 + 44, 24);
                    *((char **)&g_5bbc7) = 0;
                    if (g_5b72a)
                    {
                        v132 = *((int *)(v78 + 176));
                        v117 = (unsigned int)v132 / 256;
                        *((unsigned long *)&g_5b838) = (char)v132 / 2 & 1;
                        *((char *)&v82) = g_5b83a;
                        if (!((char)v132 & 1))
                        {
LABEL_17261:
                            if (!(!g_5b83a) || !(!(!*((int *)(v78 + 68)))) && !(!(*((int *)(v78 + 72)) == g_5b91c)) && !(!((*((char **)&g_5b83a) = 1, *((char **)&g_5b72f) = 0, mp_log("map: the host has this player's moves; waiting for the turn to be carried out"), *((char *)&v82) = g_5b83a, (char)v82))))
                                goto LABEL_172a1;
LABEL_172f5:
                            if (!(*((char *)(v78 + 40)) == 0))
                                goto LABEL_172fb;
                            if (*((int *)(v78 + 68)) && *((int *)(v78 + 68)) <= 0x4000000 && v117 != g_5bd48 && *((int *)(v78 + 72)) != g_5ba24 && !(1 & *((char *)(v78 + 176))) && (char)safe_file_name@opsave(v78 + 80))
                            {
                                *((char **)&v1) = 0;
                                assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(&g_5b808, *((int *)(v78 + 68)), &v1);
                                v145 = *((int *)(v78 + 68));
                                g_5ba24 = *((int *)(v78 + 72));
                                v132 = &(999)[v145];
                                *((char **)&v1) = 0;
                                assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(&g_5b880, v132 / 1000, &v1);
                                *((char **)&v4) = 0;
                                assign@?$vector@KV?$allocator@K@std(&g_5b7b0, g_5b888 - g_5b880, &v4);
                                g_5bdc8 = *((int *)(v78 + 76));
                                *((char **)&g_5ba28) = 0;
                                g_5bd4c = v117;
                                memcpy(&g_5bdd0, v78 + 80, 96);
                                *((char **)&g_5be2f) = 0;
                                mp_log("map: the host offers the map of the next turn (%u bytes)", *((int *)(v78 + 68)));
                            }
                        }
                        else
                        {
                            if (!(!g_5b729))
                                goto LABEL_17261;
                            if (!g_5b83a)
                            {
                                *((char **)&g_5b72d) = 1;
                                goto LABEL_17261;
                            }
LABEL_172a1:
                            if (!(*((int *)(v78 + 68))) || !(v117 == g_5bd48) || !(!(1 & *((char *)(v78 + 176)))))
                                goto LABEL_172f5;
                            if (*((char *)(v78 + 40)))
                            {
LABEL_172fb:
                                if (!g_5be30)
                                    *((char **)&g_5be30) = &(300)[v75];
                            }
                            else
                            {
                                *((char **)&g_5b72f) = 0;
                                *((char **)&g_5b729) = 0;
                                *((char **)&g_5b83a) = 0;
                                if (*((long long *)&g_5b5d0.field_0) != *((long long *)&g_5b5d8))
                                    *((long long *)&g_5b5d8) = *((long long *)&g_5b5d0.field_0);
                                mp_log("map: the host's game did not carry the turn out; this player goes on giving orders");
                                goto LABEL_172f5;
                            }
                        }
                    }
                    *((char *)&v132) = *((char *)(v78 + 42)) != 0;
                    set_versus(v132);
                }
                else
                {
                    tmp_7 = v132;
                    v87 = v132 - 2;
                    if ((int)tmp_7 != 2)
                    {
                        tmp_7 = v87;
                        v91 = v87 - 1;
                        if ((int)tmp_7 == 1)
                        {
                            if ((g_5b8e0 == 1 || g_5b72a) && *((char *)(v78 + 5)) == 1 && g_5b808 != g_5b810 && (unsigned int)v63 >= 16)
                            {
                                v102 = *((int *)(v78 + 8));
                                if (v102 < g_5b888 - g_5b880 && !v102[g_5b880])
                                {
                                    v111 = *((int *)(v78 + 12));
                                    if ((unsigned int)v111 <= 1000)
                                    {
                                        v113 = v102 * 1000;
                                        if (&v113[v111] <= g_5b810 - g_5b808 && (unsigned int)v63 >= (unsigned int)&(16)[v111])
                                        {
                                            memcpy(&v113[g_5b808], v78 + 16);
                                            *((char **)(*((int *)(v78 + 8)) + g_5b880)) = 1;
                                            *((char **)&g_5ba28) = &(1)[g_5ba28];
                                            if (g_5ba28 == g_5b888 - g_5b880)
                                            {
                                                v123 = 2166136261;
                                                v119 = 0;
                                                if (g_5b810 != g_5b808)
                                                {
                                                    do
                                                    {
                                                        v120 = *((char *)(v119 + g_5b808));
                                                        v119 = &(1)[v119];
                                                        v121 = v120 ^ v123;
                                                        v123 = v121 * 16777619;
                                                    } while (v119 < g_5b810 - g_5b808);
                                                }
                                                if (v123 != g_5ba24)
                                                {
                                                    mp_log("map: the map arrived damaged, fetching it again");
                                                    *((char **)&g_5ba24) = 0;
                                                    if (g_5b808 != g_5b810)
                                                        g_5b810 = g_5b808;
                                                }
                                                else
                                                {
                                                    if (g_5b72a)
                                                    {
                                                        *((char **)&g_5bdcc) = &(200)[v76];
                                                    }
                                                    else
                                                    {
                                                        g_5ba2c = g_5ba24;
                                                        *((char **)&g_5bd40) = &(2500)[v76];
                                                        mp_log("map: the other player's map received");
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            tmp_7 = v91;
                            v93 = v91 - 1;
                            if ((int)tmp_7 == 1)
                            {
                                if ((unsigned int)v63 >= 36)
                                {
                                    v126 = *((short *)(v78 + 6));
                                    if ((unsigned int)v63 >= ((unsigned int)(v126 + (v126 << 2)) << 4) + 36)
                                    {
                                        v98 = *((int *)(v78 + 28));
                                        v99 = 0;
                                        v99 = 0x3f800000;
                                        if ((CmpF(v98, 1028443341) & 1) || (CmpF(0x41800000, v98) & 1))
                                            v98 = v99;
                                        v104 = *((char *)(v78 + 32)) != 0;
                                        if (*((char *)(v78 + 32)) != 0 != g_5b898)
                                        {
                                            v110 = (*((char *)(v78 + 32)) != 0 ? "no longer asks for a pause" : "asks for a pause");
                                            v112 = "clock: the other player %s";
                                            mp_log();
                                            v83 = g_5b8e0;
                                        }
                                        if (((char)((CmpF((unsigned int)v98, g_5a21c) & 69) >> 2) & 1) || (CmpF((unsigned int)v98, g_5a21c) & 64))
                                        {
                                            mp_log("clock: the other player asks for speed x%.2f", (unsigned int)v98);
                                            v83 = g_5b8e0;
                                        }
                                        v122 = *((char *)(v78 + 25));
                                        g_5a21c = (unsigned int)v98;
                                        g_5b898 = v104;
                                        if ((unsigned int)v83 != 2)
                                        {
                                            g_5be80 = *((int *)(v78 + 12));
                                            v126 = ((unsigned int)v75 != 0 ? 1 : (unsigned int)v75);
                                            g_5be84 = v122;
                                            g_5be88 = v126;
                                        }
                                        else
                                        {
                                            v124 = 0;
                                            *((unsigned int *)&v124) = *((int *)(v78 + 16));
                                            v125 = *((int *)(v78 + 20));
                                            g_5b83c = *((char *)(v78 + 24));
                                            g_5be44 = *((int *)(v78 + 12));
                                            *((unsigned int *)&g_5a1c8) = (unsigned int)v124;
                                            g_5a210 = v125;
                                            g_5bec0 = v122;
                                            *((char **)&g_5b83d) = 1;
                                        }
                                        v129 = 0xbff0000000000000;
                                        if ((char)v122 == 2 && g_5beb4 == 2 && *((int *)(v78 + 12)) && !*((char *)(v78 + 24)))
                                        {
                                            tmp_19 = DivV(tmp_20, tmp_17);
                                            v129 = tmp_19;
                                            seconds_now();
                                            v131 = 0;
                                            *((unsigned int *)&v131) = *((int *)(v78 + 16));
                                            if ((((char)(CmpF((unsigned int)v131, 1028443341) & 69) | (char)((CmpF((unsigned int)v131, 1028443341) & 69) >> 6)) & 1) == 1 || (CmpF(0x41800000, (unsigned int)v131) & 1))
                                                v131 = v99;
                                            tmp_13 = SubV(tmp_14, tmp_11);
                                            *((char *)&v126) = g_5be48;
                                            tmp_24 = SubV(tmp_26, tmp_25);
                                            v134 = tmp_24;
                                            if (!g_5be48)
                                            {
LABEL_16dda:
                                                v42 = (unsigned long long)v129;
                                                *((char **)&v40) = 1;
                                                v143 = (unsigned int)v131;
                                                v144 = (unsigned int)v131;
                                                if ((char)v126)
                                                {
                                                    v146 = 0;
                                                    v146 = g_5be70;
                                                }
                                                else
                                                {
                                                    v146 = 0;
                                                    *((unsigned long long *)&v146) = 4589420221869668172;
                                                }
                                                v129 = (unsigned long long)v129;
                                                *((int128_t *)&g_5be48) = *((int128_t *)&v40);
                                                g_5be68 = (unsigned long long)v146 CONCAT 4581421828931458171;
                                                g_5be78 = v129;
                                                g_5be58 = xmm3<16> CONCAT (unsigned long long)v124;
                                            }
                                            else
                                            {
                                                fabs();
                                                if ((((char)(CmpF((unsigned long long)v134, 0x3fe0000000000000) & 69) | (char)((CmpF((unsigned long long)v134, 0x3fe0000000000000) & 69) >> 6)) & 1) != 1 || (((char)(CmpF(tmp_13, 0x4000000000000000) & 69) | (char)((CmpF(tmp_13, 0x4000000000000000) & 69) >> 6)) & 1) != 1)
                                                {
                                                    *((char *)&v126) = g_5be48;
                                                    goto LABEL_16dda;
                                                }
                                                else
                                                {
                                                    if ((((char)(CmpF((unsigned long long)v129, g_5be78) & 69) | (char)((CmpF((unsigned long long)v129, g_5be78) & 69) >> 6)) & 1) != 1)
                                                    {
                                                        tmp_34 = SubV(tmp_35, tmp_32);
                                                        xmm1<16> = (t34 * Conv(64->128, Load(addr=0x5be60<64>, size=8, endness=Iend_LE)))
                                                        v138 = (unsigned int)v131;
                                                        v139 = (unsigned int)v131;
                                                        xmm1<16> = ((xmm1<16> + Conv(64->128, Load(addr=0x5be50<64>, size=8, endness=Iend_LE))) + (xmm9<16> * 0x3fb999999999999a<128>))
                                                        g_5be50 = (unsigned long long)v140;
                                                        tmp_69 = MaxV(tmp_71, tmp_70);
                                                        v142 = tmp_69;
                                                        if ((((char)(CmpF((unsigned long long)v142, (unsigned long long)v142) & 69) | (char)((CmpF((unsigned long long)v142, (unsigned long long)v142) & 69) >> 6)) & 1) == 1)
                                                            v142 = xmm4<16> * 0x3ff8000000000000;
                                                        g_5be60 = (unsigned long long)v142;
                                                        g_5be58 = (unsigned long long)v124;
                                                        fabs();
                                                        v147 = 0;
                                                        v147 = 4587366580439587226;
                                                        tmp_8 = SubV(tmp_10, tmp_9);
                                                        tmp_14 = SubV(tmp_15, tmp_12);
                                                        v150 = tmp_8 * v147 + (0 CONCAT g_5be68);
                                                        g_5be68 = (unsigned long long)v150;
                                                        if ((((char)(CmpF(0x3ff0000000000000, tmp_14) & 69) | (char)((CmpF(0x3ff0000000000000, tmp_14) & 69) >> 6)) & 1) != 1)
                                                        {
                                                            tmp_3 = SubV(tmp_5, tmp_4);
                                                            g_5be70 = (0 CONCAT g_5be70) + tmp_3 * v147;
                                                        }
                                                        g_5be78 = (unsigned long long)v129;
                                                    }
                                                }
                                            }
                                        }
                                        if ((char)[D] amd64g_calculate_condition(0x2<64>, 0x6<64>, 0x0<64>, Conv(16->64, Load(addr=(rsi<8> + 0x6<64>), size=2, endness=Iend_LE)), cc_ndep<8>))
                                        {
                                            v154 = g_5b5f0;
                                            do
                                            {
                                                v46 = (unsigned long long)v129;
                                                v156 = v157 * 10;
                                                *((int128_t *)&v40) = *((int128_t *)((char *)v78 + v156 * 8 + 36));
                                                *((int128_t *)&v43) = *((int128_t *)((char *)v78 + v156 * 8 + 52));
                                                *((int128_t *)&v44) = *((int128_t *)((char *)v78 + v156 * 8 + 68));
                                                *((int128_t *)&s_f8) = *((int128_t *)((char *)v78 + v156 * 8 + 84));
                                                *((int128_t *)&v45) = *((int128_t *)((char *)v78 + v156 * 8 + 100));
                                                if (v154 == g_5b5f8)
                                                {
                                                    $_Emplace_reallocate@AEBUSaveSlot(&g_5b5e8);
                                                    v154 = g_5b5f0;
                                                }
                                                else
                                                {
                                                    *((int128_t *)v154) = *((int128_t *)&v40);
                                                    *((int128_t *)&(16)[v154]) = (int128_t)v43;
                                                    *((int128_t *)&(32)[v154]) = (int128_t)v44;
                                                    *((int128_t *)&(48)[v154]) = (int128_t)s_f8;
                                                    *((int128_t *)&(64)[v154]) = (int128_t)v45;
                                                    *((unsigned long *)&(80)[v154]) = v46;
                                                    v154 = g_5b5f0 + 88;
                                                    g_5b5f0 = v154;
                                                }
                                            } while ((v157 += 1, (unsigned int)v157 < (unsigned int)(unsigned short)*((short *)(v78 + 6))));
                                        }
                                        *((char **)&g_5be9c) = &(1)[g_5be9c];
                                    }
                                }
                            }
                            else
                            {
                                tmp_7 = v93;
                                if ((int)tmp_7 == 1)
                                {
                                    mp_log("net: the other player's game closed");
                                    *((char **)&g_5b61b) = 0;
                                    if (g_5b8e0 != 2)
                                    {
LABEL_16a93:
                                        if (g_5b8e0 == 1)
                                        {
                                            *((char **)&g_5b61a) = 0;
                                            *((char **)&g_5b838) = 0;
                                            *((char **)&g_5b839) = 0;
                                        }
                                    }
                                    else
                                    {
                                        if (g_5b618 && !g_5bec8)
                                        {
                                            g_5bec8 = ((unsigned int)v75 != 0 ? 1 : (unsigned int)v75);
                                            goto LABEL_16a93;
                                        }
                                    }
                                }
                                else
                                {
                                    if (v93 - 1 == 1)
                                    {
                                        v97 = g_5b619;
                                        sub_31bfc(v78, v63, send_peer_packet, r9<8>);
                                        security_cookie_check(v47 ^ &v0);
                                        v174 = v60;
                                        return;
                                    }
                                }
                            }
                        }
                    }
                    else if ((unsigned int)v63 >= 12 && (g_5b8e0 == 1 || *((char *)(v78 + 5)) == 1))
                    {
                        v95 = *((char *)(v78 + 5));
                        if (*((char *)(v78 + 5)) != 1)
                        {
                            v101 = (v95 == 2 ? &g_5b6a0 : &g_5b820);
                        }
                        else
                        {
                            v101 = &g_5b5d0.field_0;
                            if (!g_5b729)
                            {
                                security_cookie_check(v47 ^ &v0);
                                v174 = v60;
                                return;
                            }
                            else if (g_5b8e0 == 1)
                            {
                                g_5b72b = g_5b8e0;
                            }
                        }
                        v105 = *((long long *)v101);
                        v106 = *((int *)(v78 + 8));
                        v107 = *((long long *)&(8)[v101]);
                        v108 = *((int *)&v9) * 1000;
                        v109 = v107 - v105;
                        if (v108 < v109)
                        {
                            strncpy(&v5, "GTMP", 4);
                            *((char **)&v6) = 5;
                            *((char **)&v8) = 0;
                            v9 = eax<4>;
                            memset(&v11, 0, 1000);
                            v114 = v109 - v108;
                            v7 = v95;
                            v115 = v108 + v105;
                            v116 = &v11;
                            if (v114 > 1000)
                                v114 = 1000;
                            v118 = v114;
                            v10 = v114;
                            memcpy();
                            input_action_hook(v80, &v5, &(16)[v10]);
                        }
                    }
                }
            }
        }
    }
    security_cookie_check(v47 ^ &v0);
    v174 = v60;
    return;
}


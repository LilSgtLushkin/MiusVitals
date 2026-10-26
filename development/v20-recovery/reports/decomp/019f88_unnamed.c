// v20 RVA 019f88..01b20f  (4743 bytes)  session_frame
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char paddin0[60];
    unsigned int field_3c;
} struct_2;

typedef struct SOCKADDR {
    unsigned int sa_family;
    char sa_data[14];
} SOCKADDR;

typedef struct struct_0 {
    char field_0;
} struct_0;

typedef struct struct_7 {
    char paddin0[3560];
    unsigned int field_de8;
} struct_7;


long long session_frame(struct struct_4 **a0)
{
    char v0;  // [bp-0x958]
    int tmp_4;  // tmp #4
    int tmp_6;  // tmp #6
    int tmp_5;  // tmp #5
    int tmp_25;  // tmp #25
    int tmp_26;  // tmp #26
    int tmp_23;  // tmp #23
    int tmp_27;  // tmp #27
    int tmp_28;  // tmp #28
    int tmp_9;  // tmp #9
    int tmp_11;  // tmp #11
    int tmp_10;  // tmp #10
    int tmp_21;  // tmp #21
    int tmp_22;  // tmp #22
    int tmp_2;  // tmp #2
    int tmp_3;  // tmp #3
    unsigned long v1;  // [bp-0x928]
    void* v2;  // [bp-0x918], Other Possible Types: unsigned int
    void* v3;  // [bp-0x910], Other Possible Types: unsigned long
    unsigned int v4;  // [bp-0x90c]
    unsigned int v5;  // [bp-0x908]
    int v6;  // [bp-0x8f8], Other Possible Types: unsigned long
    unsigned long v7;  // [bp-0x8f0]
    void* v8;  // [bp-0x8e8], Other Possible Types: struct_0 *
    void* v9;  // [bp-0x8e0]
    SOCKADDR v10;  // [bp-0x8d8]
    char v11;  // [bp-0x8c8]
    char v12;  // [bp-0x8c7]
    unsigned int v13;  // [bp-0x8c4]
    char v14;  // [bp-0x8c0]
    unsigned int v15;  // [bp-0x8b4]
    unsigned int v16;  // [bp-0x8b0]
    char v17;  // [bp-0x8ac]
    char v18;  // [bp-0x8ab]
    char v19;  // [bp-0x8aa]
    char v20;  // [bp-0x8a8]
    unsigned int v21;  // [bp-0x890]
    unsigned int v22;  // [bp-0x88c]
    unsigned int v23;  // [bp-0x888]
    unsigned int v24;  // [bp-0x884]
    char v25;  // [bp-0x868]
    unsigned long long v26;  // [bp-0x68]
    struct struct_4 **v28;  // rsi
    unsigned long long v29;  // rdi
    struct_2 *v30;  // rax
    unsigned long long v31;  // rax
    unsigned long long v32;  // rax
    unsigned long long *v33;  // r15, Other Possible Types: unsigned long
    unsigned long long *v34;  // r12, Other Possible Types: unsigned long
    struct_7 *v35;  // rax, Other Possible Types: unsigned int *, unsigned long, unsigned long long
    unsigned int v36;  // r9d
    unsigned int v37;  // r8d
    unsigned long long v38;  // r15
    unsigned long long v39;  // rcx, Other Possible Types: unsigned long
    struct struct_3 **v40;  // r8
    struct struct_3 **v41;  // r8
    unsigned long long v42;  // r8, Other Possible Types: unsigned long
    char *v43;  // rdx, Other Possible Types: unsigned long
    unsigned long long v44;  // rcx
    unsigned int v45;  // r15d
    unsigned long long v46;  // rcx
    unsigned long long v47;  // rcx
    unsigned int v48;  // ecx
    unsigned long long v49;  // r8, Other Possible Types: unsigned long
    struct_2 *v50;  // r14d, Other Possible Types: unsigned long, unsigned long long, unsigned int
    char *v51;  // rbx, Other Possible Types: unsigned int [5], unsigned int, unsigned long
    unsigned long v52;  // rdx
    unsigned int v53;  // r12d
    unsigned long long v54;  // r9
    unsigned long v55;  // rdx
    struct_0 *v56;  // r8
    unsigned long long v57;  // rcx
    struct struct_3 **v58;  // r8
    struct struct_6 **v59;  // r14
    unsigned long long v60;  // rdx
    unsigned long v61;  // rdx
    unsigned long long v62;  // rcx
    struct struct_3 **v63;  // r8
    unsigned long long v64;  // rax
    unsigned long v65;  // r8
    unsigned long v66;  // rdx
    unsigned long long v67;  // rcx
    char v69;  // al
    unsigned int v71;  // ecx, Other Possible Types: unsigned long
    unsigned long long v73;  // xmm7
    int v74;  // xmm6
    int v76;  // xmm0
    int v77;  // xmm3
    int v78;  // xmm2
    unsigned long long v79;  // rcx, Other Possible Types: unsigned long
    unsigned long long v82;  // r9
    unsigned long long v83;  // r8
    unsigned long long v84;  // rdx
    unsigned long long v85;  // rcx

    v26 = g_5a380 ^ &v0;
    v28 = a0;
    v29 = GetTickCount();
    *((char **)&g_5bed4) = &(1)[g_5bed4];
    shots_install(a0);
    if (!g_5c10d && g_5b8e0 == 2)
    {
        if (!g_5b618)
            sub_1eac4(a0);
        *((char **)&g_5c10d) = 1;
        v30 = GetModuleHandleA("base_shell.codelib");
        v50 = v30;
        if (v30)
        {
            v31 = *((int *)(&v30->paddin0[*((int *)(v30 + 60))] + 144));
            if ((unsigned int)v31)
            {
                for (v51 = &v50->paddin0[v31]; *((int *)&(12)[v51]); v51 += 20)
                {
                    if (!_stricmp(&v50->paddin0[*((int *)&(12)[v51])], "KERNEL32.dll") && *((int *)v51))
                    {
                        v34 = &v50->paddin0[*((int *)v51)];
                        v33 = &v50->paddin0[*((int *)&(16)[v51])];
                        while (true)
                        {
                            v32 = *((long long *)v34);
                            if (!v32)
                                break;
                            if (v32 < 0 || strcmp(v50 + 2 + v32, "CreateProcessA"))
                            {
                                v34 = &(8)[v34];
                                v33 = &(8)[v33];
                            }
                            else if (VirtualProtect(v33, 8, 4, &v2))
                            {
                                g_5c000 = *((long long *)v33);
                                *((void* *)v33) = leave_battle_for_menu;
                                VirtualProtect(v33, 8, v2, &v2);
                                mp_log("net: when this battle ends the game goes back to the menu (and from there to the host)");
                                goto LABEL_1a0fc;
                            }
                        }
                    }
                }
            }
        }
    }
LABEL_1a0fc:
    if (g_5b618)
    {
        if (g_5b8e0 || GetEnvironmentVariableA("MP_LOG_SHOTS", 0, 0))
            shots_install(a0);
        if (g_5b618 && g_5b8e0)
            trench_install();
    }
    sub_1eac4(a0);
    if (g_5b8e0 == 1 && g_5b61c && (unsigned int)v29 - g_5b918 >= 500)
        host_listen();
    v35 = g_5a160;
    if (g_5a160 == -1)
        return security_cookie_check(v26 ^ &v0);
    while (true)
    {
        *((char **)&v2) = 16;
        v36 = 0;
        v37 = 2048;
        (unsigned int)v35 = recvfrom(v35, &v25, 0x800, 0, &v10, &v2);
        if ((unsigned int)v35 <= 0)
            break;
        g_5bea0 = g_5bea0 + (unsigned int)v35;
        on_packet(&v10, &v25, v35, v29);
        v35 = g_5a160;
        if (g_5a160 == -1)
            return security_cookie_check(v26 ^ &v0);
        if (false)
            break;
    }
    sub_3ede4();
    if ((g_5b8e0 == 1 || g_5b8e0 == 2 && g_5b61a && g_5b8f0 == 21332) && (char)steam_init(&g_5354))
    {
        while (true)
        {
            v2 = 0;
            (unsigned int)v35 = mp_log(&v2, &v25, 2048);
            if ((unsigned int)v35 <= 0)
                break;
            g_5bea0 = g_5bea0 + (unsigned int)v35;
            v3 = v2;
            *((char **)&v10.sa_family) = 0;
            *((unsigned long long **)&v10.sa_family) = &g_5354;
            memcpy(&v10, &v3, 8);
            *((int128_t *)&v6) = *((int128_t *)&v10.sa_family);
            on_packet(&v6, &v25, v35, v29);
            if (g_5a160 == -1)
                return security_cookie_check(v26 ^ &v0);
            if (false)
                break;
        }
    }
    install_detours();
    if (g_5b8e0 != 1 || g_5b618 || (*((char *)&v50) = 1, g_5b8e4 != 2))
        *((char *)&v50) = 0;
    if (!g_5c070 && (g_5b72a || (char)v50))
    {
        v35 = GetModuleHandleA("adv_opgame.codelib");
        if (v35)
        {
            *((char **)&g_5c070) = 1;
            v38 = &(&g_36b40)[v35];
            if (memcmp(v38, &g_48da0, 16))
            {
                v39 = "map: unknown game build, ending the turn is not coordinated between the players";
            }
            else
            {
                g_5bfd8 = install_detour(v38, 16, next_turn_hook);
                if ((char)v50)
                {
                    v50 = v35 + 485392;
                    if (!memcmp(v50, &g_48db0, 15))
                        g_5bfe0 = install_detour(v50, 15, turn_worker_hook);
                    if (memcmp(&(&g_44120)[v35], &g_48dc0, 18))
                        mp_log("map: unknown game build, what the other player's intelligence finds out is not kept for him");
                    else
                        g_5bfe8 = install_detour(&(&g_44120)[v35], 18, net_dispatch);
                }
                v39 = "map: this player's side of the operational map; the turn ends when both players have pressed next turn";
                if (!g_5b72a)
                    v39 = "map: the turn ends when both players have pressed next turn; the other player's moves are collected then";
            }
            mp_log(v39);
        }
    }
    if (g_5b8ec)
    {
        if (!g_5b618)
        {
            sub_14458(v28);
        }
        else if (g_5bee8)
        {
            *((char **)&g_5b8ec) = 0;
            v3 = 0;
            v6 = "i_intereff";
            v7 = "intereff";
            v8 = &v3;
            v9 = 0;
            (unsigned int)v35 = v40(v28, &v6, *((long long *)(*(v28) + 72)));
            if (v35)
            {
                mp_log("test: leaving the battle through the game's own exit (box 6)");
                *((char **)(v35 + 3560)) = 6;
                g_5bee8(v35, 1, 0, 0);
            }
        }
    }
    if (g_5b618 && !g_5bf30)
    {
        v3 = 0;
        v6 = "i_intereff";
        v7 = "intereff";
        v8 = &v3;
        v9 = 0;
        g_5bf30 = v41(v28, &v6, *((long long *)(*(v28) + 72)));
    }
    sub_14458(v28);
    if (g_5b61b && g_5b618)
        base_load_hook();
    ui_start(v28, send_peer_packet);
    host_map_frame(v28, v29);
    if ((g_5b8e0 == 1 && !g_5b618 || g_5b72a) && g_5b808 != g_5b810)
    {
        v42 = g_5b888;
        v43 = g_5b880;
        v44 = g_5b888 - (char *)g_5b880;
        if (g_5ba28 != v44)
        {
            v45 = 0;
            v51 = 0;
            if (v44)
            {
                while (v45 < 24)
                {
                    if (!*((char *)(v51 + v43)))
                    {
                        v46 = (unsigned int)v29 - *((int *)((char *)&g_5b7b0->field_0 + v51 * 4));
                        if ((unsigned int)v46 >= 300)
                        {
                            strncpy(&v3, "GTMP", 4);
                            *((char **)&v4) = 260;
                            v5 = v51;
                            input_action_hook(&g_5b8f0, &v3, 12);
                            *((unsigned int *)((char *)&g_5b7b0->field_0 + v51 * 4)) = v29;
                            v45 = &(1)[v45];
                            v42 = g_5b888;
                            v43 = g_5b880;
                        }
                    }
                    v51 = &(1)[v51];
                    if (v51 >= v42 - v43)
                        break;
                }
            }
        }
    }
    if (g_5b8e0 == 1 && (!g_5b618 || (char)ui_blocks_input()) && g_5b7c8 != g_5b7d0 && g_5b61a)
    {
        v50 = 0;
        v51 = 0;
        v49 = g_5b6c0;
        v52 = g_5b6b8;
        if (g_5b6c0 != g_5b6b8)
        {
            while (v50 < 24)
            {
                if (!*((char *)(v52 + v51)))
                {
                    v47 = (unsigned int)v29 - *((int *)((char *)&g_5b710->field_0 + v51 * 4));
                    if ((unsigned int)v47 >= 300)
                    {
                        strncpy(&v3, "GTMP", 4);
                        *((char **)&v4) = 14;
                        v5 = v51;
                        input_action_hook(&g_5b8f0, &v3, 12);
                        if ((unsigned int)v29)
                            v48 = v29;
                        else
                            v48 = 1;
                        *((unsigned int *)((char *)&g_5b710->field_0 + v51 * 4)) = v48;
                        v50 = &(1)[v50];
                        v49 = g_5b6c0;
                        v52 = g_5b6b8;
                    }
                }
                v51 = &(1)[v51];
                if (v51 >= v49 - v52)
                    break;
            }
        }
    }
    if (g_5bd40 && (unsigned int)v29 >= g_5bd40)
    {
        *((char **)&g_5bd40) = 0;
        host_merge_and_restart(v28);
    }
    if (g_5b72a)
    {
        if (g_5b72d && !g_5b729 && operation_on_screen(v28))
        {
            *((char **)&g_5b72d) = 0;
            session_map_give(v28);
        }
        if (g_5b72a)
        {
            if (!g_5b89d)
            {
                if (!operation_on_screen(v28))
                {
                    *((char **)&g_5c110) = 0;
                }
                else if (!g_5c110)
                {
                    g_5c110 = v29;
                }
                else if ((unsigned int)(v29 - g_5c110) > 2500)
                {
                    *((char **)&g_5b89d) = 1;
                    launch_hook(v28);
                }
            }
            if (g_5b72a)
            {
                if (g_5b89f && operation_on_screen(v28))
                {
                    *((char **)&g_5b89f) = 0;
                    *((char **)&v6) = 0;
                    v8 = 0;
                    *((char **)&v11) = 0;
                    memset(&v12, 0, 95);
                    *((char *)&v50) = save_operation(v28, &v6, &v11, 96);
                    if (*((char *)&v50))
                    {
                        v51 = rows_of@opsave(&v6, "user_orders");
                        v53 = turn_phase@opsave@opsave(&v6);
                    }
                    v54 = (long long)(&v6)[8] - (long long)v6;
                    v55 = ((char)v50 ? "not saved" : "saved");
                    v56 = &v11;
                    v57 = "test: operation %s (%s, %zu bytes, turn phase %d, %u orders)";
                    mp_log();
                    _Tidy@?$vector@EV?$allocator@E@std(&v6);
                }
                if (g_5b72a)
                {
                    if (g_5b89e)
                    {
                        *((char **)&g_5b89e) = 0;
                        if (!g_5b72f && !g_5b729 && !g_5b83a)
                            sub_183b0();
                    }
                    if (g_5b72a)
                    {
                        if (g_5be30 && (unsigned int)v29 >= g_5be30)
                            joiner_into_battle(); /* do not return */
                        if (g_5bdcc && (unsigned int)v29 >= g_5bdcc)
                            joiner_reload_map(); /* do not return */
                    }
                }
            }
        }
    }
    if (!g_5c0a8 && g_5b8e0 && g_5b618)
    {
        v3 = 0;
        v6 = "i_ctrltimer";
        v7 = "ctrltimer";
        v8 = &v3;
        v9 = 0;
        v59 = v58(v28, &v6, *((long long *)(*(v28) + 72)));
        v60 = GetModuleHandleA("base_main.codelib");
        if (v59 && v60)
        {
            *((char **)&g_5c0a8) = 1;
            v51 = *(v59);
            if (v51 != v60 + 146376 || *((long long *)&(24)[v51]) != v60 + 18048 || *((long long *)&(40)[v51]) != v60 + 15792 || *((long long *)&(48)[v51]) != v60 + 15808)
            {
                mp_log("net: unknown game build, speed and pause stay local");
            }
            else
            {
                memcpy(&g_5c0b0, v51 - 8, 72);
                g_5bef8 = *((long long *)&(24)[v51]);
                g_5bf00 = *((long long *)&(40)[v51]);
                g_5bf08 = *((long long *)&(48)[v51]);
                g_5c0d0 = box_result_hook;
                g_5c0e0 = sub_1e530;
                g_5c0e8 = sub_10ddc;
                *(v59) = &g_5c0b8;
                v61 = (g_5b8e0 == 2 ? &g_45ce6 : " (this game's clock follows the host's)");
                v62 = "net: pause and speed are both players': the battle pauses or runs faster only when both ask for it%s";
                mp_log();
            }
        }
    }
    if (g_5b61b && g_5b618 && g_5bed8 && *((int *)&g_5beb4) < 2 && (unsigned int)v29 - g_5bec4 >= 1500)
    {
        if (*((int *)&g_5becc) <= *((int *)&g_5beb4) || (*((char *)&v51) = 1, g_5bed0 <= *((int *)&g_5beb4)))
            *((char *)&v51) = 0;
        if (g_5b8e0 != 2 || !g_5b83d || g_5bec0 > 2 || (*((char *)&v35) = 1, g_5bec0 <= *((int *)&g_5beb4)))
            *((char *)&v35) = 0;
        if ((char)v51 || (char)v35)
        {
            v3 = 0;
            v6 = "i_intereff";
            v7 = "intereff";
            v8 = &v3;
            v9 = 0;
            (unsigned int)v64 = v63(v28, &v6, *((long long *)(*(v28) + 72)));
            if (v64)
            {
                g_5bec4 = v29;
                v65 = ((char)v51 ? "the host is already further" : "both players are ready");
                v66 = (*((int *)&g_5beb4) == 0 ? "initial orders" : "deployment");
                v67 = "phase: leaving %s (%s)";
                mp_log();
                *((char **)&g_5b899) = 1;
                g_5bed8(rax<8>, 0, 0, 0);
                *((char **)&g_5b899) = 0;
                *((char **)&g_5b83f) = 0;
            }
        }
    }
    if (g_5b8e0 == 2 && (unsigned int)v29 - g_5b90c > (unsigned int)(unsigned long long)&(500)[-(0 < g_5b61b) & 500])
    {
        g_5b90c = v29;
        strncpy(&v11, "GTMP", 4);
        *((char **)&v13) = 1;
        memset(&v14, 0, 76);
        memcpy(&v14, "GTMF_TA_A1", 12);
        *((char **)&v15) = 20;
        v16 = g_5b8e8;
        v17 = g_5becc;
        v69 = g_5beb4;
        if (g_5b72a)
            v69 = 254;
        v18 = v69;
        *((char **)&v19) = 1;
        strcpy_s(&v20, 24, "Player");
        if (!g_5b72a)
        {
            *((char **)&v23) = 0;
        }
        else
        {
            if (g_5b729 && !g_5b83a)
            {
                v21 = g_5b5d8 - g_5b5d0;
                v22 = g_5b91c;
            }
            *((char **)&v23) = 1;
            if (!g_5b72f)
                *((char **)&v23) = 0;
        }
        v24 = g_5bd48;
        input_action_hook(&g_5b8f0, &v11, 84);
    }
    if (g_5b61b)
    {
        if (g_5b618)
        {
            if ((unsigned int)v29 - g_5b910 >= 66)
            {
                g_5b910 = v29;
                send_transforms(v28, v29);
                R<lambda_4850e5e4fe7cad2db8d8b8fa5a62506f>(send_peer_packet);
            }
            seconds_now();
            if ((char)sync_state_enabled())
                sub_32010(v28, v29, owns_object, send_peer_packet);
            seconds_now();
            tmp_4 = SubV(tmp_6, tmp_5);
            g_5bf18 = (long long)((0 CONCAT g_5bf18) + tmp_4);
            xlate_frame(v28);
            sub_34368(send_peer_packet);
        }
        if (!g_5b61b)
            goto LABEL_1ade1;
        if ((unsigned int)v29 - g_5b908 > 8000)
        {
            mp_log("net: connection lost (nothing received for 8 s)");
            *((char **)&g_5b61b) = 0;
            *((char **)&g_5b898) = 0;
            g_5a21c = 0x3f800000;
            *((char **)&g_5bed0) = 0;
            if (g_5b8e0 != 2)
            {
                v71 = g_5bec8;
                goto LABEL_1adbf;
            }
            if (!g_5b618)
                goto LABEL_1ade1;
            v71 = g_5bec8;
            if (!g_5bec8)
            {
                v71 = g_5b8e0 - 1;
                if ((unsigned int)v29)
                    v71 = v29;
                g_5bec8 = v71;
LABEL_1adbf:
                if (g_5b8e0 == 1)
                {
                    *((char **)&g_5b61a) = 0;
                    *((char **)&g_5b838) = 0;
                    *((char **)&g_5b839) = 0;
                }
LABEL_1ade1:
                if (!(unsigned int)v71)
                {
LABEL_1ae07:
                    if ((unsigned int)v29 - g_5b914 < 5000)
                        return security_cookie_check(v26 ^ &v0);
                    if (g_5b61b && g_5b618 && (g_5be94 || g_5be98))
                    {
                        tmp_25 = DivV(tmp_26, tmp_23);
                        *((int128_t *)&v1) = tmp_25;
                        mp_log("net: 5 s: sent %u and applied %u unit transforms, mean correction %.2f m, largest %.2f m, %u jumps, %.1f KB/s in total");
                        v73 = 0;
                        v73 = 0x408f400000000000;
                        if (g_5be48 && *((int *)&g_5beb4) == 2)
                        {
                            tmp_27 = MaxV(tmp_28, tmp_25);
                            v74 = tmp_27;
                            seconds_now();
                            tmp_9 = SubV(tmp_11, tmp_10);
                            v76 = tmp_9 * v73;
                            mp_log("net: the other game's units shown by its battle clock: transforms %.0f ms apart, its clock %+.0f ms from this game's and running x%.3f, arrivals wandering %.0f ms, shown %.0f ms behind", (0 CONCAT g_5be70) * v73, (unsigned long long)v76, g_5be60, (0 CONCAT g_5be68) * v73);
                        }
                        sub_324fc();
                        if (g_5bff8)
                        {
                            mp_log("select: 5 s: %llu of the other player's units kept out of this player's selection");
                            g_5bff8 = 0;
                        }
                        sub_34d68();
                        sub_382f8();
                        sub_3b6b8();
                        sub_3cc1c();
                        sub_3fba0();
                        if (g_5bfa0 || g_5bfa8)
                        {
                            mp_log("net: AI commander: %llu steps for formations of this side, %llu skipped for the other player's");
                            g_5bfa8 = 0;
                            g_5bfa0 = 0;
                        }
                        if (g_5b8e0 != 2)
                        {
                            if (g_5b8e0 == 1)
                            {
                                v77 = 0;
                                *((unsigned int *)&v77) = g_5a214;
                                tmp_21 = DivV(tmp_23, tmp_22);
                                v78 = tmp_21;
                                v79 = "clock: battle time %.1f s, in the other game %.1f s (time scaled by %.2f to wait for it)";
                            }
                        }
                        else
                        {
                            if (g_5b83d)
                            {
                                v77 = 0;
                                *((unsigned int *)&v77) = g_5a214;
                                tmp_21 = DivV(tmp_23, tmp_22);
                                v78 = tmp_21;
                                v79 = "clock: battle time here %.1f s, on the host %.1f s (time scaled by %.2f to stay on it)";
                            }
                        }
                        tmp_2 = DivV(tmp_4, tmp_3);
                        mp_log(v79, tmp_2, (unsigned long long)v78, (unsigned long long)v77);
                        *((int *)&v1) = (GetForegroundWindow() == main_window() ? "in the background" : "in front");
                        v82 = g_5bf14;
                        v83 = g_5bf10;
                        v84 = g_5bed4;
                        v85 = "load: 5 s: %u frames, %u simulation steps covering %u ms, %.0f ms saving state, %.0f ms loading state, window %s";
                        mp_log();
                    }
                    *((char **)&g_5bf14) = 0;
                    *((char **)&g_5bf10) = 0;
                    *((char **)&g_5bed4) = 0;
                    g_5bf20 = 0;
                    g_5bf18 = 0;
                    g_5b914 = v29;
                    *((char **)&g_5bea4) = 0;
                    *((char **)&g_5bea0) = 0;
                    *((char **)&g_5be9c) = 0;
                    *((char **)&g_5be98) = 0;
                    *((char **)&g_5be94) = 0;
                    g_5bea8 = 0;
                    *((char **)&g_5beb0) = 0;
                    return security_cookie_check(v26 ^ &v0);
                }
            }
            if ((unsigned int)v29 - (unsigned int)v71 > 4000 && !(char)sub_3dbc4() && *((int *)&g_5beb4) != 3)
                leave_battle_for_menu();
            goto LABEL_1ae07;
        }
    }
    v71 = g_5bec8;
    goto LABEL_1ade1;
}


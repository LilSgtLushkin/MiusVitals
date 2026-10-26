// v20 RVA 02bb0c..02c8ac  (3488 bytes)  sub_2bb0c
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[4696];
    unsigned int field_1258;
} struct_0;

typedef struct struct_1 {
    char paddin0[379072];
    unsigned long long field_5c8c0;
} struct_1;


long long sub_2bb0c(struct struct_3 **a0)
{
    char v0;  // [bp-0x128]
    int tmp_107;  // tmp #107
    int tmp_109;  // tmp #109
    int tmp_108;  // tmp #108
    int tmp_131;  // tmp #131
    int tmp_133;  // tmp #133
    int tmp_132;  // tmp #132
    int tmp_3;  // tmp #3
    int tmp_8;  // tmp #8
    int tmp_4;  // tmp #4
    char v1;  // [bp-0x108]
    unsigned int v2;  // [bp-0x104]
    unsigned int v3;  // [bp-0x100]
    unsigned int v4;  // [bp-0xfc]
    unsigned int v5[2];  // [bp-0xf8]
    unsigned int v6[4];  // [bp-0xf0]
    char v7;  // [bp-0xe8]
    char v8;  // [bp-0xe4]
    char v9;  // [bp-0xe0]
    char v10;  // [bp-0xb8]
    unsigned long long v11;  // [bp-0x38]
    char v13;  // bl
    unsigned long long v14;  // rsi, Other Possible Types: unsigned long
    unsigned int *v15;  // rdi
    char v16;  // r14b
    char v19;  // al
    char v20;  // al
    unsigned int v21[1175];  // rax
    struct_0 *v22;  // rax
    unsigned long long v23;  // r13, Other Possible Types: unsigned long
    unsigned int v24;  // edi
    char v25;  // eax, Other Possible Types: unsigned int
    unsigned long long v26;  // r14
    char v27;  // r12b
    unsigned long long v28;  // r15
    unsigned int v29[1218];  // rdi
    unsigned int v30[1218];  // rdi
    unsigned long long v31;  // dl, Other Possible Types: char, unsigned long
    unsigned int v32;  // rcx, Other Possible Types: unsigned long long, unsigned long
    unsigned int v33;  // r12d
    unsigned int v34;  // eax
    unsigned int v35;  // eax
    unsigned long v36;  // rdx
    unsigned long long v37;  // rcx
    unsigned long long v38;  // rax, Other Possible Types: unsigned long
    struct_0 *v39;  // rax
    unsigned int v40;  // r8
    unsigned int v41;  // eax
    struct_0 *v42;  // rax
    unsigned long long v43;  // r9
    unsigned long v44;  // r9
    unsigned long long v45;  // rax
    unsigned long long v46;  // rcx
    unsigned long v47;  // rdx
    unsigned long long v49;  // rax
    unsigned long long v50;  // rdx
    struct_1 *v51;  // r8, Other Possible Types: char, unsigned long, unsigned long long
    unsigned int v52;  // r13d
    unsigned long v53;  // rdi
    unsigned long v54;  // r14, Other Possible Types: unsigned long long
    unsigned long long v57;  // rax, Other Possible Types: unsigned long
    unsigned long long v58;  // rax, Other Possible Types: unsigned long
    char *v59;  // r15, Other Possible Types: unsigned long
    unsigned long v60;  // rcx
    char *v61;  // rdx

    v11 = g_5a380 ^ &v0;
    v13 = 1;
    *((char **)&g_5c68c) = &(1)[g_5c68c];
    v14 = a0;
    if (g_5c688 && GetTickCount() > g_5c688)
        ExitProcess(0); /* do not return */
    v15 = main_window();
    if (v15 && GetForegroundWindow() == v15)
    {
        v16 = 1;
        *((char **)&v1) = 1;
    }
    else
    {
        *((char **)&v1) = 0;
        v16 = 0;
        if (v15 == 0)
            goto LABEL_2bc3c;
    }
    if (GetCursorPos(v5) && ScreenToClient(v15, v5) && GetClientRect(v15, v6) && *((int *)&v7) > 0 && *((int *)&v8) > 0 && (((char)(CmpF(g_5caa8, 0) & 69) | (char)((CmpF(g_5caa8, 0) & 69) >> 6)) & 1) != 1)
    {
        tmp_107 = DivV(tmp_109, tmp_108);
        *((int128_t *)&g_5caa0) = tmp_107;
        tmp_131 = DivV(tmp_133, tmp_132);
        *((int128_t *)&g_5caa4) = tmp_131;
    }
LABEL_2bc3c:
    g_5c46b = g_5c46a;
    if (!v16 || (tmp_3 = (unsigned long long)(unsigned short)GetAsyncKeyState(1), v19 = 1, (short)tmp_3 >= 0))
        v19 = 0;
    g_5c46a = v19;
    if (!g_5c46b || (tmp_3 = (unsigned long long)v19, v20 = 1, (char)tmp_3))
        v20 = 0;
    g_5c46c = v20;
    if (!g_5c148)
    {
        v21 = menu_controller(a0);
        if (!v21 || *((int *)(v21 + &g_1258)) || (tmp_8 = (unsigned long long)(unsigned int)*((int *)(v21 + &g_1200)), *((char *)&v21[0]) = 1, !(int)tmp_8))
            *((char *)&v21[0]) = 0;
        g_5c14b = v21[0];
        g_5cbe8 = a0;
        v22 = menu_controller(a0);
        v23 = -1;
        if (v22)
        {
            v24 = *((int *)(v22 + &g_1258));
            if (v24 != 17)
                *((char **)&g_5c46e) = 0;
LABEL_2bcf9:
            if (g_5cbf0 && GetTickCount() >= g_5cbf0)
            {
                *((char **)&g_5cbf0) = 0;
                start_program(0);
            }
            v25 = GetTickCount() - g_5cd40;
            if (v25 >= 400 && v24 == 17)
            {
                *((char **)&v3) = 0;
                g_5cd40 = GetTickCount();
                *((char **)&v4) = 0;
                *((char **)&v2) = 0;
                v26 = v3;
                v27 = end_turn_here(a0, &v3, &v4, &v2, *((long long *)&v1));
                v28 = v4;
                if (v27 && sub_1c334() == 1 && sub_1b380() == 1)
                {
                    if ((unsigned int)v26)
                        hook_srand(v26);
                    if ((unsigned int)v28 && *((int *)&(&v1)[4]))
                        sub_1c4a0(v28);
                    v29 = menu_controller(a0);
                    if ((char)readable(v29, &g_1310))
                        map_moved_on(*((int *)(v29 + &g_1300)), *((int *)(v29 + &g_1304)));
                }
                if (g_5c468)
                {
                    g_5c46e = v27;
                    g_5cbf4 = v26;
                    g_5cbf8 = v28;
                    v30 = menu_controller(a0);
                    if (!(char)readable(v30, &g_1310) || (!g_5c458 || *((int *)(v30 + &g_1300)) + 1 == g_5c458) && (!g_5c45c || *((int *)(v30 + &g_1304)) + 1 == g_5c45c))
                        v25 = 0;
                    else
                        v25 = 1;
                    g_5c46f = v25;
                }
            }
            v31 = g_5c14b;
            v32 = g_5c4b0;
            g_5c14c = session_on_op_map(v14);
            if (!g_5c14b || g_5c4b0)
                *((char **)&g_5c14a) = 0;
            if (!g_5c14c || g_5c4b0)
                *((char **)&g_5c14d) = 0;
            v54 = 6;
            if (g_5c4c0)
            {
LABEL_2bf02:
                if (v31 && g_5c68c > 45 && (unsigned int)v32 <= 2)
                {
                    if (_strnicmp(&g_5c4c0, "steam:", 6))
                    {
                        operation_exists@Joiner@Joiner(&g_5c4c0);
                        v31 = 128;
                        strcpy_s(&v10, 128, send_peer());
                    }
                    else
                    {
                        strcpy_s(&v10, 128, &g_5c4c0);
                        strncpy_s(&g_5ca60, 64, sub_3f6e0(_strtoui64(&g_5c4c6, 0, 10)), 63);
                    }
                    *((char **)&g_5c4c0) = 0;
                    *((char **)&g_5c7e0) = 0;
                    mp_log("ui: rejoining the lobby of '%s'", &v10);
                    g_5c2c5 = g_5c14f;
                    *((char **)&g_5c288) = 0;
                    *((char **)&g_5c2c6) = 0;
                    strcpy_s(&g_5c295, 24, sub_1b864("ui: rejoining the lobby of '%s'", v31));
                    if ((char)start@Joiner(&g_5c150, &v10))
                    {
                        *((char **)&g_5c149) = 0;
                        *((char **)&g_5c4b0) = 3;
                    }
                    else
                    {
                        strcpy_s(&g_5c7e0, 200, &g_5c164);
                        *((char **)&g_5c4b0) = 2;
                    }
                }
            }
            else
            {
                if (sub_1c334() != 1)
                {
                    v32 = g_5c4b0;
                    if (!(g_5c4b0 <= 2))
                        goto LABEL_2beef;
                    if ((char)sub_3f6b4(&g_5c4c0, 128))
                    {
                        *((char **)&g_5c14f) = 0;
                        mp_log("ui: Steam's Join Game - into the lobby of %s when the main menu is up", &g_5c4c0);
                    }
                }
                v32 = g_5c4b0;
LABEL_2beef:
                if (g_5c4c0)
                {
                    v31 = g_5c14b;
                    goto LABEL_2bf02;
                }
            }
            if (!g_5c14c || (tmp_3 = (unsigned long long)sub_1b390(), v25 = 1, !(char)tmp_3))
                v25 = 0;
            g_5c14e = v25;
            v53 = g_5c8b8;
            *((char **)&g_5c8b8) = 0;
            v59 = &g_5c8b0;
            v33 = sub_1c334();
            if ((unsigned int)v53 == 1)
            {
                *((char **)&g_5c7e0) = 0;
                g_5c149 = v33 == 1;
                if (v33 == 1 || (v34 = 1, g_5c468))
                    v34 = 3;
                g_5c4b0 = v34;
                *((char **)&g_5c14a) = 0;
                mp_log("ui: multiplayer menu opened");
                goto LABEL_2c236;
            }
            if ((unsigned int)(v53 - 2) <= 2)
            {
                if (g_5c8a8)
                {
                    v25 = strtoul(&g_5c8a8, 0, 10) - 1;
                    if (v25 > 65534)
                    {
                        strcpy_s(&g_5c7e0, 200, "The port to host on has to be a number from 1 to 65535 (empty: 1943).");
                        goto LABEL_2c0c1;
                    }
                }
                if ((unsigned int)(v53 - 2) <= 1)
                {
                    v14 = ((unsigned int)v53 == 3 ? 1 : 3);
LABEL_2c5d2:
                    tmp_8 = v53;
                    v52 = 2;
                    v53 = 1943;
                    if ((int)tmp_8 == 4)
                        v14 = 2;
                    if (!g_5c8a8)
                        v38 = 1943;
                    else
                        (unsigned int)v38 = strtoul(&g_5c8a8, 0, 10);
                    operation_exists@Joiner@Joiner((unsigned short)v38, v31, v40, 63);
                    hook_GetRawInputData(v32);
                    ui_start();
                    sub_1b2d8(v14);
                    if (g_5c8a8)
                        v53 = strtoul(&g_5c8a8, 0, 10);
                    v44 = (g_5c469 ? "off" : "on");
                    v51 = v53;
                    v46 = "ui: hosting %s chosen, on UDP port %u, UPnP %s";
                    v47 = ((unsigned int)v14 == 3 ? "skirmish" : "mission");
                    if ((unsigned int)v14 == 2)
                        v50 = "operation";
                    mp_log();
                    *((char **)&g_5c4b0) = 3;
                    v54 = 6;
                    *((char **)&g_5c7e0) = 0;
                    *((char **)&g_5c149) = 1;
                    goto LABEL_2c0c1;
                }
            }
            if ((unsigned int)v53 == 4)
                goto LABEL_2c5d2;
            if ((unsigned int)v53 == 7)
            {
                if (v33 != 1)
                {
                    goto LABEL_2c0c1;
                }
                else
                {
                    sub_247e8();
                    sub_1b380();
                    open_stock_screen(v14);
                    goto LABEL_2c0c1;
                }
            }
            if ((unsigned int)v53 == 8)
            {
                if (g_5c149)
                {
                    session_toggle_swap();
                    goto LABEL_2c0c1;
                }
                else
                {
                    *((char **)&g_5c2c6) = &(1)[g_5c2c6];
                    goto LABEL_2c0c1;
                }
            }
            if ((unsigned int)v53 == 9)
            {
                v35 = g_5c2c5;
                g_5c2c5 = !g_5c2c5;
                v36 = (v35 == 0 ? "not ready" : "ready");
                v37 = "ui: lobby: %s";
                mp_log();
LABEL_2c236:
                goto LABEL_2c0c1;
            }
            if ((unsigned int)v53 == 6)
            {
                if (g_5c149)
                {
                    sub_1d2e0();
                    mp_log("ui: stopped hosting");
                    v41 = 1;
                }
                else
                {
                    if (g_5c3b8 != -1)
                        closesocket(g_5c3b8);
                    g_5c3b8 = -1;
                    *((char **)&g_5c160) = 0;
                    joiner_reload_map();
                    v41 = 2;
                }
                g_5c4b0 = v41;
                goto LABEL_2c0c1;
            }
            if ((unsigned int)v53 == 17)
            {
                v39 = menu_controller(v14);
                if (v39 && *((int *)(v39 + &g_1258)) == (unsigned int)v53)
                {
                    mp_log("test: the Skirmish screen's start");
                    open_stock_screen(v14);
                }
                v42 = menu_controller(v14);
                if (v42)
                    v23 = *((int *)(v42 + &g_1258));
                mp_log("test: the Skirmish screen is not up (screen %d)", v23);
                goto LABEL_2c236;
            }
            else if ((unsigned int)v53 != 16)
            {
                if ((unsigned int)v53 == 5)
                {
                    *((char *)&v25) = g_5c690;
                    if (!g_5c690)
                    {
                        goto LABEL_2c0c1;
                    }
                    else
                    {
                        if (g_5c8b0)
                        {
                            v25 = strtoul(&g_5c8b0, 0, &(5)[v53]) - 1;
                            if (v25 > 65534)
                                strcpy_s(&g_5c7e0, 200, "The port has to be a number from 1 to 65535 (empty: 1943).");
                            *((char *)&v25) = g_5c690;
                        }
                        if (!(char)v25)
                        {
                            goto LABEL_2c0c1;
                        }
                        else
                        {
                            ui_start();
                            *((char **)&g_5c7e0) = 0;
                            v45 = send_peer();
                            mp_log("ui: connecting to '%s'", v45);
                            *((char **)&g_5c288) = 0;
                            *((char **)&g_5c2c5) = 0;
                            strcpy_s(&g_5c295, 24, sub_1b864("ui: connecting to '%s'", v45));
                            send_peer();
                            if (!(char)start@Joiner(&g_5c150))
                                strcpy_s(&g_5c7e0, 200, &g_5c164);
                            *((char **)&g_5c149) = 0;
                            *((char **)&g_5c4b0) = 3;
                            goto LABEL_2c236;
                        }
                    }
                }
                else if ((unsigned int)v53 == 10)
                {
                    v52 = (unsigned int)(v53 - 8);
                    if (g_5c4b0 == v52)
                    {
                        *((char **)&g_5c4b0) = 1;
                        goto LABEL_2c0c1;
                    }
                    else
                    {
                        sub_247e8();
                        goto LABEL_2c0c1;
                    }
                }
                else if ((unsigned int)v53 == 11)
                {
                    v51 = g_5c8bc;
                    if (g_5c8bc >= 0)
                    {
                        v43 = g_5c130;
                        if (g_5c8bc < (unsigned int)((7378697629483820647 * (g_5c138 - g_5c130) >> 64 >> 6) + (7378697629483820647 * (g_5c138 - g_5c130) >> 64 >> 6 >> 63)))
                        {
                            restore_slot(g_5c8bc * 160 + g_5c130);
                            goto LABEL_2c236;
                        }
                    }
                }
                else if ((unsigned int)v53 == 15 && g_5c8bc >= 0 && g_5c8bc < g_5c7d4)
                {
                    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v9, "steam:%llu", *((long long *)(g_5c8bc * 104 + (char *)&g_5c8c0)), 63);
                    strcpy_s(&g_5ca60, &(49)[v53], &(&g_5c8c8)[g_5c8bc * 104]);
                    *((char **)&g_5c7e0) = 0;
                    mp_log("ui: joining %s through Steam ('%s')", &g_5ca60, &v9);
                    *((char **)&g_5c288) = 0;
                    *((char **)&g_5c2c5) = 0;
                    (unsigned int)v49 = sub_1b864("ui: joining %s through Steam ('%s')", &g_5ca60, v51);
                    strcpy_s(&g_5c295, &(9)[v53], v49);
                }
            }
            else
            {
                if (!g_5c149 && g_5c4b0 == 3)
                {
                    v52 = (unsigned int)(v53 - 14);
                    if (g_5c160 == v52)
                    {
                        *((char **)&g_5c468) = 1;
                        *((char **)&g_5cc00) = 0;
                        *((char **)&g_5c46e) = 0;
                        strcpy_s(&g_5c540, &(112)[v53], &g_5c204);
                        *((char **)&g_5c2c5) = 0;
                        sub_245c0(&g_5c540, v31, &g_5c204);
                        load_battle_description(g_5c44c, g_5c460, g_5c458, g_5c45c);
                        sub_247e8(g_5c44c, g_5c460, g_5c458, g_5c45c);
                        open_stock_screen(v14, 1);
                        *((char **)&g_5cbf0) = &(1500)[GetTickCount()];
                        mp_log("ui: CHOOSE MY FORCE - Skirmish screen open; its start button hands this player's force to the host (%s)", &g_5c540);
                    }
LABEL_2c0c1:
                    *((char *)&v14) = v1;
                    if (v1 && (char)key_pressed(120) && !g_5c4b0 && (v33 == 1 || g_5c14b))
                        *((char **)&g_5c8b8) = 1;
                    if (g_5c468)
                    {
                        if (!(g_5c4b0 == 0))
                            goto LABEL_2c12e;
                        if (g_5c149)
                            return security_cookie_check(v11 ^ &v0);
                        poll@Joiner(&g_5c150);
                    }
                    if (!g_5c4b0)
                        return security_cookie_check(v11 ^ &v0);
LABEL_2c12e:
                    if (!(char)v14 || !(char)key_pressed(27))
                        *((char *)&v53) = 0;
                    else
                        *((char *)&v53) = 1;
                    if (g_5c4b0 == v52)
                    {
                        if (GetTickCount() - g_5c7d8 >= 1000)
                        {
                            g_5c7d8 = GetTickCount();
                            g_5c7d4 = R<lambda_4850e5e4fe7cad2db8d8b8fa5a62506f>(&g_5c8c0, 4);
                        }
                        if ((char)v14)
                        {
                            tmp_4 = key_pressed(9);
                            v57 = g_5c7dc;
                            if ((char)tmp_4)
                            {
                                v57 = g_5c7dc ^ 1;
                                g_5c7dc = v57;
                            }
                            if (!(unsigned int)v57)
                            {
                                v59 = &g_5c690;
                                v13 = 0;
                                v54 = &(64)[v57];
                            }
                            *((char *)&v51) = v13;
                            type_address(v59, v54, v51);
                            if ((char)key_pressed(13))
                                *((char **)&g_5c8b8) = 5;
                        }
                    }
                    else if (g_5c4b0 == 1)
                    {
                        if ((char)v14)
                        {
                            *((char *)&v51) = 1;
                            type_address(&g_5c8a8, 6, v51);
                        }
                    }
                    else
                    {
                        if (g_5c4b0 != 3)
                        {
                            return security_cookie_check(v11 ^ &v0);
                        }
                        else if (g_5c149)
                        {
                            if (sub_1b380() == v52 && GetTickCount() - g_5cbdc > 2000)
                                read_slots();
                            if (sub_1c334() != 1)
                            {
                                *((char **)&g_5c4b0) = 1;
                                return security_cookie_check(v11 ^ &v0);
                            }
                            return security_cookie_check(v11 ^ &v0);
                        }
                        else
                        {
                            poll@Joiner(&g_5c150);
                            if (g_5c160 != 5)
                            {
                                if (g_5c160 != 4)
                                    return security_cookie_check(v11 ^ &v0);
                                if (!g_5c2d0)
                                    (char)v58 = launch@Joiner@Joiner(&g_5c150, &g_5c6d0);
                                else
                                    (char)v58 = launch_map@Joiner(&g_5c150, &g_5c6d0);
                                if ((char)v58)
                                {
                                    v60 = (!g_5c2d0 ? "ui: operational map received from %s, opening it" : "ui: battle received from %s, starting it");
                                    v61 = &g_5c690;
                                    mp_log();
                                    *((char **)&g_5c4b0) = 4;
                                    *((char **)&g_5c688) = &(2500)[GetTickCount()];
                                    return security_cookie_check(v11 ^ &v0);
                                }
                            }
                            strcpy_s(&g_5c7e0, 200, &g_5c164);
                            g_5c4b0 = v52;
                            return security_cookie_check(v11 ^ &v0);
                        }
                    }
                    if (!(char)v53)
                        return security_cookie_check(v11 ^ &v0);
                    *((char **)&g_5c8b8) = 10;
                }
            }
            v52 = 2;
            goto LABEL_2c0c1;
        }
        else
        {
            *((char **)&g_5c46e) = 0;
            goto LABEL_2bcf9;
        }
    }
    else
    {
        if (g_5c8b8 == 14)
            sub_3dd88();
        *((char **)&g_5c8b8) = 0;
        return security_cookie_check(v11 ^ &v0);
    }
}


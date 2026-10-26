// v20 RVA 024be4..026091  (5293 bytes)  sub_24be4
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long sub_24be4(unsigned long long a0, unsigned long a1, unsigned long long a2, unsigned long a3, unsigned int a4)
{
    char v0;  // [bp-0x378]
    int tmp_228;  // tmp #228
    int tmp_230;  // tmp #230
    int tmp_229;  // tmp #229
    int tmp_241;  // tmp #241
    int tmp_243;  // tmp #243
    int tmp_242;  // tmp #242
    int tmp_8;  // tmp #8
    int tmp_10;  // tmp #10
    int tmp_9;  // tmp #9
    int tmp_25;  // tmp #25
    int tmp_0;  // tmp #0
    int tmp_2;  // tmp #2
    int tmp_1;  // tmp #1
    char v1;  // [bp-0x348]
    char v2;  // [bp-0x338]
    char v3;  // [bp-0x337]
    char v4;  // [bp-0x336]
    struct_0 *v5;  // [bp-0x330]
    unsigned int v6;  // [bp-0x328]
    struct_0 *v8;  // [bp-0x320]
    unsigned int v9;  // [bp-0x318]
    struct_0 *v10;  // [bp-0x314]
    unsigned int v11;  // [bp-0x310]
    unsigned int v12;  // [bp-0x30c]
    unsigned int v13;  // [bp-0x308]
    unsigned int v14;  // [bp-0x304]
    unsigned int v15;  // [bp-0x300]
    unsigned int v17;  // [bp-0x2f8]
    unsigned int v18;  // [bp-0x2f4]
    unsigned int v19;  // [bp-0x2f0]
    unsigned int v20;  // [bp-0x2ec]
    unsigned int v21;  // [bp-0x2e8]
    unsigned long v22;  // [bp-0x2e0]
    char v23;  // [bp-0x2d8]
    char v24;  // [bp-0x1f8]
    char v25;  // [bp-0x1d8]
    char v26;  // [bp-0x178]
    unsigned long long v27;  // [bp-0xf8]
    int v28;  // [bp-0xd8]
    int v29;  // [bp-0xb8]
    int v30;  // [bp-0x98]
    int v31;  // [bp-0x88]
    unsigned int v33;  // [bp+0x30]
    int v34;  // xmm9
    int v35;  // xmm10
    int v36;  // xmm12
    int v37;  // xmm14
    int v38;  // xmm6
    int v39;  // xmm2
    int v40;  // xmm0
    int v41;  // xmm1
    int v42;  // xmm14
    int v44;  // xmm3
    int v46;  // xmm0
    unsigned int v48;  // r13d
    char v49;  // r15b
    char v50;  // bl
    unsigned long long v51;  // rax
    unsigned long long v52;  // r12, Other Possible Types: unsigned long
    unsigned int v53;  // esi
    unsigned long v54;  // rcx
    unsigned long long v55;  // rcx
    unsigned long long v58;  // rbx
    struct_0 *v60;  // rcx
    unsigned long v61;  // rax
    unsigned long long v62;  // rdi, Other Possible Types: unsigned long
    char v63;  // cc_dep1
    unsigned long long v64;  // rbx, Other Possible Types: unsigned long
    struct_0 *v66;  // rsi, Other Possible Types: unsigned long
    unsigned long long v67;  // rdi, Other Possible Types: unsigned long
    unsigned long v69;  // r9, Other Possible Types: unsigned long long
    unsigned long v70;  // rcx, Other Possible Types: unsigned long long
    char v71;  // rbx, Other Possible Types: unsigned long long, unsigned long
    unsigned long long v72;  // rax
    unsigned long v73;  // rcx
    unsigned long long v74;  // rcx, Other Possible Types: unsigned long
    unsigned long long v75;  // rdi, Other Possible Types: unsigned long
    unsigned long long v76;  // rcx, Other Possible Types: unsigned long
    struct_0 *v77;  // rcx, Other Possible Types: unsigned long
    unsigned long long v78;  // rsi, Other Possible Types: unsigned long
    unsigned long long v79;  // r15, Other Possible Types: unsigned long
    unsigned long long v80;  // rcx, Other Possible Types: unsigned long
    int v81;  // xmm8
    int v82;  // xmm0
    unsigned long long v83;  // rcx
    void* v84;  // rbx, Other Possible Types: unsigned long
    unsigned long long v85;  // rdx
    unsigned long v86;  // rax
    void* v87;  // rdi, Other Possible Types: unsigned long
    int v88;  // xmm0
    struct_0 *v89;  // rcx
    unsigned long long v90;  // rdx, Other Possible Types: unsigned long
    unsigned long v93;  // r8
    unsigned long long v94;  // rax
    unsigned long long v95;  // rcx, Other Possible Types: unsigned long
    unsigned long long v96;  // rax, Other Possible Types: unsigned long
    unsigned int v97;  // eax

    v31 = v34;
    v30 = v35;
    v29 = v36;
    v28 = v37;
    v27 = g_5a380 ^ &v0;
    v38 = 0;
    *((unsigned int *)&v38) = g_5cbd8;
    v17 = (unsigned int)v39;
    v40 = v38 * 0x43880000;
    v15 = (unsigned int)v41;
    *((char **)&v3) = 0;
    v42 = v38 * 0x42980000;
    v19 = (unsigned int)v40;
    v4 = g_5c4b0 == 4;
    v6 = (unsigned int)v38 * 0x440c0000;
    v9 = (unsigned int)v44;
    v18 = (unsigned int)v42;
    tmp_228 = SubV(tmp_230, tmp_229);
    v46 = tmp_228 + v40 * 0x3f000000;
    v20 = (unsigned int)v46;
    tmp_241 = SubV(tmp_243, tmp_242);
    *((int128_t *)&v21) = tmp_241;
    s_324 =L Conv(128->32, ((xmm6<16> * 0x44048000<128>) + Conv(32->128, Load(addr=stack_base-760, size=4, endness=Iend_LE))))<4>
    s_2fc =L Conv(128->32, ((xmm6<16> * 0x441d8000<128>) + Conv(32->128, Load(addr=stack_base-760, size=4, endness=Iend_LE))))<4>
    if (g_5c149)
    {
        v48 = sub_1b380();
        v49 = sub_1e354();
        a1 = 24;
        (char)v51 = session_peer(&v24, 24, &v3);
        v50 = v51;
        v2 = v51;
        v52 = sub_1b864(a0, 24, a2);
        v22 = -(0 < v50) & &v24;
    }
    else
    {
        v48 = g_5c284;
        v49 = g_5c2c7;
        if (g_5c160 == 1 || (v50 = 1, !g_5c2ad))
            v50 = 0;
        v3 = g_5c2c5;
        v2 = v50;
        v22 = &g_5c295;
        v52 = -(0 < v50) & &g_5c2ad;
    }
    *((char **)&v1) = 1;
    text("MULTIPLAYER LOBBY");
    *((char *)&a1) = g_5c149;
    if (g_5c149 || v50)
    {
        *((char **)&v1) = 0;
        v53 = 0;
        *((char *)&v53) = v48 == 2;
        v54 = (v48 == 3 ? "Skirmish" : "Custom mission");
        if (v48 == 2)
            v55 = "Operation";
        text();
        *((char *)&a1) = g_5c149;
        if (g_5c149)
        {
            if (sub_1e4ec() != 2)
            {
                a2 = sub_1bf4c();
                $sprintf_s@$0NM(&v23);
            }
            else
            {
                v58 = (*((char *)sub_1c248()) != 0 ? &g_45ce6 : "  Your public address: ");
                $sprintf_s@$0NM(&v23, "UPnP: the router passes UDP port %u on to this PC.%s%s", sub_1bf4c(), v58);
            }
            a1 = 4287241232;
            v60 = &v23;
            *((char **)&v1) = 0;
            text();
            *((char *)&a1) = g_5c149;
        }
    }
    else
    {
        v53 = 0;
        *((char *)&v53) = v48 == 2;
    }
    *((char **)&v11) = 0;
    v10 = 0;
    v69 = "the mission's PLAYER force";
    v61 = (v49 ? "the mission's PLAYER force" : "the mission's ENEMY force");
    v62 = (v49 ? "OWN force of the battle setup" : "ENEMY force of the battle setup");
    if (v48 == 3)
        v62 = v61;
    if (v53)
        v62 = "the side of the operation";
    v63 = v49;
    v14 = (unsigned int)(v38 * 0x43960000 + v41);
    v64 = (v49 ? "ENEMY force of the battle setup" : "OWN force of the battle setup");
    v79 = "Germans";
    v70 = (v63 ? "the mission's ENEMY force" : "the mission's PLAYER force");
    if (v48 == 3)
        v64 = v70;
    v13 = (unsigned int)(v41 + v38 * 0x42700000);
    if (v53)
        v64 = "the opposing side";
    tmp_8 = SubV(tmp_10, tmp_9);
    v8 = v64;
    v71 = &g_5b5c8;
    *((int128_t *)&v12) = tmp_8;
    s_330 =L Conv(128->32, ((xmm6<16> * 0x434c0000<128>) + Conv(32->128, Load(addr=stack_base-760, size=4, endness=Iend_LE))))<4>
    if (v48 - 2 <= 1)
    {
LABEL_251d4:
        v66 = v8;
    }
    else if (!(char)a1)
    {
LABEL_2516e:
        if (g_5c4b4 && g_5c444 == g_5c440)
        {
            *((int *)&v8) = (int)g_5c4b8;
            if ((int)g_5c4b8 == 5391687)
            {
                v69 = "Germans";
                goto LABEL_25143;
            }
            else
            {
                if ((int)g_5c4b8 == 1381192533)
                {
                    v69 = "Soviets";
                    goto LABEL_25143;
                }
                else
                {
                    v70 = &g_5b5c8;
                    memcpy(&g_5b5c8, &v8, 4);
                    v69 = &g_5b5c8;
                    *((char **)&g_5b5cc) = 0;
                    goto LABEL_25143;
                }
            }
LABEL_25148:
            $sprintf_s@$0GA(&v25, "his own force: %u squads of the %s");
            goto LABEL_2515b;
        }
    }
    else if (!(char)sub_1bd20(&v11, &v10))
    {
        if (!(g_5c149 == 0))
            goto LABEL_251d4;
        goto LABEL_2516e;
    }
    else
    {
        tmp_9 = sub_1bd48();
        *((int *)&v8) = (int)v10;
        if (!(int)tmp_9)
        {
            if ((int)v8 == 5391687)
            {
                v69 = "Germans";
            }
            else if ((int)v8 == 1381192533)
            {
                v69 = "Soviets";
            }
            else
            {
                v70 = &g_5b5c8;
                memcpy(&g_5b5c8, &v8, 4);
                v69 = &g_5b5c8;
                *((char **)&g_5b5cc) = 0;
            }
LABEL_25143:
            goto LABEL_25148;
        }
        else
        {
            if ((int)v8 == 5391687)
            {
                v71 = "Germans";
            }
            else if ((int)v8 == 1381192533)
            {
                v71 = "Soviets";
            }
            else
            {
                v70 = &g_5b5c8;
                memcpy(&g_5b5c8, &v8, 4);
                *((char **)&g_5b5cc) = 0;
            }
            sub_1bd48();
            $sprintf_s@$0GA(&v25, "his own force: %u squads of the %s, cost %u", v11, v71);
LABEL_2515b:
            v66 = &v25;
        }
    }
    if (v52)
    {
        $sprintf_s@$0NM(&v23, "%s  (host)", v52);
        *((char **)&v1) = 1;
        text(&v23);
        *((char **)&v1) = 0;
        text(v62);
        if (!g_5c149)
        {
            a2 = "in the battle";
            *((char **)&v1) = 0;
            a1 = (-(0 < g_5c2c8) & -2876958) - 10858168;
            v70 = (g_5c2c8 ? "setting up" : "in the battle");
            text();
        }
    }
    else
    {
        if (!g_5c150)
        {
            v67 = "Looking for the host at %s ...  (%u s)";
            v71 = &g_5c204;
        }
        else
        {
            v71 = (g_5ca60 != 0 ? "the host" : &g_5ca60);
            v67 = "Looking for %s through Steam ...  (%u s)";
        }
        v69 = (GetTickCount() - g_5c424) / 1000;
        $sprintf_s@$0NM(&v23, v67, v71, v69);
        *((char **)&v1) = 0;
        text(&v23);
    }
    v72 = v22;
    if (v72)
    {
        *((char **)&v1) = 1;
        text(v72);
        *((char **)&v1) = 0;
        text(v66);
        a1 = v3;
        *((char **)&v1) = 0;
        a2 = (-(0 < (unsigned int)a1) & -6009062) + -7726064;
        v73 = ((unsigned int)a1 != 0 ? "not ready" : "READY");
        text();
        goto LABEL_25436;
    }
    else
    {
        v71 = (*((char *)sub_1b388()) == 63 ? sub_1b388() : "your address");
        $sprintf_s@$0NM(&v23, "Waiting for a player to join at  %s : %u", v71, sub_1bf4c());
        *((char **)&v1) = 0;
        text(&v23);
        if (!(char)steam_init(v70))
        {
LABEL_25436:
        }
        else
        {
            *((char **)&v1) = 0;
            text("Your Steam friends find this game on their JOIN A GAME screen - they need no address and no open port.");
        }
    }
    quad();
    *((char **)&v1) = 0;
    if (v48 == 2)
    {
        text("The host opens the operation from the side he wants to play; modifiers are set on the Operations screen.");
        v74 = "On the operational map both players move their own forces at the same time; the turn ends when both have pressed next turn.";
        goto LABEL_255c7;
    }
    if (v48 == 3)
    {
        text("The host picks a mission on the Missions screen: the prepared ones and those made in the battle editor.");
        v74 = "The mission travels to the other player with the battle. SWAP SIDES exchanges who commands which force.";
LABEL_255c7:
        *((char **)&v1) = 0;
        text(v74);
        *((char *)&v52) = v2;
        goto LABEL_255ed;
    }
    else
    {
        text("The host sets up the battle on the Skirmish screen. The other player may pick his own force with CHOOSE MY FORCE;");
        *((char **)&v1) = 0;
        text("otherwise he commands the one the setup gives him. SWAP SIDES exchanges who commands which.");
        if (!g_5c149)
        {
            *((char *)&v52) = v2;
            v75 = -(0 < v2) & g_5c450;
        }
        else
        {
            *((char *)&v52) = v2;
            v75 = sync_state_enabled();
        }
        if (!g_5c149)
        {
LABEL_25712:
            if (!(char)v52)
                goto LABEL_255ed;
            if (!v4)
            {
                a2 = g_5c4b4;
                if (g_5c4b4)
                {
                    *((int *)&v5) = (int)g_5c4b8;
                    if ((int)g_5c4b8 == 5391687)
                    {
                        v69 = "Germans";
                    }
                    else if ((int)g_5c4b8 == 1381192533)
                    {
                        v69 = "Soviets";
                    }
                    else
                    {
                        memcpy(&g_5b5c8, &v5, 4);
                        a2 = g_5c4b4;
                        v69 = &g_5b5c8;
                        *((char **)&g_5b5cc) = 0;
                    }
                    $sprintf_s@$0NM(&v23, "Your own force: %u squads of the %s - %s");
                    *((char **)&v1) = 0;
                    v77 = &v23;
                }
                else
                {
                    if (g_5c5c0.field_0 == 0)
                        goto LABEL_25842;
                    *((char **)&v1) = 0;
                    v77 = &g_5c5c0.field_0;
                }
                text(v77);
LABEL_25842:
                if (!(int)g_5c44c)
                {
                    goto LABEL_259d0;
                }
                else
                {
                    *((int *)&v5) = (int)g_5c44c;
                    if ((unsigned int)v75)
                    {
                        if ((int)g_5c44c == 5391687)
                        {
                            v78 = "Germans";
                        }
                        else if ((int)g_5c44c == 1381192533)
                        {
                            v78 = "Soviets";
                        }
                        else
                        {
                            memcpy(&g_5b5c8, &v5, 4);
                            *((char **)&g_5b5cc) = 0;
                            *((int *)&v5) = (int)g_5c44c;
                            v78 = &g_5b5c8;
                            memcpy(&g_5b5c8, &v5, 4);
                            *((char **)&g_5b5cc) = 0;
                        }
                        $sprintf_s@$0NM(&v23, "You command the %s. CHOOSE MY FORCE opens the Skirmish screen: the %s as your side, units costing up to %u (the host's force), then its start button.", v78, v78);
                        if (g_5c2c8)
                        {
                            *((int *)&v5) = (int)g_5c44c;
                            if ((int)g_5c44c != 5391687)
                            {
                                if ((int)g_5c44c == 1381192533)
                                {
                                    v79 = "Soviets";
                                }
                                else
                                {
                                    memcpy(&g_5b5c8, &v5, 4);
                                    *((char **)&g_5b5cc) = 0;
                                    v79 = &g_5b5c8;
                                }
                            }
                            $sprintf_s@$0NM(&v23, "The host's battle is still in deployment: CHOOSE MY FORCE (the %s, up to %u) puts your own force in - his battle starts again with it.", v79, v75);
                        }
                    }
                    else
                    {
                        if ((int)g_5c44c != 5391687)
                        {
                            if ((int)g_5c44c == 1381192533)
                            {
                                v79 = "Soviets";
                            }
                            else
                            {
                                memcpy(&g_5b5c8, &v5, 4);
                                *((char **)&g_5b5cc) = 0;
                                v79 = &g_5b5c8;
                            }
                        }
                        $sprintf_s@$0NM(&v23, "You command the %s. Your own force may cost as much as the host's - he has not chosen his yet.", v79);
                    }
                    *((char **)&v1) = 0;
                    text(&v23);
                    goto LABEL_259d0;
                }
            }
            else
            {
LABEL_255f8:
                v76 = (g_5c2d0 != 0 ? "Battle received. Starting..." : "Operational map received. Opening it...");
            }
        }
        else
        {
            if (!(unsigned int)v75)
                strcpy_s(&v23, 220, "Choose your force on the Skirmish screen: what it costs is the most the other player's own force may cost.");
            else
                $sprintf_s@$0NM(&v23, "Your force costs %u: the other player's own force may cost as much.", v75);
            *((char **)&v1) = 0;
            text(&v23);
            if (g_5c149 == 0)
                goto LABEL_25712;
LABEL_255ed:
            if (!(v4 == 0))
                goto LABEL_255f8;
LABEL_259d0:
            if (g_5c149)
            {
                if (v48 == 2)
                {
                    a2 = g_5c130;
                    v80 = g_5c138;
                    xmm8<16> = ((xmm6<16> * 0x43a10000<128>) + Conv(32->128, Load(addr=stack_base-760, size=4, endness=Iend_LE)))
                    if (g_5c130 == g_5c138)
                    {
                        *((char **)&v1) = 0;
                        text("No multiplayer operation saves yet. One is written every time an operation battle starts.");
                        v80 = g_5c138;
                        a2 = g_5c130;
                    }
                    v83 = v80 - a2;
                    v84 = 0;
                    v85 = 7378697629483820647 * v83 >> 64 >> 6;
                    v86 = v85 / 0x8000000000000000;
                    tmp_25 = v85;
                    a1 = v85 + v86;
                    if (tmp_25 + v86)
                    {
                        v87 = 0;
                        do
                        {
                            v93 = a2 + v87;
                            v69 = &(64)[v93];
                            $sprintf_s@$0NM(&v23, "%s  -  %s", v93, v69);
                            *((char **)&v1) = 0;
                            text(&v23);
                            (char)v94 = button("RESTORE", "%s  -  %s", v93, v69, (unsigned int)v38 * 0x41f00000, 0x41600000);
                            if ((char)v94)
                            {
                                *((char **)&g_5c8b8) = 11;
                                g_5c8bc = v84;
                            }
                        } while ((v81 += v38 * 0x42080000, a2 = g_5c130, v84 += 1, v87 += 160, a1 = ((unsigned long long)(7378697629483820647 * (g_5c138 - g_5c130) >> 64) >> 6) + ((unsigned long long)(7378697629483820647 * (g_5c138 - g_5c130) >> 64) >> 6 >> 63), v84 < a1));
                        *((unsigned int *)&v42) = v18;
                    }
                }
                else if ((char)button("SWAP SIDES", a1, a2, v69, (unsigned int)v42, 0x41d00000))
                {
                    *((char **)&g_5c8b8) = 8;
                }
                v95 = (v48 == 3 ? "SET UP THE BATTLE" : "CHOOSE THE MISSION");
                if (v48 == 2)
                    v95 = "CHOOSE THE OPERATION";
                if ((char)button(v95, a1, a2, v69, a4, v33))
                    *((char **)&g_5c8b8) = 7;
                if ((char)button("LEAVE THE LOBBY", a1, a2, v69, (unsigned int)v42, 0x41d00000))
                    *((char **)&g_5c8b8) = 6;
                v76 = &g_5c7e0;
            }
            else
            {
                if (g_5c160 != 3)
                {
                    if (v48 - 2 > 1 && (char)v52)
                    {
                        *((char *)&v71) = 1;
                        goto LABEL_25dd1;
                    }
                    else if (v48 == 2 || (*((char *)&v71) = 0, !(char)v52))
                    {
                        *((char *)&v71) = 0;
                        if ((char)button((g_5c2c5 != 0 ? "READY" : "NOT READY"), a1, a2, v69, (unsigned int)v42, 0x41d00000))
                            *((char **)&g_5c8b8) = 9;
                    }
                    else
                    {
LABEL_25dd1:
                        if ((char)button((g_5c2c5 != 0 ? "READY" : "NOT READY"), a1, a2, v69, (unsigned int)v42, 0x41d00000))
                            *((char **)&g_5c8b8) = 9;
                        if ((char)button("SWAP SIDES", a1, a2, v69, (unsigned int)v42, 0x41d00000))
                            *((char **)&g_5c8b8) = 8;
                        if ((char)v71 && (char)button("CHOOSE MY FORCE", a1, a2, v69, (unsigned int)v42, 0x41d00000))
                            *((char **)&g_5c8b8) = 16;
                    }
                    if (g_5c150)
                    {
                        strcpy_s(&v26, 120, "No answer yet. Steam is looking for a way to the host's game; that can take a moment.");
                    }
                    else
                    {
                        if (!g_5c8b0)
                            v96 = 1943;
                        else
                            (unsigned int)v96 = strtoul(&g_5c8b0, 0, 10);
                        $sprintf_s@$0BAE$sprintf_s@$0BAE(&v26, "No answer yet. Check the address and the port, and that UDP port %u reaches the host.", v96);
                    }
                    if (!(char)v52)
                        v97 = GetTickCount() - g_5c424;
                    *((char **)&v1) = 0;
                    v90 = 4287241232;
                    text();
                }
                else
                {
                    v82 = 0;
                    v69 = g_5c2d0;
                    if (g_5c3e8 != g_5c3f0)
                    {
                        tmp_0 = DivV(tmp_2, tmp_1);
                        v82 = tmp_0;
                    }
                    v88 = v82 * 0x42c80000;
                    v89 = &v23;
                    v90 = (g_5c2d0 ? "Receiving the battle...  %d%%" : "Receiving the operational map...  %d%%");
                    a2 = (unsigned int)v88;
                    $sprintf_s@$0NM();
                    *((char **)&v1) = 0;
                    text(&v23);
                }
                if ((char)button("LEAVE THE LOBBY", v90, a2, v69, (unsigned int)v42, 0x41d00000))
                {
                    *((char **)&g_5c8b8) = 6;
                    return security_cookie_check(v27 ^ &v0);
                }
                return security_cookie_check(v27 ^ &v0);
            }
        }
        *((char **)&v1) = 0;
        text(v76);
        return security_cookie_check(v27 ^ &v0);
    }
}


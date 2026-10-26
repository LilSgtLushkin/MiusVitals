// v20 RVA 026470..0270a9  (3129 bytes)  sub_26470
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long sub_26470()
{
    char v0;  // [bp-0x188]
    int tmp_171;  // tmp #171
    int tmp_173;  // tmp #173
    int tmp_172;  // tmp #172
    int tmp_45;  // tmp #45
    int tmp_47;  // tmp #47
    int tmp_46;  // tmp #46
    int tmp_59;  // tmp #59
    int tmp_61;  // tmp #61
    int tmp_60;  // tmp #60
    int tmp_6;  // tmp #6
    int tmp_8;  // tmp #8
    int tmp_7;  // tmp #7
    int tmp_95;  // tmp #95
    int tmp_97;  // tmp #97
    int tmp_96;  // tmp #96
    int tmp_107;  // tmp #107
    int tmp_109;  // tmp #109
    int tmp_108;  // tmp #108
    unsigned long v1;  // [bp-0x168], Other Possible Types: unsigned int
    void* v2;  // [bp-0x160], Other Possible Types: unsigned long, unsigned int
    char v3;  // [bp-0x158]
    unsigned int v4;  // [bp-0x148]
    unsigned int v5;  // [bp-0x144]
    unsigned int v6;  // [bp-0x140]
    unsigned int v8;  // [bp-0x138]
    unsigned int v9;  // [bp-0x134]
    unsigned int v10;  // [bp-0x130]
    unsigned int v11;  // [bp-0x12c]
    unsigned int v12;  // [bp-0x128]
    unsigned int v13;  // [bp-0x124]
    unsigned int v14;  // [bp-0x120]
    unsigned int v15;  // [bp-0x11c]
    char v16;  // [bp-0x118]
    unsigned long long v17;  // [bp-0xd8]
    int v18;  // [bp-0x88]
    int v19;  // [bp-0x78]
    int v20;  // [bp-0x68]
    int v22;  // xmm9
    int v23;  // xmm10
    int v24;  // xmm11
    unsigned int v25;  // xmm15
    int v26;  // xmm8
    int v28;  // xmm12
    int v29;  // xmm14
    int v35;  // xmm10
    int v36;  // xmm14
    unsigned long long v37;  // rcx, Other Possible Types: unsigned long
    char v38;  // bl
    int v39;  // xmm0
    unsigned long long v40;  // rcx
    void* v41;  // r14, Other Possible Types: unsigned long
    unsigned int v42;  // r12d
    unsigned long long v43;  // rsi
    unsigned long long v44;  // rbx
    unsigned long long v45;  // rdx
    unsigned long long v47;  // rcx
    unsigned long long v48;  // r9
    unsigned long v49;  // r8
    unsigned long long v50;  // rdx
    struct_0 *v51;  // rcx
    unsigned long v52;  // rdx
    unsigned long long v53;  // rcx

    v20 = v22;
    v19 = v23;
    v18 = v24;
    v17 = g_5a380 ^ &v0;
    v25 = 0;
    v25 = 0x3f000000;
    v26 = 0;
    *((unsigned int *)&v26) = g_5cbd8;
    v15 = (0 CONCAT g_5caa8) * v25;
    v9 = (unsigned int)v26 * 0x44390000;
    tmp_171 = SubV(tmp_173, tmp_172);
    *((int128_t *)&v14) = tmp_171;
    v1 = 2953842704;
    quad();
    v1 = -152444204;
    quad();
    v2 = -10858168;
    v13 = (unsigned int)v26 * 0x40400000;
    v1 = v13;
    frame();
    v28 = v26 * 0x440c0000;
    v29 = v26 * 0x42980000;
    if (g_5c4b0 == 1)
    {
        *((char **)&v3) = 1;
        *((char **)&v2) = 1;
        v1 = -14014942;
        text("MULTIPLAYER");
        *((char **)&v3) = 0;
        *((char **)&v2) = 1;
        v1 = -10858168;
        text("Two players: one hosts, the other joins");
        v2 = 0x41d00000;
        v1 = (unsigned int)v29;
        if ((char)button("HOST A SKIRMISH"))
            *((char **)&g_5c8b8) = 2;
        v2 = 0x41d00000;
        v1 = (unsigned int)v29;
        if ((char)button("HOST A CUSTOM MISSION"))
            *((char **)&g_5c8b8) = 3;
        v2 = 0x41d00000;
        v1 = (unsigned int)v29;
        if ((char)button("HOST AN OPERATION"))
            *((char **)&g_5c8b8) = 4;
        v2 = 0x41d00000;
        v1 = (unsigned int)v29;
        if ((char)button("JOIN A GAME"))
        {
            *((char **)&g_5c7e0) = 0;
            *((char **)&g_5c7dc) = 0;
            *((char **)&g_5c4b0) = 2;
        }
        v2 = 0x41d00000;
        v1 = (unsigned int)v29;
        if ((char)button("BACK"))
            *((char **)&g_5c8b8) = 10;
        *((char **)&v3) = 0;
        *((char **)&v2) = 0;
        v1 = -14014942;
        tmp_45 = SubV(tmp_47, tmp_46);
        tmp_59 = SubV(tmp_61, tmp_60);
        *((int128_t *)&v4) = tmp_59;
        text("Host port (UDP)");
        *((char **)&v3) = 1;
        v2 = "1943";
        v1 = &g_5c8a8;
        button("Host port (UDP)");
        tmp_6 = SubV(tmp_8, tmp_7);
        xmm10<16> = ((t6 * xmm15<16>) + ((xmm8<16> * 0x44210000<128>) + Conv(32->128, Load(addr=stack_base-288, size=4, endness=Iend_LE))))
        if ((CmpF(g_5caa0, (unsigned long long)(v26 * 0x43a10000 + tmp_45)) & 1) || (((char)(CmpF((unsigned int)v36, g_5caa0) & 69) | (char)((CmpF((unsigned int)v36, g_5caa0) & 69) >> 6)) & 1) == 1 || (CmpF(g_5caa4, (unsigned int)v35) & 1) || (v38 = 1, v39 = v26 * 0x42080000 + v35, (((char)(unsigned long long)(CmpF((unsigned long long)(unsigned int)v39, (unsigned long long)g_5caa4) & 69) | (char)((unsigned long long)(CmpF((unsigned long long)(unsigned int)v39, (unsigned long long)g_5caa4) & 69) >> 6)) & 1) == 1))
            v38 = 0;
        *((char **)&v1) = (-(0 < v38) & -1304) - 1040;
        quad(v1);
        v2 = -10858168;
        v1 = (unsigned int)(v26 + v26);
        frame(v40);
        if (g_5c469)
        {
            *((char **)&v3) = 1;
            *((char **)&v2) = 1;
            v1 = -14014942;
            text("X");
        }
        *((char **)&v3) = 0;
        *((char **)&v2) = 0;
        v1 = -14014942;
        text("Open it by UPnP");
        if (v38 && g_5c46c)
        {
            g_5c469 = g_5c469 == 0;
            ui_start();
            v52 = (g_5c469 != 0 ? "off" : "on");
            v53 = "ui: UPnP %s";
            mp_log();
        }
    }
    else if (g_5c4b0 == 2)
    {
        *((char **)&v3) = 1;
        *((char **)&v2) = 1;
        v1 = -14014942;
        v8 = (unsigned int)v26 * 0x42080000;
        text("JOIN A GAME");
        *((char **)&v3) = 0;
        *((char **)&v2) = 0;
        v5 = (unsigned int)v26 * 0x428c0000;
        v1 = -10858168;
        v11 = (unsigned int)v26 * 0x430c0000;
        s_148 =L Conv(128->32, ((xmm8<16> * 0x40c00000<128>) + ((xmm8<16> * 0x43160000<128>) + Conv(32->128, Load(addr=stack_base-288, size=4, endness=Iend_LE)))))<4>
        v12 = (unsigned int)v26 * 0x43c80000;
        tmp_95 = SubV(tmp_97, tmp_96);
        *((int128_t *)&v10) = tmp_95;
        s_13c =L Conv(128->32, ((xmm12<16> * xmm15<16>) + Conv(32->128, Load(addr=stack_base-284, size=4, endness=Iend_LE))))<4>
        tmp_107 = SubV(tmp_109, tmp_108);
        *((int128_t *)&v6) = tmp_107;
        text("Host address (type it, or Ctrl+V to paste)");
        *((char **)&v3) = 0;
        *((char **)&v2) = 0;
        v1 = -10858168;
        text("Port");
        v3 = g_5c7dc == 0;
        v2 = 0;
        v1 = &g_5c690;
        if ((char)button("Port"))
            *((char **)&g_5c7dc) = 0;
        *((char **)&v3) = 0;
        *((char **)&v2) = 1;
        v1 = -14014942;
        text(":");
        v3 = g_5c7dc == 1;
        v2 = "1943";
        v1 = &g_5c8b0;
        if ((char)button(":"))
            *((char **)&g_5c7dc) = 1;
        v2 = 0x41d00000;
        v1 = (unsigned int)v29;
        if ((char)button("CONNECT"))
            *((char **)&g_5c8b8) = 5;
        v2 = 0x41d00000;
        v1 = (unsigned int)v29;
        if ((char)button("BACK"))
            *((char **)&g_5c8b8) = 10;
        v1 = -10858168;
        quad();
        *((char **)&v3) = 1;
        *((char **)&v2) = 0;
        v1 = -14014942;
        text("STEAM FRIENDS HOSTING A GAME");
        if (!(char)steam_init("STEAM FRIENDS HOSTING A GAME"))
        {
            v37 = "Steam is not available to this game.";
        }
        else
        {
            if (!(g_5c7d4 == 0))
                goto LABEL_26dd3;
            *((char **)&v3) = 0;
            *((char **)&v2) = 0;
            v1 = -10858168;
            text("None of your Steam friends is hosting right now.");
            v37 = "A friend who hosts shows up here; joining him needs no address and no port.";
        }
        *((char **)&v3) = 0;
        *((char **)&v2) = 0;
        v1 = -10858168;
        text(v37);
LABEL_26dd3:
        v41 = 0;
        if (g_5c7d4 > 0)
        {
            v42 = 0;
            v11 = (unsigned int)v26 * 0x42100000;
            s_138 =L Conv(128->32, ((0x0<96> CONCAT Load(addr=stack_base-312, size=4, endness=Iend_LE)) + ((xmm8<16> * 0x43e00000<128>) + Conv(32->128, Load(addr=stack_base-288, size=4, endness=Iend_LE)))))<4>
            v10 = (unsigned int)v26 * 0x42f00000;
            do
            {
                v43 = v41 * 104;
                v44 = &(&g_5c908)[v43];
                v45 = "operation";
                v47 = v44;
                v48 = (strcmp(v44, "mission") == 0 ? v44 : "custom mission");
                v49 = (strcmp() == 0 ? "a" : &g_4c150);
                v50 = "hosts %s %s";
                v51 = &v16;
                $sprintf_s@$0EA();
                *((char **)&v3) = 1;
                *((char **)&v2) = 0;
                v1 = -14014942;
                text(&(&g_5c8c8)[v43]);
                *((char **)&v3) = 0;
                *((char **)&v2) = 0;
                v1 = -10858168;
                text(&v16);
                v2 = 0x41800000;
                v1 = v11;
                if ((char)button("JOIN"))
                {
                    *((char **)&g_5c8b8) = 15;
                    g_5c8bc = v41;
                }
                v41 = &(1)[v41];
                v42 = &(44)[v42];
            } while ((unsigned int)v41 < g_5c7d4);
        }
    }
    else if (g_5c4b0 == 5)
    {
        draw_map_wait();
        return security_cookie_check(v17 ^ &v0);
    }
    else
    {
        v1 = v9;
        sub_24be4();
        return security_cookie_check(v17 ^ &v0);
    }
    *((char **)&v3) = 0;
    *((char **)&v2) = 1;
    v1 = -7726064;
    text(&g_5c7e0);
    return security_cookie_check(v17 ^ &v0);
}


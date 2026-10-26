// v20 RVA 03f75c..03fa7e  (802 bytes)  steam_init
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long steam_init()
{
    char v0;  // [bp-0x68]
    unsigned long v1;  // [bp-0x40]
    char v2;  // [bp-0x38]
    unsigned long long v3;  // [bp-0x18]
    unsigned long long v6;  // rax
    unsigned long long v7;  // rax
    unsigned long long v9;  // rdx
    unsigned long long v11;  // rcx

    v3 = g_5a380 ^ &v0;
    if (g_5dee0)
        return security_cookie_check(v3 ^ &v0);
    *((char **)&g_5dee0) = 2;
    if (GetEnvironmentVariableA("MP_STEAM", &v2, 32) && !atoi(&v2))
    {
        mp_log("steam: switched off (MP_STEAM=0)");
        goto LABEL_3fa69;
    }
    else
    {
        if (GetEnvironmentVariableA("MP_STEAM_FAKE", &v2, 32) && _strtoui64(&v2, 0, 10))
        {
            g_5df68 = _strtoui64(&v2, 0, 10);
            if ((char)fake_steam_open("MP_STEAM_FAKE", 0, 10))
            {
                g_5de88 = g_5df68;
                $sprintf_s@$0EA(&g_5dea0, "fake%llu", g_5df68);
                *((char **)&g_5dee0) = 1;
                mp_log("steam: TEST STAND-IN for Steam: this game is 'steam:%llu', its datagrams travel over UDP on this PC", g_5df68);
            }
            else
            {
                mp_log("steam: test stand-in: UDP port %llu is taken", &(&g_9c40)[g_5df68]);
LABEL_3fa69:
                goto LABEL_3fa69;
            }
        }
        else
        {
            v6 = GetModuleHandleA("steam_api64.dll");
            if (!v6)
                mp_log("steam: the game has not loaded Steam's library - no Steam joining");
            if (!(char)steam_bind_interfaces(v6))
            {
LABEL_3fa69:
                return security_cookie_check(v3 ^ &v0);
            }
            g_5de68 = g_5ddc0();
            g_5de70 = g_5ddc8();
            g_5de78 = g_5ddd0();
            g_5de80 = g_5ddd8();
            if (!(g_5de68) || !(g_5de70) || !(g_5de78) || !(g_5de80))
                mp_log("steam: Steam is not running for this game - no Steam joining");
            g_5de88 = g_5dde0();
            g_5de90 = g_5ddf0(g_5de80);
            (unsigned int)v7 = g_5ddf8(g_5de68);
            strncpy_s(&g_5dea0, 64, (v7 != 0 ? &g_45ce6 : v7), 63);
            if (!g_5de88)
                mp_log("steam: no Steam user - no Steam joining");
            g_5de58(g_5de78, 1);
            g_5de60(&g_5a308, 1202);
            g_5de60(&g_5a318, 1203);
            g_5de60(&g_5a328, 336);
            g_5de60(&g_5a338, 337);
            g_5de10(g_5de68, g_5de88);
            *((char **)&g_5dee0) = 1;
            v9 = 4;
            v11 = g_5de68;
            *((int *)&v1) = ((char)g_5dde8(g_5de70) ? &g_4d608 : "yes");
            mp_log("steam: ready - user %llu (%s), application %u, %d friends, logged on: %s", g_5de88, &g_5dea0, g_5de90, g_5de18());
        }
        return security_cookie_check(v3 ^ &v0);
    }
}


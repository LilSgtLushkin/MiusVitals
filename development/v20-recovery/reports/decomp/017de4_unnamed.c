// v20 RVA 017de4..017efa  (278 bytes)  explode_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long explode_hook(unsigned long long a0)
{
    char v0;  // [bp-0x148]
    char v1;  // [bp-0x128]
    unsigned long long v2;  // [bp-0x18]

    v2 = g_5a380 ^ &v0;
    if (!a0)
    {
LABEL_17e2a:
        if (g_5b7c8 != g_5b7d0)
            g_5b7d0 = g_5b7c8;
    }
    else if (g_5bfb0 || g_5b7c8 != g_5b7d0)
    {
        mp_log("lobby: the other player's own force is dropped (%s)", a0);
        goto LABEL_17e2a;
    }
    if (g_5b6b8 != g_5b6c0)
        g_5b6c0 = g_5b6b8;
    if (g_5b710 != g_5b718)
        g_5b718 = g_5b710;
    *((char **)&g_5bfb4) = 0;
    *((char **)&g_5bfb0) = 0;
    *((char **)&g_5bf3c) = 0;
    *((char **)&g_5bf38) = 0;
    *((char **)&g_5bfbc) = 0;
    *((char **)&g_5bfb8) = 0;
    SetEnvironmentVariableA("MP_PEER_FORCE", 0);
    sprintf_s(&v1, 260, "%s\\users\\mp_peer_force.cfgpack", &g_5b920);
    DeleteFileA(&v1);
    return security_cookie_check(v2 ^ &v0);
}


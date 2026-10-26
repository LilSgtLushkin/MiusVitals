// v20 RVA 037f98..0382ab  (787 bytes)  trench_install
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long trench_install()
{
    char v0;  // [bp-0x48]
    char v1;  // [bp-0x28]
    unsigned long long v2;  // [bp-0x18]
    unsigned long long v4;  // rdi
    char v5;  // bl
    unsigned long long v6;  // rbx
    unsigned long long v7;  // rax, Other Possible Types: unsigned long
    unsigned long v8;  // rdx
    unsigned long long v9;  // rcx
    unsigned long long v10;  // rbx
    unsigned long long v11;  // rbx
    unsigned long long v12;  // rax
    unsigned int v13;  // r8d
    unsigned long v14;  // rdx
    unsigned long long v15;  // rcx
    unsigned long long v16;  // rdx
    unsigned long v17;  // rdx
    unsigned long long v18;  // rcx
    unsigned long long v19;  // rdx

    v2 = g_5a380 ^ &v0;
    if (g_5d3b1)
        return security_cookie_check(v2 ^ &v0);
    v4 = GetModuleHandleA("adv_game.codelib");
    if (!v4)
        return security_cookie_check(v2 ^ &v0);
    v5 = 1;
    *((char **)&g_5d3b1) = 1;
    g_5d278 = GetCurrentThreadId();
    if (GetEnvironmentVariableA("MP_SYNC_TRENCH", &v1, 16))
        g_5a2ed = atoi(&v1) != 0;
    if (!GetEnvironmentVariableA("MP_LOG_TRENCH", &v1, 16) || !atoi(&v1))
        v5 = 0;
    g_5d0eb = v5;
    if (!g_5a2ed && !v5)
    {
        mp_log("trench: each game digs for itself (MP_SYNC_TRENCH=0)");
        return security_cookie_check(v2 ^ &v0);
    }
    if (GetEnvironmentVariableA("MP_TRACE_PLACES", &v1, 16) && atoi(&v1))
    {
        v6 = v4 + 919952;
        if (memcmp(v6, &g_4da28, 19))
        {
            v7 = g_5d2f8;
        }
        else
        {
            (unsigned int)v7 = install_detour(v6, 19, send_table);
            g_5d2f8 = v7;
        }
        v8 = (v7 != 0 ? "could not be traced" : "counted by where they come from");
        v9 = "places: %s";
        mp_log();
    }
    v10 = v4 + 900128;
    if (memcmp(v10, &g_4da10, 18))
    {
        mp_log("trench: unknown game build, each game digs for itself");
    }
    else
    {
        g_5d218 = install_detour(v10, 14, dig_hook);
        v11 = v4 + 903584;
        if (g_5d218 && g_5a2ed)
        {
            (unsigned int)v12 = memcmp(v11, &g_4da40, 21);
            if (!(unsigned int)v12)
                g_5d220 = install_detour(v11, &(17)[v12], dig_hook);
        }
        v13 = 16;
        if (GetEnvironmentVariableA("MP_SYNC_CRATERS", &v1, 16))
            g_5a2ef = atoi(&v1) != 0;
        if (g_5a2ed && !memcmp(v4 + 911136, &g_4da58, 22))
            g_5d2c0 = install_detour(v4 + 911136, 18, dig_hook);
        v14 = (g_5a2ef != 0 ? "each game makes its own (MP_SYNC_CRATERS=0)" : "the ground is the host's: its craters are sent and made the same here");
        v15 = "crater: %s";
        if (!g_5d2c0)
            v16 = "each game makes its own";
        mp_log();
        v17 = (g_5a2ed != 0 ? "each game digs for itself (MP_SYNC_TRENCH=0); digs are logged" : "a squad's trenches are its owner's: dug there, sent, and dug the same here");
        v18 = "trench: %s";
        if (!g_5d218)
            v19 = "could not be hooked";
        mp_log();
        if (g_5d218)
            return security_cookie_check(v2 ^ &v0);
    }
    *((char **)&g_5a2ed) = 0;
    return security_cookie_check(v2 ^ &v0);
}


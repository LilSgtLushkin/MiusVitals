// v20 RVA 0347c4..034af1  (813 bytes)  shots_install
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long shots_install(unsigned long a0)
{
    char v0;  // [bp-0x168]
    unsigned long v1;  // [bp-0x148]
    unsigned long v2;  // [bp-0x140]
    char v3;  // [bp-0x138]
    char v4;  // [bp-0x128]
    unsigned long long v5;  // [bp-0x18]
    unsigned long long v7;  // rbx
    unsigned long long v8;  // rax, Other Possible Types: unsigned long
    unsigned long long v9;  // rdi
    unsigned long long v10;  // rbx
    unsigned long long v11;  // rdi
    unsigned long long v13;  // rcx
    unsigned long v15;  // r9
    unsigned long v16;  // r8
    unsigned long v17;  // rdx
    unsigned long long v18;  // rdx

    v5 = g_5a380 ^ &v0;
    if (g_5d0c8)
        return security_cookie_check(v5 ^ &v0);
    v7 = GetModuleHandleA("basic_physic.codelib");
    if (!v7)
        return security_cookie_check(v5 ^ &v0);
    *((char **)&g_5d0c8) = 1;
    g_5d000 = a0;
    g_5d020 = GetCurrentThreadId();
    if (GetEnvironmentVariableA("MP_SYNC_SHOTS", &v3, 16))
        g_5a2ec = atoi(&v3) != 0;
    if (!GetEnvironmentVariableA("MP_LOG_SHOTS", &v3, 16) || !atoi(&v3))
    {
        v8 = g_5d028;
    }
    else
    {
        $sprintf_s@$0BAE$sprintf_s@$0BAE(&v4, "%s\\mp_shots_%lu.txt", &g_5a570, GetCurrentProcessId());
        (unsigned int)v8 = _fsopen(&v4, "w", 64);
        g_5d028 = v8;
    }
    if (!g_5a2ec && !v8)
    {
        mp_log("shots: each game fires for itself (MP_SYNC_SHOTS=0)");
        return security_cookie_check(v5 ^ &v0);
    }
    v9 = &(&g_18430)[v7];
    if (memcmp(v9, &g_4ce48, 21))
    {
        mp_log("shots: unknown game build, each game fires for itself");
    }
    else
    {
        g_5cfe0 = install_detour(v9, 21, launch_hook);
        if (!memcmp(&(&g_19260)[v7], &g_4ce60, 16))
            g_5cfe8 = install_detour(&(&g_19260)[v7], 16, launch_hook);
        v10 = GetModuleHandleA("adv_physic.codelib");
        v11 = GetModuleHandleA("adv_game.codelib");
        if (g_5cfe8 && v10 && !memcmp(v10 + 636192, &g_4ce70, 17))
            g_5cff0 = install_detour(v10 + 636192, 17, hook_GetAsyncKeyState);
        if (v11 && !memcmp(&(&g_44c40)[v11], &g_4ce88, 14))
            g_5cff8 = install_detour(&(&g_44c40)[v11], 14, launch_hook);
        *((int *)&v2) = (g_5cff8 != 0 ? "NOT hooked" : "hooked");
        v13 = "shots: %s%s; explosions %s (vehicles blown up by their crews: %s), flares %s; off-map fire goes by the side it is fired for";
        *((int *)&v1) = (g_5cff0 != 0 ? "not known" : "known");
        v15 = (g_5cfe8 != 0 ? "NOT hooked" : "hooked");
        v16 = (g_5d028 != 0 ? &g_45ce6 : ", every launch is written to mp_shots");
        v17 = (g_5a2ec != 0 ? "watched only" : "a unit's shots are its owner's: sent to the other game, not made up locally");
        if (!g_5cfe0)
            v18 = "could not be hooked";
        mp_log();
        if (g_5cfe0)
            return security_cookie_check(v5 ^ &v0);
    }
    *((char **)&g_5a2ec) = 0;
    return security_cookie_check(v5 ^ &v0);
}


// v20 RVA 03cedc..03d0a8  (460 bytes)  trench_install
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long trench_install(unsigned long a0)
{
    char v0;  // [bp-0x48]
    char v1;  // [bp-0x28]
    unsigned long long v2;  // [bp-0x18]
    char v4;  // bl
    char v5;  // al
    unsigned long long v6;  // rax
    unsigned long long *v7;  // rbp
    unsigned long long *v8;  // rsi
    unsigned long long v9;  // rdx
    unsigned long long v10;  // r8
    unsigned long long v11;  // rcx, Other Possible Types: unsigned long
    unsigned long v12;  // rdx
    unsigned long long v13;  // rcx

    v2 = g_5a380 ^ &v0;
    v4 = 1;
    g_5db08 = a0;
    *((char **)&g_5db11) = 1;
    g_5db14 = GetCurrentThreadId();
    if (GetEnvironmentVariableA("MP_SYNC_HITS", &v1, 16))
        g_5a2f2 = atoi(&v1) != 0;
    if (!GetEnvironmentVariableA("MP_LOG_HITS", &v1, 16) || !atoi(&v1))
        v5 = 0;
    else
        v5 = 1;
    g_5db12 = v5;
    if (!g_5a2f2)
    {
        v11 = "hit: each game works out every hit for itself (MP_SYNC_HITS=0)";
    }
    else
    {
        v6 = GetModuleHandleA("adv_physic.codelib");
        if (!v6)
            return security_cookie_check(v2 ^ &v0);
        InitializeCriticalSection(&g_5db18.DebugInfo);
        v7 = v6 + 817688;
        v8 = v6 + 812960;
        if ((char)readable(v7, 8) && (char)readable(v8, 8))
        {
            v9 = &(&g_43230)[v6];
            if (*(v7) == v9 && *(v8) == g_5db00)
            {
                v10 = send_to;
                g_5daf8 = v9;
                g_5db00 = v6 + 421056;
                if (!(char)find_main_window(v7) || !(char)find_main_window(v8, g_5db00, $_Uninitialized_value_construct_n_unchecked1@PEAPEAUPending))
                {
                    v4 = 0;
                    *((char **)&g_5a2f2) = 0;
                }
                g_5db10 = v4;
                v12 = (v4 ? "could not be hooked, each game works out every hit for itself" : "a hit on a unit is its owner's game's: worked out there, sent, and carried out the same here");
                v13 = "hit: %s";
                mp_log();
                return security_cookie_check(v2 ^ &v0);
            }
        }
        *((char **)&g_5a2f2) = 0;
        v11 = "hit: unknown game build, each game works out every hit for itself";
    }
    mp_log(v11);
    return security_cookie_check(v2 ^ &v0);
}


// v20 RVA 0229e8..022c82  (666 bytes)  shots_install
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long shots_install(struct struct_1 **a0)
{
    char v0;  // [bp-0x48]
    unsigned int v1;  // [bp-0x28]
    char v2;  // [bp-0x20]
    unsigned long long v3;  // [bp-0x18]
    unsigned int v5;  // rax, Other Possible Types: unsigned long
    unsigned long long v6;  // rax
    unsigned int v7;  // eax
    unsigned int v8;  // edi
    unsigned long long v9;  // rax
    char *v10;  // rbx
    unsigned long v11;  // rdx
    unsigned long long v12;  // rcx

    v3 = g_5a380 ^ &v0;
    if (!g_5b618)
    {
        return security_cookie_check(v3 ^ &v0);
    }
    else if (!g_5b8e0)
    {
        return security_cookie_check(v3 ^ &v0);
    }
    else if (g_5a258 != 2)
    {
        if (g_5a25c < 0 && g_5b8e0 == 1 && g_5b619 && g_5bf28 > 0)
        {
            g_5a25c = menu_controller_of(a0);
            if (g_5a25c > 0)
            {
                if (!GetEnvironmentVariableA("MP_AI_ALLIES", &v2, 8) || ((unsigned int)v5 = atoi(&v2), (unsigned int)v5))
                    v5 = g_5a25c;
                else
                    g_5a25c = v5;
                if ((unsigned int)v5 > 0)
                {
                    v6 = GetModuleHandleA("adv_game.codelib");
                    if (!v6 || !(char)$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(v6))
                    {
                        *((char **)&g_5a25c) = 0;
                        mp_log("net: unknown game build, the computer-led formations on this player's side are not led");
                    }
                    else
                    {
                        *((char **)&g_5b89a) = 1;
                        mp_log("net: %d computer-led formations on this player's side: the AI commander leads them, and only them", g_5a25c);
                    }
                }
            }
        }
        v7 = g_5a260;
        if (g_5a260 < 0)
        {
            if (!GetEnvironmentVariableA("MP_TEST_AI_OFF", &v2, 8) || !atoi(&v2))
                v7 = 0;
            else
                v7 = 1;
            g_5a260 = v7;
        }
        if (g_5b619 && !g_5b89a || g_5b8e0 == 1 && !g_5b61b && g_5beb4 < 2 || v7)
            v8 = 1;
        else
            v8 = 0;
        if (!g_5b619)
            *((char **)&g_5b89a) = 0;
        if (g_5a258 == v8)
            return security_cookie_check(v3 ^ &v0);
        v9 = GetModuleHandleA("adv_game.codelib");
        if (!v9)
            return security_cookie_check(v3 ^ &v0);
        v10 = v9 + 781360;
        if ((*(v10) == 64 || *(v10) == 195) && !memcmp(v10 + 1, &g_47858, 8))
        {
            if (VirtualProtect(v10, 1, 64, &v1))
            {
                *(&v10) = &(64)[-(0 < v8) & 131];
                VirtualProtect(v10, 1, v1, &v1);
                FlushInstructionCache(GetCurrentProcess(), v10, 1);
                g_5a258 = v8;
                v11 = (v8 != 0 ? "on" : "off");
                v12 = "net: AI commander switched %s";
                mp_log();
                return security_cookie_check(v3 ^ &v0);
            }
            return security_cookie_check(v3 ^ &v0);
        }
        *((char **)&g_5a258) = 2;
        mp_log("net: unknown game build, the AI commander stays active");
        return security_cookie_check(v3 ^ &v0);
    }
    else
    {
        return security_cookie_check(v3 ^ &v0);
    }
}


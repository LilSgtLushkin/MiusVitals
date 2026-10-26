// v20 RVA 012354..012611  (701 bytes)  install_detours
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long install_detours()
{
    char v0;  // [bp-0x68]
    int tmp_9;  // tmp #9
    int tmp_11;  // tmp #11
    int tmp_10;  // tmp #10
    int tmp_4;  // tmp #4
    int tmp_6;  // tmp #6
    int tmp_5;  // tmp #5
    char v1;  // [bp-0x48]
    unsigned long long v2;  // [bp-0x38]
    unsigned long long v4;  // rbx
    unsigned long long v5;  // rax
    unsigned long long v6;  // r14
    unsigned long long v7;  // rsi
    unsigned long long v8;  // rbp
    unsigned long long v9;  // rdi
    int v10;  // xmm1
    unsigned long long v11;  // rax, Other Possible Types: unsigned long
    int v13;  // xmm6
    unsigned long v14;  // r9
    void* v15;  // r9
    unsigned long v16;  // r8
    unsigned long long v17;  // r8
    unsigned long long v18;  // rcx
    unsigned long v19;  // rdx

    v2 = g_5a380 ^ &v0;
    if (g_5c100)
    {
        return security_cookie_check(v2 ^ &v0);
    }
    else if (!g_5b618)
    {
        return security_cookie_check(v2 ^ &v0);
    }
    else if (g_5b8e0)
    {
        v4 = GetModuleHandleA("adv_interf.codelib");
        v5 = GetModuleHandleA("base_main.codelib");
        if (!v4)
        {
            return security_cookie_check(v2 ^ &v0);
        }
        else if (v5)
        {
            v6 = &(&g_3b3b0)[v4];
            *((char **)&g_5c100) = 1;
            v7 = &(&g_3b4f0)[v4];
            v8 = &(&g_41700)[v4];
            v9 = &(&g_4200)[v5];
            if (!memcmp(v6, &g_49780, 16) && !memcmp(v7, &g_497a8, 16) && !memcmp(v8, &g_497b8, 15))
            {
                g_5bee0 = install_detour(v7, 16, input_action_hook);
                g_5bee8 = install_detour(v8, 15, assign@?$vector@EV?$allocator@E@std);
                g_5bed8 = install_detour(v6, 16, start_action_hook);
            }
            if (memcmp(&(&g_57470)[v4], &g_49790, 19))
                mp_log("net: unknown game build, the other player's units can be selected");
            else
                g_5bff0 = install_detour(&(&g_57470)[v4], 19, _Unchecked_erase@?$_Hash@V?$_Umap_traits@_KUTrack);
            if (GetEnvironmentVariableA("MP_SIM_HZ", &v1, 16))
            {
                if (atoi(&v1) <= 0)
                {
                    v10 = 0;
                }
                else
                {
                    atoi(&v1);
                    tmp_9 = DivV(tmp_11, tmp_10);
                    v10 = tmp_9;
                }
                g_5a220 = (unsigned long long)v10;
            }
            if (memcmp(v9, &g_497c8, 15))
            {
                v11 = g_5bef0;
            }
            else
            {
                (unsigned int)v11 = install_detour(v9, 15, timer_step_hook);
                g_5bef0 = v11;
            }
            if (!v11 || (((char)(CmpF(g_5a220, 0) & 69) | (char)((CmpF(g_5a220, 0) & 69) >> 6)) & 1) == 1)
            {
                v15 = 0;
            }
            else
            {
                tmp_4 = DivV(tmp_6, tmp_5);
                v13 = tmp_4 + 0x3fe0000000000000;
                v14 = (unsigned long long)v13;
            }
            v16 = (g_5b8e0 == 2 ? "is this game's" : "follows the host");
            if (!v11)
                v17 = "not hooked (unknown game build)";
            v18 = "net: Start button %s, battle clock %s, simulation at %d steps per second";
            v19 = (g_5bed8 != 0 ? "not hooked (unknown game build)" : "shared between the players");
            mp_log();
            return security_cookie_check(v2 ^ &v0);
        }
        else
        {
            return security_cookie_check(v2 ^ &v0);
        }
    }
    else
    {
        return security_cookie_check(v2 ^ &v0);
    }
}


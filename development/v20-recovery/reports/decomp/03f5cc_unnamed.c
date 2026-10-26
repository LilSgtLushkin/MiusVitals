// v20 RVA 03f5cc..03f6b2  (230 bytes)  end_turn_here
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


int end_turn_here()
{
    unsigned long v0;  // [bp-0x8]
    unsigned long v1;  // [bp+0x8]
    unsigned long v2;  // [bp+0x10]
    char *v3;  // rcx
    unsigned long v4;  // rbx
    unsigned long v5;  // rsi
    unsigned long v6;  // rdi
    unsigned long long v7;  // rdi
    unsigned long long v8;  // rax
    unsigned long v9;  // rdx
    unsigned long v10;  // r8
    unsigned long long v11;  // rdx
    unsigned long long v12;  // rcx

    if (!v3)
        return;
    v1 = v4;
    v2 = v5;
    v0 = v6;
    v7 = strstr();
    if (v7 && !_strnicmp(v7 + strlen("+gtmf_join="), "steam:", 6))
    {
        (unsigned int)v8 = _strtoui64((unsigned long long)&(6)[strlen("+gtmf_join=")] + v7, 0, 10);
        if (v8)
        {
            $sprintf_s@$0BAE$sprintf_s@$0BAE(&g_5df38, "steam:%llu", v8);
            v10 = (v9 != 0 ? " (on the command line)" : " (a friend's game, while this game runs)");
            v11 = &g_5df38;
            v12 = "steam: Steam's Join Game asks to join %s%s";
            mp_log();
            return;
        }
        else if (!v3)
        {
            return;
        }
    }
    if (*(v3))
    {
        mp_log("steam: a join request that is not a multiplayer game's ('%.80s') - ignored", v3);
        return;
    }
    return;
}


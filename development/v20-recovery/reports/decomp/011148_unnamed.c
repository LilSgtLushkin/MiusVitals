// v20 RVA 011148..0112e8  (416 bytes)  host_listen
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long host_listen()
{
    char v0;  // [bp-0x68]
    char v1;  // [bp-0x38], Other Possible Types: unsigned long long
    unsigned int v2;  // [bp-0x34]
    char v3;  // [bp-0x30]
    unsigned long long v4;  // [bp-0x10]
    unsigned long v6;  // rdx
    char v7;  // rcx
    unsigned long long v8;  // rdx
    unsigned long long v9;  // rcx
    unsigned long long v10;  // rax
    unsigned long long v11;  // r9
    unsigned long v12;  // rdx
    unsigned long long v13;  // r8
    unsigned long long v14;  // rcx

    v4 = g_5a380 ^ &v0;
    v6 = (g_5b8e4 == 3 ? "skirmish" : "mission");
    v7 = "operation";
    if (g_5b8e4 == 2)
        v8 = "operation";
    v7 = 1;
    snapshot_operation_saves();
    v9 = "MP_STEAM_PEER";
    if (GetEnvironmentVariableA("MP_STEAM_PEER", &v3, 32) && (char)steam_init())
    {
        (unsigned int)v10 = _strtoui64(&v3, 0, 10);
        strncpy(&v1, "GTMP", 4);
        *((char **)&v2) = 3;
        v1 = *((long long *)&v1);
        hook_GetAsyncKeyState(rax<8>);
        mp_log(rax<8>, &v1, 8);
    }
    g_5b918 = GetTickCount();
    if (!(char)sub_1755c(g_5a168))
    {
        if (!g_5b61c)
            mp_log("net: UDP port %u is still in use, retrying", g_5a168);
        *((char **)&g_5b61c) = 1;
        return security_cookie_check(v4 ^ &v0);
    }
    else
    {
        v11 = g_5a168;
        v12 = (g_5b618 != 0 ? &g_45ce6 : "battle ");
        *((char **)&g_5b61c) = 0;
        v13 = "?";
        v14 = "net: hosting %son %s UDP %u";
        mp_log();
        if (!g_5b61d)
        {
            return security_cookie_check(v4 ^ &v0);
        }
        else if (g_5b900 != g_5a168)
        {
            g_5b900 = g_5a168;
            *((char **)&g_5b904) = 1;
            CloseHandle(CreateThread(0, 0, upnp_thread, 0, 0, 0));
            return security_cookie_check(v4 ^ &v0);
        }
        else
        {
            return security_cookie_check(v4 ^ &v0);
        }
    }
}


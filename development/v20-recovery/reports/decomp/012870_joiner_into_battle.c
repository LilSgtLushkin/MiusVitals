// v20 RVA 012870..0129c2  (338 bytes)  joiner_into_battle
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct PROCESS_INFORMATION {
    unsigned int *hProcess;
    unsigned int *hThread;
    unsigned int dwProcessId;
    unsigned int dwThreadId;
} PROCESS_INFORMATION;


long long joiner_into_battle()
{
    char v0;  // [bp-0x328]
    PROCESS_INFORMATION v1;  // [bp-0x2d8]
    unsigned int v2;  // [bp-0x2b8]
    char v3;  // [bp-0x2b0]
    char v4;  // [bp-0x248]
    char v5;  // [bp-0x247]
    char v6;  // [bp-0x1c8]
    unsigned long v7;  // [bp-0x18]
    unsigned long v8;  // [bp-0x8]
    char v9;  // [bp+0x0]
    unsigned long v10;  // [bp+0x8]
    unsigned long v11;  // rbx
    unsigned long long v12;  // rax

    v10 = v11;
    v8 = &v9;
    v7 = g_5a380 ^ &v0;
    *((char **)&v4) = 0;
    memset(&v5, 0, 127);
    GetEnvironmentVariableA("MP_PEER", &v4, 128);
    mp_log("map: the host has started a battle; entering it");
    if (!_strnicmp(&v4, "steam:", 6))
        joiner_back_to_lobby(); /* do not return */
    SetEnvironmentVariableA("MP_AUTOJOIN", &v4);
    SetEnvironmentVariableA("MP_MAP", 0);
    SetEnvironmentVariableA("MP_MAP_EPOCH", 0);
    sub_1e2c4("MP_MAP_EPOCH", 0);
    $sprintf_s@$0BKE(&v6, "\"%s\\mp_join.exe\" %s", &g_5b920, &v4);
    *((char **)&v2) = 104;
    memset(&v3, 0, 96);
    if (!CreateProcessA(0, &v6, 0, 0, 0, 0x8000000, 0, &g_5b920, &v2, &v1))
    {
        (unsigned int)v12 = GetLastError();
        mp_log("map: could not start mp_join.exe (%lu)", v12);
        joiner_back_to_lobby("map: could not start mp_join.exe (%lu)", v12); /* do not return */
    }
    ExitProcess(0); /* do not return */
}


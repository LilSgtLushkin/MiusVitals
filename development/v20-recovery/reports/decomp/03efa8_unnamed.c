// v20 RVA 03efa8..03f1e0  (568 bytes)  snapshot_operation_saves
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long snapshot_operation_saves(unsigned long a0, unsigned long a1)
{
    char v0;  // [bp-0x218]
    unsigned long v1;  // [bp-0x1f8]
    char v2;  // [bp-0x1e8]
    int v3;  // [bp-0x1e7]
    int v4;  // [bp-0x1d7]
    void* v5;  // [bp-0x1c7]
    unsigned int v6;  // [bp-0x1bf]
    unsigned short v7;  // [bp-0x1bb]
    char v8;  // [bp-0x1b9]
    char v9;  // [bp-0x1b8]
    char v10;  // [bp-0x1b7]
    char v11;  // [bp-0x138]
    unsigned long long v12;  // [bp-0x28]
    unsigned long v13;  // [bp-0x8]
    char v14;  // [bp+0x0]
    char v16;  // dil
    unsigned long long v17;  // r8
    unsigned long v18;  // r9
    unsigned long long v19;  // rdx
    struct_0 *v20;  // rcx
    unsigned long long v21;  // r8
    unsigned long long v22;  // rax
    struct_0 *v23;  // rdx
    unsigned long v24;  // rcx
    struct_0 *v25;  // rdx
    unsigned long long v26;  // rcx
    unsigned long v27;  // r8
    unsigned long long v28;  // r8
    struct_0 *v29;  // rdx
    unsigned long long v30;  // rdx
    unsigned long v31;  // rcx

    v13 = &v14;
    v12 = g_5a380 ^ &v0;
    v16 = a0;
    if (!(char)steam_init())
        return security_cookie_check(v12 ^ &v0);
    *((char **)&v2) = 0;
    v5 = 0;
    *((char **)&v6) = 0;
    *((char **)&v7) = 0;
    *((char **)&v8) = 0;
    *((char **)&v3) = 0;
    *((char **)&v4) = 0;
    if ((char)a0)
    {
        v17 = 20;
        v18 = (a1 != 0 ? "game" : a1);
        v19 = "%u %s";
        v20 = &v2;
        $sprintf_s@$0BAE$sprintf_s@$0BAE();
    }
    if (v16 == g_5de94 && !strcmp(&v2, &g_5dee8))
        return security_cookie_check(v12 ^ &v0);
    g_5de94 = v16;
    strcpy_s(&g_5dee8, 48, &v2);
    v21 = *((long long *)&g_5df68);
    *((char **)&g_5df70) = 0;
    if (!*((long long *)&g_5df68))
    {
        (unsigned int)v22 = g_5de08(g_5de68, g_5de88, "gtmf");
        if (!(char)g_5de00(g_5de68, "gtmf", &v2) || (v23 = &v2, v24 = (v22 != 0 ? &g_45ce6 : v22), strcmp()))
        {
            v25 = &v2;
            v26 = "steam: Steam did not take the note for friends (asked for '%s', it holds '%s')";
            v27 = (v22 != 0 ? &g_45ce6 : v22);
            mp_log();
        }
        *((char **)&v9) = 0;
        v28 = 127;
        memset(&v10, 0, 127);
        if (v16)
        {
            v1 = g_5de88;
            sprintf_s(&v9, 128, "root\\programs\\opgame.progpack, %ssteam:%llu", "+gtmf_join=");
        }
        if (!(char)g_5de00(g_5de68, "connect", &v9))
            mp_log("steam: Steam did not take the \"connect\" text for its Join Game");
    }
    else if (!v16)
    {
        hook_srand(&v11, 260, *((long long *)&g_5df68));
        DeleteFileA(&v11);
    }
    v29 = &v2;
    if (!v16)
        v30 = &g_45ce6;
    v31 = (v16 ? "steam: no longer shown to friends as a game to join%s" : "steam: friends are told that this game can be joined (%s)");
    mp_log();
    return security_cookie_check(v12 ^ &v0);
}


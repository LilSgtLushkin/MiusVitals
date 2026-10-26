// v20 RVA 00ffe4..01014b  (359 bytes)  leave_battle_for_menu
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long leave_battle_for_menu(unsigned long a0, struct_0 *a1, unsigned long long a2, unsigned long a3, unsigned int a4, unsigned int a5, unsigned long long a6, unsigned long a7, unsigned long a8, unsigned long a9)
{
    char v0;  // [bp-0x1b8]
    unsigned long long v1;  // [bp-0x180], Other Possible Types: unsigned long
    unsigned long long v2;  // [bp-0x178], Other Possible Types: unsigned long
    unsigned long v3;  // [bp-0x170]
    char v4;  // [bp-0x168]
    unsigned long long v5;  // [bp-0x58]

    v5 = g_5a380 ^ &v0;
    if (!a1 || g_5b8e0 != 2 || !g_5b618 || !strstr(a1, "opgame.progpack"))
    {
        v3 = a9;
        v2 = a8;
        v1 = a7;
    }
    else
    {
        mp_log("net: the battle is over - back to the menu, not to the host's operation (%s)", a1);
        SetEnvironmentVariableA("MP_ROLE", 0);
        SetEnvironmentVariableA("MP_PEER", 0);
        SetEnvironmentVariableA("MP_VERSUS", 0);
        SetEnvironmentVariableA("MP_WINDOW", 0);
        $sprintf_s@$0BAM(&v4, "\"%s\\starter_x64.exe\"", &g_5b920, a3);
        v3 = v3;
        a1 = &v4;
        v2 = v2;
        v1 = &g_5b920;
    }
    g_5c000(0, a1, a2, a3, a4, a5, a6, v1, v2);
    return security_cookie_check(v5 ^ &v0);
}


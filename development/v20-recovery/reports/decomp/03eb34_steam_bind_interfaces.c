// v20 RVA 03eb34..03ed87  (595 bytes)  steam_bind_interfaces
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    struct struct_1 *field_0;
    unsigned long long field_8;
} struct_0;

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_1 {
    unsigned long long field_0;
} struct_1;


long long steam_bind_interfaces(unsigned long a0)
{
    unsigned long long *v0;  // [bp-0x158]
    unsigned long v1;  // [bp-0x150]
    unsigned long v2;  // [bp-0x148]
    unsigned long v3;  // [bp-0x140]
    unsigned long v4;  // [bp-0x138]
    unsigned long v5;  // [bp-0x130]
    unsigned long v6;  // [bp-0x128]
    unsigned long v7;  // [bp-0x120]
    unsigned long v8;  // [bp-0x118]
    unsigned long v9;  // [bp-0x110]
    unsigned long v10;  // [bp-0x108]
    unsigned long v11;  // [bp-0x100]
    unsigned long v12;  // [bp-0xf8]
    unsigned long v13;  // [bp-0xf0]
    unsigned long v14;  // [bp-0xe8]
    unsigned long v15;  // [bp-0xe0]
    unsigned long v16;  // [bp-0xd8]
    unsigned long v17;  // [bp-0xd0]
    unsigned long v18;  // [bp-0xc8]
    unsigned long v19;  // [bp-0xc0]
    unsigned long v20;  // [bp-0xb8]
    unsigned long v21;  // [bp-0xb0]
    unsigned long v22;  // [bp-0xa8]
    unsigned long v23;  // [bp-0xa0]
    unsigned long v24;  // [bp-0x98]
    unsigned long v25;  // [bp-0x90]
    unsigned long v26;  // [bp-0x88]
    unsigned long v27;  // [bp-0x80]
    unsigned long v28;  // [bp-0x78]
    unsigned long v29;  // [bp-0x70]
    unsigned long v30;  // [bp-0x68]
    unsigned long v31;  // [bp-0x60]
    unsigned long v32;  // [bp-0x58]
    unsigned long v33;  // [bp-0x50]
    unsigned long v34;  // [bp-0x48]
    unsigned long v35;  // [bp-0x40]
    unsigned long v36;  // [bp-0x38]
    unsigned long v37;  // [bp-0x30]
    unsigned long v38;  // [bp-0x28]
    unsigned long v39;  // [bp-0x20]
    unsigned long v40;  // [bp-0x18]
    unsigned long v41;  // [bp-0x10]
    char v42;  // [bp-0x8]
    struct_0 *v45;  // rbx, Other Possible Types: unsigned long
    char *v46;  // rdi
    unsigned long long v47;  // rax
    unsigned long long *v48;  // rcx
    struct_2 *v49;  // rax, Other Possible Types: unsigned long

    v0 = &g_5ddc0;
    v1 = "SteamAPI_SteamFriends_v017";
    v2 = &g_5ddc8;
    v3 = "SteamAPI_SteamUser_v021";
    v4 = &g_5ddd0;
    v5 = "SteamAPI_SteamNetworking_v006";
    v6 = &g_5ddd8;
    v7 = "SteamAPI_SteamUtils_v010";
    v8 = &g_5dde0;
    v9 = "SteamAPI_ISteamUser_GetSteamID";
    v10 = &g_5dde8;
    v11 = "SteamAPI_ISteamUser_BLoggedOn";
    v12 = &g_5ddf0;
    v13 = "SteamAPI_ISteamUtils_GetAppID";
    v14 = &g_5ddf8;
    v15 = "SteamAPI_ISteamFriends_GetPersonaName";
    v16 = &g_5de00;
    v17 = "SteamAPI_ISteamFriends_SetRichPresence";
    v18 = &g_5de08;
    v19 = "SteamAPI_ISteamFriends_GetFriendRichPresence";
    v20 = &g_5de10;
    v21 = "SteamAPI_ISteamFriends_RequestFriendRichPresence";
    v22 = &g_5de18;
    v23 = "SteamAPI_ISteamFriends_GetFriendCount";
    v24 = &g_5de20;
    v25 = "SteamAPI_ISteamFriends_GetFriendByIndex";
    v26 = &g_5de28;
    v27 = "SteamAPI_ISteamFriends_GetFriendPersonaName";
    v28 = &g_5de30;
    v29 = "SteamAPI_ISteamFriends_GetFriendGamePlayed";
    v30 = &g_5de38;
    v31 = "SteamAPI_ISteamNetworking_SendP2PPacket";
    v32 = &g_5de40;
    v33 = "SteamAPI_ISteamNetworking_IsP2PPacketAvailable";
    v34 = &g_5de48;
    v35 = "SteamAPI_ISteamNetworking_ReadP2PPacket";
    v36 = &g_5de50;
    v37 = "SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser";
    v38 = &g_5de58;
    v39 = "SteamAPI_ISteamNetworking_AllowP2PPacketRelay";
    v40 = &g_5de60;
    v45 = &v0;
    v41 = "SteamAPI_RegisterCallback";
    while (v47)
    {
        v46 = *((long long *)&(8)[v45]);
        v47 = GetProcAddress(a0, v46);
        v48 = *((long long *)v45);
        v45 = &v2;
        *(v48) = v47;
        v49 = &v42;
        if (v45 == &v42)
        {
            *((char *)&v49) = 1;
            return v49;
        }
    }
    v49 = mp_log("steam: this game's Steam library has no %s - no Steam joining", v46);
    *((char *)&v49) = 0;
    return v49;
}


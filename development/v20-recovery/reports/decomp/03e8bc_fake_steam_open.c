// v20 RVA 03e8bc..03ea3d  (385 bytes)  fake_steam_open
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct WSADATA {
    unsigned short wVersion;
    unsigned short wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char paddin394[6];
    char *lpVendorInfo;
} WSADATA;


long long fake_steam_open()
{
    char v0;  // [bp-0x228]
    unsigned int v1;  // [bp-0x1d8]
    unsigned int v2;  // [bp-0x1d4]
    unsigned int v3;  // [bp-0x1d0]
    unsigned int v4;  // [bp-0x1cc]
    int v5;  // [bp-0x1c8], Other Possible Types: unsigned short
    unsigned short v6;  // [bp-0x1c6]
    WSADATA v7;  // [bp-0x1b8]
    unsigned long long v8;  // [bp-0x18]
    unsigned long v9;  // [bp-0x8]
    char v10;  // [bp+0x0]

    v9 = &v10;
    v8 = g_5a380 ^ &v0;
    WSAStartup(514, &v7);
    *((char **)&v1) = 1;
    g_5a300 = socket(2, 2, 17);
    ioctlsocket(g_5a300, 2147772030, &v1);
    *((char **)&v3) = 0;
    *((char **)&v2) = 0;
    WSAIoctl(g_5a300, 2550136844, &v3, 4, 0, 0, &v2, 0, 0);
    *((char **)&v4) = 1;
    setsockopt(g_5a300, 65535, 4, &v4, 4);
    *((char **)&v5) = 0;
    *((char **)&v5) = 2;
    v6 = htons(g_5df68 - &g_63c0);
    inet_pton(2, "127.0.0.", &v5);
    if (!bind(g_5a300, &v5, 16))
        return security_cookie_check(v8 ^ &v0);
    closesocket(g_5a300);
    g_5a300 = -1;
    return security_cookie_check(v8 ^ &v0);
}


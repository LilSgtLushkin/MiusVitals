// v20 RVA 01d33c..01d9e3  (1703 bytes)  sub_1d33c
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_1 {
    char paddin0[32];
    struct struct_0 *field_20;
} struct_1;

typedef struct ADDRINFOA {
    unsigned int ai_flags;
    unsigned int ai_family;
    unsigned int ai_socktype;
    unsigned int ai_protocol;
    unsigned int *ai_addrlen;
    char *ai_canonname;
    struct SOCKADDR *ai_addr;
    void* ai_next;
} ADDRINFOA;

typedef struct struct_0 {
    char paddin0[4];
    unsigned int field_4;
} struct_0;

typedef struct SOCKADDR {
    unsigned int sa_family;
    char sa_data[14];
} SOCKADDR;


long long sub_1d33c()
{
    char v0;  // [bp-0x188]
    int tmp_7;  // tmp #7
    int v1;  // [bp-0x168], Other Possible Types: unsigned short
    ADDRINFOA v2;  // [bp-0x158]
    ADDRINFOA *v6;  // [bp-0x128], Other Possible Types: void*, unsigned int
    char v7;  // [bp-0x124]
    char v8;  // [bp-0x118]
    char v9;  // [bp-0x98]
    char v10;  // [bp-0x92]
    unsigned short v11;  // [bp-0x90]
    char v12;  // [bp-0x8e]
    unsigned long long v13;  // [bp-0x18]
    unsigned long v14;  // [bp-0x8]
    char v15;  // [bp+0x0]
    char *v17;  // rax
    unsigned long long v18;  // r8
    unsigned long long v19;  // r8
    unsigned int v20;  // eax
    unsigned long long v22;  // r9
    unsigned int v23;  // r8d
    unsigned long long v24;  // rax
    unsigned long v25;  // r8
    unsigned int v26;  // r8d
    struct_2 *v27;  // rcx
    void* v28;  // rdx
    unsigned long long v29;  // rax
    struct_1 *v30;  // rbx, Other Possible Types: unsigned long
    unsigned long long v31;  // rdi, Other Possible Types: unsigned long
    char *v32;  // rax
    ADDRINFOA *v33;  // rcx

    v14 = &v15;
    v13 = g_5a380 ^ &v0;
    g_5b618 = strcmp("opgame", "tacgame") == 0;
    strcpy_s(&g_5b920, 260, &g_5a570);
    (unsigned int)v17 = strrchr(&g_5b920, 92);
    if (v17)
        *(&v17) = 0;
    read_profile_name();
    if (GetEnvironmentVariableA("MP_SWAP", &v8, 128))
        g_5b61f = atoi(&v8) != 0;
    if (g_5b618)
    {
LABEL_1d449:
        *((char *)&v17) = 0;
    }
    else
    {
        SetEnvironmentVariableA("MP_SWAPPED", 0);
        SetEnvironmentVariableA("MP_BATTLE_SETUP", 0);
        if (g_5b618 || !GetEnvironmentVariableA("MP_MAP", &v8, 128) || (tmp_7 = (unsigned long long)(unsigned int)atoi(&v8), *((char *)&v17) = 1, !(int)tmp_7))
            goto LABEL_1d449;
    }
    g_5b72a = (char)v17;
    if (!g_5b618)
    {
        if (GetEnvironmentVariableA("MP_ENDTURN", &v8, 128))
        {
            *((char **)&g_5b72e) = 1;
            SetEnvironmentVariableA("MP_ENDTURN", 0);
            if (GetEnvironmentVariableA("MP_MAP_ACK", &v8, 128))
            {
                v18 = 10;
                g_5ba2c = strtoul(&v8, 0, 10);
            }
            SetEnvironmentVariableA("MP_MAP_ACK", 0);
        }
        if (!g_5b618 && GetEnvironmentVariableA("MP_TURNDONE", &v8, 128))
        {
            strcpy_s(&g_5bd50, 120, "turn ended with both players' moves");
            SetEnvironmentVariableA("MP_TURNDONE", 0);
        }
    }
    if (GetEnvironmentVariableA("MP_MAP_EPOCH", &v8, 128))
    {
        v19 = 10;
        g_5bd48 = strtoul(&v8, 0, 10);
        g_5bd44 = g_5bd48;
    }
    if (GetEnvironmentVariableA("MP_PORT", &v8, 128))
        g_5a168 = atoi(&v8);
    if (!GetEnvironmentVariableA("MP_UPNP", &v8, 128) || (tmp_7 = (unsigned long long)(unsigned int)atoi(&v8), *((char *)&v17) = 1, !(int)tmp_7))
        *((char *)&v17) = 0;
    g_5b61d = (char)v17;
    if (GetEnvironmentVariableA("MP_HOST_MODE", &v8, 128))
    {
        if (!_stricmp(&v8, "operation"))
            v20 = 2;
        else
            v20 = &(3)[-(0 < _stricmp(&v8, "mission")) & -2];
        g_5b8e4 = v20;
    }
    if (GetEnvironmentVariableA("MP_PREFER", &v8, 128))
    {
        *((char **)&v6) = 0;
        *((char **)&v7) = 0;
        v22 = 4;
        strncpy_s(&v6, 5, &v8, 4);
        memcpy(&g_5b8e8, &v6, 4);
    }
    v23 = 128;
    if (!GetEnvironmentVariableA("MP_ROLE", &v8, 128))
        return security_cookie_check(v13 ^ &v0);
    if (!_stricmp(&v8, "host"))
    {
        *((char **)&g_5b8e0) = 1;
        goto LABEL_1d6bd;
    }
    else
    {
        (unsigned int)v24 = _stricmp(&v8, "join");
        *((unsigned long *)&g_5b8e0) = ~(-(0 < (unsigned int)v24)) & 2;
        if (!(unsigned int)v24)
        {
LABEL_1d6bd:
            if (g_5b618)
                load_battle_description();
        }
    }
    if (g_5b8e0 == 1)
    {
        if (g_5b618)
        {
            operation_id@opsave@opsave();
            snapshot_operation_saves();
            if (!(g_5b8e0 == 1))
                goto LABEL_1d745;
        }
        v25 = g_5bd44;
        if (!g_5bd44)
        {
            v25 = &g_10000 | GetTickCount();
            g_5bd44 = v25;
        }
        if (g_5b618)
        {
            map_moved_on();
        }
        else
        {
            $sprintf_s@$0IA(&v8, &g_46780, v25, 4);
            SetEnvironmentVariableA("MP_MAP_EPOCH", &v8);
        }
    }
LABEL_1d745:
    if (g_5b8e0 == 2)
    {
        if (g_5b618 == 0)
            goto LABEL_1d7c5;
        v26 = 128;
        if (GetEnvironmentVariableA("MP_VERSUS", &v8, 128))
            set_versus(1132 CONCAT atoi(&v8) != 0);
    }
    if (g_5b8e0 == 1)
    {
        if (!g_5b8e4)
            *((char **)&g_5b8e4) = 1;
        find_local_ip();
        host_listen();
        return security_cookie_check(v13 ^ &v0);
    }
    if (g_5b8e0 != 2)
        return security_cookie_check(v13 ^ &v0);
    if (!g_5b618)
    {
LABEL_1d7c5:
        if (!g_5b72a)
            return security_cookie_check(v13 ^ &v0);
    }
    v27 = &v12;
    v28 = 0;
    strncpy(&v9, "127.0.0.", 8);
    *((char **)&v11) = 49;
    memset(&v12, 0, 118);
    GetEnvironmentVariableA("MP_PEER", &v9, 128);
    (unsigned int)v29 = _strnicmp(&v9, "steam:", 6);
    if ((unsigned int)v29)
    {
        v30 = 0;
    }
    else
    {
        v30 = _strtoui64(&v10, 0, &(10)[v29]);
        if (v30)
            strcpy_s(&v9, 128, "127.0.0.");
    }
    v31 = g_5a168;
    (unsigned int)v32 = strchr(&v9, 58);
    if (v32)
    {
        *(&v32) = 0;
        v31 = atoi(v32 + 1);
    }
    *((char **)&g_5b8f0) = 0;
    *((char **)&g_5b8f0) = 2;
    g_5b8f2 = htons(v31);
    if (inet_pton(2, &v9, &g_5b8f4) != 1)
    {
        v6 = 0;
        *((char **)&v2.ai_flags) = 0;
        *((char **)&v2.ai_family) = 2;
        v2.ai_addrlen = 0;
        v2.ai_addr = 0;
        if (getaddrinfo(&v9, 0, &v2, &v6) || (v33 = v6, !v33))
        {
            mp_log("net: cannot resolve '%s'", &v9);
        }
        else
        {
            g_5b8f4 = *((int *)&(4)[*((long long *)(v33 + 32))]);
            freeaddrinfo(v33);
        }
    }
    if (v30)
    {
        if (!(char)steam_init())
            mp_log("net: the host is to be reached through Steam, and Steam is not there");
        v6 = v30;
        *((char **)&v1) = 0;
        *((char **)&v1) = &g_5354;
        memcpy(&v1, &v6, 8);
        *((int128_t *)&g_5b8f0) = (int128_t)v1;
        hook_GetAsyncKeyState(v30);
    }
    if (!(char)sub_1755c(0))
        return security_cookie_check(v13 ^ &v0);
    *((char **)&g_5b61a) = 1;
    if (!v30)
    {
        mp_log("net: joining %s:%u", &v9, (unsigned short)v31);
        return security_cookie_check(v13 ^ &v0);
    }
    mp_log("net: joining through Steam (steam:%llu)", v30);
    return security_cookie_check(v13 ^ &v0);
}


// v20 RVA 02aa74..02b046  (1490 bytes)  sub_2aa74
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct FILE_t {
    unsigned int _flags;
    char paddin4[4];
    char *_IO_read_ptr;
    char *_IO_read_end;
    char *_IO_read_base;
    char *_IO_write_base;
    char *_IO_write_ptr;
    char *_IO_write_end;
    char *_IO_buf_base;
    char *_IO_buf_end;
    char *_IO_save_base;
    char *_IO_backup_base;
    char *_IO_save_end;
    struct _IO_marker *_markers;
    struct _IO_marker *_chain;
    unsigned int _fileno;
    unsigned int _flags2;
    unsigned int _old_offset;
    char paddin124[4];
    unsigned short _cur_column;
    char _vtable_offset;
    char _shortbuf[1];
    char paddin132[4];
    struct _IO_marker *_lock;
    unsigned long long _offset;
    struct _IO_codecvt *_codecvt;
    struct _IO_wide_data *_wide_data;
    void* _freeres_list;
    char __pad5;
    char paddin177[7];
    unsigned int _mode;
    char _unused2[20];
} FILE_t;

typedef struct _IO_marker {
    struct _IO_marker *_next;
    struct _IO_marker *_sbuf;
    unsigned int _pos;
} _IO_marker;

typedef struct _IO_codecvt {
    char __cd_out;
} _IO_codecvt;

typedef struct _IO_wide_data {
    unsigned short *_IO_read_ptr;
    unsigned short *_IO_read_end;
    unsigned short *_IO_read_base;
    unsigned short *_IO_write_base;
    unsigned short *_IO_write_ptr;
    unsigned short *_IO_write_end;
    unsigned short *_IO_buf_base;
    unsigned short *_IO_buf_end;
    unsigned short *_IO_save_base;
    unsigned short *_IO_backup_base;
    unsigned short *_IO_save_end;
    __mbstate_t _IO_state;
    char paddin93[3];
    char _IO_last_state;
    char paddin97[7];
    unsigned short _shortbuf[1];
    _IO_marker _wide_vtable;
} _IO_wide_data;

typedef struct __mbstate_t {
    unsigned int __count;
    char __value;
} __mbstate_t;


long long sub_2aa74(unsigned long a0, unsigned long a1)
{
    char v0;  // [bp-0x278]
    unsigned int v1;  // [bp-0x258]
    unsigned int v2;  // [bp-0x254]
    void* v3;  // [bp-0x250]
    char v4;  // [bp-0x248]
    char v5;  // [bp-0x247]
    char v6;  // [bp-0x244]
    char v7;  // [bp-0x242]
    char v8;  // [bp-0x240]
    char v9;  // [bp-0x23f]
    char v10;  // [bp-0x23b]
    char v11;  // [bp-0x1c8]
    char v12;  // [bp-0x1c7]
    char v13;  // [bp-0x148]
    unsigned long long v14;  // [bp-0x38]
    unsigned long v15;  // [bp-0x8]
    char v16;  // [bp+0x0]
    unsigned int v20;  // cc_dep1
    FILE_t *v22;  // r8
    unsigned long long v23;  // rax
    unsigned long long v25;  // rax
    unsigned long long v26;  // rax
    unsigned long long v27;  // rax
    unsigned long long v28;  // rax
    unsigned long long v29;  // rax

    v15 = &v16;
    v14 = g_5a380 ^ &v0;
    v20 = g_5a280;
    if (g_5a280 < 0)
    {
        a0 = 0;
        *((char *)&a0) = GetEnvironmentVariableA("MP_TEST", 0, 0) != 0;
        g_5a280 = a0;
        v20 = a0;
    }
    if (!(v20 && g_5c68c == (unsigned int)(g_5c68c / 6 + (g_5c68c / 6 << 1)) << 1))
        return security_cookie_check(v14 ^ &v0);
    *((char **)&v4) = 0;
    memset(&v5, 0, 127);
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v13, "%s\\mp_cmd_%lu.txt", &g_5a570, GetCurrentProcessId());
    v3 = 0;
    if (!fopen_s(&v3, &v13, "r"))
    {
        v22 = v3;
        if (v22)
        {
            fgets(&v4, 128, v22);
            fclose(v3);
            DeleteFileA(&v13);
            if (!strncmp(&v4, "ready", 5))
            {
                session_test_ready();
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "resign", 6))
            {
                sub_1e3e4();
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "withdraw", 8))
            {
                sub_1e440();
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "pause", 5))
            {
                sub_1e3a8();
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "speed ", 6))
            {
                sub_1e3f4(atoi(&v7));
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "time", 4))
            {
                sub_1e3fc();
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "ceasefire", 9))
            {
                *((char **)&g_5c8b8) = 14;
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "chooseforce", 11))
            {
                *((char **)&g_5c8b8) = 16;
                return security_cookie_check(v14 ^ &v0);
            }
            else if (!strncmp(&v4, "hostskirmish", 12))
            {
                *((char **)&g_5c8b8) = 2;
                return security_cookie_check(v14 ^ &v0);
            }
            else
            {
                (unsigned int)v23 = strncmp(&v4, "setup", 5);
                if (!(unsigned int)v23)
                {
                    *((char **)&g_5c8b8) = &(7)[v23];
                }
                else if (!strncmp(&v4, "lobbyready", 10))
                {
                    *((char **)&g_5c8b8) = 9;
                    return security_cookie_check(v14 ^ &v0);
                }
                else if (!strncmp(&v4, "skirmishstart", 13))
                {
                    *((char **)&g_5c8b8) = 17;
                    return security_cookie_check(v14 ^ &v0);
                }
                else
                {
                    (unsigned int)v25 = strncmp(&v4, "connect ", 8);
                    if (!(unsigned int)v25)
                    {
                        v11 = v25;
                        memset(&v12, 0, &(127)[v25]);
                        if (sscanf_s(&v8, "%127s", &v11, 128) == 1)
                        {
                            operation_exists@Joiner@Joiner(&v11);
                            *((char **)&g_5c8b8) = 5;
                        }
                    }
                    else
                    {
                        (unsigned int)v26 = strncmp(&v4, "steamrequest ", 13);
                        if (!(unsigned int)v26)
                        {
                            sub_3edc0(&v11, 128, _strtoui64(&v10, 0, &(10)[v26]));
                            end_turn_here(&v11, 1);
                            return security_cookie_check(v14 ^ &v0);
                        }
                        (unsigned int)v27 = strncmp(&v4, "steamjoin", 9);
                        if (!(unsigned int)v27)
                        {
                            *((char **)&g_5c8b8) = 15;
                            *((unsigned int *)&g_5c8bc) = v27;
                            return security_cookie_check(v14 ^ &v0);
                        }
                        else if (!strncmp(&v4, "menuoff", 7))
                        {
                            sub_1e35c(0);
                        }
                        else if (!strncmp(&v4, "menu", 4))
                        {
                            *((char *)&a0) = 1;
                            sub_1e35c(a0);
                        }
                        else if (!strncmp(&v4, "endturn", 7))
                        {
                            sub_1e3a0();
                        }
                        else if (!strncmp(&v4, "savemap", 7))
                        {
                            sub_1e3ec();
                        }
                        else if (!strncmp(&v4, "newmap", 6))
                        {
                            sub_1e368();
                        }
                        else if (!strncmp(&v4, "flares", 6))
                        {
                            sub_353a4(atoi(&v7));
                        }
                        else if (!strncmp(&v4, "blowup", 6))
                        {
                            sub_35390(atoi(&v7));
                        }
                        else
                        {
                            (unsigned int)v28 = strncmp(&v4, "artillery", 9);
                            if (!(unsigned int)v28)
                            {
                                v2 = v28;
                                v1 = v28;
                                sscanf_s(&v9, "%d %d", &v2, &v1);
                                sub_35378(v2, v1);
                            }
                            else
                            {
                                (unsigned int)v29 = strncmp(&v4, "dig ", 4);
                                if ((unsigned int)v29)
                                {
                                    sscanf_s(&v4, "shot %63s", a0, a1);
                                    return security_cookie_check(v14 ^ &v0);
                                }
                                v1 = v29;
                                v2 = v29;
                                if (sscanf_s(&v6, "%d %f", &v1, &v2) == 2)
                                    sub_38844(v1);
                            }
                        }
                    }
                }
            }
        }
    }
}


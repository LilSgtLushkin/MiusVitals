// v20 RVA 02ce78..02d207  (911 bytes)  sub_2ce78
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


long long sub_2ce78()
{
    char v0;  // [bp-0x4b8]
    void* v1;  // [bp-0x498]
    char v2;  // [bp-0x488]
    char v3;  // [bp-0x438]
    char v4;  // [bp-0x3e8]
    char v5;  // [bp-0x398]
    char v6;  // [bp-0x348]
    char v7;  // [bp-0x238]
    char v8;  // [bp-0x128]
    unsigned long long v9;  // [bp-0x18]
    char *v11;  // rax, Other Possible Types: unsigned long
    FILE_t *v12;  // r8, Other Possible Types: unsigned long
    char *v13;  // rsi
    void* v14;  // rbx, Other Possible Types: unsigned long
    void* v15;  // rdi, Other Possible Types: unsigned long

    v9 = g_5a380 ^ &v0;
    g_5c148 = strcmp("opgame", "tacgame") == 0;
    strcpy_s(&g_5c6d0, 260, &g_5a570);
    (unsigned int)v11 = strrchr(&g_5c6d0, 92);
    if (v11)
        *(&v11) = 0;
    memset(&v2, 0, 320);
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v7, "%s\\users\\mp_last_host.txt", &g_5c6d0);
    v1 = 0;
    if (!fopen_s(&v1, &v7, "r"))
    {
        v12 = v1;
        if (v12)
        {
            v14 = 0;
            v15 = 0;
            while (true)
            {
                v13 = &v2;
                if (!(fgets(v13, 80, v12)) || !((v11 = strcspn(v13, "\r\n") + v15, v14 = (unsigned long long)((unsigned int)v14 + 1), v15 += 80, *((char **)&(&v2)[v11]) = 0, (unsigned int)v14 < 4)))
                    break;
                v12 = v1;
            }
            fclose(v1);
        }
    }
    g_5c469 = strcmp(&v5, "upnp") == 0;
    operation_exists@Joiner@Joiner(&v2);
    if (v3)
        0bad_array_new_length@std@std(&g_5c8b0, &v3);
    0bad_array_new_length@std@std(&g_5c8a8, &v4);
    g_5c118 = mp_log;
    g_5c120 = mp_log;
    if (g_5c148)
    {
LABEL_2d077:
        *((char *)&v11) = 0;
        g_5c4c0 = 0;
    }
    else
    {
        if (sub_1c334() == 2 && !(char)sub_1b810())
            sub_1d2e0();
        if (!(!g_5c148) || !(GetEnvironmentVariableA("MP_AUTOJOIN", &g_5c4c0, 128)))
            goto LABEL_2d077;
        SetEnvironmentVariableA("MP_AUTOJOIN", 0);
        *((char *)&v11) = (char)g_5c4c0;
    }
    g_5c14f = (char)v11;
    if (g_5c148)
    {
LABEL_2d109:
        *((char **)&g_5c5c0) = 0;
    }
    else
    {
        if (GetEnvironmentVariableA("MP_AUTOJOIN_READY", &v6, 260))
        {
            if (!atoi(&v6))
                *((char **)&g_5c14f) = 0;
            SetEnvironmentVariableA("MP_AUTOJOIN_READY", 0);
        }
        if (!(!g_5c148) || !(GetEnvironmentVariableA("MP_FORCE_NOTE", &g_5c5c0, 200)))
            goto LABEL_2d109;
        SetEnvironmentVariableA("MP_FORCE_NOTE", 0);
    }
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v8, "%s\\users\\mp_my_force.cfgpack", &g_5c6d0);
    if (!g_5c148 && (GetEnvironmentVariableA("MP_FORCE_FILE", &v6, 260) || (char)g_5c4c0))
        operation_id@opsave@opsave(&v8);
    SetEnvironmentVariableA("MP_FORCE_FILE", 0);
    if (GetEnvironmentVariableA("MP_FORCE_COST", &v6, 260))
        g_5c454 = strtoul(&v6, 0, 10);
    if (g_5c148)
        return security_cookie_check(v9 ^ &v0);
    steam_init();
    if (!strstr(GetCommandLineA(), "+gtmf_join="))
        return security_cookie_check(v9 ^ &v0);
    end_turn_here(GetCommandLineA(), 0);
    return security_cookie_check(v9 ^ &v0);
}


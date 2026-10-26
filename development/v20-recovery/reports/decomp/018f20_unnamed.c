// v20 RVA 018f20..019240  (800 bytes)  host_swap_sides_if_asked
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    void* field_0;
    unsigned long long field_8;
} struct_1;

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

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct PROCESS_INFORMATION {
    unsigned int *hProcess;
    unsigned int *hThread;
    unsigned int dwProcessId;
    unsigned int dwThreadId;
} PROCESS_INFORMATION;

typedef struct struct_0 {
    struct struct_0 *field_-8;
} struct_0;

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


long long host_swap_sides_if_asked(unsigned long long a0[2], unsigned long long a1[2], unsigned long long a2)
{
    char v0;  // [bp-0x258]
    void* v1;  // [bp-0x208]
    PROCESS_INFORMATION v2;  // [bp-0x200]
    unsigned int v3;  // [bp-0x1e8]
    char v4;  // [bp-0x1e0]
    int v5;  // [bp-0x178], Other Possible Types: char
    int v6;  // [bp-0x168]
    char v7;  // [bp-0x158]
    unsigned int v8;  // [bp-0x157]
    unsigned short v9;  // [bp-0x153]
    char v10;  // [bp-0x151]
    char v11;  // [bp-0x148]
    unsigned long long v12;  // [bp-0x38]
    unsigned long v13;  // [bp-0x8]
    char v14;  // [bp+0x0]
    struct_1 *v16;  // rbx
    struct_1 *v17;  // r14
    unsigned long long v19;  // rax
    FILE_t *v20;  // r9
    unsigned long v21;  // r13
    FILE_t *v22;  // r9
    unsigned long long v23;  // rbx
    struct_2 *v24;  // rdx, Other Possible Types: unsigned long
    FILE_t *v25;  // r9
    unsigned long long v26;  // rdx
    unsigned long long v27;  // rdx, Other Possible Types: unsigned long
    struct_0 *v28;  // rcx, Other Possible Types: unsigned long
    struct_0 *v29;  // rax

    v13 = &v14;
    v12 = g_5a380 ^ &v0;
    v16 = a1;
    v17 = a0;
    *((char **)&v7) = 0;
    *((char **)&v8) = 0;
    *((char **)&v9) = 0;
    *((char **)&v10) = 0;
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v11, "%s\\users\\mp_battle_setup.cfgpack", &g_5b920);
    v1 = 0;
    (unsigned int)v19 = fopen_s(&v1, &v11, &g_46b50);
    if (!(unsigned int)v19)
    {
        v20 = v1;
        if (v20)
        {
            v21 = &(1)[v19];
            fwrite(v16->field_0, v21, *((long long *)(v16 + 8)) - v16->field_0, v20);
            fclose(v1);
            if (!fopen_s(&v1, &g_5bbd0, &g_46b50))
            {
                v22 = v1;
                if (v22)
                {
                    fwrite(v17->field_0, v21, *((long long *)(v17 + 8)) - v17->field_0, v22);
                    fclose(v1);
                    SetEnvironmentVariableA("MP_BATTLE_SETUP", &v11);
                    SetEnvironmentVariableA("MP_SWAPPED", "1");
                    v23 = GetCommandLineA();
                    *((char **)&v5) = 0;
                    *((char **)&v6) = 0;
                    $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v5, v23, strlen(v23));
                    *((char **)&v3) = 104;
                    memset(&v4, 0, &(95)[v21]);
                    v24 = &v5;
                    if ((long long)(&v6)[8] > 15)
                        v24 = (long long)v5;
                    if (CreateProcessA(0, v24, 0, 0, 0, 0, 0, &g_5b920, &v3, &v2))
                    {
                        mp_log("%s", a2);
                        ExitProcess(0); /* do not return */
                    }
                    mp_log("lobby: could not restart the battle (%lu), playing it as it is", GetLastError());
                    if (!fopen_s(&v1, &g_5bbd0, &g_46b50))
                    {
                        v25 = v1;
                        if (v25)
                        {
                            fwrite(g_5b6a0, v21, g_5b6a8 - g_5b6a0, v25);
                            fclose(v1);
                        }
                    }
                    if (!GetEnvironmentVariableA("MP_SWAPPED", &v7, 8))
                    {
                        SetEnvironmentVariableA("MP_SWAPPED", 0);
                        SetEnvironmentVariableA("MP_BATTLE_SETUP", 0);
                    }
                    v26 = (long long)(&v6)[8];
                    if (v26 > 15)
                    {
                        v27 = &(1)[v26];
                        v28 = (long long)v5;
                        v29 = v28;
                        if (v27 >= __Eg_my_units)
                        {
                            v27 = &(39)[v27];
                            v28 = *((long long *)(v28 - 8));
                            if (-8 + (char *)v29 - v28 > 31)
                            {
                                _invoke_watson(0, 0, 0, 0);
                                [D] Unsupported jumpkind Ijk_SigTRAP at address 102883
                            }
                        }
                        sub_4030c(v28, v27);
                    }
                    *((int128_t *)&v6) = 0xf0000000000000000;
                    *((char **)&v5) = 0;
                    return security_cookie_check(v12 ^ &v0);
                }
            }
            mp_log("lobby: cannot rewrite the battle description, playing it as it is");
            return security_cookie_check(v12 ^ &v0);
        }
    }
    mp_log("lobby: cannot keep the battle setup in %s, playing the battle as it is", &v11);
    return security_cookie_check(v12 ^ &v0);
}


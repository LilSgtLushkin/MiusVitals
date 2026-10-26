// v20 RVA 02a47c..02aa73  (1527 bytes)  launch@Joiner
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


long long launch@Joiner(char a0, unsigned long long a1, unsigned long long a2, unsigned long long a3, unsigned int a4, unsigned int a5, unsigned long a6, unsigned long long a7, unsigned long a8, unsigned long a9)
{
    char v0;  // [bp-0x628]
    unsigned int v1;  // [bp-0x608], Other Possible Types: unsigned long long
    unsigned long v2;  // [bp-0x5e0]
    char v3;  // [bp-0x5d8], Other Possible Types: unsigned int
    char v4;  // [bp-0x5d0]
    unsigned int v5;  // [bp-0x5cc]
    void* v6;  // [bp-0x5c8]
    int v7;  // [bp-0x5c0]
    void* v8;  // [bp-0x5b0]
    unsigned long v9;  // [bp-0x5a8]
    unsigned long long v10;  // [bp-0x5a0]
    unsigned long long v11;  // [bp-0x598]
    unsigned long long v12;  // [bp-0x590]
    unsigned long long v13;  // [bp-0x588]
    char v14;  // [bp-0x580]
    char v15;  // [bp-0x568]
    char v16;  // [bp-0x498]
    char v17;  // [bp-0x497]
    char v18;  // [bp-0x388]
    char v19;  // [bp-0x278]
    char v20;  // [bp-0x168]
    unsigned long long v21;  // [bp-0x58]
    unsigned long v22;  // [bp-0x30]
    unsigned long v23;  // [bp-0x28]
    unsigned long v24;  // [bp-0x8]
    char v25;  // [bp+0x0]
    unsigned long v27;  // r12
    unsigned long v28;  // r13
    unsigned long long v29;  // rax, Other Possible Types: unsigned long
    unsigned long long v30;  // rax
    char *v31;  // rbx, Other Possible Types: unsigned long
    unsigned long v32;  // rdi
    unsigned long long v33;  // rax
    FILE_t *v34;  // rcx
    unsigned long long v35;  // rax
    unsigned long long v36;  // rcx
    unsigned long long v37;  // rdi
    unsigned long long v38;  // r13
    unsigned long long v40;  // rbx, Other Possible Types: unsigned long
    unsigned long long v41;  // r13, Other Possible Types: unsigned long
    FILE_t *v42;  // r9
    unsigned long long v43;  // rcx
    unsigned long long v44;  // rbx, Other Possible Types: unsigned long
    struct_0 *v45;  // r8, Other Possible Types: unsigned long
    struct_0 *v46;  // rcx

    v24 = &v25;
    v23 = v27;
    v22 = v28;
    v21 = g_5a380 ^ &v0;
    v12 = a3;
    v13 = a2;
    v11 = a6;
    v10 = a8;
    v29 = a9;
    v9 = v29;
    if (g_5c468 && a1)
    {
        if (!strstr(a1, "tacgame.progpack"))
        {
            v29 = v9;
        }
        else
        {
            a0 = 1;
            start_program(a0);
            *((char **)&v16) = 0;
            memset(&v17, 0, 259);
            (unsigned int)v30 = strchr(a1, 44);
            if (v30)
            {
                for (v31 = &(1)[v30]; *((char *)v31) == 32; v31 = &(1)[v31]);
                v32 = strcspn(v31, ",");
                if (v32)
                {
                    while (*(v31 + v32 - 1) == 32 && (v32 -= 1, v32 != 1))
                    ;
                }
                if (v32 < 260)
                {
                    memcpy(&v16, v31, v32);
                    *((char **)&(&v16)[v32]) = 0;
                }
            }
            $sprintf_s@$0BAE$sprintf_s@$0BAE(&v19, "%s\\%s", &g_5c6d0, &v16);
            $sprintf_s@$0BAE$sprintf_s@$0BAE(&v18, "%s\\users\\mp_my_force.cfgpack", &g_5c6d0);
            *((char **)&v7) = 0;
            v8 = 0;
            v6 = 0;
            if (v16)
            {
                (unsigned int)v33 = fopen_s(&v6, &v19, "rb");
                if (!(unsigned int)v33)
                {
                    v34 = v6;
                    if (v34)
                    {
                        fseek(v34, 0, &(2)[v33]);
                        v35 = ftell(v6);
                        fseek(v6, 0, 0);
                        if ((unsigned int)(v35 - 1) <= 16777214)
                        {
                            $_Resize@U_Value_init_tag@std$_Resize@U_Value_init_tag@std(&v7, v35, &v4);
                            v36 = (long long)v7;
                            $_Resize@U_Value_init_tag@std$_Resize@U_Value_init_tag@std(&v7, fread(v36, 1, (long long)(&v7)[8] - v36, v6), &v4);
                        }
                        fclose(v6);
                    }
                }
            }
            *((char **)&v5) = 0;
            v37 = -(0 < sides@image(&v7, &v5, &v3)) & v5;
            *((char **)&v5) = 0;
            *((char **)&v3) = 0;
            squad_owners@image@image(&v7, v37, &v5, &v3);
            v38 = v5;
            if (!(unsigned int)v38)
                strcpy_s(&v15, 200, "Your force could not be read from the Skirmish screen. Press CHOOSE MY FORCE to try again.");
            if (!(!g_5c44c || (unsigned int)v37 == g_5c44c))
            {
                v3 = g_5c44c;
                v40 = "Germans";
                if (g_5c44c == 5391687)
                {
                    v41 = "Germans";
                }
                else
                {
                    if (g_5c44c == 1381192533)
                    {
                        v41 = "Soviets";
                    }
                    else
                    {
                        memcpy(&g_5b5c8, &v3, 4);
                        *((char **)&g_5b5cc) = 0;
                        v41 = &g_5b5c8;
                        v3 = g_5c44c;
                        memcpy(&g_5b5c8, &v3, 4);
                        *((char **)&g_5b5cc) = 0;
                    }
                }
                v3 = v37;
                if ((unsigned int)v37 != 5391687)
                {
                    if ((unsigned int)v37 == 1381192533)
                    {
                        v40 = "Soviets";
                    }
                    else
                    {
                        memcpy(&g_5b5c8, &v3, 4);
                        *((char **)&g_5b5cc) = 0;
                        v40 = &g_5b5c8;
                    }
                }
                v1 = v41;
                $sprintf_s@$0MI(&v15, "That force is of the %s; you command the %s in this battle. CHOOSE MY FORCE again with the %s as your side.", v40, v41);
            }
            else if (fopen_s(&v6, &v18, &g_46b50) || (v42 = v6, !v42))
            {
                strcpy_s(&v15, 200, "Your force could not be kept (users folder).");
            }
            else
            {
                v43 = (long long)v7;
                fwrite(v43, 1, (long long)(&v7)[8] - v43, v42);
                fclose(v6);
                SetEnvironmentVariableA("MP_FORCE_FILE", &v18);
                $sprintf_s@$0BA(&v14, &g_46780, g_5c454);
                SetEnvironmentVariableA("MP_FORCE_COST", &v14);
                v3 = v37;
                if ((unsigned int)v37 == 5391687)
                {
                    v44 = "Germans";
                }
                else if ((unsigned int)v37 == 1381192533)
                {
                    v44 = "Soviets";
                }
                else
                {
                    memcpy(&g_5b5c8, &v3, 4);
                    *((char **)&g_5b5cc) = 0;
                    v44 = &g_5b5c8;
                }
                $sprintf_s@$0MI(&v15, "Your own force: %u squads of the %s, cost %u.", v38, v44);
            }
            mp_log("ui: CHOOSE MY FORCE - the Skirmish screen's battle is not started: %s Back to %s's lobby.", &v15, &g_5c540);
            SetEnvironmentVariableA("MP_FORCE_NOTE", &v15);
            SetEnvironmentVariableA("MP_AUTOJOIN", &g_5c540);
            SetEnvironmentVariableA("MP_AUTOJOIN_READY", "0");
            $sprintf_s@$0BAM(&v20, "\"%s\\starter_x64.exe\"", &g_5c6d0);
            v2 = v9;
            v1 = a4;
            g_5cbe0(0, &v20, v13, v12, v1, a5, v11, &g_5c6d0, v10);
            v45 = (long long)v7;
            if (!v45)
                return security_cookie_check(v21 ^ &v0);
            v46 = v45;
            if (v8 - v45 >= __Eg_my_units)
            {
                v45 = *((long long *)(v45 - 8));
                if (-8 + (char *)v46 - v45 > 31)
                {
                    _invoke_watson(0, 0, 0, 0);
                    [D] Unsupported jumpkind Ijk_SigTRAP at address 174579
                }
            }
            sub_4030c(v45);
            *((char **)&v7) = 0;
            v8 = 0;
        }
    }
    v2 = v29;
    g_5cbe0(a0, a1, a2, a3, a4, a5, v11, a7, v10);
    return security_cookie_check(v21 ^ &v0);
}


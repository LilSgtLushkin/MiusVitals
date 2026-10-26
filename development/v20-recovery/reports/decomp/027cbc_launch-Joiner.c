// v20 RVA 027cbc..028159  (1181 bytes)  launch@Joiner
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[16];
    unsigned int field_10;
    char paddin20[304];
    char field_144;
    char paddin325[163];
    unsigned long long field_1e8;
    unsigned long long field_1f0;
    char paddin504[112];
    unsigned long long field_268;
    char paddin624[16];
    unsigned long long field_280;
    unsigned long long field_288;
} struct_0;

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

typedef struct struct_1 {
    char field_0;
} struct_1;

typedef struct PROCESS_INFORMATION {
    unsigned int *hProcess;
    unsigned int *hThread;
    unsigned int dwProcessId;
    unsigned int dwThreadId;
} PROCESS_INFORMATION;

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


long long launch@Joiner(struct_0 *a0, unsigned long long a1)
{
    char v0;  // [bp-0x908]
    void* v1;  // [bp-0x8b8]
    PROCESS_INFORMATION v2;  // [bp-0x8b0]
    char v3;  // [bp-0x8a8]
    unsigned int v4;  // [bp-0x898]
    char v5;  // [bp-0x890]
    void* v6;  // [bp-0x828]
    char v7;  // [bp-0x820]
    char v8;  // [bp-0x818]
    unsigned short v9;  // [bp-0x814]
    int v10;  // [bp-0x812]
    int v11;  // [bp-0x802]
    int v12;  // [bp-0x7f2]
    void* v13;  // [bp-0x7e2]
    unsigned short v14;  // [bp-0x7da]
    char v15;  // [bp-0x7d8]
    unsigned short v16;  // [bp-0x7d0]
    char v17;  // [bp-0x7ce]
    char v18;  // [bp-0x738]
    char v19;  // [bp-0x628]
    char v20;  // [bp-0x627]
    char v21;  // [bp-0x428]
    unsigned long long v22;  // [bp-0x28]
    unsigned long v23;  // [bp-0x8]
    char v24;  // [bp+0x0]
    char *v26;  // r14
    struct_0 *v27;  // rdi
    unsigned long long v28;  // rax
    FILE_t *v29;  // r9
    unsigned long v30;  // rax
    unsigned long long v31;  // rax
    char *v32;  // rbx
    unsigned long v34;  // r8, Other Possible Types: unsigned long long
    unsigned long long v35;  // rax
    FILE_t *v36;  // r9
    void* v37;  // rcx
    unsigned int *v38;  // rbx
    struct_1 *v40;  // rcx
    void* v41;  // rdx
    unsigned long long v42;  // r8
    unsigned long long v43;  // rsi
    unsigned int *v44;  // rcx

    v23 = &v24;
    v22 = g_5a380 ^ &v0;
    strncpy(&v8, "clou", 4);
    v26 = a1;
    v27 = a0;
    *((char **)&v9) = 100;
    v13 = 0;
    *((char **)&v14) = 0;
    *((char **)&v19) = 0;
    *((char **)&v10) = 0;
    *((char **)&v11) = 0;
    *((char **)&v12) = 0;
    memset(&v20, 0, 511);
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v18, "%s\\users\\profiles\\cur_profile.engcfg", a1);
    v1 = 0;
    (unsigned int)v28 = fopen_s(&v1, &v18, "rb");
    if (!(!(unsigned int)v28) || !((v29 = v1, v29)))
    {
LABEL_27e24:
        if (!(*((char *)(a0 + 324))) || !((a0 += 640, !(char)mirror@image(a0))))
        {
            $sprintf_s@$0BAE$sprintf_s@$0BAE(&v18, "%s\\%s", a1, "data\\common\\shared\\config\\tacgame_image.cfgpack");
            (unsigned int)v35 = fopen_s(&v1, &v18, &g_46b50);
            if ((unsigned int)v35 || (v36 = v1, !v36))
            {
                v34 = "Could not save the battle to the game folder.";
                goto LABEL_280f6;
            }
            else
            {
                v37 = *((long long *)(a0 + 640));
                fwrite(v37, &(1)[v35], *((long long *)(v27 + 648)) - v37, v36);
                fclose(v1);
                SetEnvironmentVariableA("MP_ROLE", "join");
                SetEnvironmentVariableA("MP_PEER", v27 + 180);
                v38 = v27 + 312;
                v6 = 0;
                memcpy(&v6, v38, 4);
                SetEnvironmentVariableA("MP_PREFER", -(0 < *(v38)) & &v6);
                SetEnvironmentVariableA("MP_VERSUS", (*((char *)(v27 + 324)) != 0 ? "0" : "1"));
                v40 = &v17;
                v41 = 0;
                v42 = 150;
                strncpy(&v15, "*:*, 0, ", 8);
                *((char **)&v16) = 49;
                memset(&v17, 0, 150);
                if ((!GetEnvironmentVariableA("MP_JOIN_PLAIN", &v7, 8) || !atoi(&v7)) && *((long long *)(v27 + 488)) != *((long long *)(v27 + 496)))
                {
                    v43 = v27 + 386;
                    if ((char)__Fg_held(v43) && _stricmp(&v8, "mp_guest") && (char)restore_slot(v27, v26, &v8))
                    {
                        strcpy_s(&v15, 160, v43);
                        strcpy_s(&v8, 64, "mp_guest");
                    }
                }
                GetEnvironmentVariableA("MP_JOIN_BATTLE_OF", &v15, 160);
                GetEnvironmentVariableA("MP_JOIN_PROFILE", &v8, 64);
                $sprintf_s@$0EAA(&v21, "\"%s\\starter_x64.exe\" root\\programs\\tacgame.progpack, %s, users\\profiles, %s, %s", v26, "data\\common\\shared\\config\\tacgame_image.cfgpack", &v8);
                *((char **)&v4) = 104;
                memset(&v5, 0, 96);
                if (CreateProcessA(0, &v21, 0, 0, 0, 0, 0, v26, &v4, &v2))
                {
                    CloseHandle(v2.hProcess);
                    CloseHandle(*((long long *)&v3));
                    return security_cookie_check(v22 ^ &v0);
                }
                v34 = "Could not start the battle.";
            }
        }
        else
        {
            v34 = "The host's battle could not be prepared for the other side.";
LABEL_280f6:
        }
        strcpy_s(v27 + 20, 160, v34);
        v44 = *((long long *)(v27 + 616));
        if (v44 != -1)
            closesocket(v44);
        *((long long *)(v27 + 616)) = -1;
        *((char **)(v27 + 16)) = 5;
        return security_cookie_check(v22 ^ &v0);
    }
    v30 = fread(&v19, &(1)[v28], 511, v29);
    if (v30 >= 0x200)
    {
        sub_4078c();
        [D] Unsupported jumpkind Ijk_SigTRAP at address 164184
    }
    *((char **)&(&v19)[v30]) = 0;
    fclose(v1);
    (unsigned int)v31 = strstr(&v19, "cur_id");
    if (!v31)
        goto LABEL_27e24;
    v32 = strchr(v31, 61);
}


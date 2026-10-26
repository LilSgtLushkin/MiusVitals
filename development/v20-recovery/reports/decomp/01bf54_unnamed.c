// v20 RVA 01bf54..01c245  (753 bytes)  load_battle_description
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    uint128_t field_0;
    unsigned int field_10;
} struct_1;

typedef struct struct_0 {
    char *field_0;
    unsigned int field_8;
    unsigned int field_c;
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


long long load_battle_description(unsigned long a0, unsigned long long a1, unsigned long long a2, unsigned long a3)
{
    char v0;  // [bp-0x248]
    unsigned long v1;  // [bp-0x228]
    unsigned int v2;  // [bp-0x218]
    char v3;  // [bp-0x210]
    unsigned int v4;  // [bp-0x208]
    void* v5;  // [bp-0x200]
    int v6;  // [bp-0x1f8]
    void* v7;  // [bp-0x1e8]
    char *v8;  // [bp-0x1d8]
    unsigned int v9;  // [bp-0x1d0]
    unsigned int v10;  // [bp-0x1cc]
    unsigned long v11;  // [bp-0x1c8]
    unsigned int v12;  // [bp-0x1c0]
    unsigned int v13;  // [bp-0x1bc]
    unsigned long v14;  // [bp-0x1b8]
    unsigned int v15;  // [bp-0x1b0]
    unsigned int v16;  // [bp-0x1ac]
    unsigned long v17;  // [bp-0x1a8]
    unsigned int v18;  // [bp-0x1a0]
    unsigned int v19;  // [bp-0x19c]
    int v20;  // [bp-0x198]
    unsigned int v21;  // [bp-0x188]
    char v22;  // [bp-0x168]
    unsigned long long v23;  // [bp-0x58]
    unsigned long v24;  // [bp-0x8]
    char v25;  // [bp+0x0]
    unsigned int v27;  // r12d
    unsigned int v28;  // r14d
    struct_1 *v29;  // rax
    unsigned long long v30;  // rax
    unsigned int v31;  // edi
    unsigned int v32;  // r15d
    struct_0 *v33;  // rbx, Other Possible Types: unsigned long
    unsigned long long v34;  // rax
    FILE_t *v35;  // r9
    unsigned long long v36;  // rcx
    unsigned long long v37;  // rbx, Other Possible Types: unsigned long
    unsigned long long v38;  // rdi, Other Possible Types: unsigned long

    v24 = &v25;
    v23 = g_5a380 ^ &v0;
    v27 = a3;
    v2 = a1;
    v28 = a0;
    v1 = "cloud";
    sprintf_s(&v22, 260, "%s\\users\\profiles\\%s\\ccombat.cfgpack", &g_5b920);
    *((char **)&v6) = 0;
    v7 = 0;
    if (!(char)read_file(&v22, &v6))
    {
        _Tidy@?$vector@EV?$allocator@E@std(&v6);
        return security_cookie_check(v23 ^ &v0);
    }
    if (g_5b600 == g_5b608)
    {
        a1 = (long long)v6;
        $_Assign_counted_range@PEBE@?$vector@EV?$allocator@E@std(&g_5b600, a1, (long long)(&v6)[8] - a1);
    }
    (unsigned int)v29 = find@image(&g_5b600, a1, a2);
    *((uint128_t *)&v20) = v29->field_0;
    v21 = *((int *)(v29 + 16));
    v8 = "user_side";
    v9 = a0;
    *((char **)&v10) = 0;
    v11 = "enemy_side";
    v12 = a1;
    *((char **)&v13) = 0;
    v14 = "year";
    v15 = a2;
    *((char **)&v16) = 1;
    v17 = "period";
    v18 = a3;
    *((char **)&v19) = 1;
    v32 = 0;
    v33 = &v8;
    do
    {
        v30 = field@image@image(&v6, &v20, *((long long *)v33));
        if (v30)
        {
            v31 = *((int *)&(8)[v33]);
            if (v31)
            {
                memcpy(&v3, &(16)[v6 + v30], 4);
                if (*((int *)&v3) == 4)
                {
                    v4 = v31 - *((int *)&(12)[v33]);
                    memcpy((long long)v6 + v30, &v4, 4);
                    v32 = &(1)[v32];
                }
            }
        }
    } while ((v33 = &v11, v33 != &v20));
    v5 = 0;
    if (v32)
    {
        (unsigned int)v34 = fopen_s(&v5, &v22, &g_46b50);
        if (!(unsigned int)v34)
        {
            v35 = v5;
            if (v35)
            {
                v36 = (long long)v6;
                fwrite(v36, &(1)[v34], (long long)(&v6)[8] - v36, v35);
                fclose(v5);
                v2 = v2;
                v37 = "Germans";
                if (v2 == 5391687)
                {
                    v38 = "Germans";
                }
                else if (v2 == 1381192533)
                {
                    v38 = "Soviets";
                }
                else
                {
                    memcpy(&g_5b5c8, &v2, 4);
                    *((char **)&g_5b5cc) = 0;
                    v38 = &g_5b5c8;
                }
                v2 = v28;
                if (v28 != 5391687)
                {
                    if (v28 == 1381192533)
                    {
                        v37 = "Soviets";
                    }
                    else
                    {
                        memcpy(&g_5b5c8, &v2, 4);
                        *((char **)&g_5b5cc) = 0;
                        v37 = &g_5b5c8;
                    }
                }
                v1 = v38;
                mp_log("ui: CHOOSE MY FORCE - the Skirmish screen is set to the host's year (%u) and season (%u), the %s against the %s", -(0 < (unsigned int)a2) & a2 - 1, -(0 < v27) & v27 - 1, v37);
            }
        }
    }
}


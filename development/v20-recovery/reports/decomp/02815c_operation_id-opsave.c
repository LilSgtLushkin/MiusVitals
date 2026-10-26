// v20 RVA 02815c..028348  (492 bytes)  operation_id@opsave
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
    unsigned long long field_-8;
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


long long operation_id@opsave(unsigned long long a0)
{
    void* v0;  // [bp-0x38]
    void* v1;  // [bp-0x28]
    int v2;  // [bp-0x20]
    void* v3;  // [bp-0x10]
    unsigned long v4;  // [bp-0x8]
    char v5;  // [bp+0x0]
    char v7;  // [bp+0x10], Other Possible Types: unsigned int
    unsigned int v8;  // [bp+0x18]
    char v9;  // [bp+0x20]
    unsigned long v10;  // rax, Other Possible Types: unsigned long long
    FILE_t *v11;  // rcx
    unsigned long long v12;  // rax
    unsigned long long v13;  // rcx
    unsigned long long v15;  // rbx, Other Possible Types: unsigned long
    struct_0 *v16;  // rcx

    v4 = &v5;
    *((char **)&v2) = 0;
    v3 = 0;
    v1 = 0;
    (unsigned int)v10 = fopen_s(&v1, a0, "rb");
    if (!(unsigned int)v10)
    {
        v11 = v1;
        if (v11)
        {
            fseek(v11, 0, &(2)[v10]);
            v12 = ftell(v1);
            fseek(v1, 0, 0);
            if ((unsigned int)(v12 - 1) <= 16777214)
            {
                $_Resize@U_Value_init_tag@std$_Resize@U_Value_init_tag@std(&v2, v12, &v7);
                v13 = (long long)v2;
                $_Resize@U_Value_init_tag@std$_Resize@U_Value_init_tag@std(&v2, fread(v13, 1, (long long)(&v2)[8] - v13, v1), &v7);
            }
            fclose(v1);
            *((char **)&v8) = 0;
            *((char **)&v7) = 0;
            g_5c4b8 = -(0 < sides@image(&v2, &v7, &v9)) & v7;
            v10 = squad_owners@image@image(&v2, g_5c4b8, &g_5c4b4, &v8);
            if (g_5c4b4)
            {
                clear@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&g_5c150, &v2);
                v7 = g_5c4b8;
                if (g_5c4b8 == 5391687)
                {
                    v15 = "Germans";
                }
                else if (g_5c4b8 == 1381192533)
                {
                    v15 = "Soviets";
                }
                else
                {
                    v15 = &g_5b5c8;
                    memcpy(&g_5b5c8, &v7, 4);
                    *((char **)&g_5b5cc) = 0;
                }
                v10 = mp_log("ui: this player's own force is offered to the host: %u squads, %u units of the %s", g_5c4b4, v8, v15);
            }
        }
    }
    v16 = (long long)v2;
    if (!v16)
        return v10;
    if (v3 - v16 >= __Eg_my_units && -8 + (char *)v16 - *((long long *)(v16 - 8)) > 31)
    {
        v0 = 0;
        _invoke_watson(0, 0, 0, 0);
        [D] Unsupported jumpkind Ijk_SigTRAP at address 164647
    }
    v10 = sub_4030c();
    *((char **)&v2) = 0;
    v3 = 0;
    return v10;
}


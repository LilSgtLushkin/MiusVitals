// v20 RVA 01c250..01c331  (225 bytes)  start_program
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


long long start_program(unsigned long a0)
{
    char v0;  // [bp-0x168]
    unsigned long v1;  // [bp-0x148]
    void* v2;  // [bp-0x138]
    char v3;  // [bp-0x128]
    unsigned long long v4;  // [bp-0x18]
    unsigned long long v6;  // rax
    FILE_t *v7;  // r9

    v4 = g_5a380 ^ &v0;
    if (g_5b600 == g_5b608)
        return security_cookie_check(v4 ^ &v0);
    v1 = "cloud";
    sprintf_s(&v3, 260, "%s\\users\\profiles\\%s\\ccombat.cfgpack", &g_5b920);
    v2 = 0;
    (unsigned int)v6 = fopen_s(&v2, &v3, &g_46b50);
    if (!(unsigned int)v6)
    {
        v7 = v2;
        if (v7)
        {
            fwrite(g_5b600, &(1)[v6], g_5b608 - g_5b600, v7);
            fclose(v2);
        }
    }
    if (!((char)a0 && g_5b600 != g_5b608))
        return security_cookie_check(v4 ^ &v0);
    g_5b608 = g_5b600;
}


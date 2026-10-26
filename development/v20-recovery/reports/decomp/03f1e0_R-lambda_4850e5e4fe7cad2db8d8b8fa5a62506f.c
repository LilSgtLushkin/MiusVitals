// v20 RVA 03f1e0..03f5ca  (1002 bytes)  R<lambda_4850e5e4fe7cad2db8d8b8fa5a62506f>
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


long long R(unsigned long long *a0, unsigned long a1)
{
    char v0;  // [bp-0x248]
    void* v1;  // [bp-0x228], Other Possible Types: unsigned long long *
    unsigned int v2;  // [bp-0x218]
    unsigned int v3;  // [bp-0x214]
    unsigned int v4[2];  // [bp-0x210]
    int v5;  // [bp-0x208]
    void* v6;  // [bp-0x1f8]
    char v7;  // [bp-0x1f4]
    char v8;  // [bp-0x1f0]
    char v9;  // [bp-0x1e0]
    char v10;  // [bp-0x1c8]
    char v11;  // [bp-0x188]
    char v12;  // [bp-0x148]
    unsigned long long v13;  // [bp-0x38]
    unsigned int v15;  // ebx
    void* v16;  // rdi, Other Possible Types: unsigned long, unsigned long long
    unsigned long long v17;  // rsi, Other Possible Types: unsigned long
    unsigned long long *v18;  // r13
    unsigned long long v19;  // rax
    char *v20;  // r15
    unsigned long v21;  // rax
    void* v22;  // rbx, Other Possible Types: unsigned long
    void* v23;  // r14, Other Possible Types: unsigned long
    FILE_t *v24;  // r8, Other Possible Types: unsigned long
    unsigned long long v25;  // rbx
    unsigned long long v26;  // r13
    unsigned int v27;  // r14d
    void* v28;  // rsi, Other Possible Types: unsigned long
    unsigned int v29;  // r13d
    unsigned long long v30;  // rbx
    char *v31;  // r15
    unsigned long long v32;  // rax
    unsigned long long v33;  // rbx

    v13 = g_5a380 ^ &v0;
    v15 = a1;
    if (!(char)steam_init())
        return security_cookie_check(v13 ^ &v0);
    v16 = 0;
    $sprintf_s@$0BA(&v9, "%u ", 20);
    if (!g_5df68)
    {
        (unsigned int)v19 = GetTickCount();
        v27 = (unsigned int)v19 - g_5df1c;
        if ((unsigned int)v19 - g_5df1c > 5000)
            g_5df1c = v19;
        v28 = 0;
        v29 = g_5de18(g_5de68, 4);
        if (v29 > 0)
        {
            v1 = a0;
            while ((unsigned int)v28 < 2000 && (unsigned int)v16 < v15)
            {
                v30 = g_5de20(g_5de68, v28, 4);
                v6 = 0;
                *((char **)&v5) = 0;
                if (v30 && (char)g_5de30(g_5de68, v30, &v5) && ((int)v5 & 16777215) == g_5de90)
                {
                    v31 = g_5de08(g_5de68, v30, "gtmf");
                    if (!v31 || !*(v31))
                    {
                        if (v27 > 5000)
                            g_5de10(g_5de68, v30);
                    }
                    else
                    {
                        if (!strncmp(v31, &v9, strlen(&v9)))
                        {
                            (unsigned int)v32 = g_5de28(g_5de68, v30);
                            *(v1) = v30;
                            v33 = v16 * 104;
                            strncpy_s(a0 + 8 + v33, 64, (v32 != 0 ? "?" : v32), 63);
                            strncpy_s(a0 + 72 + v33, 32, &v31[strlen(&v9)], 31);
                            v16 = &(1)[v16];
                            v1 += 104;
                        }
                    }
                }
                v15 = a1;
                v28 = &(1)[v28];
                if ((unsigned int)v28 >= v29)
                    break;
            }
        }
    }
    else
    {
        v17 = 1;
        v18 = a0;
        while ((unsigned int)v16 < v15)
        {
            if (v17 != g_5df68)
            {
                memset(&v10, 0, 128);
                hook_srand(&v12, 260, v17);
                GetSystemTimeAsFileTime(v4);
                if (GetFileAttributesExA(&v12, 0, &v5))
                {
                    v2 = *((int *)&v7);
                    v3 = *((int *)&v8);
                    if (v4 - *((long long *)&v2) <= 0x4c4b400)
                    {
                        v1 = 0;
                        if (!fopen_s(&v1, &v12, "r"))
                        {
                            v24 = v1;
                            if (v24)
                            {
                                v22 = 0;
                                v23 = 0;
                                while (true)
                                {
                                    v20 = &v10;
                                    if (!(fgets(v20, 64, v24)) || !((v21 = strcspn(v20, "\r\n") + v23, v22 = (unsigned long long)((unsigned int)v22 + 1), v23 += 64, *((char **)&(&v10)[v21]) = 0, (unsigned int)v22 < 2)))
                                        break;
                                    v24 = v1;
                                }
                                fclose(v1);
                                if (!strncmp(&v10, &v9, strlen(&v9)))
                                {
                                    v25 = v16 * 104;
                                    *(v18) = v17;
                                    strncpy_s(a0 + 8 + v25, 64, &v11, 63);
                                    strncpy_s(a0 + 72 + v25, 32, &(&v10)[strlen(&v9)], 31);
                                    v16 += 1;
                                    v26 = v18 + 104;
                                }
                                v15 = a1;
                            }
                        }
                    }
                }
            }
            v17 = &(1)[v17];
            if (v17 > 8)
                break;
        }
    }
    return security_cookie_check(v13 ^ &v0);
}


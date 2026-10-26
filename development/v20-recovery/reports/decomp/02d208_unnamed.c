// v20 RVA 02d208..02d581  (889 bytes)  restore_slot
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[488];
    struct struct_2 *field_1e8;
    unsigned long long field_1f0;
} struct_1;

typedef struct struct_0 {
    char field_0;
} struct_0;

typedef struct WIN32_FIND_DATAA {
    unsigned int dwFileAttributes;
    FILETIME ftCreationTime;
    char ftLastAccessTime;
    char paddin13[7];
    char ftLastWriteTime;
    char paddin21[7];
    unsigned int nFileSizeHigh;
    unsigned int nFileSizeLow;
    unsigned int dwReserved0;
    unsigned int dwReserved1;
    char cFileName[260];
    char cAlternateFileName[14];
} WIN32_FIND_DATAA;

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
    char paddin1[3];
    char field_4;
} struct_2;

typedef struct FILETIME {
    unsigned int dwLowDateTime;
    unsigned int dwHighDateTime;
} FILETIME;

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


long long restore_slot(struct_1 *a0, unsigned long long a1, unsigned long long a2)
{
    char v0;  // [bp-0x588]
    struct_0 *v1;  // [bp-0x568]
    char v2;  // [bp-0x558]
    char v3;  // [bp-0x554]
    void* v4;  // [bp-0x550]
    struct_0 *v5;  // [bp-0x548]
    unsigned long v6;  // [bp-0x540]
    WIN32_FIND_DATAA v7;  // [bp-0x538]
    char v8;  // [bp-0x50c]
    char v9;  // [bp-0x3f8]
    char v10;  // [bp-0x2e8]
    char v11;  // [bp-0x268]
    char v12;  // [bp-0x158]
    unsigned long long v13;  // [bp-0x48]
    unsigned long v14;  // [bp-0x8]
    char v15;  // [bp+0x0]
    struct_1 *v17;  // rbx
    struct struct_0 **v18;  // rdi, Other Possible Types: unsigned long
    unsigned int *v19;  // rsi
    unsigned int *v20;  // rdi
    unsigned int v21;  // eax
    unsigned long v22;  // rcx
    unsigned long long v23;  // rsi
    struct_0 *v25;  // rdi
    unsigned long long v26;  // r12
    unsigned long long v27;  // r14
    unsigned long long v28;  // rdx
    unsigned long long v29;  // rax
    FILE_t *v30;  // r9
    char v31[5];  // rdi, Other Possible Types: unsigned long

    v14 = &v15;
    v13 = g_5a380 ^ &v0;
    v17 = a0;
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v11, "%s\\users\\profiles\\%s", a1, "mp_guest");
    CreateDirectoryA(&v11, 0);
    v5 = &"*.cfgpack";
    v18 = &v5;
    v6 = "*.tcsprofile";
    do
    {
        v1 = *((long long *)v18);
        $sprintf_s@$0BAE$sprintf_s@$0BAE(&v9, "%s\\users\\profiles\\%s\\%s", a1, a2);
        v19 = FindFirstFileA(&v9, &v7);
        if (v19 != -1)
        {
            do
            {
                if (!((char)v7.dwFileAttributes & 16))
                {
                    v1 = &v8;
                    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v12, "%s\\users\\profiles\\%s\\%s", a1, a2);
                    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v9, "%s\\%s", &v11, &v8);
                    CopyFileA(&v12, &v9, 0);
                }
            } while (FindNextFileA(v19, &v7));
            FindClose(v19);
        }
    } while ((v18 = &v6, v18 != &v7));
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v9, "%s\\*.damsav2", &v11);
    v20 = FindFirstFileA(&v9, &v7);
    if (v20 != -1)
    {
        do
        {
            $sprintf_s@$0BAE$sprintf_s@$0BAE(&v9, "%s\\%s", &v11, &v8);
            DeleteFileA(&v9);
        } while (FindNextFileA(v20, &v7));
        FindClose(v20);
    }
    v31 = 4;
    if (*((long long *)(a0 + 496)) - *((long long *)(a0 + 488)) < 4)
        return security_cookie_check(v13 ^ &v0);
    memcpy(&v3);
    v21 = *((int *)&v3);
    if (v21 <= __Eg_my_units)
    {
        if (!v21)
            return security_cookie_check(v13 ^ &v0);
        while (true)
        {
            v22 = *((long long *)(a0 + 488));
            v23 = &(1)[v31];
            if (!(v23 <= *((long long *)(v17 + 496)) - v22) || !(v27 <= *((long long *)(v17 + 496)) - v22) || !((a0 = v22 + v23, (char)field@image@image(a0, v25))))
                break;
            memcpy(&v10, *((long long *)(v17 + 488)) + v23, v25);
            if (v25 >= 128)
            {
                sub_4078c();
                [D] Unsupported jumpkind Ijk_SigTRAP at address 185728
            }
            v28 = *((long long *)(v17 + 488)) + v26;
            *((char **)(&v10 + v25)) = 0;
            memcpy(&v2, v28, 4);
            if (*((int *)&v2) + v27 > *((long long *)(v17 + 496)) - *((long long *)(v17 + 488)))
                break;
            $sprintf_s@$0BAE$sprintf_s@$0BAE(&v9, "%s\\%s", &v11, &v10);
            v4 = 0;
            (unsigned int)v29 = fopen_s(&v4, &v9, &g_46b50);
            if (!(!(unsigned int)v29) || !((v30 = v4, v30)))
                break;
            fwrite(*((long long *)(v17 + 488)) + v27, &(1)[v29], *((int *)&v2), v30);
            fclose(v4);
            v31 = *((int *)&v2) + v27;
            if (1 >= *((int *)&v3))
                return security_cookie_check(v13 ^ &v0);
        }
    }
}


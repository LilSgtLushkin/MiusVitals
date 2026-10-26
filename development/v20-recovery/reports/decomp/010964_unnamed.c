// v20 RVA 010964..010ddc  (1144 bytes)  load_battle_description
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    struct struct_0 *field_-8;
    char field_0;
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


long long load_battle_description(unsigned int a0[4], unsigned long a1)
{
    char v0;  // [bp-0x1a8]
    int tmp_2;  // tmp #2
    void* v1;  // [bp-0x188]
    unsigned int v2;  // [bp-0x178]
    char v3;  // [bp-0x170], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x168]
    struct_0 *v5;  // [bp-0x160]
    struct_0 *v6;  // [bp-0x158]
    void* v7;  // [bp-0x150], Other Possible Types: unsigned long
    void* v8;  // [bp-0x148]
    char v9;  // [bp-0x138]
    unsigned long long v10;  // [bp-0x28]
    unsigned long v11;  // [bp-0x8]
    char v12;  // [bp+0x0]
    unsigned int v15[4];  // rbx
    char *v16;  // rdx
    unsigned long long v17;  // rcx
    char *v18;  // rdi
    struct_0 *v19;  // r8
    unsigned int v20;  // r14d
    unsigned long long v21;  // r8
    unsigned long v22;  // rcx
    unsigned int v23;  // edx
    unsigned long long v24;  // rdi
    unsigned int v25;  // ebx
    unsigned int v26;  // esi
    unsigned long long v27;  // rsi, Other Possible Types: unsigned long
    unsigned long long v28;  // rax
    unsigned long long v29;  // rbx, Other Possible Types: unsigned long
    FILE_t *v30;  // r9
    struct_0 *v31;  // rbx, Other Possible Types: unsigned long
    unsigned long long v32;  // rsi, Other Possible Types: unsigned long
    struct_0 *v33;  // rax

    v11 = &v12;
    v10 = g_5a380 ^ &v0;
    v15 = a0;
    if (g_5b7c8 == g_5b7d0)
        return security_cookie_check(v10 ^ &v0);
    v16 = *((int *)(a0 + 8));
    if (v16 >= g_5b6c0 - g_5b6b8)
    {
        return security_cookie_check(v10 ^ &v0);
    }
    else if (v16[g_5b6b8])
    {
        return security_cookie_check(v10 ^ &v0);
    }
    else
    {
        v17 = *((int *)(a0 + 12));
        if ((unsigned int)v17 > 1000)
            return security_cookie_check(v10 ^ &v0);
        v18 = v16 * 1000;
        if (&v18[v17] > g_5b7d0 - g_5b7c8)
        {
            return security_cookie_check(v10 ^ &v0);
        }
        else if ((unsigned int)a1 >= (unsigned int)&(16)[v17])
        {
            memcpy(v18 + g_5b7c8, v15 + 16);
            *((char **)(*((int *)(v15 + 8)) + g_5b6b8)) = 1;
            *((char **)&g_5bf3c) = &(1)[g_5bf3c];
            if (g_5bf3c < g_5b6c0 - g_5b6b8)
                return security_cookie_check(v10 ^ &v0);
            v5 = 0;
            v7 = 0;
            v31 = g_5b7c8;
            v5 = g_5b7c8;
            g_5b7c8 = 0;
            v19 = g_5b7d0;
            v6 = g_5b7d0;
            g_5b7d0 = 0;
            v7 = g_5b7d8;
            g_5b7d8 = 0;
            v20 = g_5bf38;
            *((char **)&g_5bf38) = 0;
            if (g_5b6b8 != g_5b6c0)
                g_5b6c0 = g_5b6b8;
            tmp_2 = v19;
            v21 = v19 - v31;
            v23 = 2166136261;
            v22 = 0;
            if (tmp_2 != v31)
            {
                do
                {
                    v23 = (*(v22 + (char *)&v31->field_-8) ^ v23) * 16777619;
                    v22 = &(1)[v22];
                } while (v22 < v21);
            }
            if (v23 != v20)
            {
                mp_log("lobby: the other player's force arrived damaged, fetching it again");
            }
            else
            {
                *((char **)&v2) = 0;
                v24 = -(0 < sides@image(&v5, &v2, &v3)) & v2;
                if (g_5b618)
                    v25 = g_5be3c;
                else
                    v25 = operation_id@opsave@opsave();
                *((char **)&v2) = 0;
                *((char **)&v4) = 0;
                squad_owners@image@image(&v5, v24, &v2, &v4);
                v26 = v2;
                if (!v26 || v25 && (unsigned int)v24 != v25)
                {
                    g_5bfb4 = v20;
                    v3 = v25;
                    v27 = "Germans";
                    if (v25 == 5391687)
                    {
                        v29 = "Germans";
                    }
                    else if (v25 == 1381192533)
                    {
                        v29 = "Soviets";
                    }
                    else
                    {
                        memcpy(&g_5b5c8, &v3, 4);
                        *((char **)&g_5b5cc) = 0;
                        v29 = &g_5b5c8;
                    }
                    v3 = v24;
                    if ((unsigned int)v24 != 5391687)
                    {
                        if ((unsigned int)v24 == 1381192533)
                        {
                            v27 = "Soviets";
                        }
                        else
                        {
                            memcpy(&g_5b5c8, &v3, 4);
                            *((char **)&g_5b5cc) = 0;
                            v27 = &g_5b5c8;
                        }
                    }
                    mp_log("lobby: the other player's force (%u squads of the %s) is not used: he commands the %s", v2, v27, v29);
                    v31 = v5;
                }
                else
                {
                    sprintf_s(&v9, 260, "%s\\users\\mp_peer_force.cfgpack", &g_5b920);
                    v8 = 0;
                    (unsigned int)v28 = fopen_s(&v8, &v9, &g_46b50);
                    if (!(!(unsigned int)v28) || !((v30 = v8, v30)))
                    {
                        mp_log("lobby: cannot keep the other player's force in %s", &v9);
                    }
                    else
                    {
                        v31 = v5;
                        fwrite(v31, &(1)[v28], (char *)v6 - v31, v30);
                        fclose(v8);
                        SetEnvironmentVariableA("MP_PEER_FORCE", &v9);
                        g_5bfb0 = v20;
                        g_5bfb8 = v26;
                        g_5bfbc = v24;
                        v3 = v24;
                        if ((unsigned int)v24 == 5391687)
                        {
                            v32 = "Germans";
                        }
                        else if ((unsigned int)v24 == 1381192533)
                        {
                            v32 = "Soviets";
                        }
                        else
                        {
                            memcpy(&g_5b5c8, &v3, 4);
                            *((char **)&g_5b5cc) = 0;
                            v32 = &g_5b5c8;
                        }
                        mp_log("lobby: the other player's own force arrived: %u squads, %u units of the %s - his side of the battle will be this force", v2, v4, v32);
                        if (g_5b618)
                            xlate_frame();
                    }
                }
            }
            if (!v31)
                return security_cookie_check(v10 ^ &v0);
            v33 = v31;
            if (v7 - v31 >= __Eg_my_units)
            {
                v31 = *((long long *)(v31 - 8));
                if (&(&v33->field_0)[-1 * v31] > 31)
                {
                    v1 = 0;
                    _invoke_watson(0, 0, 0, 0);
                    [D] Unsupported jumpkind Ijk_SigTRAP at address 69018
                }
            }
            sub_4030c(v31);
            v5 = 0;
            v7 = 0;
            return security_cookie_check(v10 ^ &v0);
        }
        else
        {
            return security_cookie_check(v10 ^ &v0);
        }
    }
}


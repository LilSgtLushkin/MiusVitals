// v20 RVA 0112e8..011a35  (1869 bytes)  host_map_frame
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    unsigned long long field_-8;
} struct_2;

typedef struct struct_0 {
    struct struct_0 *field_-8;
} struct_0;

typedef struct struct_3 {
    char field_0;
} struct_3;

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
    char paddin0[4704];
    unsigned int field_1260;
} struct_1;

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


long long host_map_frame(unsigned long long a0, unsigned long a1)
{
    char v0;  // [bp-0x538]
    void* v1;  // [bp-0x518], Other Possible Types: unsigned long, unsigned int
    struct_3 *v2;  // [bp-0x510], Other Possible Types: unsigned int
    int v3;  // [bp-0x508]
    void* v4;  // [bp-0x4f8]
    unsigned int v5;  // [bp-0x4f0]
    unsigned int v6;  // [bp-0x4ec]
    unsigned int v7;  // [bp-0x4e8]
    void* v8;  // [bp-0x4e0]
    unsigned int v9;  // [bp-0x4d8]
    int v10;  // [bp-0x4b0], Other Possible Types: char
    int v11;  // [bp-0x4a0]
    char v12;  // [bp-0x490]
    char v13;  // [bp-0x480]
    char v14;  // [bp-0x470]
    char v15;  // [bp-0x448]
    char v16;  // [bp-0x447]
    char v17;  // [bp-0x3e8]
    char v18;  // [bp-0x2d8]
    char v19;  // [bp-0x1c8]
    unsigned long long v20;  // [bp-0x38]
    FILE_t *v22;  // esi, Other Possible Types: unsigned int, unsigned long
    unsigned long long v23;  // rbx
    char v24;  // dil
    unsigned int v25;  // eax
    struct_2 *v26;  // eax, Other Possible Types: char, unsigned long, unsigned long long, unsigned int
    unsigned int v28;  // r15, Other Possible Types: unsigned long, unsigned long long
    struct_0 *v29;  // rcx, Other Possible Types: unsigned long
    struct_0 *v30;  // rax
    char v31;  // r12b
    unsigned int v32;  // r14d
    unsigned int v33;  // eax, Other Possible Types: unsigned long
    unsigned long long v34;  // r9
    struct_3 *v35;  // r8
    unsigned long long v36;  // rcx
    struct_2 *v37;  // rcx
    unsigned int v38;  // ecx
    unsigned long v39;  // rdx
    unsigned long long v40;  // rcx
    struct_1 *v41;  // rax

    v20 = g_5a380 ^ &v0;
    v22 = a1;
    v23 = a0;
    v24 = 1;
    if (g_5b8e0 != 1)
        return security_cookie_check(v20 ^ &v0);
    if (g_5b618)
        return security_cookie_check(v20 ^ &v0);
    if (g_5b8e4 != 2)
    {
        return security_cookie_check(v20 ^ &v0);
    }
    else if (operation_on_screen())
    {
        v25 = g_5bcd8;
        if (!g_5bcd8)
        {
            v25 = a1;
            g_5bcd8 = a1;
        }
        v28 = (unsigned int)a1 - v25;
        if (!g_5c071 && v28 > 4000)
        {
            if (g_5b72e)
            {
                *((char **)&g_5b72e) = 0;
                end_turn_here(a0);
                *((char **)&g_5bcdc) = &(&g_1770)[a1];
                return security_cookie_check(v20 ^ &v0);
            }
            v26 = g_5bcdc;
            if (!g_5bcdc)
            {
                *((char **)&g_5c071) = 1;
                snapshot_operation_saves(a0);
                goto LABEL_113b4;
            }
        }
        else
        {
LABEL_113b4:
            if (g_5b72e)
            {
                if (v28 > 4000)
                {
                    *((char **)&g_5b72e) = 0;
                    end_turn_here(a0);
                    *((char **)&g_5bcdc) = &(&g_1770)[a1];
                    return security_cookie_check(v20 ^ &v0);
                }
                return security_cookie_check(v20 ^ &v0);
            }
            v26 = g_5bcdc;
            if (!g_5bcdc)
            {
LABEL_1172a:
                if (g_5b89f)
                {
                    *((char **)&g_5b89f) = 0;
                    *((char **)&v3) = 0;
                    v4 = 0;
                    *((char **)&v15) = 0;
                    memset(&v16, 0, 95);
                    v31 = save_operation(a0, &v3, &v15, 96);
                    if (!v31)
                    {
                        v32 = 0;
                        v33 = -1;
                    }
                    else
                    {
                        v32 = rows_of@opsave(&v3, "user_orders");
                        (unsigned int)v33 = turn_phase@opsave@opsave(&v3);
                    }
                    v34 = (long long)(&v3)[8] - (long long)v3;
                    a1 = (v31 ? "not saved" : "saved");
                    v2 = v32;
                    v1 = v33;
                    v35 = &v15;
                    v36 = "test: operation %s (%s, %zu bytes, turn phase %d, %u orders)";
                    mp_log();
                    v37 = (long long)v3;
                    if (v37)
                    {
                        if (v4 - v37 >= __Eg_my_units && -8 + (char *)v37 - *((long long *)(v37 - 8)) > 31)
                        {
                            v1 = 0;
                            _invoke_watson(0, 0, 0, 0);
                            [D] Unsupported jumpkind Ijk_SigTRAP at address 71718
                        }
                        sub_4030c();
                        *((char **)&v3) = 0;
                        v4 = 0;
                    }
                }
                if (!g_5b89e)
                {
LABEL_11898:
                    *((char *)&v26) = g_5b72f;
                    if (g_5b72f == 0)
                        goto LABEL_11934;
                    v38 = g_5b8e0;
                    *((char *)&a1) = g_5b618;
                    *((char *)&v35) = (char)g_5b729;
                    *((char *)&v34) = g_5b839;
                }
                else
                {
                    *((char **)&g_5b89e) = 0;
                    v38 = g_5b8e0;
                    if (g_5b8e0 != 1 || (*((char *)&a1) = g_5b618, !(!g_5b618) || !((char)g_5b729) || !(g_5b839) || !(g_5b61b)))
                    {
                        end_turn_here(v23);
                        goto LABEL_11898;
                    }
                    *((char *)&v26) = g_5b72f;
                    if (!g_5b72f)
                    {
                        sub_183b0();
                        goto LABEL_11898;
                    }
                }
                if (g_5b838 && v38 == 1 && !(char)a1 && (char)v35 && (char)v34)
                {
                    if (g_5b61b == 0)
                        goto LABEL_1193d;
                    if (!g_5b72c && !g_5bd40)
                    {
                        *((char **)&g_5b72c) = 1;
                        g_5bbcc = GetTickCount();
                        v39 = (g_5bbb0 != 0 ? "the other player" : &g_5bbb0);
                        v40 = "map: ending the turn - asking %s for his moves";
                        mp_log();
                        v26 = g_5b72f;
                    }
                }
LABEL_11934:
                if (g_5b61b)
                    menu_controller_of(v23);
LABEL_1193d:
                if (((char)v26 || g_5b72c) && !g_5bd40)
                {
                    mp_log("map: the other player's game is gone; the turn is not ended for him");
                    *((char **)&g_5b72f) = 0;
                    *((char **)&g_5b72c) = 0;
                    strcpy_s(&g_5bd50, 120, "the other player's game is gone - next turn ends the turn without him");
                }
                (unsigned int)v41 = menu_controller_of(v23);
                if (!v41 || *((int *)(v41 + &g_1260)) != 2)
                    v24 = 0;
                if ((unsigned int)v28 <= 4000)
                {
                    return security_cookie_check(v20 ^ &v0);
                }
                else if (v24)
                {
                    return security_cookie_check(v20 ^ &v0);
                }
                else if ((char)g_5b729)
                {
                    return security_cookie_check(v20 ^ &v0);
                }
                else if (g_5b72c)
                {
                    return security_cookie_check(v20 ^ &v0);
                }
                else if (g_5bd40)
                {
                    return security_cookie_check(v20 ^ &v0);
                }
                else if (!g_5b61b)
                {
                    return security_cookie_check(v20 ^ &v0);
                }
                else if (!g_5b61e)
                {
                    return security_cookie_check(v20 ^ &v0);
                }
                else
                {
                    if (g_5bcd4 && (unsigned int)v22 <= g_5bcd4)
                        return security_cookie_check(v20 ^ &v0);
                    session_map_give(v23);
                    if (!(char)g_5b729)
                    {
                        *((char **)&g_5bcd4) = &(&g_3a98)[v22];
                        return security_cookie_check(v20 ^ &v0);
                    }
                    return security_cookie_check(v20 ^ &v0);
                }
            }
        }
        if ((unsigned int)a1 < v26)
            return security_cookie_check(v20 ^ &v0);
        *((char **)&g_5bcdc) = 0;
        0Sidecar@opsave(&v9);
        *((char **)&v3) = 0;
        v4 = 0;
        $sprintf_s@$0BAE$sprintf_s@$0BAE(&v17, "%s\\users\\mp_saves\\_map_sidecar.txt", &g_5b920);
        if (!(char)read_sidecar@opsave@opsave(&v17, &v9) || !(char)save_operation(a0, &v3, &v15, 96))
        {
            mp_log("map: the command tags could not be put back after the turn");
            a0 = (long long)v3;
            if (!a0)
            {
                1Sidecar@opsave(a0);
                return security_cookie_check(v20 ^ &v0);
            }
        }
        else
        {
            v28 = restore_tags@opsave(&v3, &v9);
            *((char **)&v6) = 0;
            *((char **)&v7) = 0;
            *((char **)&v5) = 0;
            all_player_led@opsave@opsave(&v3, &v6, &v7, &v5);
            mp_log("map: after the turn the host sees %u of the other player's %u battle groups, has identified %u (the game's own scouting)", v7, v6, v5);
            *((char **)&v10) = 0;
            *((char **)&v11) = 0;
            $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v10, &v15, strlen(&v15));
            operation_id@opsave@opsave(&v12, &v10, operation_name@opsave@opsave(&v14, &v3));
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v14);
            a1 = (long long)(&v11)[8];
            if (a1 > 15)
            {
                a1 = &(1)[a1];
                v29 = (long long)v10;
                v30 = v29;
                if (a1 >= __Eg_my_units)
                {
                    a1 = &(39)[a1];
                    v29 = *((long long *)(v29 - 8));
                    if (-8 + (char *)v30 - v29 > 31)
                    {
                        v1 = 0;
                        _invoke_watson(0, 0, 0, 0);
                        [D] Unsupported jumpkind Ijk_SigTRAP at address 71004
                    }
                }
                sub_4030c(v29, a1);
            }
            *((int128_t *)&v11) = 0xf0000000000000000;
            *((char **)&v10) = 0;
            v2 = &v15;
            v1 = "cloud";
            sprintf_s(&v18, 260, "%s\\users\\profiles\\%s\\%s", &g_5b920);
            v8 = 0;
            if (*((long long *)&v13) && !fopen_s(&v8, &v18, &g_46b50))
            {
                v22 = v8;
                if (v22)
                {
                    fwrite(sub_1014c(&v3), 1, sub_1eddc(&v3), v22);
                    fclose(v8);
                    DeleteFileA(&v17);
                    mp_log("map: turn processed with both players' moves; %u formations and groups back with the computer's tags, reloading", v28);
                    SetEnvironmentVariableA("MP_TURNDONE", "1");
                    v1 = turns@opsave@opsave(&v3);
                    $sprintf_s@$0BJA(&v19, "root\\programs\\opgame.progpack, %s, %s, 0, %u", "cloud", sub_da00(&v12));
                    start_program(&v19);
                    ExitProcess(0); /* do not return */
                }
            }
            mp_log("map: cannot write the operation after the turn");
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v12);
            a0 = (long long)v3;
            if (!a0)
            {
                1Sidecar@opsave(a0);
                return security_cookie_check(v20 ^ &v0);
            }
        }
        v26 = a0;
        if (!(v4 - a0 >= __Eg_my_units) || !((a0 = *((long long *)(a0 - (char *)8)), v26 - a0 - 8 > 31)))
        {
            sub_4030c();
            v4 = 0;
            *((char **)&v3) = 0;
            1Sidecar@opsave(a0);
            return security_cookie_check(v20 ^ &v0);
        }
        v1 = 0;
        _invoke_watson(0, 0, 0, 0);
        goto LABEL_1172a;
        goto LABEL_1172a;
    }
    else
    {
        *((char **)&g_5bcd8) = 0;
        return security_cookie_check(v20 ^ &v0);
    }
}


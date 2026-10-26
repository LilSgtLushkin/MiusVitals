// v20 RVA 01b3d0..01b80d  (1085 bytes)  session_map_give
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_0;

typedef struct struct_1 {
    unsigned long long field_-8;
} struct_1;

extern char g_0;
extern char g_1;
extern char g_10;
extern char g_1000;
extern char g_101;
extern char g_104;
extern char g_18;
extern char g_1f;
extern char g_2;
extern char g_48;
extern unsigned long long g_5a380;
extern struct_2 *g_5b5d0;
extern unsigned long long g_5b5d8;
extern unsigned long long g_5b5e0;
extern char g_5b618;
extern char g_5b61b;
extern char g_5b729;
extern char g_5b72a;
extern char g_5b72b;
extern unsigned int g_5b7e0;
extern unsigned long long g_5b808;
extern unsigned long long g_5b810;
extern unsigned int g_5b8e0;
extern unsigned int g_5b8e4;
extern unsigned int g_5b91c;
extern unsigned int g_5ba24;
extern char g_5bbb0;
extern unsigned int g_5bbc8;
extern unsigned long long g_5bce0;
extern char g_5bd50;
extern char g_60;
extern char g_78;
extern char g_8;

long long session_map_give()
{
    char v0;  // [bp-0x218]
    int tmp_14;  // tmp #14
    unsigned int v1;  // [bp-0x1f0]
    char *v2;  // [bp-0x1e8], Other Possible Types: struct_0 *
    unsigned long v3;  // [bp-0x1e0]
    void* v4;  // [bp-0x1d8], Other Possible Types: unsigned long
    struct_0 *v5;  // [bp-0x1d0]
    unsigned int v6;  // [bp-0x1c0]
    unsigned int v7;  // [bp-0x1bc]
    char v8;  // [bp-0x1b8]
    char v9;  // [bp-0x198]
    char v10;  // [bp-0x138]
    unsigned long long v11;  // [bp-0x28]
    unsigned long v12;  // [bp-0x8]
    char v13;  // [bp+0x0]
    char v15;  // bl
    unsigned int v16;  // esi
    struct_0 *v17;  // rax, Other Possible Types: struct_1 *, unsigned long
    char *v18;  // rax
    char *v19;  // r9
    unsigned long long v20;  // rax
    unsigned long long v21;  // r8
    unsigned long long v22;  // rcx
    unsigned long long v23;  // r8
    unsigned int v24;  // edx
    unsigned long v25;  // rcx
    unsigned long v26;  // rdx
    unsigned long long v27;  // r9
    unsigned long long v28;  // r8
    unsigned long long v29;  // rcx
    struct_1 *v30;  // rax

    v12 = &v13;
    v11 = g_5a380 ^ &v0;
    if (g_5b729)
        return security_cookie_check(v11 ^ &v0);
    if (!g_5b61b)
        return security_cookie_check(v11 ^ &v0);
    if (g_5b8e0 != 1 || g_5b618 || g_5b8e4 != 2)
    {
        v15 = &g_0;
        if (!g_5b72a)
            return security_cookie_check(v11 ^ &v0);
    }
    else
    {
        v15 = &g_1;
    }
    v2 = &g_0;
    v4 = &g_0;
    if (!(char)save_operation())
    {
        strcpy_s(&g_5bd50, 120, "the operation could not be saved");
        goto LABEL_1b48e;
    }
    *((char **)&v6) = &g_0;
    v16 = &g_0;
    *((char **)&v7) = &g_0;
    if (v15)
    {
        ::0x17ac0::operation_name@opsave(&v8, &v2);
        ::0x12e08::append@?$basic_string@DU?$char_traits@D@std(&v10, &g_104, &v8);
        v5 = &g_0;
        (unsigned int)v17 = sub_402d0(&g_48);
        v17->field_0 = v17;
        *((struct_0 **)(v17 + &g_8)) = v17;
        *((struct_0 **)(v17 + &g_10)) = v17;
        *((char **)(v17 + &g_18)) = &g_101;
        v5 = v17;
        ::0x184fc::read_sidecar@opsave(&v10, &v8, &v5);
        ::0x20004::user_side@opsave(&v2, &v10, &v8, &v5);
        if (!(char)mirror@opsave(&v2, &g_5b7e0, &v6, &v5, &v7))
        {
            ::0x27dc::mp_log("map: %s has no formations to hand over", &v9);
            strcpy_s(&g_5bd50, 120, "this operation cannot be handed over");
            ::0x5e50::$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v5, &v5, *((long long *)(v5 + &g_8)));
        }
        while (::0x21974::turn_phase@opsave(&v2))
        {
            ::0x27dc::mp_log("map: not shared, the turn is past its orders phase");
            ::0x5e50::$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v5, &v5, *((long long *)(v5 + &g_8)));
            sub_4030c(v5, &g_48);
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v8);
            if (!v17)
                return security_cookie_check(v11 ^ &v0);
            v17 = v2;
            if (!(v4 - v17 >= __Eg_my_units && -8 + (char *)v17 - *((long long *)(v17 - &g_8)) > 31))
            {
LABEL_1b4c5:
                sub_4030c();
                v4 = &g_0;
                v2 = &g_0;
                return security_cookie_check(v11 ^ &v0);
            }
            _invoke_watson(0, 0, 0, 0);
        }
        ::0x232dc::write_sidecar@opsave(&v10, &v8, &v5);
        g_5bbc8 = ::0x21e80::turns@opsave(&v2);
        if (g_5b808 != g_5b810)
            g_5b810 = g_5b808;
        *((char **)&g_5ba24) = &g_0;
        *((char **)&g_5b72b) = &g_0;
        ::0x5e50::$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v5, &v5, *((long long *)(v5 + &g_8)));
        sub_4030c(v5, &g_48);
        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v8);
        v16 = v7;
    }
    strcpy_s(&g_5bce0, 96, &v9);
    v18 = g_5b5d0;
    v19 = v2;
    g_5b5d0 = v19;
    v2 = v18;
    v20 = g_5b5d8;
    v21 = (&v2)[1];
    g_5b5d8 = v21;
    v3 = v20;
    v22 = g_5b5e0;
    g_5b5e0 = v4;
    v4 = v22;
    tmp_14 = v21;
    v23 = v21 - v19;
    v24 = 2166136261;
    v25 = &g_0;
    if (tmp_14 != v19)
    {
        do
        {
            v24 = (v19[v25] ^ v24) * 16777619;
            v25 = &(&g_1)[v25];
        } while (v25 < v23);
    }
    g_5b91c = v24;
    *((char **)&g_5b729) = &g_1;
    *((char **)&g_5bd50) = &g_0;
    if (!v15)
    {
        ::0x27dc::mp_log("map: sending this player's moves to the host: %s, %zu bytes", &g_5bce0);
    }
    else
    {
        v26 = (g_5bbb0 != &g_0 ? "the other player" : &g_5bbb0);
        v1 = v16;
        v27 = v23;
        v28 = &g_5bce0;
        v29 = "map: shared with %s: %s, %zu bytes, %u formations and battle groups turned round; he has identified %u of the host's groups";
        ::0x27dc::mp_log();
    }
LABEL_1b48e:
    if (!(&v2)[1])
        return security_cookie_check(v11 ^ &v0);
    v30 = v2;
    if (v4 - (char *)v30 >= __Eg_my_units && -8 + (char *)v30 - *((long long *)(v30 - &g_8)) > 31)
    {
        _invoke_watson(0, 0, 0, 0);
        [D] Unsupported jumpkind Ijk_SigTRAP at address 112652
    }
    goto LABEL_1b4c5;
}


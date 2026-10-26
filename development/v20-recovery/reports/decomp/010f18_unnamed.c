// v20 RVA 010f18..011145  (557 bytes)  snapshot_operation_saves
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_0;

typedef struct struct_2 {
    unsigned long long field_0;
    void* field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_2;

typedef struct struct_1 {
    unsigned long long field_-8;
} struct_1;


long long snapshot_operation_saves(unsigned long long a0)
{
    char v0;  // [bp-0x218]
    void* v1;  // [bp-0x1f8]
    struct_0 *v2;  // [bp-0x1e8], Other Possible Types: struct_2 *
    struct_0 *v3;  // [bp-0x1d8], Other Possible Types: char
    void* v4;  // [bp-0x1c8]
    char v5;  // [bp-0x1c0]
    char v6;  // [bp-0x1a0]
    char v7;  // [bp-0x188]
    char v8;  // [bp-0x78]
    unsigned long long v9;  // [bp-0x18]
    unsigned long v10;  // [bp-0x8]
    char v11;  // [bp+0x0]
    struct_0 *v13;  // rax
    unsigned long long v14;  // rax
    struct_1 *v15;  // rcx

    v10 = &v11;
    v9 = g_5a380 ^ &v0;
    strncpy(&v3, "player", 6);
    v4 = 0;
    if (!g_5bbb0)
        GetEnvironmentVariableA("MP_PEER_NAME", &v6, 24);
    else
        strcpy_s(&v6, 24, &g_5bbb0);
    if ((char)save_operation(a0, &v3, &v8, 96))
    {
        operation_name@opsave@opsave(&v5, &v3);
        append@?$basic_string@DU?$char_traits@D@std@?$basic_string@DU?$char_traits@D@std(&v7, 260, &v5);
        v2 = 0;
        (unsigned int)v13 = sub_402d0(72);
        v13->field_0 = v13;
        *((struct_0 **)(v13 + 8)) = v13;
        *((struct_0 **)(v13 + 16)) = v13;
        *((char **)(v13 + 24)) = 257;
        v2 = &v13->field_0;
        read_sidecar@opsave@opsave(&v7, &v5, &v2);
        user_side@opsave@opsave(&v3, &v7, &v5, &v2);
        (unsigned int)v14 = all_player_led@opsave@opsave(&v3, &v2);
        if ((unsigned int)v14)
            write_sidecar@opsave@opsave(&v7, &v5, &v2);
        mp_log("map: %u more of the host's battle groups have fought %s - identified for him from now on (%zu in all)", v14, &v6, (&v2)[1]);
        $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v2, &v2, *((long long *)(v2 + 8)));
        sub_4030c(v2, 72);
        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v5);
    }
    v15 = *((long long *)&v3);
    if (!v15)
        return security_cookie_check(v9 ^ &v0);
    if (v4 - v15 >= __Eg_my_units && -8 + (char *)v15 - *((long long *)(v15 - 8)) > 31)
    {
        v1 = 0;
        _invoke_watson(0, 0, 0, 0);
        [D] Unsupported jumpkind Ijk_SigTRAP at address 69901
    }
    sub_4030c();
    v3 = 0;
    v4 = 0;
    return security_cookie_check(v9 ^ &v0);
}


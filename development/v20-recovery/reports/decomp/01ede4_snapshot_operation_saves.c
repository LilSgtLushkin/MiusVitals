// v20 RVA 01ede4..01f21c  (1080 bytes)  snapshot_operation_saves
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_0;
    unsigned long long field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_0;

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_1 {
    unsigned long long field_-8;
} struct_1;


long long snapshot_operation_saves()
{
    char v0;  // [bp-0x568]
    void* v1;  // [bp-0x548], Other Possible Types: struct_2 *
    struct_0 *v2;  // [bp-0x538]
    struct_0 *v3;  // [bp-0x528]
    void* v4;  // [bp-0x518]
    char v5;  // [bp-0x510]
    char v6;  // [bp-0x4f0]
    struct_0 *v7;  // [bp-0x4e9]
    char v8;  // [bp-0x4d9]
    char v9;  // [bp-0x4d8]
    struct_0 *v10;  // [bp-0x4d7]
    struct_0 *v11;  // [bp-0x4c7]
    struct_0 *v12;  // [bp-0x4b7]
    void* v13;  // [bp-0x4a7]
    unsigned int v14;  // [bp-0x49f]
    unsigned short v15;  // [bp-0x49b]
    char v16;  // [bp-0x499]
    char v17;  // [bp-0x498]
    struct_0 *v18;  // [bp-0x492]
    struct_0 *v19;  // [bp-0x482]
    struct_0 *v20;  // [bp-0x472]
    void* v21;  // [bp-0x462]
    unsigned short v22;  // [bp-0x45a]
    char v23;  // [bp-0x458]
    char v24;  // [bp-0x348]
    char v25;  // [bp-0x238]
    char v26;  // [bp-0x128]
    unsigned long long v27;  // [bp-0x18]
    unsigned long v28;  // [bp-0x8]
    char v29;  // [bp+0x0]
    char *v31;  // rbx, Other Possible Types: unsigned long
    unsigned long v34;  // rax
    struct_0 *v35;  // rax
    char *v36;  // rax, Other Possible Types: unsigned long

    v28 = &v29;
    v27 = g_5a380 ^ &v0;
    if (g_5b8e4 != 2)
        return security_cookie_check(v27 ^ &v0);
    strncpy(&v17, "cloud", 5);
    v18 = 0;
    v19 = 0;
    v20 = 0;
    v21 = 0;
    *((char **)&v22) = 0;
    strcpy(&v6, "nobody");
    v7 = 0;
    *((char **)&v8) = 0;
    for (v31 = GetCommandLineA(); v31; v31 = (strchr(v31, 44) == 0 ? (unsigned long long)&(1)[strchr(v31, 44)] : 0));
    GetEnvironmentVariableA("MP_PEER_NAME", &v6, 24);
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v24, "%s\\users\\profiles\\%s", &g_5b920, &v17);
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v23, "%s\\users\\mp_saves", &g_5b920);
    CreateDirectoryA(&v23, 0);
    v1 = &v6;
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v23, "%s\\users\\mp_saves\\%s_vs_%s", &g_5b920, "Player");
    CreateDirectoryA(&v23, 0);
    mp_log("lobby: operation saved before the battle: %d files in users\\mp_saves\\%s_vs_%s", copy_matching(&v24, &v23, "*.damsav2") + copy_matching(&v24, &v23, "*.opsave_") + copy_matching(&v24, &v23, "*.opsave"), "Player", &v6);
    *((char **)&v9) = 0;
    v10 = 0;
    v11 = 0;
    v12 = 0;
    v13 = 0;
    *((char **)&v14) = 0;
    *((char **)&v15) = 0;
    *((char **)&v16) = 0;
    v34 = strcspn(&g_5bf40, ",");
    if (v34 - 1 <= 62)
    {
        memcpy(&v9, &g_5bf40, v34);
        *((char **)&(&v9)[v34]) = 0;
    }
    (unsigned int)v36 = strchr(&v9, 58);
    if (v36)
        *(&v36) = 95;
    v3 = 0;
    v4 = 0;
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v26, "%s\\%s.opsave", &v24, &v9);
    if (v9 && (char)read_file(&v26, &v3))
    {
        operation_name@opsave@opsave(&v5, &v3);
        append@?$basic_string@DU?$char_traits@D@std@?$basic_string@DU?$char_traits@D@std(&v25, 260, &v5);
        v2 = 0;
        (unsigned int)v35 = sub_402d0(72);
        v35->field_0 = v35;
        *((struct_0 **)(v35 + 8)) = v35;
        *((struct_0 **)(v35 + 16)) = v35;
        *((char **)(v35 + 24)) = 257;
        v2 = v35;
        read_sidecar@opsave@opsave(&v25, &v5, &v2);
        user_side@opsave@opsave(&v3, &v25, &v5, &v2);
        (unsigned int)v36 = all_player_led@opsave@opsave(&v3, &v2);
        if ((unsigned int)v36 != 0)
            write_sidecar@opsave@opsave(&v25, &v5, &v2);
        mp_log("map: %u of the host's battle groups fight %s in this battle - identified for him from now on", v36, &v6);
        $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&v2, &v2, *((long long *)(v2 + 8)));
        sub_4030c(v2, 72);
        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v5);
    }
    if (!v36)
        return security_cookie_check(v27 ^ &v0);
    v36 = v3;
    if (v4 - v36 >= __Eg_my_units && v36 - *((long long *)(v36 - 8)) - 8 > 31)
    {
        v1 = 0;
        _invoke_watson(0, 0, 0, 0);
        [D] Unsupported jumpkind Ijk_SigTRAP at address 127456
    }
    sub_4030c();
    v4 = 0;
    v3 = 0;
    return security_cookie_check(v27 ^ &v0);
}


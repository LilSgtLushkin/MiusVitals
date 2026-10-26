// v20 RVA 013cf4..014053  (863 bytes)  load_battle_description
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    struct struct_0 *field_0;
    struct struct_2 *field_8;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
} struct_0;

typedef struct struct_2 {
    unsigned long long field_0;
} struct_2;


long long load_battle_description()
{
    char v0;  // [bp-0x2b8]
    unsigned long v1;  // [bp-0x298]
    unsigned int v2;  // [bp-0x288]
    char v3;  // [bp-0x280]
    char v4;  // [bp-0x278]
    char v5;  // [bp-0x268]
    char v6;  // [bp-0x238]
    char v7;  // [bp-0x128]
    unsigned long long v8;  // [bp-0x18]
    unsigned long long v10;  // rax
    char *v11;  // rbx, Other Possible Types: unsigned long
    unsigned long v12;  // rax
    char *v13;  // rcx, Other Possible Types: unsigned long
    unsigned int v14;  // r8d
    struct_1 *v15;  // rcx
    struct struct_0 **v16;  // rcx, Other Possible Types: unsigned long
    struct_1 *v17;  // rcx
    struct struct_0 **v18;  // rcx, Other Possible Types: unsigned long
    struct_1 *v19;  // rcx
    struct struct_0 **v20;  // rcx, Other Possible Types: unsigned long
    unsigned long long v21;  // rbx, Other Possible Types: unsigned long

    v8 = g_5a380 ^ &v0;
    host_map_frame();
    strcpy_s(&v6, 260, "data\\common\\shared\\config\\tacgame_image.cfgpack");
    (unsigned int)v10 = strchr(GetCommandLineA(), 44);
    if (v10)
    {
        for (v11 = &(1)[v10]; *((char *)v11) == 32; v11 = &(1)[v11]);
        v12 = strcspn(v11, ",");
        if (v12 - 1 <= 258)
        {
            memcpy(&v6, v11, v12);
            *((char **)&(&v6)[v12]) = 0;
        }
    }
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v7, "%s\\%s", &g_5b920, &v6);
    strcpy_s(&g_5bbd0, 260, &v7);
    if (!(char)read_file(&v7, &g_5b6a0))
    {
        mp_log("net: cannot read the battle description %s", &v7);
        return security_cookie_check(v8 ^ &v0);
    }
    v13 = 0;
    v14 = 2166136261;
    if (g_5b6a8 != g_5b6a0)
    {
        do
        {
            v14 = (g_5b6a0[v13].field_0 ^ v14) * 16777619;
            v13 = &(1)[v13];
        } while (v13 < g_5b6a8 - (char *)g_5b6a0);
    }
    g_5be34 = v14;
    sides@image(&g_5b6a0, &g_5be38, &g_5be3c);
    4?$_Hash@V?$_Uset_traits@_KV?$_Uhash_compare@_KU?$hash@_K@std(&g_5b8a0, units_of@image@image(&v3, &g_5b6a0, 1380275029));
    _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v5);
    v15 = *((long long *)&v4);
    *((char **)*((long long *)(v15 + 8))) = 0;
    v16 = v15->field_0;
    if (v16)
    {
        do
        {
            sub_4030c(v16, 24);
            v16 = *((long long *)v16);
        } while (v16);
    }
    sub_4030c(*((long long *)&v4), 24);
    4?$_Hash@V?$_Uset_traits@_KV?$_Uhash_compare@_KU?$hash@_K@std(&g_5b770, units_of@image@image(&v3, &g_5b6a0, g_5be38));
    _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v5);
    v17 = *((long long *)&v4);
    *((char **)*((long long *)(v17 + 8))) = 0;
    v18 = v17->field_0;
    if (v18)
    {
        do
        {
            sub_4030c(v18, 24);
            v18 = *((long long *)v18);
        } while (v18);
    }
    sub_4030c(*((long long *)&v4), 24);
    4?$_Hash@V?$_Uset_traits@_KV?$_Uhash_compare@_KU?$hash@_K@std(&g_5b6d0, units_of@image@image(&v3, &g_5b6a0, g_5be3c));
    _Tidy@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std@?$_Hash_vec@V?$allocator@V?$_List_unchecked_iterator@V?$_List_val@U?$_List_simple_types@U?$pair@$$CBII@std(&v5);
    v19 = *((long long *)&v4);
    *((char **)*((long long *)(v19 + 8))) = 0;
    v20 = v19->field_0;
    if (v20)
    {
        do
        {
            sub_4030c(v20, 24);
            v20 = *((long long *)v20);
        } while (v20);
    }
    sub_4030c(*((long long *)&v4), 24);
    find_position_only_units();
    v2 = g_5be38;
    if (g_5be38 == 5391687)
    {
        v21 = "Germans";
    }
    else if (g_5be38 == 1381192533)
    {
        v21 = "Soviets";
    }
    else
    {
        v21 = &g_5b5c8;
        memcpy(&g_5b5c8, &v2, 4);
        *((char **)&g_5b5cc) = 0;
    }
    v1 = g_5b8b0;
    mp_log("net: battle description %zu bytes (hash %08x), this player commands %s (%zu units)", g_5b6a8 - (char *)g_5b6a0, g_5be34, v21);
    mp_log("net: %zu units on this player's side, %zu on the other side", g_5b780, g_5b6e0);
    return security_cookie_check(v8 ^ &v0);
}


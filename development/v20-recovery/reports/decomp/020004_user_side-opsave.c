// v20 RVA 020004..02018f  (395 bytes)  user_side@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    unsigned long long field_0;
    struct struct_0 *field_8;
    unsigned long long field_10;
    unsigned short field_18;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
    char paddin8[8];
    unsigned long long field_10;
    char paddin24[1];
    char field_19;
} struct_0;


long long user_side@opsave(unsigned long long a0, unsigned long long a1, unsigned long long a2, unsigned long long a3)
{
    char v0;  // [bp-0x1a8]
    char v1;  // [bp-0x188]
    void* v2;  // [bp-0x180]
    struct_1 *v3;  // [bp-0x178]
    void* v4;  // [bp-0x170]
    char v5;  // [bp-0x168]
    char v6;  // [bp-0x158]
    unsigned long long v7;  // [bp-0x48]
    unsigned long v8;  // [bp-0x8]
    char v9;  // [bp+0x0]
    unsigned long long v11;  // rax
    unsigned long long v12;  // r12
    struct_1 *v13;  // rax
    unsigned long long v14;  // rax
    struct_0 *v15;  // rbx, Other Possible Types: unsigned long
    struct_0 *v16;  // rcx
    struct_1 *v17;  // rcx, Other Possible Types: unsigned long

    v8 = &v9;
    v7 = g_5a380 ^ &v0;
    1?$list@U?$pair@$$CBII@std?$list@U?$pair@$$CBII@std(&v6, 260);
    v2 = 0;
    (unsigned int)v11 = fopen_s(&v2, &v6, "r");
    if (!(!(unsigned int)v11 && v2))
        return security_cookie_check(v7 ^ &v0);
    v3 = 0;
    v4 = 0;
    v12 = &(32)[v11];
    (unsigned int)v13 = sub_402d0(v12);
    v13->field_0 = v13;
    *((struct_1 **)(v13 + 8)) = v13;
    *((struct_1 **)(v13 + 16)) = v13;
    *((char **)(v13 + 24)) = 257;
    v3 = v13;
    while (fscanf_s(v2, &g_46780, &v1) == 1)
    {
        $_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std$_Try_emplace@AEBI$$V@?$map@IHU?$less@I@std(&v3, &v5, &v1);
    }
    fclose(v2);
    (unsigned int)v14 = all_player_led@opsave@opsave(a0, &g_46780, "r");
    if ((unsigned int)v14)
        write_sidecar@opsave@opsave(a1, a2, a3);
    DeleteFileA(&v6);
    mp_log("map: the other player's intelligence found out %zu of the host's battle groups - %u newly identified for him", v4, v14);
    v17 = v3;
    v15 = *((long long *)(v17 + 8));
    if (!*((char *)(v15 + 25)))
    {
        do
        {
            $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std(&v3, &v3, *((long long *)&(16)[v15]));
            v16 = v15;
            v15 = *((long long *)v15);
            sub_4030c(v16, v12);
        } while (!(25)[v15]);
        v17 = v3;
    }
    sub_4030c(v17, v12);
    return security_cookie_check(v7 ^ &v0);
}


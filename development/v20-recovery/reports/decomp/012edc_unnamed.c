// v20 RVA 012edc..013085  (425 bytes)  xlate_frame
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long xlate_frame()
{
    char v0;  // [bp-0x188]
    void* v1;  // [bp-0x168]
    int v2;  // [bp-0x158], Other Possible Types: unsigned long long (64 bits)[3]
    void* v3;  // [bp-0x148]
    int v4;  // [bp-0x140]
    void* v5;  // [bp-0x130]
    char v6;  // [bp-0x128]
    unsigned long long v7;  // [bp-0x18]
    unsigned long v8;  // [bp-0x8]
    char v9;  // [bp+0x0]
    struct_0 *v11;  // rcx
    struct_0 *v12;  // rcx

    v8 = &v9;
    v7 = g_5a380 ^ &v0;
    *((char **)v2) = 0;
    v3 = 0;
    *((char **)&v4) = 0;
    v5 = 0;
    if (!GetEnvironmentVariableA("MP_BATTLE_SETUP", &v6, 260) || !(char)read_file(&v6, v2))
        $_Assign_counted_range@PEBE@?$vector@EV?$allocator@E@std(v2, g_5b6a0, g_5b6a8 - g_5b6a0);
    if (!(char)read_sidecar@opsave@opsave(v2, &v4))
    {
        mp_log("lobby: the other player's own force could not be put into this battle - his side is played as it is");
    }
    else
    {
        *((char **)&g_5b89c) = 1;
        host_swap_sides_if_asked(&v4, v2, "lobby: the other player's own force arrived during deployment - restarting the battle with it");
        *((char **)&g_5b89c) = 0;
    }
    v11 = (long long)v4;
    if (v11)
    {
        if (v5 - v11 >= __Eg_my_units && -8 + (char *)v11 - *((long long *)(v11 - 8)) > 31)
        {
            v1 = 0;
            _invoke_watson(0, 0, 0, 0);
            [D] Unsupported jumpkind Ijk_SigTRAP at address 77812
        }
        sub_4030c();
        *((char **)&v4) = 0;
        v5 = 0;
    }
    v12 = v2[0];
    if (!v12)
        return security_cookie_check(v7 ^ &v0);
    if (v3 - v12 >= __Eg_my_units && -8 + (char *)v12 - *((long long *)(v12 - 8)) > 31)
    {
        v1 = 0;
        _invoke_watson(0, 0, 0, 0);
        [D] Unsupported jumpkind Ijk_SigTRAP at address 77912
    }
    sub_4030c();
    *((char **)&v2) = 0;
    v3 = 0;
    return security_cookie_check(v7 ^ &v0);
}


// v20 RVA 0273a8..0274a6  (254 bytes)  joiner_reload_map
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long joiner_reload_map()
{
    char v0;  // [bp-0x178]
    void* v1;  // [bp-0x158]
    int v2;  // [bp-0x148]
    void* v3;  // [bp-0x138]
    char v4;  // [bp-0x128]
    unsigned long long v5;  // [bp-0x18]
    struct_0 *v7;  // rcx

    v5 = g_5a380 ^ &v0;
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v4, "%s\\users\\mp_my_force.cfgpack", &g_5c6d0);
    DeleteFileA(&v4);
    *((char **)&v2) = 0;
    v3 = 0;
    clear@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@?$list@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(&g_5c150, &v2);
    v7 = (long long)v2;
    if (v7)
    {
        if (v3 - v7 >= __Eg_my_units && -8 + (char *)v7 - *((long long *)(v7 - 8)) > 31)
        {
            v1 = 0;
            _invoke_watson(0, 0, 0, 0);
            [D] Unsupported jumpkind Ijk_SigTRAP at address 160852
        }
        sub_4030c();
        *((char **)&v2) = 0;
        v3 = 0;
    }
    *((char **)&g_5c4b8) = 0;
    *((char **)&g_5c4b4) = 0;
    *((char **)&g_5c468) = 0;
    *((char **)&g_5c5c0) = 0;
    return security_cookie_check(v5 ^ &v0);
}


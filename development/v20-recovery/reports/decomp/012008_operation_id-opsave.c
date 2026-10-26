// v20 RVA 012008..01214f  (327 bytes)  operation_id@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long operation_id@opsave()
{
    char v0;  // [bp-0x68]
    void* v1;  // [bp-0x48]
    int v2;  // [bp-0x38]
    void* v3;  // [bp-0x28]
    char v4;  // [bp-0x20]
    unsigned long long v5;  // [bp-0x18]
    struct_0 *v7;  // rcx

    v5 = g_5a380 ^ &v0;
    if (g_5b8e4 == 2)
    {
        return security_cookie_check(v5 ^ &v0);
    }
    else if (g_5b6a0 != g_5b6a8)
    {
        turn_phase@opsave@opsave();
        if (GetEnvironmentVariableA("MP_SWAPPED", &v4, 8))
            return security_cookie_check(v5 ^ &v0);
        SetEnvironmentVariableA("MP_BATTLE_SETUP", 0);
        *((char **)&v2) = 0;
        v3 = 0;
        if ((char)read_sidecar@opsave@opsave(&g_5b6a0, &v2))
            host_swap_sides_if_asked(&v2, &g_5b6a0);
        v7 = (long long)v2;
        if (!v7)
            return security_cookie_check(v5 ^ &v0);
        if (v3 - v7 >= __Eg_my_units && -8 + (char *)v7 - *((long long *)(v7 - 8)) > 31)
        {
            v1 = 0;
            _invoke_watson(0, 0, 0, 0);
            [D] Unsupported jumpkind Ijk_SigTRAP at address 74021
        }
        sub_4030c();
        *((char **)&v2) = 0;
        v3 = 0;
        return security_cookie_check(v5 ^ &v0);
    }
    else
    {
        return security_cookie_check(v5 ^ &v0);
    }
}


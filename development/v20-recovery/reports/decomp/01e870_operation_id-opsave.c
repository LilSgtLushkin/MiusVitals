// v20 RVA 01e870..01ea88  (536 bytes)  operation_id@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    uint128_t field_0;
    unsigned int field_10;
} struct_1;

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long operation_id@opsave(unsigned int *a0, unsigned int *a1)
{
    char v0;  // [bp-0x1d8]
    void* v1;  // [bp-0x1b8], Other Possible Types: unsigned long
    char v2;  // [bp-0x1a8]
    int v3;  // [bp-0x1a0]
    void* v4;  // [bp-0x190]
    int v5;  // [bp-0x188]
    unsigned int v6;  // [bp-0x178]
    char v7;  // [bp-0x158]
    char v8;  // [bp-0x138]
    unsigned long long v9;  // [bp-0x28]
    unsigned long v10;  // [bp-0x8]
    char v11;  // [bp+0x0]
    struct_1 *v13;  // rax
    unsigned long long v14;  // r14
    unsigned long long v15;  // rax
    struct_0 *v16;  // rcx

    v10 = &v11;
    v9 = g_5a380 ^ &v0;
    if (!g_5bfd0 && GetEnvironmentVariableA("MP_SETUP_DATE", &v7, 32))
        sscanf_s(&v7, "%u,%u", &g_5bfd0, &g_5bfd4);
    *(a0) = g_5bfd0;
    *(a1) = g_5bfd4;
    if (*(a0))
    {
        return security_cookie_check(v9 ^ &v0);
    }
    else if (!g_5b618)
    {
        v1 = "cloud";
        sprintf_s(&v8, 260, "%s\\users\\profiles\\%s\\ccombat.cfgpack", &g_5b920);
        *((char **)&v3) = 0;
        v4 = 0;
        if ((char)read_file(&v8, &v3))
        {
            (unsigned int)v13 = find@image("MP_SETUP_DATE", 260, "%s\\users\\profiles\\%s\\ccombat.cfgpack");
            *((uint128_t *)&v5) = v13->field_0;
            v6 = *((int *)(v13 + 16));
            v14 = field@image@image(&v3, &v5, "year");
            v15 = field@image@image(&v3, &v5, "period");
            if (v14 && v15)
            {
                memcpy(&v2, (long long)v3 + v14, 4);
                *((char **)&a0) = &(1)[*((int *)&v2)];
                memcpy(&v2, (long long)v3 + v15, 4);
                *((char **)&a1) = &(1)[*((int *)&v2)];
            }
        }
        v16 = (long long)v3;
        if (!v16)
            return security_cookie_check(v9 ^ &v0);
        if (v4 - v16 >= __Eg_my_units && -8 + (char *)v16 - *((long long *)(v16 - 8)) > 31)
        {
            v1 = 0;
            _invoke_watson(0, 0, 0, 0);
            [D] Unsupported jumpkind Ijk_SigTRAP at address 125513
        }
        sub_4030c();
        *((char **)&v3) = 0;
        v4 = 0;
        return security_cookie_check(v9 ^ &v0);
    }
    else
    {
        return security_cookie_check(v9 ^ &v0);
    }
}


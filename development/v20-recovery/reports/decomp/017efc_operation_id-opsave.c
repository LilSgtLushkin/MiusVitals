// v20 RVA 017efc..0180da  (478 bytes)  operation_id@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    uint128_t field_0;
    unsigned int field_10;
} struct_0;

typedef struct struct_1 {
    unsigned long long field_-8;
} struct_1;


long long operation_id@opsave()
{
    char v0;  // [bp-0x1a8]
    void* v1;  // [bp-0x188], Other Possible Types: unsigned long
    char v2;  // [bp-0x178]
    char v3;  // [bp-0x174]
    int v4;  // [bp-0x170], Other Possible Types: unsigned long long
    void* v5;  // [bp-0x160]
    int v6;  // [bp-0x158]
    unsigned int v7;  // [bp-0x148]
    char v8;  // [bp-0x128]
    unsigned long long v9;  // [bp-0x18]
    struct_0 *v12;  // rax
    unsigned long long v13;  // rsi
    unsigned long long v14;  // rax
    struct_1 *v15;  // rcx

    v9 = g_5a380 ^ &v0;
    if (g_5bfc8 && g_5bfcc)
        return security_cookie_check(v9 ^ &v0);
    v1 = "cloud";
    sprintf_s(&v8, 260, "%s\\users\\profiles\\%s\\ccombat.cfgpack", &g_5b920);
    v4 = 0;
    v5 = 0;
    if (!(operation_id@opsave@opsave()) || !((char)read_file(&v8, &v4)))
    {
LABEL_1804a:
    }
    else
    {
        (unsigned int)v12 = find@image(g_5bfcc, 260, "%s\\users\\profiles\\%s\\ccombat.cfgpack");
        *((uint128_t *)&v6) = v12->field_0;
        v7 = *((int *)(v12 + 16));
        v13 = field@image@image(&v4, &v6, "user_side");
        v14 = field@image@image(&v4, &v6, "enemy_side");
        if (!(v13) || !(v14))
            goto LABEL_1804a;
        memcpy(&v3, v4 + v13, 4);
        memcpy(&v2, v4 + v14, 4);
    }
    v15 = v4;
    if (v15)
    {
        if (v5 - v15 >= __Eg_my_units && -8 + (char *)v15 - *((long long *)(v15 - 8)) > 31)
        {
            v1 = 0;
            _invoke_watson(0, 0, 0, 0);
            [D] Unsupported jumpkind Ijk_SigTRAP at address 98456
        }
        sub_4030c();
        *((char **)&v4) = 0;
        v5 = 0;
    }
    return security_cookie_check(v9 ^ &v0);
}


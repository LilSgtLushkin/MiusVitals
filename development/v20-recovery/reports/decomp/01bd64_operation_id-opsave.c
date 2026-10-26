// v20 RVA 01bd64..01bf4c  (488 bytes)  operation_id@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    uint128_t field_0;
    unsigned int field_10;
} struct_1;

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long operation_id@opsave()
{
    char v0;  // [bp-0x1a8]
    void* v1;  // [bp-0x188]
    char v2;  // [bp-0x178]
    int v3;  // [bp-0x170]
    void* v4;  // [bp-0x160]
    int v5;  // [bp-0x158]
    unsigned int v6;  // [bp-0x148]
    char v7;  // [bp-0x140]
    char v8;  // [bp-0x128]
    unsigned long long v9;  // [bp-0x18]
    unsigned long v10;  // [bp-0x8]
    char v11;  // [bp+0x0]
    unsigned long long v14;  // r9
    struct_1 *v15;  // rax
    unsigned long long v16;  // rbx
    unsigned long long v17;  // rax, Other Possible Types: unsigned long
    unsigned long long v18;  // rax
    unsigned long long v19;  // r8
    unsigned long long v20;  // rdx
    struct_2 *v21;  // rcx
    struct_0 *v22;  // rcx

    v10 = &v11;
    v9 = g_5a380 ^ &v0;
    if (g_5bfc8 && g_5bfcc)
        return security_cookie_check(v9 ^ &v0);
    if (g_5c068 && GetTickCount() - g_5c068 < 2000)
        return security_cookie_check(v9 ^ &v0);
    *((unsigned long *)&g_5c068) = GetTickCount() | 1;
    v14 = "cloud";
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v8, "%s\\users\\profiles\\%s\\ccombat.cfgpack", &g_5b920, "cloud");
    *((char **)&v3) = 0;
    v4 = 0;
    *((char **)&g_5c06c) = 0;
    if (!(char)read_file(&v8, &v3))
    {
LABEL_1bec0:
    }
    else
    {
        (unsigned int)v15 = find@image(&v7, &v3, "i_opgame:opgame");
        *((uint128_t *)&v5) = v15->field_0;
        v6 = *((int *)(v15 + 16));
        v16 = field@image@image(&v3, &v5, "user_side");
        v17 = field@image@image(&v3, &v5, "enemy_side");
        if (!(v16) || !(v17))
            goto LABEL_1bec0;
        if (g_5b61f)
            v17 = v16;
        v18 = v17 + (long long)v3;
        v19 = 4;
        v20 = v18;
        v21 = &v2;
        memcpy();
        g_5c06c = *((int *)&v2);
    }
    v22 = (long long)v3;
    if (v22)
    {
        if (v4 - v22 >= __Eg_my_units && -8 + (char *)v22 - *((long long *)(v22 - 8)) > 31)
        {
            v1 = 0;
            _invoke_watson(0, 0, 0, 0);
            [D] Unsupported jumpkind Ijk_SigTRAP at address 114450
        }
        sub_4030c();
        *((char **)&v3) = 0;
        v4 = 0;
    }
    return security_cookie_check(v9 ^ &v0);
}


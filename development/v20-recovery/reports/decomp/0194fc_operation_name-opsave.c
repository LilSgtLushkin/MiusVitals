// v20 RVA 0194fc..0195f7  (251 bytes)  operation_name@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long operation_name@opsave(unsigned long long *a0, unsigned int a1[4], unsigned long a2, unsigned long a3)
{
    char v0;  // [bp-0x68]
    char v1;  // [bp-0x48]
    char v2;  // [bp-0x40]
    char v3;  // [bp-0x30]
    char v4;  // [bp-0x28]
    unsigned long long v5;  // [bp-0x20]
    unsigned long long *v7;  // r10
    unsigned long long v8;  // rcx
    struct_0 *v9;  // rdi, Other Possible Types: unsigned long
    unsigned long long v10;  // rcx
    unsigned long v11;  // rbx
    unsigned long long v12;  // rbx
    char v13;  // r8b
    unsigned long long v14;  // rdx
    struct_0 *v15;  // rcx
    unsigned long long v16;  // rax
    unsigned long long v17;  // rcx
    struct_0 *v18;  // rax, Other Possible Types: unsigned long

    v5 = g_5a380 ^ &v0;
    v7 = a0;
    v8 = a2 * *((int *)(a1 + 4)) * 32 + *((int *)(a1 + 12));
    if (!(char)a3)
    {
        memcpy(&v1, &(32)[*(v7) + v8], 4);
        return security_cookie_check(v5 ^ &v0);
    }
    $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v2, v7, v8);
    v9 = &v2;
    if (*((long long *)&v4) > 15)
        v9 = *((long long *)&v2);
    v10 = *((long long *)&v3);
    if (v10)
    {
        v11 = (v11 < -1 ? -1 : v10 - 1);
        v12 = &(1)[v11 + v9];
        v13 = 95;
        v14 = v12;
        v15 = v9;
        v16 = sub_40160();
        if (v16 != v12)
        {
            v17 = v16 - v9;
            if (v17 != -1)
            {
                v18 = &v2;
                if (*((long long *)&v4) > 15)
                    v18 = *((long long *)&v2);
                strtoul(&(1)[v18 + v17], 0, 16);
            }
        }
    }
    _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v2);
    return security_cookie_check(v5 ^ &v0);
}


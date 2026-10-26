// v20 RVA 018c94..018f20  (652 bytes)  counterpart@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    struct struct_0 *field_0;
    char padding_8[8];
    unsigned long long field_10;
    unsigned long long field_18;
} struct_0;

typedef struct struct_2 {
    uint128_t field_0;
    unsigned long long field_10;
    unsigned long long field_18;
} struct_2;

typedef struct struct_3 {
    char padding_0[8];
    unsigned long long field_8;
    char padding_10[8];
    struct struct_4 *field_18;
    char padding_20[16];
    unsigned long long field_30;
} struct_3;

typedef struct struct_5 {
    char padding_0[8];
    struct struct_5 *field_8;
    unsigned int field_10;
} struct_5;

typedef struct struct_1 {
    struct struct_1 *field_-8;
} struct_1;

typedef struct struct_4 {
    unsigned long long field_0;
} struct_4;

extern char g_0;
extern char g_1;
extern char g_10;
extern char g_1000;
extern char g_18;
extern char g_1f;
extern char g_2;
extern char g_27;
extern char g_3;
extern char g_30;
extern unsigned long long g_5a380;
extern char g_5f;
extern char g_8;
extern char g_f;

long long counterpart@opsave(struct_2 *a0, struct_0 *a1, struct_3 *a2, unsigned long a3, unsigned int a4)
{
    char v0;  // [bp-0xb8]
    void* v1;  // [bp-0x98]
    unsigned int v2;  // [bp-0x88]
    unsigned int v3;  // [bp-0x84]
    struct_2 *v4;  // [bp-0x80]
    int v5;  // [bp-0x78], Other Possible Types: char
    int v6;  // [bp-0x68]
    char v7;  // [bp-0x58]
    unsigned long long v8;  // [bp-0x48]
    unsigned long v9;  // [bp-0x8]
    char v10;  // [bp+0x0]
    struct_0 *v12;  // rdi, Other Possible Types: unsigned long
    struct_2 *v13;  // rsi
    struct_0 *v14;  // r15, Other Possible Types: unsigned long
    unsigned long long v15;  // rcx
    unsigned long v16;  // rbx
    unsigned long long v17;  // rbx
    unsigned long long v18;  // rdx
    struct_0 *v19;  // rcx
    unsigned long long v20;  // rax
    unsigned long long v21;  // r14
    struct_0 *v22;  // rax, Other Possible Types: unsigned long
    unsigned int v23;  // ebx
    unsigned long long v24;  // rcx
    unsigned long long *v25;  // rdx
    struct_5 *v26;  // rax, Other Possible Types: unsigned long
    unsigned long long v27;  // rdx
    unsigned long long v28;  // rdx, Other Possible Types: unsigned long
    struct_1 *v29;  // rcx, Other Possible Types: unsigned long
    struct_1 *v30;  // rax

    v9 = &v10;
    v8 = g_5a380 ^ &v0;
    v12 = a1;
    v13 = a0;
    v4 = a0;
    *((char **)&v2) = &g_0;
    v14 = a1;
    if (*((long long *)(a1 + &g_18)) > 15)
        v14 = a1->field_0;
    v15 = *((long long *)(a1 + &g_10));
    if (!v15 || (v16 = (v16 < -1 ? -1 : v15 - &g_1), v17 = v16 + &g_1 + v14, *((char *)&a2) = &g_5f, v18 = v17, v19 = v14, v20 = sub_40160(), !((unsigned long long)sub_40160() != v17) || !(v21 != -1) || !(~(v21) + *((long long *)(v12 + &g_10)) == 8)))
    {
        *((char **)&v13->field_0) = &g_0;
        *((char **)(v13 + &g_10)) = &g_0;
        *((char **)(v13 + &g_18)) = &g_0;
        $_Construct@$01PEBD@?$basic_string@DU?$char_traits@D@std(v13);
    }
    else
    {
        v22 = v12;
        if (*((long long *)(v12 + &g_18)) > 15)
            v22 = v12->field_0;
        v23 = strtoul(&(&g_1)[v22 + v21], 0, 16);
        v3 = v23;
        if ((unsigned int)(v3 >> &g_18) == (unsigned int)a3)
        {
            v24 = (sub_619c(&v3) & *((long long *)(a2 + &g_30))) * &g_2;
            v25 = *((long long *)(a2 + &g_18));
            if (v26 == *((long long *)(a2 + &g_8)))
            {
                v26 = &g_0;
            }
            else
            {
                for (v26 = *((long long *)((char *)v25 + v24 * &g_8 + &g_8)); v3 != *((int *)&(&g_10)[v26]); v26 = *((long long *)&(&g_8)[v26]))
                {
                    if (v26 == *((long long *)((char *)v25 + (v24 << &g_3))))
                        break;
                }
            }
            if (v26)
            {
                $sprintf_s@$0BA(&v7, "%08x", a4 * 0x1000000 | v3 & 16777215);
                *((char **)&v5) = &g_0;
                *((char **)&v6) = &g_0;
                if (*((long long *)(v12 + &g_18)) > 15)
                    v12 = v12->field_0;
                ::0x498c::$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v5, v12);
                *((char **)&v2) = &g_2;
                ::0x4600::$?HDU?$char_traits@D@std(v13, &v5, &v7);
                *((char **)&v2) = &g_3;
                *((char **)&v2) = &g_1;
                v27 = (long long)(&v6)[8];
                if (v27 > 15)
                {
                    v28 = &(&g_1)[v27];
                    v29 = (long long)v5;
                    v30 = v29;
                    if (v28 >= __Eg_my_units)
                    {
                        v28 = &(&g_27)[v28];
                        v29 = *((long long *)(v29 - &g_8));
                        if (-8 + (char *)v30 - v29 > 31)
                        {
                            v1 = &g_0;
                            _invoke_watson(0, 0, 0, 0);
                            [D] Unsupported jumpkind Ijk_SigTRAP at address 102004
                        }
                    }
                    sub_4030c(v29, v28);
                }
                *((int128_t *)&v6) = 0xf0000000000000000;
                *((char **)&v5) = &g_0;
                return security_cookie_check(v8 ^ &v0);
            }
        }
        *((char **)&v13->field_0) = &g_0;
        *((char **)(v13 + &g_10)) = &g_0;
        *((char **)(v13 + &g_18)) = &g_0;
        $_Construct@$01PEBD@?$basic_string@DU?$char_traits@D@std(v13);
    }
    *((char **)&v2) = &g_1;
    return security_cookie_check(v8 ^ &v0);
}


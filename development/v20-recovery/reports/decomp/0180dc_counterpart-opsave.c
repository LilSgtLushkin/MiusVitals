// v20 RVA 0180dc..0183ad  (721 bytes)  counterpart@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    uint128_t field_0;
    char field_10;
} struct_2;

typedef struct struct_1 {
    char field_0;
    char paddin1[15];
    unsigned long long field_10;
    unsigned long long field_18;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_-8;
} struct_0;


long long counterpart@opsave(unsigned long long a0, unsigned long long *a1, unsigned long a2)
{
    char v0;  // [bp-0xd8]
    void* v1;  // [bp-0xb8]
    unsigned int v2;  // [bp-0xa8]
    char v3;  // [bp-0xa4]
    unsigned long v4;  // [bp-0x98]
    void* v5;  // [bp-0x90], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x8c]
    int v7;  // [bp-0x88], Other Possible Types: char
    int v8;  // [bp-0x78]
    char v9;  // [bp-0x68]
    char v10;  // [bp-0x60]
    unsigned long long v11;  // [bp-0x40]
    unsigned long v12;  // [bp-0x8]
    char v13;  // [bp+0x0]
    struct_2 *v16;  // rax
    int v17;  // xmm1
    void* v18;  // rbx, Other Possible Types: unsigned long
    unsigned int v19;  // edi
    unsigned long long v20;  // r14
    struct_1 *v21;  // rdi
    unsigned long long v22;  // rdx, Other Possible Types: unsigned long
    unsigned long long v23;  // rdx
    unsigned long long v24;  // rdx, Other Possible Types: unsigned long
    struct_0 *v25;  // rcx, Other Possible Types: unsigned long
    struct_0 *v26;  // rax
    struct_0 *v27;  // rcx, Other Possible Types: unsigned long
    unsigned long long v28;  // rdx, Other Possible Types: unsigned long
    struct_0 *v29;  // rax

    v12 = &v13;
    v11 = g_5a380 ^ &v0;
    v4 = a0;
    *((char **)&v2) = 0;
    v5 = 0;
    *((char **)&v7) = 0;
    *((char **)&v8) = 0;
    $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v7, &g_45ce6, strlen(&g_45ce6));
    *((char **)&v9) = 0;
    (unsigned int)v16 = find@image(&v10, a1, "pools");
    v17 = v16->field_0;
    if (!*((char *)(v16 + 16)) || (unsigned int)v17 != 2 || (unsigned int)((unsigned long long)v17 >> 32) < 2)
    {
        0Tagged@opsave@opsave(a0, &v5);
        *((char **)&v2) = 1;
        goto LABEL_18383;
    }
    v18 = 0;
    v19 = 0 CONCAT (unsigned long long)v17 / 0x10000000000000000;
    if (v19)
    {
        do
        {
            v20 = (v16->field_0 >> 64 >> 32 CONCAT v16->field_0 >> 32 | (v16->field_0 >> 64) * 0x100000000) * v18 * 32 + (0 CONCAT v16->field_0 >> 64 >> 32);
            memcpy(&v3, &(36)[*(a1) + v20], 4);
            if (*((int *)&v3) == (unsigned int)a2)
            {
                memcpy(&v3, &(32)[*(a1) + v20], 4);
                v5 = *((int *)&v3) >> 24;
                v6 = a2;
                v21 = $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v10, a1, v20);
                if (&v7 != v21)
                {
                    v23 = (long long)(&v8)[8];
                    if (v23 > 15)
                    {
                        v24 = &(1)[v23];
                        v25 = (long long)v7;
                        v26 = v25;
                        if (v24 >= __Eg_my_units)
                        {
                            v24 = &(39)[v24];
                            v25 = *((long long *)(v25 - 8));
                            if (-8 + (char *)v26 - v25 > 31)
                            {
                                v1 = 0;
                                _invoke_watson(0, 0, 0, 0);
                                [D] Unsupported jumpkind Ijk_SigTRAP at address 98976
                            }
                        }
                        sub_4030c(v25, v24);
                    }
                    *((int128_t *)&v8) = 0xf0000000000000000;
                    *((char **)&v7) = 0;
                    memcpy(&v7, v21, 32);
                    *((char **)(v21 + 16)) = 0;
                    *((char **)(v21 + 24)) = 15;
                    *((char **)&v21->field_0) = 0;
                }
                _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v10);
                v9 = v5 != 0;
                0Tagged@opsave@opsave(a0, &v5);
                *((char **)&v2) = 1;
                v22 = (long long)(&v8)[8];
                if (v22 <= 15)
                {
                    *((int128_t *)&v8) = 0xf0000000000000000;
                    *((char **)&v7) = 0;
                    return security_cookie_check(v11 ^ &v0);
                }
LABEL_1830c:
                v27 = (long long)v7;
                v28 = &(1)[v22];
                v29 = v27;
                if (v28 >= __Eg_my_units)
                {
                    v28 = &(39)[v28];
                    v27 = *((long long *)(v27 - 8));
                    if (-8 + (char *)v29 - v27 > 31)
                    {
                        v1 = 0;
                        _invoke_watson(0, 0, 0, 0);
                        [D] Unsupported jumpkind Ijk_SigTRAP at address 99244
                    }
                }
                sub_4030c(v27, v28);
            }
        } while ((v18 = (unsigned long long)((unsigned int)v18 + 1), (unsigned int)v18 < v19));
    }
    0Tagged@opsave@opsave(a0, &v5);
    *((char **)&v2) = 1;
LABEL_18383:
    v22 = (long long)(&v8)[8];
    if (v22 <= 15)
    {
        *((int128_t *)&v8) = 0xf0000000000000000;
        *((char **)&v7) = 0;
        return security_cookie_check(v11 ^ &v0);
    }
    goto LABEL_1830c;
}


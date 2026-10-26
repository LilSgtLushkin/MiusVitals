// v20 RVA 00f8cc..00fc25  (857 bytes)  R<lambda_4850e5e4fe7cad2db8d8b8fa5a62506f>
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
    char paddin1[7];
    struct struct_1 *field_8;
    unsigned long long field_10;
} struct_0;

typedef struct struct_1 {
    char field_0;
} struct_1;


long long R(struct_0 *a0, unsigned long a1, unsigned long long a2[2], unsigned long long a3)
{
    char v0;  // [bp-0xd8]
    char v1;  // [bp-0xb8]
    unsigned int v2;  // [bp-0xb0]
    char v3;  // [bp-0xa8], Other Possible Types: unsigned int
    char v4;  // [bp-0xa4]
    char v5;  // [bp-0xa0]
    char v6;  // [bp-0x9c]
    unsigned int v7;  // [bp-0x98]
    char v8;  // [bp-0x90]
    int v9;  // [bp-0x88]
    void* v10;  // [bp-0x78]
    unsigned long v11;  // [bp-0x70]
    unsigned long v12;  // [bp-0x68]
    char v13;  // [bp-0x60]
    unsigned long long v14;  // [bp-0x40]
    unsigned long v15;  // [bp-0x8]
    char v16;  // [bp+0x0]
    struct_0 *v18;  // rdi
    unsigned long long v19;  // rax
    unsigned long long v20;  // r13
    unsigned long long v21;  // rcx
    unsigned int v22;  // ebx
    unsigned long long v23;  // rbx
    unsigned int v24;  // eax
    void* v25;  // r15, Other Possible Types: unsigned long
    unsigned long long v26;  // rbx, Other Possible Types: unsigned long
    void* v27;  // rbx, Other Possible Types: unsigned long
    void* v28;  // r14, Other Possible Types: unsigned long
    char *v29;  // rdx, Other Possible Types: unsigned long

    v15 = &v16;
    v14 = g_5a380 ^ &v0;
    v12 = a1;
    v18 = a0;
    (unsigned int)v19 = turns@opsave@opsave(a2, a3);
    if (!v19)
        return security_cookie_check(v14 ^ &v0);
    memcpy(&v5, &(68)[a2[0] + v19], 4);
    memcpy(&v3, &(72)[a2[0] + v19], 4);
    memcpy(&v8, &(52)[*((long long *)&a0->field_0) + a1], 4);
    memcpy(&v4, &(56)[*((long long *)&a0->field_0) + a1], 4);
    v20 = *((int *)&v5);
    if ((unsigned int)(v20 - 1) <= 255)
    {
        v21 = *((int *)&v3);
        if (v22)
        {
            v26 = v21;
            v11 = v21;
            if (v21 + (v20 << 3) <= *((long long *)(a2 + 8)) - a2[0] && *((int *)&v8) + (*((int *)&v4) << 2) <= *((long long *)(v18 + 8)) - *((long long *)&v18->field_0))
            {
                *((char **)&v1) = 0;
                *((char **)&v9) = 0;
                v10 = 0;
                $_Construct_n@AEBD@?$vector@DV?$allocator@D@std(&v9);
                v24 = 0;
                *((char **)&v3) = 0;
                v25 = 0;
                if ((unsigned int)v20)
                {
                    do
                    {
                        v23 = v26 + v25 * 8;
                        memcpy(&v5, a2[0] + v23, 4);
                        memcpy(&v6, &(4)[a2[0] + v23], 4);
                        if (!*((int *)&v5))
                        {
                            v24 = v3;
                        }
                        else
                        {
                            v2 = send_table(v18, operation_name@opsave@opsave(&v13, a2, *((int *)&v6)));
                            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v13);
                            if (v2 == -1)
                            {
                                _Tidy@?$vector@EV?$allocator@E@std(&v9);
                                return security_cookie_check(v14 ^ &v0);
                            }
                            v7 = *((int *)&v5);
                            memcpy((long long)v9 + v25 * 8, &v7, 4);
                            v2 = v2;
                            memcpy(&(4)[v9 + v25 * 8], &v2, 4);
                            v24 = &(1)[v3];
                            v3 = v24;
                        }
                    } while ((v25 += 1, v26 = v11, (unsigned int)v25 < (unsigned int)v20));
                }
                if (v24 == *((int *)&v4))
                {
                    v27 = 0;
                    if (*((int *)&v4) <= 0)
                        goto LABEL_fb69;
                    else
                        goto LABEL_fadc;
                }
                do
                {
LABEL_fadc:
                    memcpy(&v7, *((int *)&v8) + v27 * 4 + *((long long *)&v18->field_0), 4);
                    *((char *)&v24) = 0;
                    v28 = 0;
                    if (!(unsigned int)v20)
                    {
                        _Tidy@?$vector@EV?$allocator@E@std(&v9);
                        return security_cookie_check(v14 ^ &v0);
                    }
                    if (!(char)v24)
                    {
                        memcpy(&v2, (long long)v9 + v28 * 8, 4);
                        if (!v2 || (memcpy(&v6, (long long)v9 + 4 + v28 * 8, 4), *((int *)&v6) != v7))
                            *((char *)&v24) = 0;
                        else
                            *((char *)&v24) = 1;
                        v28 = &(1)[v28];
                        if ((unsigned int)v28 < (unsigned int)v20)
                            continue;
                    }
                    v27 += 1;
                } while ((unsigned int)v27 < *((int *)&v4));
LABEL_fb69:
                v29 = *((long long *)(v18 + 8));
                if (((char)v29 - v18->field_0 & 15))
                {
                    *((char **)&v1) = 0;
                    do
                    {
                        if (v29 != *((long long *)(v18 + 16)))
                        {
                            *((char **)v29) = 0;
                            *((char **)(v18 + 8)) = &(1)[*((long long *)(v18 + 8))];
                        }
                        else
                        {
                            $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(v18);
                        }
                    } while ((v25 += 1, v26 = v11, (unsigned int)v25 < (unsigned int)v20));
                }
                $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v18);
                v2 = v20;
                memcpy(&(68)[*((long long *)&v18->field_0) + v12], &v2, 4);
                v2 = (unsigned int)v29 - *((int *)&v18->field_0);
                memcpy(&(72)[*((long long *)&v18->field_0) + v12], &v2, 4);
                _Tidy@?$vector@EV?$allocator@E@std(&v9);
                return security_cookie_check(v14 ^ &v0);
            }
        }
    }
}


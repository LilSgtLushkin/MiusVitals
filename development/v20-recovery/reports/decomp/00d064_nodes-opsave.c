// v20 RVA 00d064..00d58d  (1321 bytes)  nodes@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char field_0;
    char paddin1[7];
    struct struct_2 *field_8;
    unsigned long long field_10;
} struct_1;

typedef struct struct_3 {
    unsigned int field_0;
    char paddin4[4];
    unsigned long long field_8;
} struct_3;

typedef struct struct_0 {
    struct struct_0 *field_-8;
} struct_0;

typedef struct struct_2 {
    char field_0;
} struct_2;


long long nodes@opsave(struct_1 *a0, unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long long a5[2], unsigned long a6)
{
    char v0;  // [bp-0x188]
    void* v1;  // [bp-0x168]
    char v2;  // [bp-0x158]
    char v3;  // [bp-0x150], Other Possible Types: unsigned long, unsigned long long, unsigned int
    char v4;  // [bp-0x148], Other Possible Types: unsigned long, unsigned long long
    char v5;  // [bp-0x140]
    char v6;  // [bp-0x13c]
    int v7;  // [bp-0x138], Other Possible Types: unsigned long long (64 bits)[3]
    void* v8;  // [bp-0x128]
    int v9;  // [bp-0x120]
    void* v10;  // [bp-0x110]
    unsigned long long v11;  // [bp-0x108]
    unsigned long long v12[2];  // [bp-0x100]
    unsigned long long v13;  // [bp-0xf8]
    unsigned long long v14;  // [bp-0xf0]
    unsigned long long v15;  // [bp-0xe8]
    char v16;  // [bp-0xd8]
    char v17;  // [bp-0xb8]
    char v18;  // [bp-0xb4]
    char v19;  // [bp-0xb0]
    char v20;  // [bp-0xac]
    char v21;  // [bp-0xa8]
    char v22;  // [bp-0xa4]
    unsigned long long v23;  // [bp-0x58]
    unsigned long v24;  // [bp-0x8]
    char v25;  // [bp+0x0]
    unsigned long long v27;  // r12
    struct_1 *v28;  // rsi
    unsigned long long v29;  // rdi
    unsigned long long v30;  // r14
    unsigned long long v31;  // rdx
    unsigned long long v32;  // r13
    unsigned long long v33;  // rbx
    unsigned long long v34;  // r9
    unsigned long long v35;  // r15
    unsigned long long v36;  // rdx
    unsigned long long v37;  // rdx
    unsigned long long v38;  // rcx
    struct_3 *v39;  // rbx
    char *v40;  // rcx, Other Possible Types: unsigned long
    char *v41;  // rdx, Other Possible Types: unsigned long
    unsigned long long v42;  // r14
    unsigned long long v43;  // r8
    char *v44;  // rdx, Other Possible Types: unsigned long
    unsigned long long v45;  // r8
    unsigned long long v46;  // rdi
    struct_0 *v47;  // r8, Other Possible Types: unsigned long
    struct_0 *v48;  // rcx

    v24 = &v25;
    v23 = g_5a380 ^ &v0;
    v27 = a3;
    v11 = a2;
    v14 = a1;
    v28 = a0;
    v12[0] = a5;
    v13 = a4;
    v15 = a6;
    v29 = turns@opsave@opsave();
    v30 = turns@opsave@opsave(a0, "squads");
    memcpy(&v4, &(4)[*((long long *)&a0->field_0)], 4);
    memcpy(&v5, &(8)[*((long long *)&a0->field_0)], 4);
    if (!(v29 && v30 && (unsigned int)a3))
        return security_cookie_check(v23 ^ &v0);
    v31 = *((long long *)&a0->field_0);
    v32 = *((int *)&v4);
    v33 = v32 * 128;
    if (*((int *)&v5) + v33 <= *((long long *)(a0 + 8)) - v31)
    {
        memcpy(&v4, &(52)[v31 + v29], 4);
        memcpy(&v3, &(56)[*((long long *)&a0->field_0) + v29], 4);
        v34 = *((int *)&v4);
        v35 = *((int *)&v3);
        v36 = v35 * 4;
        if (!((unsigned int)v35) || !(v34 + v36 > *((long long *)(v28 + 8)) - *((long long *)&v28->field_0)))
        {
            v37 = (v36 + v34 & -(0 < (unsigned int)v35)) + *((long long *)&a0->field_0);
            *((char **)&v9) = 0;
            v10 = 0;
            v4 = (-(0 < (unsigned int)v35) & *((int *)&v4)) + *((long long *)&a0->field_0);
            v3 = v37;
            $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std(a0, v3, 4, v34);
            v38 = *((int *)&v5) + *((long long *)&a0->field_0);
            *((char **)v7) = 0;
            v8 = 0;
            v4 = v38;
            v3 = v4 + v33;
            $_Construct_n@PEAEPEAE@?$vector@EV?$allocator@E@std(v4, v37, 4, &v3);
            memset(&v16, 0, 128);
            strncpy_s(&v16, 32, v11, -1);
            *((char **)&v3) = 2;
            memcpy(&v17, &v3, 4);
            v3 = v27;
            memcpy(&v18, &v3, 4);
            v39 = v12;
            v3 = (0 CONCAT *((long long *)(v39 + 8)) - *((long long *)&v39->field_0)) /m (v27 * 32);
            memcpy(&v19, &v3, 4);
            v3 = put_varint(v28, v13);
            memcpy(&v20, &v3, 4);
            v40 = *((long long *)(v28 + 8));
            if (((char)v40 - v28->field_0 & 15))
            {
                *((char **)&v2) = 0;
                do
                {
                    if (v40 != *((long long *)(v28 + 16)))
                    {
                        *((char **)v40) = 0;
                        *((char **)(v28 + 8)) = &(1)[*((long long *)(v28 + 8))];
                    }
                    else
                    {
                        $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(v28, v40, &v2);
                    }
                } while ((v40 = *((long long *)(v28 + 8)), ((char)v40 - v28->field_0 & 15)));
            }
            v3 = *((int *)(v28 + 8)) - *((int *)&v28->field_0);
            memcpy(&v21, &v3, 4);
            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v28, *((long long *)(v28 + 8)), *((long long *)&v39->field_0), *((long long *)(v39 + 8)) - *((long long *)&v39->field_0));
            memcpy(&v3, &(52)[*((long long *)&v28->field_0) + v30], 4);
            v3 = v3;
            memcpy(&v22, &v3, 4);
            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v7, v7[1], &v16, 128);
            v41 = *((long long *)(v28 + 8));
            if (((char)v41 - v28->field_0 & 15))
            {
                *((char **)&v2) = 0;
                do
                {
                    if (v41 != *((long long *)(v28 + 16)))
                    {
                        *((char **)v41) = 0;
                        *((char **)(v28 + 8)) = &(1)[*((long long *)(v28 + 8))];
                    }
                    else
                    {
                        $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(v28, v41, &v2);
                    }
                } while ((v40 = *((long long *)(v28 + 8)), ((char)v40 - v28->field_0 & 15)));
            }
            v42 = (unsigned int)v41 - *((int *)&v28->field_0);
            v43 = v7[0];
            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v28, v41, v43, v7[1] - v43);
            v3 = v32;
            memcpy(&v6, &v3, 4);
            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v9, (long long)(&v9)[8], &v6, 4);
            v44 = *((long long *)(v28 + 8));
            if (((char)v44 - v28->field_0 & 15))
            {
                *((char **)&v2) = 0;
                do
                {
                    if (v44 == *((long long *)(v28 + 16)))
                    {
                        $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(v28, v44, &v2);
                    }
                    else
                    {
                        *((char **)v44) = 0;
                        *((char **)(v28 + 8)) = &(1)[*((long long *)(v28 + 8))];
                    }
                } while ((v40 = *((long long *)(v28 + 8)), ((char)v40 - v28->field_0 & 15)));
            }
            v45 = (long long)v9;
            $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(v28, v44, v45, (long long)(&v9)[8] - v45);
            v46 = v29 + v42 - *((int *)&v5);
            v3 = (unsigned int)v44 - *((int *)&v28->field_0);
            memcpy(&(52)[*((long long *)&v28->field_0) + v46], &v3, 4);
            *((char **)&v3) = &(1)[v35];
            memcpy(&(56)[*((long long *)&v28->field_0) + v46], &v3, 4);
            *((char **)&v3) = &(1)[v32];
            memcpy(&(4)[*((long long *)&v28->field_0)], &v3, 4);
            v3 = v42;
            memcpy(&(8)[*((long long *)&v28->field_0)], &v3, 4);
            R<lambda_4850e5e4fe7cad2db8d8b8fa5a62506f>(v28, v46, v15, v14);
            v47 = v7[0];
            if (v47)
            {
                v48 = v47;
                if (v8 - v47 >= __Eg_my_units)
                {
                    v47 = *((long long *)(v47 - 8));
                    if (-8 + (char *)v48 - v47 > 31)
                    {
                        v1 = 0;
                        _invoke_watson(0, 0, 0, 0);
                        [D] Unsupported jumpkind Ijk_SigTRAP at address 54598
                    }
                }
                sub_4030c(v47);
                *((char **)&v7) = 0;
                v8 = 0;
            }
            _Tidy@?$vector@EV?$allocator@E@std(&v9);
            return security_cookie_check(v23 ^ &v0);
        }
    }
}


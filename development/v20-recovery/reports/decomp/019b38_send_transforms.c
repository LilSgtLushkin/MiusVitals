// v20 RVA 019b38..019f0e  (982 bytes)  send_transforms
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;

typedef struct struct_4 {
    struct struct_2 *field_0;
    char paddin8[232];
    struct struct_5 *field_f0;
    char paddin248[4];
    unsigned int field_fc;
} struct_4;

typedef struct struct_3 {
    char paddin0[4656];
    unsigned int field_1230;
} struct_3;

typedef struct struct_6 {
    char paddin0[48];
    unsigned int field_30;
} struct_6;

typedef struct struct_2 {
    char paddin0[72];
    struct struct_1 *field_48;
} struct_2;

typedef struct struct_5 {
    struct struct_3 *field_0;
} struct_5;

typedef struct struct_1 {
    struct struct_1 *field_0;
} struct_1;


long long send_transforms(struct_4 *a0, unsigned long a1)
{
    char v0;  // [bp-0x5b8]
    int <0x19b38[is_18]|Stack bp-0x598, 1 B>;  // [bp-0x598]
    int tmp_5;  // tmp #5
    int tmp_14;  // tmp #14
    int tmp_15;  // tmp #15
    int tmp_12;  // tmp #12
    int tmp_19;  // tmp #19
    int tmp_20;  // tmp #20
    int tmp_17;  // tmp #17
    int tmp_13;  // tmp #13
    int tmp_10;  // tmp #10
    unsigned int v1;  // [bp-0x598]
    void* v2;  // [bp-0x590]
    struct_0 *v3;  // [bp-0x588]
    struct_0 *v4;  // [bp-0x580], Other Possible Types: unsigned long
    struct_0 *v5;  // [bp-0x578], Other Possible Types: unsigned long
    void* v6;  // [bp-0x570]
    unsigned long v7;  // [bp-0x568]
    char v8;  // [bp-0x560]
    char v9;  // [bp-0x548]
    char v13;  // [bp-0x540]
    char v15;  // [bp-0x534]
    char v16;  // [bp-0x528]
    char v21;  // [bp-0x4f8]
    unsigned long long v27;  // [bp-0x68]
    struct struct_3 **v29;  // rdi, Other Possible Types: unsigned long
    unsigned int v31;  // r13d
    unsigned long v32;  // r13, Other Possible Types: unsigned long long
    struct_3 *v33;  // r14
    unsigned long long v34;  // r12
    struct_6 *v35;  // rsi
    int v36;  // xmm0
    int v37;  // xmm7
    int v40;  // xmm1
    int v41;  // xmm0
    int v42;  // xmm7
    int v45;  // xmm1
    struct_0 *v46;  // rsi, Other Possible Types: unsigned long
    unsigned long long v47;  // r12, Other Possible Types: unsigned long
    unsigned long long v48;  // rbx, Other Possible Types: unsigned long

    v27 = g_5a380 ^ &v0;
    if (!g_5c078)
    {
        v2 = 0;
        v3 = &"i_ctrltimer";
        v6 = 0;
        v4 = "ctrltimer";
        v5 = &v2;
        g_5c078 = *((long long *)(a0->field_0 + 72))();
    }
    *((char **)&g_5be90) = &(1)[g_5be90];
    v29 = *((long long *)(a0 + 240));
    v7 = &v21;
    v3 = &v7;
    v4 = &<0x19b38[is_18]|Stack bp-0x598, 1 B>;
    v5 = &v21;
    *((char **)&v1) = 0;
    if (v31)
    {
        v32 = *((int *)(a0 + 252));
        do
        {
            memset(&v9, 0, 80);
            if ((char)unit_key(*((long long *)v29), &v2, &v9) && (char)owns_object(*((long long *)v29)))
            {
                v33 = *((long long *)v29);
                v34 = v33 + 432;
                v48 = v33 + 240;
                v35 = *((long long *)$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KUSent$_Try_emplace@AEB_K$$V@?$_Hash@V?$_Umap_traits@_KUSent(&g_5b660, &v8, &v2));
                tmp_14 = SubV(tmp_15, tmp_12);
                v36 = tmp_14;
                fabs(&g_5b660, 0, 80);
                v37 = 0;
                tmp_19 = SubV(tmp_20, tmp_17);
                *((unsigned int *)&v37) = (unsigned long long)v36;
                *((unsigned long long *)&v36) = tmp_19;
                fabs();
                tmp_19 = SubV(tmp_20, tmp_17);
                fabs();
                v40 = (0 CONCAT tmp_19) + (0 CONCAT (unsigned int)(unsigned long long)v36) + v37;
                if ((((char)(CmpF((unsigned int)v40, 990057071) & 69) | (char)((CmpF((unsigned int)v40, 990057071) & 69) >> 6)) & 1) != 1)
                {
LABEL_19d63:
                    memcpy(v35 + 24, v34, 12);
                    memcpy(v35 + 36, v48, 12);
                    *((unsigned int *)(v35 + 48)) = a1;
                    memcpy(&v13, v34, 12);
                    v46 = &v15;
                    v47 = 3;
                    while (true)
                    {
                        memcpy(v46, v48, 12);
                        v48 = &(16)[v48];
                        v46 = &v16;
                        v47 -= 1;
                    }
                }
                else
                {
                    tmp_12 = SubV(tmp_13, tmp_10);
                    v41 = tmp_12;
                    fabs();
                    v42 = 0;
                    tmp_19 = SubV(tmp_20, tmp_17);
                    *((unsigned int *)&v42) = (unsigned long long)v41;
                    *((unsigned long long *)&v41) = tmp_19;
                    fabs();
                    tmp_19 = SubV(tmp_20, tmp_17);
                    fabs();
                    v45 = (0 CONCAT tmp_19) + (0 CONCAT (unsigned int)(unsigned long long)v41) + v42;
                    if (!((((char)(CmpF((unsigned int)v45, 973279855) & 69) | (char)((CmpF((unsigned int)v45, 973279855) & 69) >> 6)) & 1) == 1) || !((unsigned int)a1 - *((int *)(v35 + 48)) < (*((int *)&v9) & 15) * 30 + 2000))
                        goto LABEL_19d63;
                }
            }
        } while ((v29 += 8, tmp_5 = v32, v32 -= 1, tmp_5 != 1));
        if (!v1 && false)
            return security_cookie_check(v27 ^ &v0);
    }
    R(&v3);
    return security_cookie_check(v27 ^ &v0);
}


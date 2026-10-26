// v20 RVA 03c650..03c90d  (701 bytes)  shots_sim_step
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[20];
    unsigned int field_14;
    char paddin24[12];
    unsigned long long field_24;
    unsigned long long field_2c;
    unsigned long long field_34;
} struct_0;

typedef struct struct_1 {
    char paddin0[72];
    unsigned long long field_48;
} struct_1;

typedef struct struct_2 {
    char field_0;
} struct_2;


long long shots_sim_step(struct_0 *a0)
{
    char v0;  // [bp-0x188]
    unsigned long v1;  // [bp-0x168]
    unsigned int v2;  // [bp-0x160]
    char v3;  // [bp-0x158]
    char v4;  // [bp-0x148]
    unsigned long v5;  // [bp-0x140]
    unsigned long v6;  // [bp-0x138]
    unsigned long v7;  // [bp-0x130]
    char v8;  // [bp-0x128]
    char v9;  // [bp-0xc0]
    char v10;  // [bp-0xbc]
    char v11;  // [bp-0xb8]
    char v12;  // [bp-0xb7]
    char v13;  // [bp-0xa8]
    char v14;  // [bp-0x48]
    char v15;  // [bp-0x40]
    char v16;  // [bp-0x38]
    char v17;  // [bp-0x30]
    unsigned long long v18;  // [bp-0x28]
    unsigned long v19;  // [bp-0x8]
    char v20;  // [bp+0x0]
    struct_0 *v22;  // rbx
    unsigned long long v23;  // rcx
    struct_1 *v24;  // rdi
    unsigned int v25;  // r14d
    void* v26;  // rax, Other Possible Types: unsigned long
    void* v27;  // rax, Other Possible Types: unsigned long
    unsigned long long v28;  // rcx
    void* v29;  // rax, Other Possible Types: unsigned long
    unsigned long long v30;  // rcx
    struct_2 *v32;  // r8
    struct_1 *v33;  // rdx
    unsigned long long v34;  // rcx
    unsigned long long v35;  // rcx
    unsigned long long v36;  // r9

    v19 = &v20;
    v18 = g_5a380 ^ &v0;
    v22 = a0;
    memcpy(&v8, a0 + 76, 116);
    v23 = *((long long *)(a0 + 44));
    v24 = session_unit(v23);
    if (v24 && !(char)sub_1bacc(v24))
    {
        v25 = strcmp(*((long long *)(v24 + 72)), "i_techn");
        if ((char)readable(v24, &(2928)[(unsigned int)-(0 < v25) & &g_1c00]) && v25 == 0 == v12 != 0)
        {
            memset(&v13, 0, 128);
            memcpy(&v13, &v8, 104);
            if (xlate_desc(v24) == -1)
                v26 = 0;
            else
                (unsigned int)v26 = *((long long *)(g_5db08->field_0 + 456))(g_5db08);
            v5 = v26;
            if (xlate_desc(v23) == -1)
                v27 = 0;
            else
                (unsigned int)v27 = *((long long *)(g_5db08->field_0 + 456))(g_5db08);
            v28 = *((long long *)(v22 + 52));
            v6 = v27;
            if (!v28)
                v29 = 0;
            else
                (unsigned int)v29 = session_unit(v28);
            v30 = *((long long *)(v22 + 36));
            v7 = v29;
            memcpy(&v15, &v5, 8);
            memcpy(&v16, &v6, 8);
            memcpy(&v17, &v7, 8);
            memcpy(&v4, &v14, 4);
            if (!(char)read_file(v30, &v4))
            {
                g_5db68 = &(1)[g_5db68];
                return security_cookie_check(v18 ^ &v0);
            }
            memcpy(&v14, &v4, 4);
            v32 = &v13;
            v33 = v24;
            *((int *)(&v24->paddin0[(unsigned int)-(0 < v25) & &g_1c04] + 2916)) = *((int *)&v10);
            *((char *)&v29) = v11;
            v34 = g_5db00;
            if (!v25)
                v35 = g_5daf8;
            v36 = *((int *)(v22 + 20));
            v3 = v29;
            v2 = *((int *)&v9);
            *((int *)&v1) = (v30 == 0 ? session_unit(v30) : 0);
            if ((char)append@?$basic_string@DU?$char_traits@D@std@?$basic_string@DU?$char_traits@D@std())
                g_5db50 = &(1)[g_5db50];
            else
                g_5db60 = &(1)[g_5db60];
            if (!g_5db12)
                return security_cookie_check(v18 ^ &v0);
            mp_log("hit: %s struck in its owner's game (kind %u, part %x), carried out here", v24 + 8, *((int *)(v22 + 20)), *((int *)&v14));
            return security_cookie_check(v18 ^ &v0);
        }
    }
    g_5db58 = &(1)[g_5db58];
    return security_cookie_check(v18 ^ &v0);
}


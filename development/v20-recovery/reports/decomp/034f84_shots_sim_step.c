// v20 RVA 034f84..035378  (1012 bytes)  shots_sim_step
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[4];
    char field_4;
} struct_0;

typedef struct struct_5 {
    char field_0;
} struct_5;

typedef struct struct_4 {
    char field_-4;
    char field_-3;
    char field_-2;
    char padding_-1[1];
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    char paddin20[8];
    unsigned long long field_1c;
    unsigned long long field_24;
    unsigned long long field_2c;
} struct_4;


long long shots_sim_step(struct struct_2 **a0)
{
    char v0;  // [bp-0x128]
    void* v1;  // [bp-0x100], Other Possible Types: unsigned int
    void* v2;  // [bp-0xf8]
    char v3;  // [bp-0xf0]
    unsigned long v4;  // [bp-0xe8]
    unsigned long v5;  // [bp-0xe0]
    struct_5 *v6;  // [bp-0xd8]
    void* v7;  // [bp-0xd0]
    char v8;  // [bp-0xc8]
    unsigned long v9;  // [bp-0x90]
    unsigned long v10;  // [bp-0x88]
    unsigned long v11;  // [bp-0x80]
    void* v12;  // [bp-0x78], Other Possible Types: unsigned long
    unsigned long v13;  // [bp-0x68]
    unsigned long long v14;  // [bp-0x38]
    struct struct_2 **v16;  // rdi
    struct_0 *v17;  // rbx, Other Possible Types: unsigned long
    char v18;  // cl
    struct struct_1 **v19;  // r8
    struct_0 *v20;  // cc_dep2, Other Possible Types: unsigned long
    struct_4 *v21;  // rbx, Other Possible Types: unsigned long
    char v22;  // al
    unsigned long long v23;  // r15
    unsigned long v24;  // r12
    unsigned long long v25;  // rcx
    unsigned long v26;  // r14
    unsigned long long v27;  // rcx
    void* v28;  // rax, Other Possible Types: unsigned long
    char v29;  // dl
    unsigned long long v31;  // rcx
    struct_0 *v33;  // rax, Other Possible Types: unsigned long
    struct_0 *v34;  // cc_dep1, Other Possible Types: unsigned long

    v14 = g_5a380 ^ &v0;
    v16 = a0;
    g_5d054 = GetTickCount();
    if (g_5d044)
        send_transforms(a0);
    if (g_5d04c)
        safe_str(a0);
    if (g_5d050)
        sub_35720(a0);
    v17 = g_5cfa0;
    if (g_5cfa0 == g_5cfa8)
        return security_cookie_check(v14 ^ &v0);
    do
    {
        v18 = (4)[v17];
        if ((4)[v17] == 2)
        {
            trench_received(v17);
        }
        else if (rcx<8> - 3 <= 1)
        {
            xlate_received(v17, v16);
        }
        else if (v18 == 5)
        {
            trench_received(v17);
        }
        else if (v18 == 6)
        {
            trench_received(v17);
        }
        else if (v18 == 7)
        {
            trench_received(v17);
        }
        else if (v18 == 8)
        {
            trench_received(v17);
        }
    } while ((v17 += 220, v17 != g_5cfa8));
    if (!g_5cfe0 || !g_5a2ec || !((v2 = (void*)0, v4 = (unsigned long)"i_shellphys", v7 = (void*)0, v5 = (unsigned long)"shellphys", v6 = (struct_5 *)&v2, g_5d008 = (unsigned long long)v19(v16, &v4, *((long long *)(*(v16) + 72))), g_5d008)))
    {
        v33 = g_5cfa0;
        v34 = g_5cfa0;
        v20 = g_5cfa8;
    }
    else
    {
        g_5d000 = v16;
        if (g_5cfa0 == g_5cfa8)
            return security_cookie_check(v14 ^ &v0);
        v21 = g_5cfa0 + 8;
        do
        {
            v22 = *(v21 - 4);
            if (*(v21 - 4) == 1)
            {
                trench_sim_step(v21 - 8);
            }
            else if (v22 == 9)
            {
                restore_slot(v21 - 8, v16);
            }
            else if (!v22)
            {
                v23 = (xlate_desc(*((int *)v21)) == -1 ? *((long long *)(g_5d000->field_0 + 456))(g_5d000) : 0);
                v24 = (xlate_desc(*((int *)&(4)[v21])) == -1 ? *((long long *)(g_5d000->field_0 + 456))(g_5d000) : 0);
                ammo_check(v23, &v3, &v2);
                v25 = *((long long *)&(28)[v21]);
                v26 = (v25 == 0 ? session_unit(v25) : 0);
                if (!v23 || !v24 || memcmp(&v3, &(20)[v21], 8) || (int)v2 != *((int *)&(16)[v21]) || *((long long *)&(28)[v21]) && !v26)
                {
                    g_5d070 = &(1)[g_5d070];
                }
                else
                {
                    memcpy(&v8, &(68)[v21], 144);
                    v27 = *((long long *)&(36)[v21]);
                    v9 = v23;
                    v10 = v24;
                    v11 = v26;
                    if (!v27)
                        v12 = 0;
                    else
                        v12 = session_unit(v27);
                    if (xlate_desc(*((int *)&(8)[v21])) == -1)
                        v28 = 0;
                    else
                        (unsigned int)v28 = *((long long *)(g_5d000->field_0 + 456))(g_5d000);
                    v13 = v28;
                    memcpy(&v4, &(52)[v21], 16);
                    v29 = 76;
                    v1 = 0;
                    log_launch("received", rdx<8>, &v8, *(v21 - 3), 0);
                    v31 = *((long long *)&(44)[v21]);
                    *((char **)&g_5d024) = 1;
                    v1 = *((int *)&(12)[v21]);
                    g_5cfe0(g_5d008, &v8, *(v21 - 3) & 4294967293, -(0 < *(v21 - 2)) & &v4, (v31 == 0 ? session_unit(v31) : 0));
                    g_5d068 = &(1)[g_5d068];
                    *((char **)&g_5d024) = 0;
                }
            }
        } while ((v17 += 220, v17 != g_5cfa8));
        v33 = g_5cfa0;
        v34 = g_5cfa0;
        v20 = g_5cfa8;
    }
    if (v34 == v20)
        return security_cookie_check(v14 ^ &v0);
    g_5cfa8 = v33;
}


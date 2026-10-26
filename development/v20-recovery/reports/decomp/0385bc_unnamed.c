// v20 RVA 0385bc..038841  (645 bytes)  trench_sim_step
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char field_0;
} struct_2;


long long trench_sim_step(struct struct_1 **a0)
{
    char v0;  // [bp-0x2c8]
    void* v2;  // [bp-0x298]
    unsigned long v3;  // [bp-0x290]
    unsigned long v4;  // [bp-0x288]
    struct_2 *v5;  // [bp-0x280]
    void* v6;  // [bp-0x278]
    unsigned short v7;  // [bp-0x268]
    char v8;  // [bp-0x266]
    char v9;  // [bp-0x158]
    char v10;  // [bp-0xf8]
    unsigned long long v12;  // [bp-0x38]
    unsigned long v13;  // [bp-0x8]
    char v14;  // [bp+0x0]
    struct struct_1 **v16;  // rsi
    unsigned long long v17[4];  // rbx, Other Possible Types: unsigned long
    char *v18;  // rdx
    unsigned int *v19;  // rcx
    unsigned long long v20;  // rax
    unsigned long long v21;  // rcx
    unsigned int v23;  // r9
    struct_2 *v25;  // r8, Other Possible Types: unsigned long
    struct_2 *v26;  // r15, Other Possible Types: unsigned long, unsigned long long
    unsigned long long v27[4];  // r14, Other Possible Types: unsigned long
    unsigned long v28;  // r12, Other Possible Types: unsigned long long
    unsigned long long v29;  // rbx, Other Possible Types: unsigned long
    unsigned long long v30;  // rdi, Other Possible Types: unsigned long
    struct struct_0 **v31;  // r8

    v13 = &v14;
    v12 = g_5a380 ^ &v0;
    v16 = a0;
    if (!g_5d3b0)
    {
        v17 = g_5d0d0;
        if (g_5d0d0 != g_5d0d8)
        {
            *((char **)&g_5d3b0) = 1;
            v27 = v17;
            v26 = &v9;
            v28 = 3;
            while (true)
            {
                v2 = 0;
                *((char **)&v7) = 63;
                memset(&v8, 0, 258);
                v18 = *((long long *)v27);
                if (v18 && GetModuleHandleExA(6, v18, &v2))
                {
                    v19 = v2;
                    if (v19)
                        GetModuleFileNameA(v19, &v7, 260);
                }
                (unsigned int)v20 = strrchr(&v7, 92);
                v21 = v2;
                v23 = (v21 == 0 ? *((long long *)v27) - v21 : 0);
                v25 = &v7;
                if (v20)
                    v25 = &(1)[v20];
                $sprintf_s@$0GA(v26, "%s+0x%llx", v25, v23);
                v26 = &v10;
                v27 = &(8)[v27];
                v28 -= 1;
            }
        }
    }
    queue_shot(v16);
    sub_368dc(v16);
    if (g_5d27c)
        test_move(v16);
    v29 = g_5d130;
    v30 = g_5d138;
    if (g_5d130 == g_5d138)
        return security_cookie_check(v12 ^ &v0);
    if (g_5d218 && g_5a2ed)
    {
        if (!g_5d270)
        {
            v2 = 0;
            v3 = "i_trencher";
            v6 = 0;
            v4 = "trencher";
            v5 = &v2;
            g_5d270 = v31(v16, &v3, *((long long *)(*(v16) + 72)));
            if (!g_5d270)
                return security_cookie_check(v12 ^ &v0);
            v30 = g_5d138;
            v29 = g_5d130;
        }
        if (v29 == v30)
            return security_cookie_check(v12 ^ &v0);
        do
        {
            memcpy(&v7, &(76)[v29], 140);
            apply(&v7);
            v29 = &(220)[v29];
        } while (v29 != v30);
        v29 = g_5d130;
        if (g_5d130 == g_5d138)
            return security_cookie_check(v12 ^ &v0);
    }
    g_5d138 = v29;
    return security_cookie_check(v12 ^ &v0);
}


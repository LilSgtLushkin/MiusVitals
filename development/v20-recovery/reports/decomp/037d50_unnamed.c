// v20 RVA 037d50..037f45  (501 bytes)  test_move
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;

typedef struct struct_1 {
    char paddin0[64];
    struct struct_2 *field_40;
    char paddin72[760];
    char field_340;
    char paddin833[162];
    char field_3e3;
    char field_3e4;
} struct_1;

typedef struct struct_2 {
    char paddin0[432];
    unsigned int field_1b0;
    unsigned int field_1b4;
    unsigned int field_1b8;
} struct_2;


long long test_move(struct struct_4 **a0)
{
    char v0;  // [bp-0x88]
    unsigned int *v1;  // [bp-0x68]
    unsigned long v2;  // [bp-0x60]
    struct_0 *v3;  // [bp-0x58]
    void* v4;  // [bp-0x50]
    void* v5;  // [bp-0x48], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x44]
    unsigned int v7;  // [bp-0x40]
    unsigned int v8;  // [bp-0x3c]
    unsigned long long v9;  // [bp-0x38]
    unsigned long v10;  // [bp-0x8]
    char v11;  // [bp+0x0]
    unsigned int v13;  // r13d
    void* v14;  // rsi, Other Possible Types: unsigned long
    unsigned long long v15;  // rax, Other Possible Types: unsigned long
    unsigned long long v16;  // r15
    struct_1 *v17;  // rax
    unsigned int v18[111];  // rbx
    int v19;  // xmm1
    int v20;  // xmm0
    int v21;  // xmm0
    int v22;  // xmm1
    void* v23;  // r12, Other Possible Types: unsigned long long, unsigned long
    unsigned int *v24;  // rdi, Other Possible Types: unsigned long

    v10 = &v11;
    v9 = g_5a380 ^ &v0;
    v13 = g_5d27c;
    v14 = 0;
    v23 = 0;
    *((char **)&g_5d27c) = 0;
    v15 = g_5d270;
    if (!g_5d270)
    {
        v5 = 0;
        v1 = &"i_trencher";
        v2 = "trencher";
        v3 = &v5;
        v4 = 0;
        (unsigned int)v15 = *((long long *)(*(a0) + 72))();
        g_5d270 = v15;
    }
    if (g_5d218 && v15)
    {
        $_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(&v1);
        for (v24 = v1; v24 != v2 && (v16 = (unsigned long long)(unsigned int)*((int *)v24), (unsigned int)v14 < v13); v24 = &(4)[v24])
        {
            v17 = squad_record(g_5d270, v16);
            if (v17 && (*((char *)(v17 + 832)) & 1) && *((char *)(v17 + 995)) && *((char *)(v17 + 996)))
            {
                v18 = *((long long *)(v17 + 64));
                if ((char)readable(v18, 704))
                {
                    v19 = 0;
                    *((unsigned int *)&v19) = g_5d300;
                    v20 = v19;
                    if (((char)v23 & 1))
                        v20 = v19 ^ 0x80000000800000008000000080000000;
                    xmm0<16> = (xmm0<16> + Conv(32->128, Load(addr=(rbx<8> + 0x1b0<64>), size=4, endness=Iend_LE)))
                    v5 = (unsigned int)v21;
                    v6 = *((int *)(v18 + 436));
                    xmm1<16> = ((xmm1<16> * 0x3f19999a<128>) + Conv(32->128, Load(addr=(rbx<8> + 0x1b8<64>), size=4, endness=Iend_LE)))
                    v7 = (unsigned int)v22;
                    *((char **)&v8) = 0;
                    v23 = &(1)[v23];
                    dig_hook(g_5d270, v16, &v5);
                }
            }
        }
        _Tidy@?$vector@KV?$allocator@K@std(&v1);
        mp_log("test: %d squads of this player dug in again %.0f m away", v14, g_5d300);
        return security_cookie_check(v9 ^ &v0);
    }
    mp_log("test: no trench manager to dig with");
    return security_cookie_check(v9 ^ &v0);
}


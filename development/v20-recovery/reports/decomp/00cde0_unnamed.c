// v20 RVA 00cde0..00cfa9  (457 bytes)  net_dispatch
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[472];
    unsigned long long field_1d8;
} struct_0;

typedef struct struct_1 {
    char paddin0[200];
    unsigned long long field_c8;
    char paddin208[4];
    unsigned int field_d4;
    char paddin216[1304];
    unsigned long long field_5f0;
} struct_1;

typedef struct struct_3 {
    char paddin0[776];
    unsigned int field_308;
} struct_3;


long long net_dispatch(struct_0 *a0, unsigned long a1, unsigned long a2, unsigned int a3)
{
    char v0;  // [bp-0x178]
    void* v1;  // [bp-0x158]
    char v2;  // [bp-0x148]
    unsigned long long v3;  // [bp-0x38]
    struct_1 *v5;  // rbx
    struct_3 *v6;  // rsi
    unsigned int *v7;  // rdi
    unsigned long long v8;  // rbx
    unsigned long long v9;  // r12
    unsigned long v10;  // r15
    unsigned long long v11;  // cc_op
    unsigned long long v12;  // cc_dep1
    void* v13;  // cc_dep2
    unsigned long v14;  // cc_ndep
    void* v15;  // rbp, Other Possible Types: unsigned long long, unsigned long
    unsigned long long v16;  // cc_op
    unsigned long long v17;  // cc_dep1
    unsigned long long v18;  // cc_dep2
    void* v19;  // rcx, Other Possible Types: unsigned long
    unsigned long v20;  // cc_ndep
    unsigned long long v21;  // cc_op
    unsigned long long v22;  // cc_dep1
    void* v23;  // cc_dep2
    void* v24;  // r14, Other Possible Types: unsigned long long, unsigned long
    unsigned long v25;  // cc_ndep
    unsigned int *v26;  // rsi, Other Possible Types: unsigned long

    v3 = g_5a380 ^ &v0;
    g_5bfe8();
    v5 = ((char)readable(a0, 512) == 0 ? is_object(*((long long *)(a0 + 472))) : 0);
    v6 = ((char)readable(v5, 1536) == 0 ? *((long long *)(v5 + 1520)) : 0);
    v7 = ((char)readable(v5, 1536) == 0 ? *((long long *)(v5 + 200)) : 0);
    v8 = ((char)readable(v5, 1536) == 0 ? *((int *)(v5 + 212)) : 0);
    if ((char)readable(v6, 784) == 0)
    {
        return security_cookie_check(v3 ^ &v0);
    }
    else if (!v7)
    {
        return security_cookie_check(v3 ^ &v0);
    }
    else if ((unsigned int)(v8 - 1) > 4095)
    {
        return security_cookie_check(v3 ^ &v0);
    }
    else if ((char)readable(v7, v8 * 1776) != 0)
    {
        v9 = *((int *)(v6 + 776));
        v24 = 0;
        v15 = 0;
        1?$list@U?$pair@$$CBII@std?$list@U?$pair@$$CBII@std(&v2, 260);
        v19 = 0;
        v26 = 0;
        v1 = 0;
        if ((unsigned int)v8)
        {
            do
            {
                v10 = v26 * 1776;
                v11 = 19;
                v12 = *((int *)(v10 + (char *)v7 + 832)) & &g_2000;
                v13 = 0;
                if (((short)*((int *)(v10 + (char *)v7 + 832)) & 0x2000))
                {
                    v14 = flags_of(0x13<64>, Conv(32->64, (Load(addr=((r15<8> + rdi<8>) + 0x340<64>), size=4, endness=Iend_LE) & 0x2000<32>)), 0x0<64>, cc_ndep<8>);
                    v15 = &(1)[v15];
                    v16 = 7;
                    v17 = *((int *)(v10 + (char *)v7));
                    v18 = v9;
                    if (*((int *)(v10 + (char *)v7)) == (unsigned int)v9)
                    {
                        if (!v19)
                        {
                            if (fopen_s(&v1, &v2, "a"))
                                return security_cookie_check(v3 ^ &v0);
                            v19 = v1;
                            if (!v19)
                                return security_cookie_check(v3 ^ &v0);
                        }
                        fprintf(v19, "%u\n", *((int *)(v10 + (char *)v7 + 1652)), a3);
                        v19 = v1;
                        v20 = flags_of(0x14<64>, cc_dep1<8>, 0x0<64>, cc_ndep<8>);
                        v21 = 23;
                        v22 = &(1)[v24];
                        v23 = 0;
                        v24 = &(1)[v24];
                    }
                }
            } while ((v25 = flags_of(cc_op<8>, cc_dep1<8>, cc_dep2<8>, cc_ndep<8>), v26 = (unsigned long long)((unsigned int)v26 + 1), (unsigned int)v26 < (unsigned int)v8));
            if (v19)
                fclose(v19);
        }
        mp_log("map: abilities pass - intelligence found out %u battle groups this turn, %u of them the host's (now known to the other player)", v15, v24);
        return security_cookie_check(v3 ^ &v0);
    }
    else
    {
        return security_cookie_check(v3 ^ &v0);
    }
}


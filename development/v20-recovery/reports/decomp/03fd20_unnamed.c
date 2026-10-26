// v20 RVA 03fd20..03fe61  (321 bytes)  sub_3fd20
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_3fd20(unsigned long a0, char *a1, unsigned long a2)
{
    int tmp_30;  // tmp #30
    int tmp_4;  // tmp #4
    int tmp_16;  // tmp #16
    int tmp_32;  // tmp #32
    int tmp_7;  // tmp #7
    int tmp_33;  // tmp #33
    int tmp_8;  // tmp #8
    int tmp_17;  // tmp #17
    int tmp_90;  // tmp #90
    int tmp_35;  // tmp #35
    int tmp_74;  // tmp #74
    int tmp_91;  // tmp #91
    int tmp_36;  // tmp #36
    int tmp_75;  // tmp #75
    unsigned long long v1;  // r9
    unsigned long long v2;  // r10
    char *v3;  // rbx
    unsigned long long v5;  // r9
    unsigned long v6;  // rax
    unsigned long long v7;  // r9
    unsigned long v10;  // rax
    unsigned long v11;  // rax

    v1 = &a1[-1 * a0];
    v2 = a0;
    v3 = a1;
    if (!(v1 & -0x20) || !((char)g_5a3e4 & 32))
    {
        v5 = v1 & -0x10;
        if (v5 && ((char)g_5a3e4 & 4))
        {
            do
            {
                a1 -= 16;
                tmp_30 = UnaryOp GetMSBs;
                tmp_16 = tmp_30;
                v11 = tmp_16;
                if ((unsigned int)v11)
                    return (ExpCmpNE(v11, 0) ? (ExpCmpNE(v11, 0) ? &(63)[-1 * Clz(v11)] : v11) : (ExpCmpNE(v11, 0) ? &(63)[-1 * Clz(v11)] : v11)) + a1;
            } while (a1 != &a1[-1 * v5]);
        }
    }
    else
    {
        do
        {
            a1 -= 32;
            tmp_32 = UnaryOp GetMSBs;
            tmp_16 = tmp_32;
            tmp_33 = UnaryOp GetMSBs;
            tmp_17 = tmp_33;
            v6 = tmp_17 CONCAT tmp_16;
            if ((unsigned int)v6)
                return (unsigned long long)&(31)[-1 * (v6 << 32 == 0 ? (unsigned int)Clz(rax<8> << 32) : 32)] + a1;
        } while (a1 != &a1[-1 * (v1 & -0x20)]);
        v7 = (unsigned int)v1 & 28;
        if ((unsigned int)v7)
        {
            a1 -= v7;
            tmp_90 = UnaryOp GetMSBs;
            tmp_74 = tmp_90;
            tmp_91 = UnaryOp GetMSBs;
            tmp_75 = tmp_91;
            v10 = tmp_75 CONCAT tmp_74;
            if ((unsigned int)v10)
                return (unsigned long long)&(31)[-1 * (v10 << 32 == 0 ? (unsigned int)Clz(rax<8> << 32) : 32)] + a1;
        }
    }
    if (a1 == v2)
        return v3;
    while (*((char *)a1) != (char)a2)
    {
        a1 -= 1;
        if (a1 == v2)
            return v3;
    }
    return a1;
}


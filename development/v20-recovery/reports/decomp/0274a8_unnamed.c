// v20 RVA 0274a8..027713  (619 bytes)  button
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long button(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long a5, char a6)
{
    char v0;  // [bp-0x128]
    int tmp_76;  // tmp #76
    int tmp_77;  // tmp #77
    int tmp_78;  // tmp #78
    unsigned int v1;  // [bp-0x108]
    unsigned int v2;  // [bp-0x100]
    char v3;  // [bp-0xf8]
    char v4;  // [bp-0xe8]
    unsigned long long v5;  // [bp-0x98]
    unsigned long v7;  // r15
    unsigned long long v8;  // r14
    int v9;  // xmm0
    int v10;  // xmm2
    int v12;  // xmm1
    int v13;  // xmm3
    char v15;  // sil
    char v16;  // rdi
    int v17;  // xmm0
    int v18;  // xmm0
    unsigned int v20;  // r9

    v5 = g_5a380 ^ &v0;
    v7 = a4;
    v8 = a5;
    if ((CmpF(g_5caa0, (unsigned long long)v9) & 1) || !((((char)(CmpF((unsigned long long)(v10 + v9), g_5caa0) & 69) | (char)((CmpF((unsigned long long)(v10 + v9), g_5caa0) & 69) >> 6)) & 1) != 1) || !(!(CmpF(g_5caa4, (unsigned long long)v12) & 1)) || !((((char)(CmpF((unsigned long long)(v13 + v12), g_5caa4) & 69) | (char)((CmpF((unsigned long long)(v13 + v12), g_5caa4) & 69) >> 6)) & 1) != 1))
        v15 = 0;
    else
        v15 = 1;
    v1 = -1040;
    quad();
    v16 = a6;
    xmm0<16> = Conv(64->128, (rdi<8> + 0x2<64>))
    tmp_76 = (unsigned int)v17;
    tmp_77 = (unsigned long long)v17 >> 32;
    tmp_78 = (unsigned int)v17 / 0x10000000000000000;
    *((unsigned int *)&v17) = (0 CONCAT &(2)[a6]) >> 64 >> 32;
    *((unsigned int *)&v17) = tmp_78;
    *((unsigned int *)&v17) = tmp_77;
    *((unsigned int *)&v17) = tmp_76;
    v2 = (-(0 < v16) & -3156774) - 10858168;
    xmm0<16> = (xmm0<16> * Conv(32->128, Load(addr=0x5cbd8<64>, size=4, endness=Iend_LE)))
    v1 = (unsigned int)v18;
    frame(v2);
    if (!v16 || !((char)(GetTickCount() / 500) & 1))
        v16 = 0;
    else
        v16 = 1;
    if (strlen(v7) != 0)
    {
        v20 = (v16 ? &g_45ce6 : "_");
        $sprintf_s@$0FA(&v4, "%s%s", v7, v20);
        *((char **)&v3) = 0;
        *((char **)&v2) = 0;
        v1 = -14014942;
        text(&v4);
    }
    else
    {
        if (v8 != 0)
        {
            *((char **)&v3) = 0;
            *((char **)&v2) = 0;
            v1 = -6055284;
            text(v8);
        }
        if (v16)
        {
            *((char **)&v3) = 0;
            *((char **)&v2) = 0;
            v1 = -14014942;
            text("_");
        }
    }
    if (v15 && g_5c46c)
        return security_cookie_check(v5 ^ &v0);
    return security_cookie_check(v5 ^ &v0);
}


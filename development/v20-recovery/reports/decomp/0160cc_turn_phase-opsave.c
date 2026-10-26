// v20 RVA 0160cc..016230  (356 bytes)  turn_phase@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long turn_phase@opsave()
{
    char v0;  // [bp-0x188]
    int tmp_3;  // tmp #3
    void* v1;  // [bp-0x168]
    unsigned int v2;  // [bp-0x158]
    unsigned int v3;  // [bp-0x154]
    int v4;  // [bp-0x150]
    void* v5;  // [bp-0x140]
    char v6;  // [bp-0x138]
    char v7;  // [bp-0x128]
    unsigned long long v8;  // [bp-0x18]
    unsigned long v9;  // [bp-0x8]
    char v10;  // [bp+0x0]
    char *v12;  // r8
    unsigned long long v13;  // rcx
    unsigned long v15;  // rdx
    unsigned int v16;  // r9d
    struct_0 *v18;  // rcx

    v9 = &v10;
    v8 = g_5a380 ^ &v0;
    *((char **)&v4) = 0;
    v5 = 0;
    if (GetEnvironmentVariableA("MP_PEER_FORCE", &v7, 260) && (char)read_file(&v7, &v4))
    {
        v12 = (long long)v4;
        v13 = (long long)(&v4)[8];
        if (v12 != v13)
        {
            *((char **)&v3) = 0;
            tmp_3 = v13;
            v15 = 0;
            v16 = 2166136261;
            if (tmp_3 != v12)
            {
                do
                {
                    v16 = (v12[v15] ^ v16) * 16777619;
                    v15 = &(1)[v15];
                } while (v15 < v13 - v12);
            }
            g_5bfb0 = v16;
            *((char **)&v2) = 0;
            g_5bfbc = -(0 < sides@image(&v4, &v2, &v6)) & v2;
            squad_owners@image@image(&v4, g_5bfbc, &g_5bfb8, &v3);
        }
    }
    v18 = (long long)v4;
    if (!v18)
        return security_cookie_check(v8 ^ &v0);
    if (v5 - v18 >= __Eg_my_units && -8 + (char *)v18 - *((long long *)(v18 - 8)) > 31)
    {
        v1 = 0;
        _invoke_watson(0, 0, 0, 0);
        [D] Unsupported jumpkind Ijk_SigTRAP at address 90627
    }
    sub_4030c();
    *((char **)&v4) = 0;
    v5 = 0;
    return security_cookie_check(v8 ^ &v0);
}


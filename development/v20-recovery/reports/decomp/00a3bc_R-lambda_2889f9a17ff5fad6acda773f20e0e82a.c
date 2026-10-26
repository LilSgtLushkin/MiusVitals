// v20 RVA 00a3bc..00a51c  (352 bytes)  R<lambda_2889f9a17ff5fad6acda773f20e0e82a>
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    struct struct_3 *field_0;
    struct struct_4 *field_8;
} struct_2;

typedef struct struct_3 {
    struct struct_1 *field_0;
} struct_3;

typedef struct struct_4 {
    unsigned short field_0;
} struct_4;

typedef struct struct_1 {
    unsigned long long field_0;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    char field_18;
    char field_19;
    char field_1a;
    char field_1b;
    unsigned int field_1c;
    char field_20;
} struct_1;


long long R(struct_2 *a0)
{
    unsigned long v0;  // [bp+0x8]
    unsigned short v1;  // [bp+0xc]
    unsigned short v2;  // [bp+0xe]
    int v3;  // xmm1, Other Possible Types: unsigned int
    unsigned int v5;  // ecx
    unsigned int *v6;  // rax

    strncpy(&v0, "GTMP", 4);
    *((char **)&v1) = 6;
    v2 = *((short *)*((long long *)(a0 + 8)));
    a0->field_0->field_0->field_0 = v0;
    *((unsigned int *)(a0->field_0->field_0 + 8)) = g_5be90;
    *((unsigned int *)(a0->field_0->field_0 + 12)) = g_5beb8;
    if (g_5c078)
    {
        v3 = 0;
        v3 = *((int *)(g_5c078 + 264));
    }
    else
    {
        v3 = 0 CONCAT 0x3f800000;
    }
    if (!g_5c078 || !*((char *)(g_5c078 + 349)) || *((int *)(g_5c078 + 344)) < 16 || (((char)(CmpF((unsigned int)v3, v3) & 69) | (char)((CmpF((unsigned int)v3, v3) & 69) >> 6)) & 1) == 1)
        v3 = v3;
    *((unsigned int *)(a0->field_0->field_0 + 16)) = (unsigned int)v3;
    *((unsigned int *)(a0->field_0->field_0 + 20)) = (unsigned int)v3;
    *((int *)(a0->field_0->field_0 + 24)) = (g_5c078 == 0 ? *((char *)(g_5c078 + 348)) : 0);
    *((char *)(a0->field_0->field_0 + 25)) = g_5beb4;
    *((char *)(a0->field_0->field_0 + 26)) = g_5becc;
    *((char **)(a0->field_0->field_0 + 27)) = 0;
    *((unsigned int *)(a0->field_0->field_0 + 28)) = g_5a218;
    *((char *)(a0->field_0->field_0 + 32)) = g_5b83f;
    memset(a0->field_0->field_0 + 33, 0, 3);
    v5 = *((int *)*((long long *)(a0 + 8)));
    input_action_hook(&g_5b8f0, *((long long *)(a0 + 16)), &(36)[v5 * 80]);
    v6 = *((long long *)(a0 + 8));
    *((char **)&g_5be9c) = &(1)[g_5be9c];
    *((char **)&v6) = 0;
    return v6;
}


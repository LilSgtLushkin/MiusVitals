// v20 RVA 02eab0..02eb93  (227 bytes)  replicated_singleton
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned int field_0;
    char paddin4[44];
    unsigned int field_30;
    char field_34;
    char field_35;
} struct_0;


long long replicated_singleton(unsigned long a0)
{
    char v0;  // [bp-0x48]
    unsigned long v1;  // [bp-0x28]
    char v2;  // [bp-0x20]
    unsigned long long v3;  // [bp-0x18]
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // ecx

    v3 = g_5a380 ^ &v0;
    v1 = a0;
    v5 = g_5a2a8;
    if (g_5a2a8 < 0)
    {
        if (!GetEnvironmentVariableA("MP_STATE_TRANSLATE", &v2, 8))
        {
            v6 = 0;
        }
        else
        {
            v6 = 0;
            *((char *)&v6) = atoi(&v2) != 0;
        }
        g_5a2a8 = v6;
        v5 = v6;
    }
    if (!v5)
        goto LABEL_0x2eb7e;
    else
        goto LABEL_0x2eb10;
}


// v20 RVA 03d1d4..03d27f  (171 bytes)  input_action_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[660];
    unsigned int field_294;
} struct_1;


long long input_action_hook(struct_1 *a0, unsigned long long a1, unsigned long long a2)
{
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x10]
    unsigned long long v3;  // rax, Other Possible Types: unsigned long

    if (!g_5dbd4 && (char)sub_3d19c())
    {
        (unsigned int)v3 = sub_1e510();
        if ((char)v3)
            return v3;
        (unsigned int)v3 = sub_1c334();
        if ((unsigned int)v3 != 1)
            return v3;
        v3 = g_5db98(a0, a1, a2);
        if (*((int *)(a0 + 660)) != *((int *)(a0 + 660)))
        {
            *((char **)&v1) = 0;
            *((char **)&v0) = 0;
            v3 = operation_exists@Joiner@Joiner(2, a1, (char)a2);
            return v3;
        }
        return v3;
    }
    v3 = g_5db98(a0, a1, a2);
    return v3;
}


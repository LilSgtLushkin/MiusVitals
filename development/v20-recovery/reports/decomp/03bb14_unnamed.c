// v20 RVA 03bb14..03bba0  (140 bytes)  input_action_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long input_action_hook(unsigned long long a0, char *a1, unsigned long long a2, unsigned long a3, unsigned int a4, unsigned int a5)
{
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x20]
    unsigned long long v3;  // rax, Other Possible Types: unsigned long

    if (a1 < g_5d7b8 - g_5d7b0 && a1[g_5d7b0])
    {
        (char)v3 = is_object();
        if ((char)v3)
        {
            g_5daa0 = &(1)[g_5daa0];
            return v3;
        }
    }
    v1 = a5;
    v0 = a4;
    v3 = g_5da90(a0, a1, a2);
    return v3;
}


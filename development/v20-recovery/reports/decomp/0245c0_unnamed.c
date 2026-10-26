// v20 RVA 0245c0..02470f  (335 bytes)  sub_245c0
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[60];
    unsigned int field_3c;
} struct_0;


int sub_245c0()
{
    unsigned int v0;  // [bp+0x8]
    struct_0 *v1;  // rax
    struct_0 *v2;  // rdi
    unsigned long long v3;  // rax
    unsigned long long v4;  // rax
    unsigned long long v5;  // rax
    unsigned long long *v6;  // rsi, Other Possible Types: unsigned long
    unsigned long long *v7;  // r14, Other Possible Types: unsigned long
    unsigned int v8[5];  // rbx, Other Possible Types: unsigned long

    if (!g_5ccc8 && (char)sub_2a044())
        g_5ccc8 = install_detour(GetModuleHandleA("adv_opgame.codelib") + 100860, 15, local_tables);
    if (g_5cbe0)
        return;
    v1 = GetModuleHandleA("base_shell.codelib");
    v2 = v1;
    if (!v1)
        return;
    v3 = *((int *)(&v1->paddin0[*((int *)(v1 + 60))] + 144));
    if (!(unsigned int)v3)
        return;
    for (v8 = &v2->paddin0[v3]; *((int *)&(12)[v8]); v8 = &(20)[v8])
    {
        (unsigned int)v4 = _stricmp(&v2->paddin0[*((int *)&(12)[v8])], "KERNEL32.dll");
        if (!(unsigned int)v4 && *((int *)v8) != (unsigned int)v4)
        {
            v7 = &v2->paddin0[*((int *)v8)];
            v6 = &v2->paddin0[*((int *)&(16)[v8])];
            while (true)
            {
                v5 = *((long long *)v7);
                if (!v5)
                    break;
                if (v5 >= 0 && !strcmp(v2 + 2 + v5, "CreateProcessA"))
                {
                    if (!VirtualProtect(v6, 8, 4, &v0))
                        return;
                    g_5cbe0 = *((long long *)v6);
                    *((void* *)v6) = launch@Joiner;
                    VirtualProtect(v6, 8, v0, &v0);
                    return;
                }
                v7 = &(8)[v7];
                v6 = &(8)[v6];
            }
        }
    }
    return;
}


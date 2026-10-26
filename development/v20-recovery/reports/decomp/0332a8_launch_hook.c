// v20 RVA 0332a8..033402  (346 bytes)  launch_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long launch_hook(unsigned long long a0, unsigned long long a1[10])
{
    char v0;  // [bp+0x0]
    unsigned long long v3;  // rax, Other Possible Types: unsigned long
    unsigned int v5;  // ecx
    char v6;  // bpl
    char v7;  // r14b
    unsigned long long v8;  // r8
    unsigned long v9;  // rcx
    unsigned long long v10;  // rcx

    if (!g_5d024 && GetCurrentThreadId() == g_5d020 && (char)readable(a1, 88))
    {
        g_5d008 = a0;
        if (!g_5a2ec || !(char)sub_19f18() || !sub_1c334())
        {
            v6 = 0;
            goto LABEL_33367;
        }
        else
        {
            v6 = 1;
            if (!g_5d018 || !(char)is_object(g_5d018))
            {
                v5 = 0;
                *((char *)&v5) = sub_1c334() == 1;
            }
            else
            {
                v5 = sub_1bacc(g_5d018);
            }
            if (v5)
            {
LABEL_33367:
                v7 = 1;
            }
            else
            {
                v7 = 0;
            }
        }
        v8 = *((long long *)&v0);
        v9 = (v7 ? "dropped" : "own");
        if (!v6)
            v10 = "free";
        v3 = explode_hook();
        if (!v7)
        {
            g_5d0a0 = &(1)[g_5d0a0];
            return v3;
        }
        v3 = g_5cfe8(a0, a1);
        if (!v6)
            return v3;
        g_5d098 = &(1)[g_5d098];
        if (!*((long long *)(a1 + 64)) && !*((long long *)(a1 + 72)))
            return v3;
        v3 = queue_shot(a1, g_5d018);
        return v3;
    }
    v3 = g_5cfe8(a0, a1);
    return v3;
}


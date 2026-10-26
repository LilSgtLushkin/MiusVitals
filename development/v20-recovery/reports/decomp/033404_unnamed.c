// v20 RVA 033404..033637  (563 bytes)  launch_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long launch_hook(unsigned long long a0, unsigned int a1[3], unsigned long long a2, unsigned long long a3, unsigned int a4, unsigned int a5)
{
    unsigned long v0;  // [bp-0x60], Other Possible Types: unsigned int
    unsigned long v1;  // [bp-0x58]
    unsigned long v2;  // [bp-0x50]
    unsigned long v4;  // rdi
    unsigned long long v5;  // rax, Other Possible Types: unsigned long

    g_5d010 = a0;
    if (!g_5d024 && GetCurrentThreadId() == g_5d020 && g_5a2ec && (char)sub_19f18() && sub_1c334() && g_5a2ec && (char)readable(a1, 12) && (char)readable(a3, 16))
    {
        v4 = a4;
        (unsigned int)v5 = sub_1bad4(v4);
        if ((char)v5)
        {
            v0 = a5;
            g_5cff8(a0, a1, a2, a3, v4);
            g_5d0b0 = &(1)[g_5d0b0];
            if (g_5d028)
            {
                v2 = *((int *)(a1 + 8));
                v1 = *((int *)(a1 + 4));
                v0 = a1[0];
                fprintf(g_5d028, "F %llu own side=%08x kind=%u pos=%.2f,%.2f,%.2f\n", sub_19f10(), v4);
                fflush(g_5d028);
            }
            v5 = queue_shot(a1, a2, a3, v4);
            return v5;
        }
        else
        {
            g_5d0b8 = &(1)[g_5d0b8];
            if (g_5d028)
            {
                v2 = *((int *)(a1 + 8));
                v1 = *((int *)(a1 + 4));
                v0 = a1[0];
                fprintf(g_5d028, "F %llu dropped side=%08x kind=%u pos=%.2f,%.2f,%.2f\n", sub_19f10(), v4);
                (unsigned int)v5 = fflush(g_5d028);
                return v5;
            }
            return v5;
        }
    }
    v0 = a5;
    v5 = g_5cff8(a0, a1, a2, a3, a4);
    return v5;
}


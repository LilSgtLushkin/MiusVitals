// v20 RVA 01083c..010963  (295 bytes)  isolate_input
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long isolate_input(unsigned long a0, unsigned long a1, unsigned long a2)
{
    unsigned int v0;  // [bp+0x8], Other Possible Types: char
    unsigned long v2;  // rax
    unsigned long v3;  // rdx
    unsigned long long v4;  // rcx
    unsigned long long v5;  // r8

    if ((unsigned int)a0)
    {
        if ((unsigned int)a1 == g_5bfb0)
        {
            *((unsigned int *)&g_5bfc0) = a2;
            return v2;
        }
        else if ((unsigned int)a1 == g_5bf38)
        {
            return v2;
        }
        else if ((unsigned int)a1 == g_5bfb4)
        {
            return v2;
        }
        else if ((unsigned int)a0 > 0x1000000)
        {
            return v2;
        }
        else
        {
            *((char **)&v0) = 0;
            assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(&g_5b7c8, a0, &v0);
            g_5bf38 = a1;
            *((char **)&v0) = 0;
            assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(&g_5b6b8, &(999)[a0] / 1000, &v0);
            *((char **)&v0) = 0;
            assign@?$vector@KV?$allocator@K@std(&g_5b710, g_5b6c0 - g_5b6b8, &v0);
            *((char **)&g_5bf3c) = 0;
            v3 = (g_5bbb0 != 0 ? "the other player" : &g_5bbb0);
            v4 = "lobby: %s is sending the force he picked (%u bytes)";
            v5 = a0;
            v2 = mp_log();
            return v2;
        }
    }
    else
    {
        if (g_5b618)
            return v2;
        if (!g_5bfb0 && g_5b7c8 == g_5b7d0)
            return g_5b7d0;
        v2 = explode_hook("he has none any more");
        return v2;
    }
}


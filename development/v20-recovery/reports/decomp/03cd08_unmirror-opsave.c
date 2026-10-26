// v20 RVA 03cd08..03cea4  (412 bytes)  unmirror@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long unmirror@opsave()
{
    int v0;  // [bp-0x38], Other Possible Types: unsigned long
    unsigned long v1;  // [bp-0x30]
    void* v2;  // [bp-0x28], Other Possible Types: unsigned long
    int v3;  // [bp-0x20], Other Possible Types: unsigned long
    unsigned long v4;  // [bp-0x18]
    void* v5;  // [bp-0x10], Other Possible Types: unsigned long
    void* v8;  // rbx, Other Possible Types: unsigned long
    unsigned long long v9;  // rdi
    void* v10;  // rbx, Other Possible Types: unsigned long
    unsigned long long v11;  // rdi

    if (!g_5db11)
        trench_install();
    if (g_5db10)
    {
        *((char **)&v3) = 0;
        v5 = 0;
        EnterCriticalSection(&g_5db18.DebugInfo);
        v8 = g_5dae0;
        v3 = g_5dae0;
        g_5dae0 = 0;
        v9 = g_5dae8;
        v4 = g_5dae8;
        g_5dae8 = 0;
        v5 = g_5daf0;
        g_5daf0 = 0;
        LeaveCriticalSection(&g_5db18.DebugInfo);
        if (g_5a2f2 && sub_1c334() && (char)sub_19f18() && (char)sub_1d2d4() && (char)sub_1e518())
        {
            for (; v8 != v9; v8 = &(220)[v8])
            {
                shots_queue(v8);
            }
        }
        if (!(g_5dac8 != g_5dad0 && (char)sub_39a38()))
            return _Tidy@?$vector@UShotRec@?$vector@UShotRec(&v3);
        *((char **)&v0) = 0;
        v2 = 0;
        v10 = g_5dac8;
        v0 = g_5dac8;
        g_5dac8 = 0;
        v11 = g_5dad0;
        v1 = g_5dad0;
        g_5dad0 = 0;
        v2 = g_5dad8;
        for (g_5dad8 = 0; v10 != v11; v10 = &(220)[v10])
        {
            shots_sim_step(v10);
        }
        _Tidy@?$vector@UShotRec@?$vector@UShotRec(&v0);
    }
    else
    {
        if (g_5dac8 == g_5dad0)
            return g_5dac8;
        g_5dad0 = g_5dac8;
    }
}


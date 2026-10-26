// v20 RVA 01e2c4..01e352  (142 bytes)  sub_1e2c4
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_1e2c4()
{
    unsigned long v0;  // [bp+0x8]
    unsigned int v1;  // [bp+0xc]
    unsigned int *v2;  // rcx, Other Possible Types: unsigned long
    unsigned long long v3;  // rax, Other Possible Types: unsigned long

    v2 = g_5a160;
    if (g_5a160 != -1)
    {
        if (g_5b61a)
        {
            strncpy(&v0, "GTMP", 4);
            *((char **)&v1) = 7;
            v0 = v0;
            input_action_hook(&g_5b8f0, &v0, 8);
            v2 = g_5a160;
        }
        (unsigned int)v3 = closesocket(v2);
        g_5a160 = -1;
    }
    if (g_5b8e0 == 1)
        v3 = snapshot_operation_saves(0, 0);
    *((char **)&g_5b61c) = 0;
    *((char **)&g_5b61b) = 0;
    *((char **)&g_5b61a) = 0;
    return v3;
}


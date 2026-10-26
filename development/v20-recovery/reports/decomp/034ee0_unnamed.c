// v20 RVA 034ee0..034f81  (161 bytes)  sub_34ee0
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_34ee0()
{
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x10]
    unsigned long v2;  // [bp-0x8]
    unsigned long v3;  // [bp+0x8]
    unsigned long v4;  // [bp+0x10]
    unsigned long v5;  // [bp+0x18]
    unsigned long v6;  // rbx
    unsigned long long v7;  // rcx, Other Possible Types: unsigned long
    unsigned long long v8;  // rax
    unsigned int v10;  // ecx
    unsigned long v11;  // rsi
    unsigned long v12;  // rdi
    unsigned long v14;  // rsi
    unsigned int v15;  // ebp
    void* v16;  // rdi, Other Possible Types: unsigned long
    void* v17;  // rax, Other Possible Types: unsigned long, unsigned long long

    v2 = v6;
    if (g_5cfa0 != g_5cfa8)
        g_5cfa8 = g_5cfa0;
    v7 = g_5cfd8;
    if (g_5cfd8)
    {
        do
        {
            v8 = v7;
            v7 -= 1;
            if (v8 == 1)
                g_5cfd0 = 0;
        } while (v7);
        g_5cfd8 = v7;
    }
    if (g_5cfc0)
        _Reset_map@?$deque@UPending(&g_5cfb8);
    *((char **)&g_5d030) = 0;
    v10 = g_5d034 * 2654435761 ^ GetTickCount() ^ GetCurrentProcessId() * &g_10000 | 1;
    g_5d034 = v10;
    trench_resend(g_5d034);
    v17 = sub_39cc0(g_5d034);
    v3 = v2;
    *((int *)&v4) = rbp<8>;
    v5 = v11;
    v2 = v12;
    *((char **)&g_5dbd7) = 0;
    if (!g_5dbd3)
        return v17;
    v14 = sub_1e510();
    v15 = sub_1c334();
    v16 = 0;
    do
    {
        (unsigned int)v17 = xlate_desc(g_5db88, v16);
        if ((char)v17)
        {
            v17 = 0;
            *((char *)&v17) = (unsigned int)v16 == 0;
            v17 = 0;
            *((char *)&v17) = v15 == 1;
            if ((char)v14)
                v17 = v17;
            if ((unsigned int)v17)
            {
                *((char **)&v1) = 0;
                *((char **)&v0) = 0;
                v17 = operation_exists@Joiner@Joiner(1);
            }
        }
    } while ((v16 = (unsigned long long)((unsigned int)v16 + 1), (unsigned int)v16 < 2));
    if (g_5dbd6)
    {
        *((char **)&v1) = 0;
        *((char **)&v0) = 0;
        v17 = operation_exists@Joiner@Joiner(&(2)[((unsigned int)v14 ^ 1) * 2]);
    }
    if (v15 == 1)
    {
        if (!(char)v14 && (*((char *)(g_5db88 + 660)) & 2))
        {
            *((char **)&v1) = 0;
            *((char **)&v0) = 0;
            v17 = operation_exists@Joiner@Joiner(2);
        }
        if (g_5dbd8)
            v17 = send_table(g_5db88);
    }
}


// v20 RVA 03e2b0..03e37e  (206 bytes)  ammo_check
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long ammo_check(unsigned long long a0)
{
    unsigned long long v1;  // rax, Other Possible Types: unsigned long
    unsigned long long v2;  // rbx, Other Possible Types: unsigned long
    unsigned long long v3;  // rsi, Other Possible Types: unsigned long
    unsigned long long v4;  // rdi, Other Possible Types: unsigned long

    v1 = g_5dbb8();
    if (!g_5dbd3)
        return v1;
    if (a0 != g_5db88)
        return v1;
    (unsigned int)v1 = squad_record(a0);
    v2 = v1;
    if (!v1)
        return v1;
    if (!g_5dbd1)
    {
        if (!g_5dbd0)
            return v1;
        v1 = sub_3d19c();
        if (!(char)v1)
            return v1;
        (unsigned int)v1 = sub_1e510();
        if ((char)v1)
        {
            v1 = memcpy(&(64)[v2], &g_5dbc0, 4);
            return v1;
        }
        return v1;
    }
    v4 = &g_5dbf0;
    v3 = 2;
    while (true)
    {
        memcpy(v2, v4 - 16, 16);
        v1 = memcpy(&(16)[v2], v4, 16);
        v4 = &(32)[v4];
        v2 = &(48)[v2];
        v3 -= 1;
    }
}


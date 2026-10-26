// v20 RVA 03dc14..03dd88  (372 bytes)  $_Reallocate_grow_by@V<lambda_65e615be2a453ca0576c979606f46740>
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_3dc14()
{
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x10]
    unsigned long v4;  // rsi
    unsigned int v5;  // ebp
    void* v6;  // rax, Other Possible Types: unsigned long long, unsigned long
    void* v7;  // rdi, Other Possible Types: unsigned long

    *((char **)&g_5dbd7) = 0;
    if (!g_5dbd3)
        return v6;
    v4 = sub_1e510();
    v5 = sub_1c334();
    v7 = 0;
    do
    {
        (unsigned int)v6 = xlate_desc(g_5db88, v7);
        if ((char)v6)
        {
            v6 = 0;
            *((char *)&v6) = (unsigned int)v7 == 0;
            v6 = 0;
            *((char *)&v6) = v5 == 1;
            if ((char)v4)
                v6 = v6;
            if ((unsigned int)v6)
            {
                *((char **)&v1) = 0;
                *((char **)&v0) = 0;
                v6 = operation_exists@Joiner@Joiner(1);
            }
        }
    } while ((v7 = (unsigned long long)((unsigned int)v7 + 1), (unsigned int)v7 < 2));
    if (g_5dbd6)
    {
        *((char **)&v1) = 0;
        *((char **)&v0) = 0;
        v6 = operation_exists@Joiner@Joiner(&(2)[((unsigned int)v4 ^ 1) * 2]);
    }
    if (v5 != 1)
        return v6;
    if (!(char)v4 && (*((char *)(g_5db88 + 660)) & 2))
    {
        *((char **)&v1) = 0;
        *((char **)&v0) = 0;
        v6 = operation_exists@Joiner@Joiner(2);
    }
    if (!g_5dbd8)
        return v6;
    v6 = send_table(g_5db88);
}


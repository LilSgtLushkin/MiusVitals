// v20 RVA 03ede4..03efa5  (449 bytes)  sub_3ede4
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long sub_3ede4()
{
    char v0;  // [bp-0x168]
    void* v1;  // [bp-0x148]
    int v2;  // [bp-0x140]
    void* v3;  // [bp-0x130]
    char v4;  // [bp-0x128]
    unsigned long long v5;  // [bp-0x18]
    unsigned long long v7;  // rax
    unsigned long long v8;  // r8
    struct_0 *v9;  // rcx
    unsigned int v11;  // ebp
    unsigned long long v12;  // rdx
    unsigned long long v13;  // rsi
    void* v14;  // rbx, Other Possible Types: unsigned long
    unsigned long long v15;  // rcx

    v5 = g_5a380 ^ &v0;
    if (g_5dee0 != 1)
    {
        return security_cookie_check(v5 ^ &v0);
    }
    else if (g_5de94)
    {
        (unsigned int)v7 = GetTickCount();
        v8 = g_5df68;
        if (g_5df68)
        {
            if ((unsigned int)v7 - g_5df70 < 2000 && g_5df70)
                return security_cookie_check(v5 ^ &v0);
            v9 = &v4;
            v12 = 260;
            g_5df70 = ((unsigned int)v7 != 0 ? 1 : (unsigned int)v7);
            hook_srand();
            v1 = 0;
            if (fopen_s(&v1, &v4, "w"))
                return security_cookie_check(v5 ^ &v0);
            v15 = v1;
            if (!v15)
                return security_cookie_check(v5 ^ &v0);
            fprintf(v15, "%s\n%s\n", &g_5dee8, &g_5dea0);
            fclose(v1);
            return security_cookie_check(v5 ^ &v0);
        }
        else if ((unsigned int)v7 - g_5df18 >= 3000)
        {
            g_5df18 = v7;
            v14 = 0;
            v11 = g_5de18(g_5de68, 4);
            if (v11 <= 0)
                return security_cookie_check(v5 ^ &v0);
            while (true)
            {
                if ((unsigned int)v14 < 2000)
                {
                    v13 = g_5de20(g_5de68, v14, 4);
                    v3 = 0;
                    *((char **)&v2) = 0;
                    if (v13 && (char)g_5de30(g_5de68, v13, &v2) && ((int)v2 & 16777215) == g_5de90)
                        g_5de50(g_5de78, v13);
                    v14 = &(1)[v14];
                    if ((unsigned int)v14 >= v11)
                        return security_cookie_check(v5 ^ &v0);
                }
                else
                {
                    return security_cookie_check(v5 ^ &v0);
                }
            }
        }
        else
        {
            return security_cookie_check(v5 ^ &v0);
        }
    }
    else
    {
        return security_cookie_check(v5 ^ &v0);
    }
}


// v20 RVA 0270ac..0273a8  (764 bytes)  shots_install
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long shots_install()
{
    char v0;  // [bp-0x188]
    int tmp_8;  // tmp #8
    unsigned long v1;  // [bp-0x168], Other Possible Types: unsigned int
    unsigned long v2;  // [bp-0x160], Other Possible Types: unsigned int
    unsigned long v3;  // [bp-0x158], Other Possible Types: char
    unsigned int v4;  // [bp-0x148]
    char v5;  // [bp-0x138]
    unsigned long long v6;  // [bp-0x48]
    unsigned long v8;  // r8
    char v9;  // r15b
    char v10;  // r14b
    unsigned long long v11;  // r8, Other Possible Types: unsigned long
    unsigned long long v12;  // rdi, Other Possible Types: unsigned long
    unsigned long long v13;  // rax, Other Possible Types: unsigned long
    char v14;  // cl
    unsigned long long v15;  // rax, Other Possible Types: unsigned long
    unsigned long v16;  // rsi
    unsigned int v17;  // eax

    v6 = g_5a380 ^ &v0;
    v8 = g_5cbf4;
    if (!g_5c450 || g_5cbf4 > g_5c450)
        v9 = 1;
    else
        v9 = 0;
    v14 = g_5c46f;
    v13 = g_5cbf8;
    if (g_5c44c && g_5cbf8 && g_5cbf8 != g_5c44c || g_5c46f)
        v10 = 1;
    else
        v10 = 0;
    if (!g_5c450)
    {
        v11 = "MULTIPLAYER - the host's battle has begun: your own force can only go into the next one";
        if (!g_5c2c8)
            v11 = "MULTIPLAYER - the host has not chosen his force yet: its cost will be your limit";
        strcpy_s(&v5, 240, v11);
    }
    else
    {
        if (!g_5c44c || !g_5cbf8 || g_5cbf8 == g_5c44c)
        {
            v12 = &g_45ce6;
        }
        else
        {
            v4 = g_5c44c;
            if (g_5c44c == 5391687)
            {
                v12 = "Germans";
            }
            else if (g_5c44c == 1381192533)
            {
                v12 = "Soviets";
            }
            else
            {
                v12 = &g_5b5c8;
                memcpy(&g_5b5c8, &v4, 4);
                v8 = g_5cbf4;
                v13 = g_5cbf8;
                v14 = g_5c46f;
                *((char **)&g_5b5cc) = 0;
            }
        }
        if (!g_5c44c || !(unsigned int)v13 || (tmp_8 = v13, v15 = " - set your own side to the ", (int)tmp_8 == g_5c44c))
            v15 = &g_45ce6;
        v16 = (v14 ? &g_45ce6 : " - set YEAR AND SEASON back to the host's");
        v3 = v16;
        v2 = v12;
        v1 = v15;
        $sprintf_s@$0BAE$sprintf_s@$0BAE(&v5, "MULTIPLAYER - your own force: cost %u of %u allowed (what the host's force costs)%s%s%s", v8, g_5c450);
    }
    v1 = 3491238928;
    quad();
    if (v9 || (v17 = -5705568, v10))
        v17 = -30096;
    *((char **)&v3) = 0;
    *((char **)&v2) = 1;
    v1 = v17;
    text(&v5);
    if (g_5cc00)
    {
        *((char **)&v3) = 0;
        *((char **)&v2) = 1;
        v1 = -30096;
        text(&g_5cc00);
        return security_cookie_check(v6 ^ &v0);
    }
    return security_cookie_check(v6 ^ &v0);
}


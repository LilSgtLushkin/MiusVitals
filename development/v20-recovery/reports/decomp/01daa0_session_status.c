// v20 RVA 01daa0..01e2c4  (2084 bytes)  session_status
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long session_status()
{
    char v0;  // [bp-0x288]
    struct_0 *v1;  // [bp-0x260]
    unsigned int v2;  // [bp-0x258]
    char v3;  // [bp-0x248]
    char v4;  // [bp-0x1a8]
    char v5;  // [bp-0x198]
    char v6;  // [bp-0x188]
    char v7;  // [bp-0xb8]
    unsigned long long v8;  // [bp-0x28]
    void* v10;  // rbx, Other Possible Types: unsigned long
    unsigned long v11;  // rdi
    unsigned long long v12;  // rsi, Other Possible Types: unsigned long
    unsigned long long v13;  // r8, Other Possible Types: unsigned long
    unsigned long v14;  // r9
    struct_0 *v15;  // r8
    unsigned long long v16;  // rsi, Other Possible Types: unsigned long
    struct_0 *v17;  // r8
    unsigned long long v18;  // rcx, Other Possible Types: unsigned long
    unsigned int v19;  // rax, Other Possible Types: unsigned long
    char v21;  // r14b
    char v22;  // r15b
    unsigned long v25;  // rdx, Other Possible Types: unsigned long long
    unsigned long long v29;  // r8, Other Possible Types: unsigned long
    unsigned long v31;  // rbx
    unsigned long long v32;  // rdx
    unsigned long v33;  // rsi
    struct_0 *v38;  // r9, Other Possible Types: unsigned long
    struct_0 *v39;  // r8, Other Possible Types: unsigned long
    unsigned long long v40;  // rdx, Other Possible Types: unsigned long

    v8 = g_5a380 ^ &v0;
    v10 = 0;
    *((char **)&g_5ba70) = 0;
    if (g_5b72a)
    {
        if (!g_5b61b)
        {
            $sprintf_s@$0MI(&v6);
        }
        else if (g_5be30)
        {
            $sprintf_s@$0MI(&v6);
        }
        else
        {
            if (!g_5b83a)
            {
                if (g_5b729)
                {
                    goto LABEL_1db8f;
                }
                else if (g_5b72f)
                {
                    $sprintf_s@$0MI(&v6);
                }
                else if (!g_5b838)
                {
LABEL_1db8f:
                }
                else
                {
                    $sprintf_s@$0MI(&v6);
                    goto LABEL_1e281;
                }
            }
            $sprintf_s@$0MI();
            v38 = &v6;
            v39 = "Player";
            v40 = "MULTIPLAYER  -  %s on the operational map  -  %s";
        }
LABEL_1e281:
        $sprintf_s@$0BEA(&g_5ba70, v40, v39, v38);
        return security_cookie_check(v8 ^ &v0);
    }
    if (!g_5b8e0)
        return security_cookie_check(v8 ^ &v0);
    if (g_5b8e0 == 2 && !g_5b618)
        return security_cookie_check(v8 ^ &v0);
    v11 = (g_5bbb0 != 0 ? "the other player" : &g_5bbb0);
    if (!g_5b618)
        $sprintf_s@$0IM(&v7);
    if (g_5b61b || g_5bbb0)
    {
        v2 = g_5be38;
        v16 = "Germans";
        if (g_5be38 == 5391687)
        {
            v13 = "Germans";
        }
        else if (g_5be38 == 1381192533)
        {
            v13 = "Soviets";
        }
        else
        {
            memcpy(&g_5b5c8, &v2, 4);
            v13 = &g_5b5c8;
            *((char **)&g_5b5cc) = 0;
        }
        strcpy_s(&v4, 16, v13);
        v2 = g_5be3c;
        if (g_5be3c != 5391687)
        {
            if (g_5be3c == 1381192533)
            {
                v16 = "Soviets";
            }
            else
            {
                memcpy(&g_5b5c8, &v2, 4);
                *((char **)&g_5b5cc) = 0;
                v16 = &g_5b5c8;
            }
        }
        strcpy_s(&v5, 16, v16);
        if (g_5b619 || g_5b8e0 == 1)
        {
            v1 = &v5;
            v17 = "Player";
            $sprintf_s@$0IM(&v7, "%s (%s)  vs  %s (%s)", "Player", &v4, v11);
        }
        else
        {
            $sprintf_s@$0IM(&v7, "%s watching %s (%s)", "Player", v11);
        }
    }
    else
    {
        v2 = g_5be38;
        if (g_5be38 == 5391687)
        {
            v12 = "Germans";
        }
        else if (g_5be38 == 1381192533)
        {
            v12 = "Soviets";
        }
        else
        {
            memcpy(&g_5b5c8, &v2, 4);
            *((char **)&g_5b5cc) = 0;
            v12 = &g_5b5c8;
        }
        v14 = v12;
        v15 = "Player";
        $sprintf_s@$0IM(&v7);
    }
    v18 = g_5beb4;
    v19 = g_5b8e0;
    v21 = g_5beb4 < g_5becc;
    v22 = g_5beb4 < g_5bed0;
    if (g_5b8e0 != 1)
    {
        if (g_5b8e0 == 2 && g_5bec8)
        {
            if ((char)sub_3dbc4())
            {
                v19 = g_5b8e0;
                v18 = g_5beb4;
            }
            else
            {
LABEL_1de38:
                goto LABEL_1dfd2;
            }
        }
    }
    else
    {
        if (!(!g_5b61c) || !(g_5a160 != -1))
            $sprintf_s@$0JG(&v3, "UDP port %u is busy, retrying", g_5a168, v14);
    }
    if (!g_5b61b)
    {
        if ((unsigned int)v19 == 1)
        {
            $sprintf_s@$0JG(&v3, "waiting for a player at %s:%u", "?", g_5a168);
LABEL_1e26e:
            goto LABEL_1e281;
        }
        else
        {
LABEL_1de82:
            $sprintf_s@$0JG(&v3, "connecting to the host", "Player", v14);
        }
    }
    if (!g_5b618)
    {
        if (!g_5b72c && !g_5bd40 && !g_5b72e)
        {
            if (!g_5b729)
            {
                if (g_5bd50)
                {
                    $sprintf_s@$0JG(&v3, "%s", &g_5bd50, v14);
LABEL_1e26e:
                    goto LABEL_1e281;
                }
                else
                {
LABEL_1df50:
                    $sprintf_s@$0JG(&v3, "%s has joined and is %s  (F9: lobby)", v11, (g_5b61e != 0 ? "not ready yet" : "ready"));
                }
            }
            else
            {
                if (!g_5b839)
                {
                    goto LABEL_1dfd2;
                }
                else if (g_5b72f)
                {
                    goto LABEL_1dfd2;
                }
                else if (!g_5b839)
                {
                    goto LABEL_1de38;
                }
                else if (g_5b838)
                {
                    goto LABEL_1dfd2;
                }
                else if (!g_5b839)
                {
                    goto LABEL_1de38;
                }
                else
                {
                    goto LABEL_1dfd2;
                }
            }
        }
        goto LABEL_1dfd2;
    }
    if ((unsigned int)v18 >= 2)
    {
        if ((unsigned int)v18 == 2)
        {
            if (g_5be8c)
            {
                if (GetTickCount() - g_5be8c > 1500)
                    goto LABEL_1dfd2;
                if (g_5beb4 == 2)
                {
                    v19 = g_5b8e0;
                    goto LABEL_1e023;
                }
            }
            else
            {
LABEL_1e023:
                if ((unsigned int)v19 == 2 && (((char)(CmpF(g_5a214, 0x3fc00000) & 69) | (char)((CmpF(g_5a214, 0x3fc00000) & 69) >> 6)) & 1) != 1)
                    goto LABEL_1dfd2;
                if (sub_3db10())
                {
                    sub_3d8a8();
                    v31 = (!(g_5a214 >> 31 & 1) ? 0 : g_5a214);
                    v32 = 2290649225 * (unsigned int)v31 >> 32;
                    v33 = (v31 + (2290649225 * (unsigned int)v31 >> 32) >> 5) + (v31 + (2290649225 * (unsigned int)v31 >> 32) >> 5 >> 31);
                    if (((char)sub_3db10() & 1))
                    {
                        $sprintf_s@$0JG(&v3, "your force is withdrawing - the battle ends in %d:%02d", v33, v31 - (unsigned int)v33 * 60);
LABEL_1e26e:
                        goto LABEL_1e281;
                    }
                    else
                    {
                        $sprintf_s@$0JG(&v3, "%s is withdrawing - the battle ends in %d:%02d", v11, v33);
LABEL_1e26e:
                        goto LABEL_1e281;
                    }
                }
                if (g_5beb4 != 2)
                    goto LABEL_1dfd7;
                if ((char)(sub_3ddb8() & 3) == 3)
                {
                    sub_3d8a8();
                    if ((((char)(CmpF(g_5a214, 0) & 69) | (char)((CmpF(g_5a214, 0) & 69) >> 6)) & 1) != 1)
                    {
                        sub_3d8a8();
                        v10 = &(1)[g_5a214];
                    }
                    $sprintf_s@$0JG(&v3, "ceasefire agreed - the battle ends in %d s", v10, v14);
LABEL_1e26e:
                    goto LABEL_1e281;
                }
                if (g_5beb4 != 2)
                    goto LABEL_1dfd7;
                if (((char)sub_3ddb8() & 2))
                    goto LABEL_1dfd2;
                if (g_5beb4 != 2)
                    goto LABEL_1dfd7;
                if (((char)sub_3ddb8() & 1))
                    goto LABEL_1dfd2;
                if (g_5beb4 != 2)
                    goto LABEL_1dfd7;
                if (g_5b61b)
                {
                    if (!g_5b83f)
                    {
LABEL_1e184:
                        if (g_5b898)
                            goto LABEL_1dfd2;
                        if (((char)((CmpF(g_5a218, 0x3f800000) & 69) >> 2) & 1) || (CmpF(g_5a218, 0x3f800000) & 64))
                        {
                            sub_d590();
                            if (!(!((char)((CmpF(g_5a214, g_5a218) & 69) >> 2) & 1)) || !(!(CmpF(g_5a214, g_5a218) & 64)))
                            {
                                $sprintf_s@$0JG(&v3, "you have asked for speed x%.1f - the battle runs at x%.1f until %s asks for it too", g_5a218, g_5a214);
LABEL_1e26e:
                                goto LABEL_1e281;
                            }
                        }
                        if (((char)((CmpF(g_5a21c, 0x3f800000) & 69) >> 2) & 1) || (CmpF(g_5a21c, 0x3f800000) & 64))
                        {
                            sub_d590();
                            if (!(!((char)((CmpF(g_5a214, g_5a21c) & 69) >> 2) & 1)) || !(!(CmpF(g_5a214, g_5a21c) & 64)))
                            {
                                $sprintf_s@$0JG(&v3, "%s asks for speed x%.1f - set your speed to agree", v11, g_5a21c);
LABEL_1e26e:
                                goto LABEL_1e281;
                            }
                        }
                        sub_d590();
                        if (((char)((CmpF(g_5a214, 0x3f800000) & 69) >> 2) & 1) || (CmpF(g_5a214, 0x3f800000) & 64))
                        {
                            $sprintf_s@$0JG(&v3, "speed x%.1f, agreed by both players", g_5a214, v14);
                            goto LABEL_1e26e;
                        }
                    }
                    else
                    {
                        if (!g_5b898)
                        {
                            if (!g_5b83f)
                                goto LABEL_1e184;
                            v25 = "you have asked for a pause - it starts when %s pauses too";
                            v29 = v11;
                            goto LABEL_1dfd2;
                        }
                        else
                        {
                            $sprintf_s@$0JG(&v3, "paused by both players - it goes on when either of you un-pauses", "Player", v14);
LABEL_1e26e:
                            v38 = &v3;
                            v39 = &v7;
                            v40 = "MULTIPLAYER  -  %s  -  %s";
                            goto LABEL_1e281;
                        }
LABEL_1e192:
                        $sprintf_s@$0JG(&v3, v25, v29, v14);
                    }
                }
                goto LABEL_1de82;
            }
        }
        goto LABEL_1de82;
    }
    else
    {
        if (g_5bed8)
        {
            if (v21 && !v22)
                goto LABEL_1df50;
            if (!g_5bed8)
            {
LABEL_1dfd7:
                goto LABEL_1dfd7;
            }
            if (v22 && !v21)
            {
                $sprintf_s@$0JG(&v3, "%s has finished %s, press Start when ready", v11, (g_5beb4 == 0 ? "the initial orders" : "deployment"));
LABEL_1e26e:
                goto LABEL_1e281;
            }
            if (g_5bed8 && !v21)
            {
                v25 = "%s ends when both players press Start";
                v29 = ((unsigned int)v18 == 0 ? "initial orders" : "deployment");
LABEL_1dfd2:
                goto LABEL_1e192;
            }
        }
LABEL_1dfd7:
        goto LABEL_1de82;
    }
}


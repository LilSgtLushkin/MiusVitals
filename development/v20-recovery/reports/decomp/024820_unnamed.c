// v20 RVA 024820..024be4  (964 bytes)  log_launch
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[8];
    unsigned int field_8;
} struct_0;


long long log_launch()
{
    char v0;  // [bp-0x1e8]
    char v1;  // [bp-0x1b8], Other Possible Types: unsigned long
    char v2;  // [bp-0x1a8]
    char v3;  // [bp-0x198]
    char v4;  // [bp-0x188]
    char v5;  // [bp-0x128]
    unsigned long long v6;  // [bp-0x88]
    unsigned long long v8;  // rax
    char v9;  // bl
    int v11;  // xmm6
    int v12;  // xmm10
    int v13;  // xmm11
    int v14;  // xmm6
    int v16;  // xmm11
    int v17;  // xmm9
    unsigned int v18;  // ymm1
    int v19;  // xmm8
    int v20;  // xmm7
    char v21;  // dil
    unsigned int v22;  // cc_dep1
    unsigned int v23;  // ymm12
    unsigned int v24;  // ecx
    struct_0 *v25;  // rax

    v6 = g_5a380 ^ &v0;
    if (g_5c148 && (char)1?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBU?$pair@V?$basic_string@DU?$char_traits@D@std?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBU?$pair@V?$basic_string@DU?$char_traits@D@std())
    {
        (unsigned int)v8 = sub_3ddb8();
        *((char **)&g_5c46d) = 0;
        v9 = v8;
        if (!(unsigned int)v8)
            return security_cookie_check(v6 ^ &v0);
        strncpy(&v3, "the other player", 16);
        session_peer(&v2, 24, 0);
        v21 = 1;
        if ((char)((unsigned int)v8 & 3) == 3)
        {
            strcpy_s(&v4, 96, "CEASEFIRE AGREED");
            $sprintf_s@$0KA(&v5, "The battle ends a few seconds after the menu is closed. Press again to take your word back.");
        }
        else
        {
            if ((v9 & 1))
            {
                strcpy_s(&v4, 96, "CEASEFIRE PROPOSED");
            }
            else if (!(v9 & 2))
            {
                strcpy_s(&v4, 96, "CEASEFIRE");
            }
            else
            {
                strcpy_s(&v4, 96, "ACCEPT CEASEFIRE");
            }
            $sprintf_s@$0KA(&v5);
        }
        v11 = 0;
        *((unsigned int *)&v11) = g_5cbd8;
        v23 = g_5caa0;
        v12 = v11;
        v13 = v11;
        v14 = v11 * 0x41000000;
        v16 = v13 * 0x42200000;
        v17 = (0 CONCAT g_5caac) * 1059783180;
        v18 = g_5caa4;
        if ((CmpF(g_5caa0, (unsigned int)v14) & 1) || (((char)(CmpF((unsigned int)v19, g_5caa0) & 69) | (char)((CmpF((unsigned int)v19, g_5caa0) & 69) >> 6)) & 1) == 1 || (CmpF(g_5caa4, (unsigned int)v17) & 1) || (v20 = v17 + v16, (((char)(unsigned long long)(CmpF((unsigned long long)(unsigned int)v20, (unsigned long long)g_5caa4) & 69) | (char)((unsigned long long)(CmpF((unsigned long long)(unsigned int)v20, (unsigned long long)g_5caa4) & 69) >> 6)) & 1) == 1))
        {
            v20 = v17 + v16;
            v21 = 0;
            v19 = v14 + v12 * 0x437a0000;
        }
        g_5c46d = v21;
        v22 = g_5a27c;
        if (g_5a27c < 0)
        {
            v23 = g_5caa0;
            v24 = 0;
            v18 = g_5caa4;
            *((char *)&v24) = GetEnvironmentVariableA("MP_TEST", 0, 0) != 0;
            g_5a27c = v24;
            v22 = v24;
        }
        if (v22 && g_5c46c)
        {
            v1 = (unsigned int)v20;
            mp_log("ui: click at %.0f, %.0f with the ESC menu open; CEASEFIRE is at %.0f..%.0f, %.0f..%.0f", v23, v18, (unsigned int)v14, (unsigned int)v19, (unsigned int)v17);
        }
        if ((char)button(&v4))
            *((char **)&g_5c8b8) = 14;
        if (!g_5c46d)
            return security_cookie_check(v6 ^ &v0);
        (unsigned int)v25 = text_tex(g_5cbd0, &v5, (0 CONCAT g_5cbd8) * (0 CONCAT 0x41700000) + (0 CONCAT 0x3f000000), 0);
        quad((v25 == 0 ? *((int *)(v25 + 8)) : 0));
        *((char **)&v1) = 0;
        text(&v5);
        return security_cookie_check(v6 ^ &v0);
    }
    *((char **)&g_5c46d) = 0;
    return security_cookie_check(v6 ^ &v0);
}


// v20 RVA 03d8d4..03db10  (572 bytes)  ui_start
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char paddin0[144];
    unsigned int field_90;
} struct_2;


long long ui_start(unsigned long long a0, struct struct_1 **a1)
{
    char v0;  // [bp-0x88]
    int v1;  // [bp-0x68], Other Possible Types: char, unsigned long
    int v2;  // [bp-0x60], Other Possible Types: unsigned int, unsigned long
    void* v3;  // [bp-0x58], Other Possible Types: unsigned long
    int v4;  // [bp-0x50]
    unsigned int v5;  // [bp-0x40]
    unsigned long long v6;  // [bp-0x38]
    unsigned long v7;  // [bp-0x8]
    char v8;  // [bp+0x0]
    unsigned int v11;  // ebx
    unsigned int v12;  // ymm6
    unsigned long long v13;  // rax
    unsigned long v14;  // rcx, Other Possible Types: unsigned int
    unsigned long long v15;  // rbx
    struct_2 *v16;  // rax
    void* v18;  // rbx, Other Possible Types: unsigned long
    unsigned long long v19;  // rdi

    v7 = &v8;
    v6 = g_5a380 ^ &v0;
    if (!g_5dbd2 && g_5a2f4 && sub_1c334() && !strcmp("opgame", "tacgame"))
        shots_install(a0);
    if (g_5dbd3)
    {
        v11 = GetTickCount();
        v12 = *((int *)(g_5db88 + 648));
        if (g_5dbb8 && (char)sub_3d19c() && (char)sub_1e510() && !g_5dbd8)
        {
            (unsigned int)v13 = sub_1bd5c();
            if ((unsigned int)v13 == 2)
            {
                if ((CmpF(v12, 0) & 1) || (v14 = v13 + 98, (((char)(unsigned long long)(CmpF((unsigned long long)(unsigned int)0x41000000, (unsigned long long)v12) & 69) | (char)((unsigned long long)(CmpF((unsigned long long)(unsigned int)0x41000000, (unsigned long long)v12) & 69) >> 6)) & 1) == 1))
                    v14 = 500;
                if (v11 - g_5dbdc >= (unsigned int)v14)
                {
                    g_5dbdc = v11;
                    v15 = squad_record(g_5db88);
                    if (v15)
                    {
                        strncpy(&v1, "GTMP\r", 5);
                        *((char **)&v2) = 0;
                        *((char **)&v4) = 0;
                        *((char **)&v5) = 0;
                        (unsigned int)v16 = squad_record(g_5db88);
                        v2 = (v16 == 0 ? *((int *)(v16 + 144)) : 0);
                        memcpy(&v2, v15, 16);
                        memcpy(&v4, &(16)[v15], 16);
                        a1(&v1, 44);
                    }
                }
            }
        }
        if (g_5db70 == g_5db78)
            return security_cookie_check(v6 ^ &v0);
        *((char **)&v1) = 0;
        v3 = 0;
        v18 = g_5db70;
        v1 = g_5db70;
        g_5db70 = 0;
        v19 = g_5db78;
        v2 = g_5db78;
        g_5db78 = 0;
        v3 = g_5db80;
        for (g_5db80 = 0; v18 != v19; v18 = &(220)[v18])
        {
            launch_hook(a0, v18);
        }
        _Tidy@?$vector@UShotRec@?$vector@UShotRec(&v1);
        return security_cookie_check(v6 ^ &v0);
    }
    else
    {
        if (g_5db70 == g_5db78)
            return security_cookie_check(v6 ^ &v0);
        g_5db78 = g_5db70;
    }
}


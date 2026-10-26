// v20 RVA 0283b0..0284ce  (286 bytes)  open_stock_screen
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[403440];
    unsigned long long field_627f0;
} struct_1;

typedef struct struct_0 {
    char field_0;
} struct_0;


long long open_stock_screen(unsigned long long a0, unsigned long a1)
{
    char v0;  // [bp-0x58]
    int tmp_26;  // tmp #26
    unsigned int v1;  // [bp-0x38]
    char v2;  // [bp-0x34]
    unsigned int v3;  // [bp-0x33]
    unsigned long long v4;  // [bp-0x28]
    struct_1 *v7;  // rbp
    unsigned long long v8;  // rdi
    unsigned long long v9;  // rbx, Other Possible Types: unsigned long
    unsigned long long v10;  // r8
    struct_0 *v11;  // rdx
    unsigned long v12;  // rax
    struct struct_2 **v13;  // rcx

    v4 = g_5a380 ^ &v0;
    v7 = GetModuleHandleA("adv_opgame.codelib");
    v8 = menu_controller(a0);
    if (!v7)
    {
        return security_cookie_check(v4 ^ &v0);
    }
    else if (!v8)
    {
        return security_cookie_check(v4 ^ &v0);
    }
    else
    {
        v9 = ((unsigned int)a1 == 3 ? "skirmish" : "missions");
        if ((unsigned int)a1 == 2)
            v9 = "campaign";
        if ((unsigned int)a1 != 1)
        {
            v1 = 3133158213;
            v10 = 9;
            v3 = 0xe9000000;
            tmp_26 = (unsigned int)a1 != 2;
            v11 = &v1;
            *((char **)&v2) = &(2)[tmp_26];
            v12 = ((unsigned int)a1 == 2 ? &g_2b250 : &g_2b210);
            v13 = &v7->paddin0[v12];
            if (memcmp())
            {
                mp_log("ui: unknown game build, open the %s screen yourself", v9);
                return security_cookie_check(v4 ^ &v0);
            }
            v13(v8);
        }
        else if ((char)sub_2a044())
        {
            (struct struct_1 *)&v7->field_627f0(v8, 0, 0, 0);
        }
        else
        {
            mp_log("ui: unknown game build, open SKIRMISH yourself");
            return security_cookie_check(v4 ^ &v0);
        }
        mp_log("ui: opened the stock %s screen", v9);
        return security_cookie_check(v4 ^ &v0);
    }
}


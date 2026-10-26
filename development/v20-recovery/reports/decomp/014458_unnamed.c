// v20 RVA 014458..014679  (545 bytes)  sub_14458
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;

typedef struct struct_4 {
    char paddin0[3560];
    unsigned int field_de8;
} struct_4;


long long sub_14458(struct struct_2 **a0)
{
    unsigned long v0;  // [bp-0x38]
    unsigned long v1;  // [bp-0x30]
    struct_0 *v2;  // [bp-0x28]
    void* v3;  // [bp-0x20]
    unsigned long v4;  // [bp-0x8]
    char v5;  // [bp+0x0]
    void* v7;  // [bp+0x10]
    struct struct_3 **v8;  // rax, Other Possible Types: unsigned long, unsigned long long
    struct struct_2 **v9;  // rbx
    char v10;  // rdx
    unsigned long long v11;  // rcx
    struct struct_1 **v12;  // r8
    struct struct_1 **v13;  // r8
    struct struct_1 **v14;  // r8

    v4 = &v5;
    v8 = g_5a228;
    v9 = a0;
    if (g_5a228 >= 0)
    {
        v10 = (g_5a228 ? "closed" : "opened");
        v11 = "test: the game's menu is %s";
        mp_log();
        v10 = g_5a228 != 0;
        v8 = end_turn_here(v9, v10);
        g_5a228 = -1;
    }
    if (g_5a22c > -100)
    {
        v7 = 0;
        v0 = "i_gamespec";
        v3 = 0;
        v1 = "gamespec";
        v2 = &v7;
        (unsigned int)v8 = v12("test: the game's menu is %s", &v0, *((long long *)(*(v9) + 72)));
        if (v8 && g_5b618 && g_5beb4 == 2)
        {
            mp_log("test: speed level %d", g_5a22c);
            v8 = *((long long *)(*(v8) + 160))(v8, g_5a22c, *((long long *)(*(v8) + 160)));
        }
        g_5a22c = -100;
    }
    if (g_5b8ee)
    {
        v7 = 0;
        v2 = &v7;
        v0 = "i_intereff";
        v1 = "intereff";
        v3 = 0;
        v13 = *((long long *)(*(v9) + 72));
        *((char **)&g_5b8ee) = 0;
        (unsigned int)v8 = v13(v9, &v0, v13);
        if (v8 && g_5b618 && g_5bed8 && g_5beb4 == 2)
        {
            mp_log("test: pause key");
            v8 = g_5bed8(v8, 0, 0, 0);
        }
    }
    if (!g_5b8ed)
    {
        return v8;
    }
    else if (!g_5b618)
    {
        return v8;
    }
    else if (!g_5bee8)
    {
        return v8;
    }
    else
    {
        v7 = 0;
        v2 = &v7;
        v0 = "i_intereff";
        v1 = "intereff";
        v3 = 0;
        v14 = *((long long *)(*(v9) + 72));
        *((char **)&g_5b8ed) = 0;
        (unsigned int)v8 = v14(v9, &v0, v14);
        if (!v8)
        {
            return v8;
        }
        else if (g_5beb4 != 2)
        {
            return v8;
        }
        else
        {
            mp_log("test: Withdraw pressed in the ESC menu and confirmed (box 0)");
            v10 = 1;
            end_turn_here(v9, v10);
            *((char **)&(3560)[v8]) = 0;
            v8 = assign@?$vector@EV?$allocator@E@std@?$vector@EV?$allocator@E@std(v8, 1, 0, 0);
            return v8;
        }
    }
}


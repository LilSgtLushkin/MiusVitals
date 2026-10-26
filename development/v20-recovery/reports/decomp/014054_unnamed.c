// v20 RVA 014054..01430c  (696 bytes)  read_sidecar@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char field_0;
} struct_1;

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long read_sidecar@opsave(unsigned long long a0[2], unsigned long long a1[3])
{
    char v0;  // [bp-0x278]
    void* v1;  // [bp-0x258], Other Possible Types: unsigned long long
    unsigned int v2;  // [bp-0x248]
    unsigned int v3;  // [bp-0x240]
    int v4;  // [bp-0x238]
    void* v5;  // [bp-0x228]
    char v6;  // [bp-0x218]
    char v7;  // [bp-0x217]
    char v8;  // [bp-0x148]
    unsigned long long v9;  // [bp-0x38]
    unsigned long v10;  // [bp-0x8]
    char v11;  // [bp+0x0]
    unsigned long long v13;  // r15
    unsigned long long v14;  // rbx, Other Possible Types: unsigned long
    void* v15;  // rdx
    unsigned long long v16;  // r8
    unsigned long long v17;  // rcx
    unsigned int v18;  // edi
    unsigned long long v19;  // rsi, Other Possible Types: unsigned long
    unsigned long long v20;  // rbx, Other Possible Types: unsigned long
    unsigned long v22;  // rcx
    struct_0 *v23;  // rcx

    v10 = &v11;
    v9 = g_5a380 ^ &v0;
    v13 = a1;
    if (a1 != a0)
        $_Assign_counted_range@PEBE@?$vector@EV?$allocator@E@std(a1, a0[0], *((long long *)(a0 + 8)) - a0[0]);
    *((char **)&v2) = 0;
    *((char **)&v3) = 0;
    sides@image(a0, &v2, &v3);
    if (g_5b8e4 == 1 && GetEnvironmentVariableA("MP_PEER_FORCE", &v8, 260))
    {
        *((char **)&v4) = 0;
        v5 = 0;
        v14 = v3;
        if (g_5b61f)
            v14 = v2;
        *((char **)&v6) = 0;
        v15 = 0;
        v16 = 199;
        v17 = &v7;
        memset();
        if (!(char)read_file(&v8, &v4))
            mp_log("lobby: the other player's force (%s) cannot be read - his side is played as set up", &v8);
        *((char **)&v2) = 0;
        v18 = -(0 < sides@image(&v4, &v2, &v3)) & v2;
        if (v18 != (unsigned int)v14)
        {
            v2 = v14;
            v19 = "Germans";
            if ((unsigned int)v14 == 5391687)
            {
                v20 = "Germans";
            }
            else if ((unsigned int)v14 == 1381192533)
            {
                v20 = "Soviets";
            }
            else
            {
                memcpy(&g_5b5c8, &v2, 4);
                *((char **)&g_5b5cc) = 0;
                v20 = &g_5b5c8;
            }
            v2 = v18;
            if (v18 != 5391687)
            {
                if (v18 == 1381192533)
                {
                    v19 = "Soviets";
                }
                else
                {
                    memcpy(&g_5b5c8, &v2, 4);
                    *((char **)&g_5b5cc) = 0;
                    v19 = &g_5b5c8;
                }
            }
            mp_log("lobby: the other player's force is of the %s and he commands the %s - his side is played as set up", v19, v20);
        }
        else
        {
            v1 = 200;
            v22 = (!(char)merge_moves@opsave@opsave(v13, &v4, v14, &v6) ? "lobby: the other player's own force is put into the battle: %s" : "lobby: the other player's force could not be put into the battle (%s) - his side is played as set up");
            mp_log(v22, &v6);
        }
        v23 = (long long)v4;
        if (v23)
        {
            if (v5 - v23 >= __Eg_my_units && -8 + (char *)v23 - *((long long *)(v23 - 8)) > 31)
            {
                v1 = 0;
                _invoke_watson(0, 0, 0, 0);
                [D] Unsupported jumpkind Ijk_SigTRAP at address 82597
            }
            sub_4030c();
            *((char **)&v4) = 0;
            v5 = 0;
        }
    }
    if (!g_5b61f)
    {
        return security_cookie_check(v9 ^ &v0);
    }
    else if (!(char)mirror@image(v13))
    {
        mp_log("lobby: this battle cannot be swapped, playing it as set up");
        return security_cookie_check(v9 ^ &v0);
    }
    else
    {
        return security_cookie_check(v9 ^ &v0);
    }
}


// v20 RVA 03dfc8..03e2ae  (742 bytes)  shots_install
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_4 {
    char field_0;
} struct_4;

typedef struct struct_2 {
    struct struct_3 *field_0;
    char paddin8[644];
    unsigned int field_28c;
    char paddin656[12];
    unsigned int field_29c;
} struct_2;

typedef struct struct_3 {
    char paddin0[104];
    unsigned long long field_68;
    unsigned long long field_70;
    char paddin120[8];
    unsigned long long field_80;
} struct_3;


long long shots_install(struct struct_1 **a0)
{
    char v0;  // [bp-0x78]
    void* v1;  // [bp-0x58]
    unsigned long v2;  // [bp-0x50]
    unsigned long v3;  // [bp-0x48]
    struct_4 *v4;  // [bp-0x40]
    void* v5;  // [bp-0x38]
    char v6;  // [bp-0x30]
    unsigned long long v7;  // [bp-0x20]
    struct_2 *v9;  // rsi
    unsigned long long v10;  // rbx
    unsigned long long v11;  // rax
    unsigned long long v13[17];  // rdi
    unsigned long long v14;  // r14
    unsigned long long v15;  // rbp
    int v16;  // xmm2
    int v17;  // xmm1

    v7 = g_5a380 ^ &v0;
    v1 = 0;
    v2 = "i_trigman";
    v5 = 0;
    v3 = "trigman";
    v4 = &v1;
    v9 = *((long long *)(*(a0) + 72))();
    v10 = GetModuleHandleA("adv_game.codelib");
    if (!v9)
    {
        return security_cookie_check(v7 ^ &v0);
    }
    else if (v10)
    {
        *((char **)&g_5dbd2) = 1;
        if (GetEnvironmentVariableA("MP_SYNC_END", &v6, 16))
        {
            (unsigned int)v11 = atoi(&v6);
            if (!(unsigned int)v11)
            {
                g_5a2f4 = v11;
                mp_log("end: each game ends its own battle (MP_SYNC_END=0)");
                return security_cookie_check(v7 ^ &v0);
            }
        }
        v13 = v9->field_0;
        if ((char)readable(v9, 1096) && v13 == v10 + 1246784 && *((long long *)(v13 + 104)) == v10 + 0xeeb00 && *((long long *)(v13 + 112)) == v10 + 978704 && *((long long *)(v13 + 128)) == v10 + 981152)
        {
            v14 = v10 + 1064688;
            if (!memcmp(v14, &g_4eb28, 16))
            {
                v15 = v10 + 980592;
                if (!memcmp(v15, &g_4eb38, 16) && squad_record(v9))
                {
                    g_5dba8 = install_detour(v14, 16, next_turn_hook);
                    g_5dbb0 = install_detour(v15, 16, $_Construct_n@AEB_K@?$vector@_KV?$allocator@_K@std);
                    if (g_5dba8 && g_5dbb0)
                    {
                        if (!memcmp(v10 + 1067808, &g_4eb48, 16))
                            g_5dbb8 = install_detour(v10 + 1067808, 16, ammo_check);
                        memcpy(&g_5dc40, v13 - 8, 384);
                        g_5db90 = *((long long *)(v13 + 128));
                        g_5db98 = *((long long *)(v13 + 104));
                        g_5dba0 = *((long long *)(v13 + 112));
                        g_5dcc8 = explode_hook;
                        g_5dcb0 = input_action_hook;
                        g_5dcb8 = end_turn_here;
                        v16 = 0;
                        *((unsigned int *)&v16) = *((int *)(v9 + 668));
                        v17 = 0;
                        *((unsigned int *)&v17) = *((int *)(v9 + 652));
                        v9->field_0 = &g_5dc48;
                        g_5db88 = v9;
                        *((char **)&g_5dbd3) = 1;
                        mp_log("end: withdrawing, a ceasefire and the end of the battle are shared between the two games (retreat time %.0f s, %.0f s of battle left)", (unsigned long long)v17, (unsigned long long)v16);
                        return security_cookie_check(v7 ^ &v0);
                    }
                    mp_log("end: could not be hooked, each game ends its own battle");
                    return security_cookie_check(v7 ^ &v0);
                }
            }
        }
        mp_log("end: unknown game build, each game ends its own battle");
        return security_cookie_check(v7 ^ &v0);
    }
    else
    {
        return security_cookie_check(v7 ^ &v0);
    }
}


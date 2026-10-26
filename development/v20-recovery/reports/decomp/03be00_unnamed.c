// v20 RVA 03be00..03c0d0  (720 bytes)  sub_3be00
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    char field_0;
} struct_3;


long long sub_3be00(struct struct_4 **a0)
{
    char v0;  // [bp-0x68]
    void* v1;  // [bp-0x48]
    unsigned long v2;  // [bp-0x40]
    unsigned long v3;  // [bp-0x38]
    struct_3 *v4;  // [bp-0x30]
    void* v5;  // [bp-0x28]
    char v6;  // [bp-0x20]
    unsigned long long v7;  // [bp-0x10]
    unsigned long v8;  // [bp-0x8]
    char v9;  // [bp+0x0]
    unsigned long long v11;  // rcx, Other Possible Types: unsigned long
    struct struct_2 **v12;  // r8
    struct struct_0 **v13;  // rdi
    unsigned long long v14;  // rdx
    unsigned long long v15[27];  // rbx
    unsigned long v16;  // rax
    unsigned long long v17;  // rcx
    char v18;  // r11b
    unsigned int v19;  // r8d
    unsigned int *v20;  // r10, Other Possible Types: unsigned long
    unsigned short *v21;  // r9, Other Possible Types: unsigned long

    v8 = &v9;
    v7 = g_5a380 ^ &v0;
    *((char **)&g_5d7c9) = 1;
    if (GetEnvironmentVariableA("MP_SYNC_ANIM", &v6, 16))
        g_5a2f0 = atoi(&v6) != 0;
    if (GetEnvironmentVariableA("MP_SYNC_FACING", &v6, 16))
        g_5a2f1 = atoi(&v6) != 0;
    if (!g_5a2f0)
    {
        v11 = "anim: each game animates every soldier for itself (MP_SYNC_ANIM=0)";
    }
    else
    {
        v1 = 0;
        v2 = "i_animch";
        v5 = 0;
        v3 = "animch";
        v4 = &v1;
        v13 = v12(a0, &v2, *((long long *)(*(a0) + 72)));
        v14 = GetModuleHandleA("adv_game.codelib");
        if (!v13 || !v14 || !((v15 = *(v13), v15 == v14 + 1241384)))
        {
            v11 = "anim: unknown game build, each game animates every soldier for itself";
        }
        else
        {
            v18 = 1;
            v21 = &g_4e1b0;
            v19 = 0;
            v20 = &g_4e1c8;
            while (v19 < 11 && (v16 = (long long)*((int *)v20), v17 = (unsigned long long)(unsigned short)(*((short *)v21) + 0xa0000) + v14, v18 = (char)(char)(*((long long *)((char *)v15 + v16 * 8)) == v17), v19 += 1, v20 += 4, v21 += 2, *((long long *)((char *)v15 + (v16 << 3))) == v17))
            ;
            if (v18)
            {
                memcpy(&g_5d900, v15 - 8, 336);
                v11 = "anim: what a soldier's animation is asked to do is its owner's: sent, and followed here";
                g_5d8f8 = *((long long *)(v15 + 32));
                g_5da68 = *((long long *)(v15 + 64));
                g_5da50 = *((long long *)(v15 + 80));
                g_5da80 = *((long long *)(v15 + 88));
                g_5da58 = *((long long *)(v15 + 96));
                g_5da88 = *((long long *)(v15 + 104));
                g_5da90 = *((long long *)(v15 + 112));
                g_5da70 = *((long long *)(v15 + 128));
                g_5da78 = *((long long *)(v15 + 160));
                g_5da60 = *((long long *)(v15 + 168));
                g_5da98 = *((long long *)(v15 + 208));
                g_5d928 = input_action_hook;
                g_5d948 = obj_by_place_hook;
                g_5d958 = input_action_hook;
                g_5d968 = input_action_hook;
                g_5d970 = desc_by_num_hook;
                g_5d978 = input_action_hook;
                g_5d988 = obj_by_place_hook;
                g_5d9a8 = obj_by_place_hook;
                g_5d9b0 = input_action_hook;
                g_5d9d8 = input_action_hook;
                *(v13) = &g_5d908;
                g_5d8f0 = v13;
                *((char **)&g_5d7c8) = 1;
            }
        }
    }
    mp_log(v11);
    return security_cookie_check(v7 ^ &v0);
}


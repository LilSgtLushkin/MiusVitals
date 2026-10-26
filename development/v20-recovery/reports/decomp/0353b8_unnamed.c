// v20 RVA 0353b8..035637  (639 bytes)  send_transforms
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[96];
    unsigned int field_60;
    char paddin100[268];
    unsigned int field_170;
    unsigned int field_174;
    unsigned int field_178;
    unsigned int field_17c;
    unsigned int field_180;
    unsigned int field_184;
    unsigned int field_188;
    unsigned long long field_18c;
    char field_194;
    char paddin405[283];
    unsigned int field_2b0;
    char paddin692[300];
    char field_3e0;
} struct_1;


long long send_transforms(struct struct_4 **a0)
{
    char v0;  // [bp-0xd8]
    int tmp_72;  // tmp #72
    int tmp_73;  // tmp #73
    int tmp_70;  // tmp #70
    unsigned int v1;  // [bp-0x98]
    unsigned int v2;  // [bp-0x88]
    unsigned int v3;  // [bp-0x84]
    unsigned int v4;  // [bp-0x80]
    struct struct_0 *v5[32];  // [bp-0x78]
    unsigned int *v6;  // [bp-0x70]
    unsigned long v7;  // [bp-0x68]
    struct struct_2 **v8;  // [bp-0x60]
    void* v9;  // [bp-0x58]
    char v10;  // [bp-0x50]
    char v11;  // [bp-0x48]
    unsigned long long v12;  // [bp-0x40]
    void* v14;  // r14, Other Possible Types: unsigned long long, unsigned long
    void* v15;  // r12, Other Possible Types: unsigned long long, unsigned long
    unsigned long long v16;  // r15
    struct_1 *v17;  // rsi
    unsigned long long v18;  // r13
    struct struct_0 *v20[32];  // rbx, Other Possible Types: unsigned long
    unsigned int *v21;  // rdi, Other Possible Types: unsigned long

    v12 = g_5a380 ^ &v0;
    v3 = g_5d044;
    v2 = g_5d048;
    v14 = 0;
    v15 = 0;
    *((char **)&g_5d044) = 0;
    if ((v5[0] = (struct struct_0 *)0, v6 = (unsigned int *)"i_divisions", v7 = (unsigned long)"divisions", v8 = (struct struct_2 **)v5, v9 = (void*)0, g_5d010 = (struct_3 *)(unsigned long long)*((long long *)(*(a0) + 72))(), g_5d010) && (char)all_player_led@opsave@opsave(&v10))
    {
        v20 = g_5d010->field_0;
        v5[0] = &v20[0];
        $_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(&v6);
        v21 = v6;
        v16 = v7;
        if (v21 != v16)
        {
            do
            {
                v4 = *((int *)v21);
                v17 = *((long long *)&(88)[v20])(g_5d010, v4);
                if ((char)readable(v17, 1088))
                {
                    v18 = *((int *)(v17 + 688));
                    if (*((char *)(v17 + 992)) && (unsigned int)v18 != -1)
                    {
                        v15 = &(1)[v15];
                        *((int *)(v17 + 368)) = (0 CONCAT *((int *)&v10)) + 0x41c80000;
                        *((int *)(v17 + 372)) = 1036831949;
                        *((int *)(v17 + 376)) = *((int *)&v11);
                        *((int *)(v17 + 380)) = 1041865114;
                        tmp_72 = SubV(tmp_73, tmp_70);
                        *((int128_t *)(v17 + 384)) = tmp_72;
                        *((int *)(v17 + 388)) = 1036831949;
                        *((int *)(v17 + 392)) = *((int *)&v11);
                        *((long long *)(v17 + 396)) = 1041865114;
                        *((char **)(v17 + 404)) = 1;
                        v1 = *((long long *)&(248)[v20])(g_5d010, v4, 0, v2, v3);
                        mp_log("test: artillery - squad %08x (support %u, side %08x) called %d rounds of kind %d onto %.0f,%.0f: mission %d", v4, v18, *((int *)(v17 + 96)), v3, v2, *((int *)&v10), *((int *)&v11));
                        v20 = v5;
                        if (v1 != -1)
                            v14 = &(1)[v14];
                    }
                }
            } while ((v21 += 4, v21 != v16));
        }
        _Tidy@?$vector@KV?$allocator@K@std(&v6);
        mp_log("test: artillery - %d of this player's squads have off-map support, %d called it", v15, v14);
        return security_cookie_check(v12 ^ &v0);
    }
    mp_log("test: artillery - no divisions manager or no other units");
    return security_cookie_check(v12 ^ &v0);
}


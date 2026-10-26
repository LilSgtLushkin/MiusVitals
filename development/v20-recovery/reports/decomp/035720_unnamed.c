// v20 RVA 035720..035946  (550 bytes)  sub_35720
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_4 {
    char field_0;
} struct_4;

typedef struct struct_1 {
    char paddin0[96];
    unsigned int field_60;
} struct_1;


long long sub_35720(struct struct_5 **a0)
{
    char v0;  // [bp-0xb8]
    int tmp_44;  // tmp #44
    int tmp_45;  // tmp #45
    int tmp_46;  // tmp #46
    int tmp_99;  // tmp #99
    int tmp_101;  // tmp #101
    int tmp_100;  // tmp #100
    unsigned int v1;  // [bp-0x90], Other Possible Types: unsigned long
    unsigned int *v2;  // [bp-0x88]
    unsigned long v3;  // [bp-0x80]
    struct_4 *v4;  // [bp-0x78]
    void* v5;  // [bp-0x70]
    char v6;  // [bp-0x68]
    char v7;  // [bp-0x64]
    char v8;  // [bp-0x60]
    void* v9;  // [bp-0x58], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x54]
    unsigned int v11;  // [bp-0x50]
    unsigned int v12;  // [bp-0x4c]
    unsigned int v13;  // [bp-0x48]
    unsigned int v14;  // [bp-0x44]
    unsigned int v15;  // [bp-0x40]
    unsigned int v16;  // [bp-0x3c]
    unsigned long long v17;  // [bp-0x38]
    unsigned long v18;  // [bp-0x8]
    char v19;  // [bp+0x0]
    unsigned long long v21;  // rsi
    struct struct_0 *v22[46];  // r13
    unsigned int *v23;  // rbx, Other Possible Types: unsigned long
    unsigned long long v24;  // r15
    struct_1 *v25;  // r14
    unsigned long long v26;  // rbx
    unsigned long long v27;  // rcx, Other Possible Types: unsigned long
    int v28;  // xmm2
    void* v30;  // rdi, Other Possible Types: unsigned long

    v18 = &v19;
    v17 = g_5a380 ^ &v0;
    v21 = g_5d050;
    *((char **)&g_5d050) = 0;
    if (!((v9 = (void*)0, v2 = (unsigned int *)"i_divisions", v3 = (unsigned long)"divisions", v4 = (struct_4 *)&v9, v5 = (void*)0, g_5d010 = (struct_3 *)(unsigned long long)*((long long *)(*(a0) + 72))(), g_5d010)) || !(char)all_player_led@opsave@opsave(&v6))
    {
        v27 = "test: flares - no divisions manager or no other units";
    }
    else
    {
        v22 = g_5d010->field_0;
        $_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(&v2);
        for (v23 = v2; v23 != v3; v23 = &(4)[v23])
        {
            v24 = *((int *)v23);
            v25 = *((long long *)(v22 + 88))(g_5d010, v24);
            if ((char)readable(v25, 1088))
            {
                v26 = *((int *)(v25 + 96));
                v30 = 0;
                if ((unsigned int)v21 > 0)
                {
                    do
                    {
                        xmm2<16> = Conv(64->128, rdi<8>)
                        tmp_44 = (unsigned int)v28;
                        tmp_45 = (unsigned long long)v28 >> 32;
                        tmp_46 = (unsigned int)v28 / 0x10000000000000000;
                        *((unsigned int *)&v28) = (0 CONCAT v30) >> 64 >> 32;
                        *((unsigned int *)&v28) = tmp_46;
                        *((unsigned int *)&v28) = tmp_45;
                        *((unsigned int *)&v28) = tmp_44;
                        s_48 =L Conv(128->32, ((xmm2<16> * 0x41a00000<128>) + Conv(32->128, Load(addr=stack_base-104, size=4, endness=Iend_LE))))<4>
                        v14 = (0 CONCAT *((int *)&v7)) + 0x3f800000;
                        v15 = *((int *)&v8);
                        *((char **)&v16) = 0;
                        v9 = (unsigned int)v28 + 0x40800000;
                        v10 = 0x420c0000;
                        tmp_99 = SubV(tmp_101, tmp_100);
                        *((int128_t *)&v11) = tmp_99;
                        *((char **)&v12) = 0;
                        *((char **)&v1) = 0;
                        *((long long *)(v22 + 360))(g_5d010, &v13, 4, &v9, v26);
                        v30 = &(1)[v30];
                    } while ((unsigned int)v30 < (unsigned int)v21);
                }
                v1 = *((int *)&v8);
                mp_log("test: flares - %d flares for squad %08x (side %08x) over %.0f,%.0f", v21, v24, v26, *((int *)&v6));
                _Tidy@?$vector@KV?$allocator@K@std(&v2);
                return security_cookie_check(v17 ^ &v0);
            }
        }
        _Tidy@?$vector@KV?$allocator@K@std(&v2);
        v27 = "test: flares - this player has no squad";
    }
    mp_log(v27);
    return security_cookie_check(v17 ^ &v0);
}


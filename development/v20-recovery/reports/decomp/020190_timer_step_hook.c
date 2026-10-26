// v20 RVA 020190..0207bf  (1583 bytes)  timer_step_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[264];
    unsigned int field_108;
    unsigned int field_10c;
    char paddin272[76];
    char field_15c;
    char field_15d;
} struct_1;


long long timer_step_hook(struct_1 *a0, unsigned long a1, unsigned long a2)
{
    char v0;  // [bp-0xa8]
    int tmp_19;  // tmp #19
    int tmp_20;  // tmp #20
    int tmp_17;  // tmp #17
    int tmp_8;  // tmp #8
    int tmp_10;  // tmp #10
    int tmp_9;  // tmp #9
    int tmp_13;  // tmp #13
    int tmp_14;  // tmp #14
    int tmp_11;  // tmp #11
    int tmp_25;  // tmp #25
    int tmp_26;  // tmp #26
    int tmp_23;  // tmp #23
    int tmp_15;  // tmp #15
    int tmp_2;  // tmp #2
    int tmp_4;  // tmp #4
    int tmp_3;  // tmp #3
    int tmp_22;  // tmp #22
    int tmp_21;  // tmp #21
    int tmp_28;  // tmp #28
    int tmp_29;  // tmp #29
    int tmp_12;  // tmp #12
    int tmp_6;  // tmp #6
    int tmp_7;  // tmp #7
    char v1;  // [bp-0x88]
    unsigned long long v2;  // [bp-0x78]
    struct_1 *v4;  // rbx
    unsigned int v5;  // rcx, Other Possible Types: unsigned long
    int v6;  // xmm1
    char v8;  // dil
    unsigned long v9;  // rdx
    unsigned long long v10;  // rcx
    char v11;  // al
    int v13;  // xmm0, Other Possible Types: unsigned long long
    int v14;  // xmm6
    int v15;  // xmm0
    int v16;  // xmm6
    unsigned long v17;  // xmm8hq
    unsigned long v18;  // xmm8lq
    int v22;  // xmm2hq, Other Possible Types: unsigned long
    int v26;  // xmm7
    unsigned long long v27;  // rax
    int v29;  // xmm0, Other Possible Types: unsigned long long
    int v30;  // xmm1
    unsigned long v31;  // rax
    int v32;  // xmm1
    int v33;  // xmm1
    int v34;  // xmm2
    unsigned int v35;  // edi
    int v36;  // xmm0
    int v37;  // xmm1

    v2 = g_5a380 ^ &v0;
    v4 = a0;
    v5 = g_5b8e0;
    if (!g_5b618 || !g_5b61b || (v8 = 1, !g_5b8e0))
        v8 = 0;
    v13 = 0;
    v13 = 0x3ff0000000000000;
    if (g_5b8e0 == 1 && v8)
    {
        if (g_5a26c < 0)
        {
            g_5a26c = *((char *)(v4 + 349));
            v9 = (g_5a26c != 0 ? "off already" : "on");
            v10 = "clock: the engine's automatic speed control is %s; off while the other player is in the battle";
            mp_log();
            v5 = g_5b8e0;
        }
        *((int *)(v4 + 268)) = *((int *)(v4 + 264));
        *((char **)(v4 + 349)) = 0;
LABEL_2027a:
        if (!((unsigned int)v5 == 1))
            goto LABEL_202e3;
        if (g_5bef8)
        {
            if (ui_blocks_input())
            {
                if (!g_5b83f || (v11 = 1, !g_5b898))
                    v11 = 0;
                *((char *)(v4 + 348)) = v11;
                sub_d590();
                v14 = v13;
                if (((char)((CmpF(*((int *)(v4 + 264)), (unsigned int)v14) & 69) >> 2) & 1) || (CmpF(*((int *)(v4 + 264)), (unsigned int)v14) & 64))
                    g_5bef8(v4);
                *((unsigned int *)(v4 + 268)) = (unsigned int)v14;
            }
            v5 = g_5b8e0;
            goto LABEL_202e3;
        }
    }
    else
    {
        if (g_5a26c < 0)
            goto LABEL_2027a;
        if (g_5b8e0 == 1)
        {
            *((char *)(v4 + 349)) = g_5a26c;
            g_5a26c = -1;
            goto LABEL_2027a;
        }
LABEL_202e3:
        if ((unsigned int)v5 == 2 && g_5b618 && g_5b61b && g_5b83d)
        {
            *((unsigned int *)(v4 + 264)) = g_5a1c8;
            *((unsigned int *)(v4 + 268)) = g_5a1c8;
            *((char *)(v4 + 348)) = g_5b83c;
            *((char **)(v4 + 349)) = 0;
            if (g_5beb4 == (unsigned int)v5 && g_5bec0 == (unsigned int)v5)
            {
                tmp_19 = MaxV(tmp_20, tmp_17);
                v15 = tmp_19;
                tmp_8 = SubV(tmp_10, tmp_9);
                v16 = tmp_8;
                v17 = (unsigned int)v15;
                v18 = (unsigned int)v15;
                if (!g_5b83e || (fabs(), (((char)(unsigned long long)(unsigned int)(CmpF((unsigned long long)v16, xmm8<16> * 0x40d3880000000000) & 69) | (char)((unsigned long long)(unsigned int)(CmpF((unsigned long long)v16, xmm8<16> * 0x40d3880000000000) & 69) >> 6)) & 1) != 1))
                {
                    fabs();
                    if ((((char)(CmpF((unsigned long long)v16, 0x409f400000000000) & 69) | (char)((CmpF((unsigned long long)v16, 0x409f400000000000) & 69) >> 6)) & 1) != 1)
                    {
                        tmp_13 = DivV(tmp_14, tmp_11);
                        v22 = tmp_13;
                        tmp_25 = DivV(tmp_26, tmp_23);
                        mp_log("clock: taking over the host's battle time (%.1f s, was %.1f s)", tmp_25, (unsigned long long)v22);
                    }
                    v16 = 0;
                    g_5beb8 = g_5be44;
                    *((char **)&g_5b83e) = 1;
                }
                fabs();
                if ((((char)(CmpF((unsigned long long)v16, 0x4044000000000000) & 69) | (char)((CmpF((unsigned long long)v16, 0x4044000000000000) & 69) >> 6)) & 1) == 1 || (v13 = 0, *((unsigned long long *)&v13) = 0x3fd0000000000000, tmp_13 = DivV(tmp_15, tmp_14), v26 = tmp_13 + 0x3ff0000000000000, (((char)(unsigned long long)(unsigned int)(CmpF(0x3fd0000000000000, (unsigned long long)v26) & 69) | (char)((unsigned long long)(unsigned int)(CmpF(0x3fd0000000000000, (unsigned long long)v26) & 69) >> 6)) & 1) == 1))
                {
                    tmp_2 = MinV(tmp_4, tmp_3);
                    v13 = tmp_2;
                }
                v13 = v13;
                goto LABEL_2057f;
            }
        }
    }
    if ((unsigned int)v5 == 1 && v8 && g_5beb4 == 2 && g_5be84 == 2 && g_5be88 && GetTickCount() - g_5be88 < 3000)
    {
        tmp_20 = SubV(tmp_22, tmp_21);
        tmp_28 = MaxV(tmp_29, tmp_26);
        v13 = tmp_28;
        v22 = (unsigned int)v13;
        v22 = (unsigned int)v13;
        if ((((char)(CmpF(tmp_20, xmm2<16> * 0x4079000000000000) & 69) | (char)((CmpF(tmp_20, xmm2<16> * 0x4079000000000000) & 69) >> 6)) & 1) != 1)
        {
            v13 = xmm2<16> * 0x40d3880000000000;
            if ((((char)(CmpF((unsigned long long)v36, (unsigned long long)v37) & 69) | (char)((CmpF((unsigned long long)v36, (unsigned long long)v37) & 69) >> 6)) & 1) != 1)
            {
                tmp_20 = MaxV(tmp_22, tmp_21);
                v13 = tmp_20;
            }
        }
    }
LABEL_2057f:
    (unsigned int)v27 = GetTickCount();
    if (!(CmpF((unsigned long long)v13, 0x3fe0000000000000) & 1) || g_5b8e0 != 1)
    {
        *((char **)&g_5be8c) = 0;
    }
    else if (!g_5be8c)
    {
        g_5be8c = ((unsigned int)v27 != 0 ? 1 : (unsigned int)v27);
    }
    g_5a214 = (int)(double)xmm7<16>;
    if (v8 && g_5beb4 == 2 && !*((char *)(v4 + 348)) && (CmpF(0, g_5a220) & 1))
    {
        v29 = 0;
        v29 = g_5a270;
        if ((((char)(CmpF(0, g_5a270) & 69) | (char)((CmpF(0, g_5a270) & 69) >> 6)) & 1) != 1)
        {
            if (!GetEnvironmentVariableA("MP_TEST_SPEED", &v1, 16) || (atof(&v1), (((char)(unsigned long long)(unsigned int)(CmpF(g_5a270, 0) & 69) | (char)((unsigned long long)(unsigned int)(CmpF(g_5a270, 0) & 69) >> 6)) & 1) == 1))
            {
                v29 = 0;
                *((unsigned long long *)&v29) = 0x3ff0000000000000;
            }
            else
            {
                atof(&v1);
            }
            g_5a270 = (unsigned long long)v29;
        }
        v30 = 0;
        *((unsigned long long *)&v30) = (unsigned long long)v6;
        v31 = (g_5b8e0 == 2 ? 5 : 12);
        v32 = v30 * v13;
        *((unsigned long long *)&v22) = v31;
        *((unsigned long long *)&v22) = v31 / 0x100000000;
        v33 = v32 * v29;
        xmm2<16> = ((xmm2<16> * Conv(64->128, Load(addr=0x5a220<64>, size=8, endness=Iend_LE))) * xmm0<16>)
        g_5c0f8 = (unsigned long long)((0 CONCAT g_5c0f8) + v33);
        if ((((char)(CmpF(g_5c0f8, g_5c0f8) & 69) | (char)((CmpF(g_5c0f8, g_5c0f8) & 69) >> 6)) & 1) != 1)
            g_5c0f8 = (unsigned long long)v34;
        seconds_now(12);
        v35 = 0;
        if (!(CmpF(g_5c0f8, g_5a220) & 1))
        {
            do
            {
                if (v35 >= 3)
                {
                    seconds_now();
                    tmp_6 = SubV(tmp_8, tmp_7);
                    v36 = tmp_6;
                    v37 = (0 CONCAT g_5a270) * 4588807732320345784;
                    if ((((char)(CmpF((unsigned long long)v36, (unsigned long long)v37) & 69) | (char)((CmpF((unsigned long long)v36, (unsigned long long)v37) & 69) >> 6)) & 1) != 1)
                        return security_cookie_check(v2 ^ &v0);
                }
            } while ((tmp_11 = SubV(tmp_13, tmp_12), *((int128_t *)&g_5c0f8) = tmp_11, g_5bef0(v4), v35 += 1, !(CmpF(g_5c0f8, g_5a220) & 1)));
            if (v35)
                return security_cookie_check(v2 ^ &v0);
        }
        *((char **)(v4 + 264)) = 0;
        g_5bef0(v4);
        *((int *)(v4 + 264)) = *((int *)(v4 + 264));
        return security_cookie_check(v2 ^ &v0);
    }
    g_5c0f8 = 0;
    *((char **)&g_5be8c) = 0;
    g_5bef0(v4);
    return security_cookie_check(v2 ^ &v0);
}


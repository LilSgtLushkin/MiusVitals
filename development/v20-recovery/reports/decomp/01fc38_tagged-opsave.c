// v20 RVA 01fc38..020002  (970 bytes)  tagged@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long tagged@opsave(unsigned long long a0[3], unsigned long long a1[2])
{
    char v0;  // [bp-0x148]
    int tmp_5;  // tmp #5
    char v1;  // [bp-0x128]
    unsigned int v2;  // [bp-0x124]
    char v3;  // [bp-0x120]
    char v4;  // [bp-0x11c]
    char v5;  // [bp-0x118]
    char v6;  // [bp-0x114]
    char v7;  // [bp-0x110]
    unsigned long v8;  // [bp-0x108]
    unsigned long v9;  // [bp-0x100]
    unsigned long long v10[3];  // [bp-0xf8]
    unsigned long v11;  // [bp-0xf0]
    unsigned long v12;  // [bp-0xe8]
    unsigned long long v13[3];  // [bp-0xe0]
    int v14;  // [bp-0xd8], Other Possible Types: char
    int v15;  // [bp-0xc8]
    void* v16;  // [bp-0xb8], Other Possible Types: unsigned long
    void* v17;  // [bp-0xb0], Other Possible Types: unsigned long
    void* v18;  // [bp-0xa8], Other Possible Types: unsigned long
    void* v19;  // [bp-0xa0], Other Possible Types: unsigned long
    void* v20;  // [bp-0x98], Other Possible Types: unsigned long
    void* v21;  // [bp-0x90], Other Possible Types: unsigned long
    void* v22;  // [bp-0x88], Other Possible Types: unsigned long
    char v23;  // [bp-0x80]
    char v24;  // [bp-0x78]
    int v25;  // [bp-0x68]
    char v26;  // [bp-0x58]
    unsigned long long v27;  // [bp-0x50]
    unsigned long long v29[2];  // r14
    unsigned long long v30[3];  // rbx, Other Possible Types: unsigned long
    void* v31;  // r12, Other Possible Types: unsigned long
    unsigned long long v32;  // rcx, Other Possible Types: unsigned long
    unsigned long long v33;  // rdx
    unsigned long long v34;  // rdi
    unsigned long long v35;  // rax
    char *v36;  // rsi
    unsigned long long v37;  // rcx, Other Possible Types: unsigned long
    void* v38;  // r15, Other Possible Types: unsigned long
    unsigned long v39;  // rbx, Other Possible Types: unsigned long long
    unsigned long long v40;  // r13, Other Possible Types: unsigned long
    void* v41;  // rax

    v27 = g_5a380 ^ &v0;
    v29 = a1;
    v30 = a0;
    v10[0] = a0;
    v13[0] = a0;
    *((char **)&v2) = 1;
    a0[0] = 0;
    *((char **)(a0 + 8)) = 0;
    *((char **)(a0 + 16)) = 0;
    *((char **)&v2) = 1;
    if (!(*((long long *)(a1 + 8)) - a1[0] >= 64 && !memcmp(a1[0], "AZCF", 4)))
    {
        *((char **)&v2) = 0;
        return security_cookie_check(v27 ^ &v0);
    }
    memcpy(&v1, &(4)[a1[0]], 4);
    memcpy(&v3, &(8)[a1[0]], 4);
    v31 = 0;
    if (*((int *)&v1) > 0)
    {
        v32 = *((int *)&v3);
        v12 = v32;
        while (true)
        {
            v40 = v31 * 128 + v32;
            v11 = v40;
            v33 = v29[0];
            if (v40 + 128 > *((long long *)(v29 + 8)) - v33)
                break;
            memcpy(&v4, &(32)[v33 + v40], 4);
            if (*((int *)&v4) == 1)
            {
                memcpy(&v6, &(40)[v29[0] + v40], 4);
                memcpy(&v5, &(48)[v29[0] + v40], 4);
                v9 = *((int *)&v5);
                v8 = *((int *)&v6);
                if ((v8 << 6) + v9 <= *((long long *)(v29 + 8)) - v29[0])
                {
                    *((char **)&v14) = 0;
                    *((char **)&v15) = 0;
                    *((int128_t *)&v15) = 0xf0000000000000000;
                    *((char **)&v14) = 0;
                    v16 = 0;
                    v17 = 0;
                    v18 = 0;
                    v19 = 0;
                    v20 = 0;
                    v21 = 0;
                    v22 = 0;
                    *((char **)&v23) = 0;
                    if ((unsigned int)v8)
                    {
                        v38 = 0;
                        v39 = v8;
                        do
                        {
                            v34 = v38 + v9;
                            v35 = v29[0];
                            v36 = &(32)[v35 + v34];
                            memcpy(&v7, &(16)[v35 + v34], 4);
                            if (*((int *)&v7) == 4)
                            {
                                if (!strncmp(v36, "ai", 32))
                                {
                                    v16 = v34;
                                }
                                else if (!strncmp(v36, "vis_mode", 32))
                                {
                                    v17 = v34;
                                }
                                else if (!strncmp(v36, "px", 32))
                                {
                                    v18 = v34;
                                }
                                else if (!strncmp(v36, "pz", 32))
                                {
                                    v19 = v34;
                                }
                                else if (!strncmp(v36, "side", 32))
                                {
                                    v20 = v34;
                                }
                                else if (!strncmp(v36, "turn_flags", 32))
                                {
                                    v21 = v34;
                                }
                                else
                                {
                                    v37 = v22;
                                    if (!strncmp(v36, "uid", 32))
                                        v37 = v34;
                                    v22 = v37;
                                }
                            }
                        } while ((v38 += 64, tmp_5 = v39, v39 -= 1, tmp_5 != 1));
                        v30 = v10;
                        v40 = v11;
                    }
                    if (v16)
                    {
                        *((char **)&v24) = 0;
                        *((char **)&v25) = 0;
                        *((char **)&v26) = 0;
                        memcpy(&v24, v29[0] + v40, 32);
                        assign@?$basic_string@DU?$char_traits@D@std(&v14, &v24);
                        v23 = strncmp(&v24, "p_", 2) == 0;
                        v41 = *((long long *)&(8)[v30]);
                        if (v41 == *((long long *)&(16)[v30]))
                        {
                            $_Emplace_reallocate@AEBUTagged@opsave(v30, v41, &v14);
                        }
                        else
                        {
                            0Tagged@opsave@opsave(v41, &v14);
                            *((char **)&(8)[v30]) = &(96)[*((long long *)&(8)[v30])];
                        }
                    }
                    _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v14);
                }
            }
            v31 = &(1)[v31];
            v32 = v12;
            if ((unsigned int)v31 >= *((int *)&v1))
                break;
        }
    }
}


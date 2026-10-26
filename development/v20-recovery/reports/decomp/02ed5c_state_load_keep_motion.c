// v20 RVA 02ed5c..02f4ce  (1906 bytes)  state_load_keep_motion
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    char paddin0[77];
    char field_4d;
} struct_3;

typedef struct struct_4 {
    char field_0;
    char paddin1[71];
    unsigned long long field_48;
    char paddin80[3488];
    unsigned long long field_df0;
    char paddin3576[1184];
    unsigned long long field_1298;
    char padding_12a0[4];
    unsigned int field_12a4;
    struct struct_0 *field_12a8;
} struct_4;

typedef struct struct_5 {
    char field_0;
} struct_5;

typedef struct struct_0 {
    char paddin0[52];
    char field_34;
} struct_0;

typedef struct struct_1 {
    unsigned short field_-4;
    unsigned short field_-2;
    unsigned short field_0;
    unsigned short field_2;
} struct_1;

typedef struct struct_2 {
    char field_0;
    char paddin1[7];
    unsigned long long field_8;
} struct_2;


long long state_load_keep_motion(struct_4 *a0, struct_3 *a1, unsigned long a2, unsigned long a3)
{
    char v0;  // [bp-0x318]
    int tmp_4;  // tmp #4
    unsigned long v1;  // [bp-0x2f8]
    unsigned int v2;  // [bp-0x2e8]
    char v3;  // [bp-0x2e4]
    char v4;  // [bp-0x2e3]
    struct_0 *v5;  // [bp-0x2e0]
    char v6;  // [bp-0x2d8]
    char v7;  // [bp-0x2d7]
    unsigned int v8;  // [bp-0x2d4]
    unsigned int v9;  // [bp-0x2d0], Other Possible Types: unsigned long long
    struct_3 *v10;  // [bp-0x2c8]
    struct_4 *v11;  // [bp-0x2c0]
    unsigned long long v12;  // [bp-0x2b8]
    char v13;  // [bp-0x2b0]
    int v14;  // [bp-0x2a8]
    void* v15;  // [bp-0x298]
    char v16;  // [bp-0x290]
    char v17;  // [bp-0x268]
    char v19;  // [bp-0x250]
    char v20;  // [bp-0x218]
    unsigned long long v21;  // [bp-0x48]
    unsigned long v22;  // [bp-0x8]
    char v23;  // [bp+0x0]
    struct_4 *v25;  // rdi
    void* v26;  // rsi, Other Possible Types: unsigned long
    unsigned short *v27;  // r14, Other Possible Types: unsigned long
    struct_5 *v28;  // r8, Other Possible Types: unsigned long
    unsigned short *v29;  // r9, Other Possible Types: unsigned long
    unsigned long v30;  // r13, Other Possible Types: unsigned long long
    unsigned long v31;  // r12, Other Possible Types: unsigned long long
    unsigned long long v32;  // rcx
    unsigned long long v33;  // r14
    unsigned long long v34;  // rsi
    struct_1 *v35;  // r14, Other Possible Types: unsigned long
    struct_1 *v36;  // rsi, Other Possible Types: unsigned long
    struct_2 *v37;  // r15, Other Possible Types: unsigned long
    unsigned long v38;  // r12, Other Possible Types: unsigned long long
    unsigned long long v39;  // rcx
    unsigned long long v40;  // r13
    unsigned long long v41;  // rsi
    unsigned long long v42;  // r15
    unsigned long long v43;  // rax
    unsigned long long v44;  // rsi
    unsigned long long v45;  // rax
    unsigned long v46;  // r8
    unsigned long v47;  // rdx
    unsigned long long v48;  // rcx
    unsigned long long v49[2];  // rsi, Other Possible Types: unsigned long
    unsigned long v50;  // r8
    unsigned long long v51;  // r15, Other Possible Types: unsigned long
    unsigned long long v52;  // r9
    unsigned int v53;  // r12d
    unsigned long long v54;  // r13
    void* v55;  // r12, Other Possible Types: unsigned long
    unsigned long long v56;  // rcx
    unsigned long long v57;  // r14
    unsigned long long v58;  // rsi
    unsigned long long v59;  // r13
    unsigned long long v60;  // rsi
    unsigned int v61;  // r12d
    unsigned long long v62;  // r15
    struct_0 *v63;  // eax, Other Possible Types: unsigned int
    unsigned long long v64;  // rsi, Other Possible Types: unsigned long
    unsigned long long v66;  // r15, Other Possible Types: unsigned long
    unsigned int v67;  // r15d
    struct_3 *v68;  // rax
    struct_0 *v69;  // r12, Other Possible Types: unsigned long
    unsigned long v70;  // rsi
    unsigned long long v71;  // r14, Other Possible Types: unsigned long
    struct_4 *v72;  // rax, Other Possible Types: unsigned long
    unsigned long long v73;  // r13
    unsigned long v75;  // rax
    unsigned long long v76;  // rsi
    unsigned int v78;  // eax
    unsigned long long v79;  // r12
    unsigned long long v81;  // rax
    unsigned long long v82;  // rsi
    void* v83;  // rsi, Other Possible Types: unsigned long
    unsigned short *v84;  // r14, Other Possible Types: unsigned long
    struct_5 *v85;  // r10, Other Possible Types: unsigned long
    unsigned short *v86;  // r11, Other Possible Types: unsigned long

    v22 = &v23;
    v21 = g_5a380 ^ &v0;
    v3 = a3;
    v12 = a2;
    v10 = a1;
    v25 = a0;
    v11 = a0;
    v8 = strcmp(*((long long *)(a0 + 72)), "i_human");
    memcpy(&v20, a0 + 240, 464);
    v26 = 0;
    v27 = &g_4c9e0;
    do
    {
        memcpy(&(&v16)[v26 * 4], &(&a0->field_0)[*((short *)v27)], 4);
        v26 = &(1)[v26];
        v27 = &(2)[v27];
    } while ((unsigned int)v26 < 7);
    v28 = &v6;
    v29 = &g_4c9f0;
    do
    {
        *((char *)v28) = (&a0->field_0)[*((short *)v29)];
        v29 = &(2)[v29];
        v28 = &v7;
    } while (true);
    v4 = *((char *)(&(&a0->field_0)[-(0 < v8) & -1062] + &g_228c));
    v30 = strcmp(*((long long *)(a0 + 72)), "i_human");
    *((unsigned int *)&v5) = v30;
    v31 = a0 + &g_1d60;
    memcpy(&v13, v31, 8);
    v2 = strcmp(*((long long *)(a0 + 72)), "i_techn");
    *((char **)&v14) = 0;
    v15 = 0;
    if (!v2)
    {
        v32 = *((int *)(a0 + __Eg_my_units));
        v33 = *((long long *)(v25 + &g_1298));
        if ((unsigned int)v32 && (unsigned int)v32 < __Eg_my_units)
        {
            v34 = v32 * 32;
            if ((char)readable(v33, v34))
                $_Assign_counted_range@PEBE@?$vector@EV?$allocator@E@std(&v14, v33, v34);
        }
    }
    v1 = sub_9980;
    sub_40a74(&v17, 24, 3, sub_8898);
    v35 = &g_4c9fc.field_-4;
    if (v2)
    {
        if (!g_5cde1)
        {
            *((char **)&g_5cde1) = 1;
            v41 = GetModuleHandleA("adv_physic.codelib");
            v42 = GetModuleHandleA("basic_render.codelib");
            if (!v41 || ((unsigned int)v43 = memcmp(v41 + 532624, &g_4c830, 16), (unsigned int)v43))
            {
                mp_log("state: unknown game build, render handles are not guarded");
            }
            else
            {
                g_5cd98 = install_detour(v41 + 532624, &(14)[v43], base_load_hook);
                if (v42)
                {
                    v44 = &(&g_13ab0)[v42];
                    (unsigned int)v45 = memcmp(v44, &g_4c840, 22);
                    if (!(unsigned int)v45)
                        g_5cda8 = install_detour(v44, &(15)[v45], node_lookup_hook);
                }
                v46 = (g_5cda8 != 0 ? &g_45ce6 : ", node look-ups are guarded");
                v47 = (g_5cd98 != 0 ? "could not be guarded" : "stay local when states are loaded");
                v48 = "state: render handles %s%s";
                mp_log();
            }
        }
        *((char **)&g_5cda0) = &(1)[g_5cda0];
        v9 = state_load(v25, v10, v12);
        *((char **)&g_5cda0) = g_5cda0 - 1;
        if (!(unsigned int)v30)
            memcpy(v31, &v13, &(8)[v30]);
        v67 = v2;
        if (!v67)
        {
            v49 = &v17;
            do
            {
                v50 = *((short *)(v35 - 2));
                v51 = *((int *)&(&v25->field_0)[v50]);
                *((int *)&v5) = *((int *)(&(&v25->field_0)[v50] + 4));
                v52 = *((long long *)&(8)[v49]) - *((long long *)v49);
                v53 = (0 CONCAT v52) /m *((short *)v35);
                v54 = *((long long *)&(&v25->field_0)[*((short *)(v35 - 4))]);
                if (v53)
                {
                    if ((unsigned int)v51 != v53 || !(char)readable(v54, v52))
                    {
                        if (!((int)v5 < v53 || !(char)readable(v54, *((long long *)&(8)[v49]) - *((long long *)v49))))
                        {
                            memcpy(v54, *((long long *)v49), *((long long *)&(8)[v49]) - *((long long *)v49));
                            *((unsigned int *)&(&v25->field_0)[*((short *)(v35 - 2))]) = v53;
                        }
                        else if (*((short *)&(2)[v35]) == *((short *)v35))
                        {
                            *((char **)&(&v25->field_0)[*((short *)(v35 - 2))]) = 0;
                        }
                    }
                    else
                    {
                        if ((unsigned int)v51)
                        {
                            v55 = 0;
                            do
                            {
                                v56 = v55 * v35->field_-4;
                                memcpy(v56 + v54, *((long long *)&v17) + v56, *((short *)(v35 + 2)));
                                v55 = &(1)[v55];
                                tmp_4 = v51;
                                v51 -= 1;
                            } while (tmp_4 != 1);
                        }
                    }
                }
                else
                {
                    if ((unsigned int)v51)
                        *((char **)&(&v25->field_0)[v50]) = 0;
                }
            } while ((v49 = &v19, v35 += 8, v35 < "TATE"));
            v57 = (long long)v14;
            v58 = (long long)(&v14)[8];
            if (v57 != v58)
            {
                v59 = *((int *)(v25 + __Eg_my_units));
                v63 = *((int *)(v25 + &g_12a8));
                v5 = v63;
                v60 = v58 - v57;
                v61 = (unsigned int)v60 / 32;
                v62 = *((long long *)(v25 + &g_1298));
                if ((unsigned int)v59 != v61)
                {
LABEL_2f22e:
                    if (v63 >= v61 && (char)readable(v62, v60))
                    {
                        memcpy(v62, v57, v60);
                        *((unsigned int *)(v25 + __Eg_my_units)) = v61;
                    }
                }
                else if (!(char)readable(v62, v60))
                {
                    v63 = (int)v5;
                    goto LABEL_2f22e;
                }
                else if ((unsigned int)v59)
                {
                    v64 = &(16)[v62];
                    v66 = v59;
                    do
                    {
                        memcpy(v64, v57 - v62 + v64, 4);
                        v64 = &(32)[v64];
                        v66 -= 1;
                    } while (v59 != 1);
                }
            }
            v67 = v2;
        }
        find@?$_Hash@V?$_Umap_traits@_KPEAUObject(&g_5cd50, &v10, &v11);
        v68 = v10;
        if (v68 != g_5cd58)
        {
            v69 = v68 + 24;
            v5 = v69;
            if (*((char *)(v69 + 52)) && !*((char *)(v68 + 77)) && (char)replicated_singleton(v25))
            {
                v70 = (unsigned int)-(0 < v67) & 2232;
                if ((char)readable(v25, &(&g_1f00)[v70]))
                {
                    v71 = $_Construct_n@AEBQEAUShotRec(v69, v25 + 240, &(&g_1e10)[v70]);
                    v72 = (v67 == 0 ? &g_4c960 : &g_4c930);
                    v11 = v72;
                    v73 = &(24)[-(0 < v67) & -23];
                    do
                    {
                        v75 = *((short *)(v72 + 0 * 2));
                        v76 = *((long long *)&(&v25->field_0)[v75]);
                        v78 = *((int *)(&(&v25->field_0)[v75] + 16));
                        if (v76 && !((char)v76 & 15) && *((int *)(&(&v25->field_0)[v75] + 12)) <= v78 && v78 - 1 <= 65535 && (char)readable(v76, 16))
                        {
                            v79 = node_lookup_hook(v76);
                            if (!v79 || !(char)readable(v76, v79))
                            {
                                v69 = v5;
                            }
                            else
                            {
                                v69 = v5;
                                v71 += $_Construct_n@AEBQEAUShotRec(v69, v76, v79);
                            }
                        }
                    } while ((v49 = &v19, v35 += 8, v35 < "TATE"));
                    if (!v2)
                    {
                        v81 = *((long long *)(v25 + 3568));
                        if (v81)
                        {
                            v82 = &(512)[v81];
                            if ((char)readable(v82, 16))
                                v71 += $_Construct_n@AEBQEAUShotRec(v69, v82, 12);
                        }
                    }
                    g_5cdb8 = &(1)[g_5cdb8];
                    g_5cdc0 = g_5cdc0 + v71;
                }
            }
        }
        memcpy(v25 + 240, &v20, 464);
        if (!v3)
        {
            sub_40ae4(&v17, 24, 3, sub_9980);
            _Tidy@?$vector@EV?$allocator@E@std(&v14);
            return security_cookie_check(v21 ^ &v0);
        }
        v83 = 0;
        v84 = &g_4c9e0;
        do
        {
            memcpy(&(&v25->field_0)[*((short *)v84)], &(&v16)[v83 * 4], 4);
            v83 = &(1)[v83];
            v84 = &(2)[v84];
        } while ((unsigned int)v83 < 7);
        v85 = &v6;
        v86 = &g_4c9f0;
        while (true)
        {
            (&v25->field_0)[*((short *)v86)] = *((char *)v85);
            v85 = &v7;
            v86 = &(2)[v86];
        }
    }
    else
    {
        v36 = &g_4c9fc.field_-4;
        v37 = &v17;
        v38 = 3;
        while (true)
        {
            v39 = *((int *)&(&v25->field_0)[*((short *)(v36 - 2))]);
            v40 = *((long long *)&(&v25->field_0)[*((short *)(v36 - 4))]);
            if ((unsigned int)v39 && (unsigned int)v39 < 0x400)
            {
                v9 = v39;
                if ((char)readable(v40, v39 * *((short *)v36)))
                    $_Assign_counted_range@PEBE@?$vector@EV?$allocator@E@std(v37, v40, v9 * *((short *)v36));
            }
            v37 = &v19;
            v36 = &(8)[v36];
            v38 -= 1;
        }
    }
}


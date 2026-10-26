// v20 RVA 00ecf0..00f6b2  (2498 bytes)  host_map_frame
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_-8;
    char field_0;
} struct_0;

typedef struct struct_2 {
    struct struct_2 *field_-8;
} struct_2;

typedef struct struct_1 {
    struct struct_1 *field_0;
    char paddin8[16];
    unsigned long long field_18;
} struct_1;

typedef struct struct_3 {
    struct struct_3 *field_0;
    char paddin8[8];
    unsigned long long field_10;
    unsigned long long field_18;
} struct_3;

typedef struct WIN32_FIND_DATAA {
    unsigned int dwFileAttributes;
    FILETIME ftCreationTime;
    char ftLastAccessTime;
    char paddin13[7];
    char ftLastWriteTime;
    char paddin21[7];
    unsigned int nFileSizeHigh;
    unsigned int nFileSizeLow;
    unsigned int dwReserved0;
    unsigned int dwReserved1;
    char cFileName[260];
    char cAlternateFileName[14];
} WIN32_FIND_DATAA;

typedef struct struct_4 {
    unsigned long long field_-8;
} struct_4;

typedef struct FILETIME {
    unsigned int dwLowDateTime;
    unsigned int dwHighDateTime;
} FILETIME;


long long host_map_frame()
{
    char v0;  // [bp-0x3b8]
    void* v1;  // [bp-0x398], Other Possible Types: struct_1 *
    char v2;  // [bp-0x388]
    unsigned int v3;  // [bp-0x384]
    struct_0 *v4;  // [bp-0x380], Other Possible Types: unsigned long
    struct_0 *v5;  // [bp-0x378], Other Possible Types: unsigned long
    void* v6;  // [bp-0x370], Other Possible Types: unsigned long
    unsigned int v7;  // [bp-0x368]
    unsigned int v8;  // [bp-0x364]
    void* v9;  // [bp-0x360], Other Possible Types: struct struct_0 **
    struct_0 *v10;  // [bp-0x358]
    void* v11;  // [bp-0x348]
    unsigned long v12;  // [bp-0x340]
    unsigned long v13;  // [bp-0x338]
    struct_0 *v14;  // [bp-0x330]
    struct struct_0 **v15;  // [bp-0x320], Other Possible Types: unsigned long
    void* v16;  // [bp-0x318], Other Possible Types: unsigned long long
    struct_0 *v17;  // [bp-0x310]
    struct_0 *v18;  // [bp-0x300]
    struct_0 *v20;  // [bp-0x2f0]
    struct_0 *v21;  // [bp-0x2e0]
    struct_0 *v22;  // [bp-0x2d0]
    struct_0 *v23;  // [bp-0x2c0]
    struct_0 *v24;  // [bp-0x2b0]
    struct_0 *v25;  // [bp-0x2a0]
    char v26;  // [bp-0x290]
    char v27;  // [bp-0x270]
    char v28;  // [bp-0x250]
    char v29;  // [bp-0x230]
    char v30;  // [bp-0x210]
    char v31;  // [bp-0x1f0]
    char v32;  // [bp-0x1d0]
    char v33;  // [bp-0x1b0]
    WIN32_FIND_DATAA v34;  // [bp-0x188]
    char v35;  // [bp-0x15c]
    unsigned long long v36;  // [bp-0x48]
    void* v38;  // rsi, Other Possible Types: struct_3 *, unsigned long
    void* v39;  // rdi, Other Possible Types: unsigned long long, unsigned long
    unsigned long long v40;  // rbx
    void* v41;  // cc_dep1, Other Possible Types: unsigned long
    unsigned long long v42;  // cc_dep2, Other Possible Types: unsigned long
    unsigned long v43;  // rax
    unsigned long v44;  // rax
    struct struct_0 **v45;  // rax, Other Possible Types: unsigned long
    char v46;  // r9b
    struct struct_0 **v47;  // rcx
    struct struct_0 **v48;  // rdx
    unsigned long v49;  // rax
    char v50;  // r8b, Other Possible Types: unsigned long
    unsigned long long v51;  // rcx
    struct struct_0 **v52;  // r8, Other Possible Types: unsigned long
    struct struct_0 **v53;  // rcx, Other Possible Types: unsigned long
    char *v55;  // r9, Other Possible Types: unsigned long
    struct struct_0 **v56;  // rax, Other Possible Types: unsigned long
    unsigned long long v57;  // rdx
    struct_2 *v58;  // rax
    struct struct_0 **v59;  // rax, Other Possible Types: unsigned long
    unsigned long long v60;  // rax
    struct struct_0 **v61;  // rbx, Other Possible Types: unsigned long long, unsigned long
    unsigned long long v62;  // rcx
    unsigned long long v63;  // r14, Other Possible Types: unsigned long
    struct_3 *v64;  // rax, Other Possible Types: unsigned long
    unsigned long long v65;  // r13
    unsigned long long v66;  // r12, Other Possible Types: unsigned long
    struct_3 *v67;  // r15, Other Possible Types: unsigned long
    unsigned long long v68;  // rax
    unsigned long long v71;  // rax
    unsigned long long v72;  // r8
    unsigned long long v73;  // rax
    unsigned long v74;  // rdx, Other Possible Types: unsigned long long
    unsigned long long v75;  // r15
    unsigned long v76;  // r12, Other Possible Types: unsigned long long
    struct_2 *v77;  // rcx, Other Possible Types: struct_3 *, unsigned long
    struct_3 *v78;  // r8, Other Possible Types: unsigned long
    struct_3 *v79;  // rbx, Other Possible Types: unsigned long
    char v80;  // al
    unsigned long long v82;  // rdx
    struct_1 *v83;  // rcx, Other Possible Types: unsigned long
    unsigned long long v84[4];  // r9
    unsigned long long v85;  // r9
    struct_3 *v86;  // rdx
    struct_3 *v87;  // rdx
    unsigned long long v88;  // r8
    unsigned long v89;  // rax
    unsigned long long v90;  // rcx
    unsigned long long v91;  // rbx
    struct_1 *v92;  // rax, Other Possible Types: unsigned long
    unsigned int *v93;  // rsi
    unsigned long long v94;  // rax
    char v95;  // bl
    struct_1 *v96;  // rax, Other Possible Types: unsigned long
    struct_0 *v97;  // rdx
    unsigned long long v98;  // r8
    struct_4 *v99;  // rcx
    struct_0 *v100;  // rax
    struct_0 *v101;  // rax
    unsigned long long v102;  // rcx
    unsigned int v103;  // r10d
    char *v104;  // rcx, Other Possible Types: unsigned long
    struct_0 *v105;  // rcx

    v36 = g_5a380 ^ &v0;
    v38 = 0;
    v39 = 0;
    *((char **)&v3) = 0;
    v10 = 0;
    v11 = 0;
    v40 = GetCommandLineA();
    v20 = 0;
    v21 = 0;
    $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v20, v40, strlen(v40));
    v14 = 0;
    v15 = 0;
    v16 = 0;
    v15 = 0;
    v63 = 15;
    v16 = 15;
    v14 = 0;
    v61 = 0;
    v75 = 1;
    v41 = 0;
    v42 = v21;
    do
    {
        if (v41 != v42)
        {
            v43 = &v20;
            if ((&v21)[1] > 15)
                v43 = v20;
            if (*((char *)(v43 + v61)) == 44)
                goto LABEL_ee10;
            v44 = &v20;
            if ((&v21)[1] > 15)
                v44 = v20;
            v46 = *((char *)(v44 + v61));
            v47 = v15;
            if (v47 >= v16)
            {
                v50 = 0;
                $_Reallocate_grow_by@V<lambda_65e615be2a453ca0576c979606f46740>$_Reallocate_grow_by@V(&v14, 1, v50);
                goto LABEL_ef61;
            }
            else
            {
                v15 = v47 + 1;
                v49 = &v14;
                if (v16 > 15)
                    v49 = v14;
                *(v49 + (char *)v47) = v46;
                *((char **)(v49 + (char *)v47 + 1)) = 0;
                goto LABEL_ef61;
            }
        }
LABEL_ee10:
        v45 = &v14;
        if (v16 > 15)
            v45 = v14;
        v48 = v15;
        if (!(v48) || !((v51 = v48 + v45, v52 = v45, v45 < v51)))
        {
            v50 = -1;
            goto LABEL_ee43;
        }
        do
        {
            if (*((char *)v52) != 32)
            {
                v50 = v52 - v45;
                goto LABEL_ee43;
            }
        } while ((v52 += 1, v52 < v51));
LABEL_ee43:
        v53 = &v14;
        if (v16 > 15)
            v53 = v14;
        if (v48)
        {
            v55 = (-1 < -1 ? -1 : v48 - 1) + v53;
            while (true)
            {
                if (*((char *)v55) != 32)
                {
                    break;
                }
                else if (v55 != v53)
                {
                    v55 -= 1;
                }
                else
                {
                    break;
                }
            }
        }
        if (v50 != -1)
        {
            v56 = substr@?$basic_string@DU?$char_traits@D@std@?$basic_string@DU?$char_traits@D@std(&v14, &v17);
            v39 = (unsigned int)v39 | 2;
        }
        else
        {
            v24 = 0;
            v25 = 0;
            $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v24, &g_45ce6, strlen(&g_45ce6));
            v56 = &v24;
            v39 = (unsigned int)v39 | 1;
        }
        v3 = v39;
        ammo_check(&v10, v56);
        if (((char)v39 & 2))
        {
            v39 = (unsigned int)v39 & -3;
            v3 = v39;
            v57 = (&v18)[1];
            if (v57 <= 15)
            {
LABEL_ef2f:
                *((int128_t *)&v18) = 0xf0000000000000000;
                v17 = 0;
                goto LABEL_ef38;
            }
            v74 = &(1)[v57];
            v77 = v17;
            v58 = v77;
            if (v74 < __Eg_my_units || (v74 += 39, v77 = *((long long *)(v77 - 8)), v58 - v77 - 8 <= 31))
            {
                sub_4030c(v77, v74);
                goto LABEL_ef2f;
            }
            v1 = 0;
            v64 = _invoke_watson(0, 0, 0, 0);
            do
            {
LABEL_f07a:
                v64 = v76 + v74;
                v77 = v64;
                if (*((long long *)&(24)[v64]) > 15)
                {
                    v77 = *((long long *)v64);
                    v78 = v77;
                    v79 = v77;
                }
                else
                {
                    v79 = v64;
                    v78 = v64;
                }
                v12 = *((long long *)&(16)[v64]) + v78;
                if (v77 != v12)
                {
                    while (true)
                    {
                        v8 = *((char *)v79);
                        if (!isalnum(v8) && v80 != 95 && v80 != 58)
                            break;
                        v79 = &(1)[v79];
                        if (v79 == v12)
                            v74 = v10;
                    }
                }
            } while ((v52 += 1, v52 < v51));
            v66 = v13;
LABEL_f106:
            v82 = v10;
            v83 = v82 - 32 + v63 * 32;
            if (*((long long *)(v83 + 24)) > 15)
                v83 = v83->field_0;
            v84 = (v63 - 2) * 32 + v82;
            if (*((long long *)(v84 + 24)) > 15)
                v85 = v84[0];
            v1 = v83;
            $sprintf_s@$0EA(&g_5bf40, "%s, %s, %s");
            v22 = 0;
            v23 = 0;
            v86 = v38;
            if (*((long long *)&(24)[v38]) > 15)
                v87 = *((long long *)v38);
            v88 = *((long long *)&(16)[v38]);
            $_Construct@$01PEBD@?$basic_string@DU?$char_traits@D@std(&v22);
            v89 = &v22;
            if ((&v23)[1] > 15)
                v89 = v22;
            *((char **)(v89 + (char *)v9)) = 95;
            v24 = 0;
            v25 = 0;
            v90 = &g_5b920;
            $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(&v24, &g_5b920, strlen());
            $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v26, $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v27, $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v28, $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v29, $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v30, &v24, "\\"), v65), "\\"), v66), "\\");
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v27);
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v28);
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v29);
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v30);
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v24);
            v4 = 0;
            v6 = 0;
            v4 = sub_402d0(4);
            v5 = v4;
            v6 = v4 + 4;
            v9 = &v4;
            v91 = v4;
            memset(v91, 0, 4);
            v5 = &(4)[v91];
            v9 = 0;
            *((char **)&v7) = 0;
            (unsigned int)v92 = $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v31, substr@?$basic_string@DU?$char_traits@D@std@?$basic_string@DU?$char_traits@D@std(&v32, &v26, &v22), "_*.damsav2");
            if (*((long long *)(v92 + 24)) > 15)
                v92 = v92->field_0;
            v93 = FindFirstFileA(v92, &v34);
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v31);
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v32);
            if (v93 != -1)
            {
                do
                {
                    v17 = 0;
                    v18 = 0;
                    (unsigned int)v94 = strlen(&v35);
                    v95 = v94;
                    if (v94 <= 120)
                    {
                        (unsigned int)v96 = $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(&v33, &v26, &v35);
                        v39 = (unsigned int)v39 | 4;
                        v3 = v39;
                        if (*((long long *)(v96 + 24)) > 15)
                            v96 = v96->field_0;
                        if (!(!(char)read_file(v96, &v17)) && !((*((char *)&v63) = 0, (&v17)[1] - v4 - v17 + v5 > 0x3000000)))
                            goto LABEL_f3e4;
                    }
                    *((char *)&v63) = 1;
LABEL_f3e4:
                    if (((char)v39 & 4))
                    {
                        v39 = (unsigned int)v39 & -5;
                        v3 = v39;
                        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v33);
                    }
                    if (!(char)v63)
                    {
                        v2 = v95;
                        v97 = v5;
                        if (v97 == v6)
                        {
                            $_Emplace_reallocate@E@?$vector@EV?$allocator@E@std(&v4);
                        }
                        else
                        {
                            *((char *)&v97->field_-8) = v95;
                            v5 = &(1)[v5];
                        }
                        $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v4);
                        v8 = (int)(&v17)[1] - (int)v17;
                        $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v4, v5, &v8, 4);
                        v98 = v17;
                        $_Insert_counted_range@PEAE@?$vector@EV?$allocator@E@std(&v4, v5, v98, (char *)(&v17)[1] - v98);
                        *((char **)&v7) = &(1)[v7];
                    }
                    v99 = v17;
                    if (v99)
                    {
                        if (v18 - v99 < __Eg_my_units || -8 + (char *)v99 - *((long long *)(v99 - 8)) <= 31)
                        {
                            sub_4030c();
                            v18 = 0;
                            v17 = 0;
                        }
LABEL_f5da:
                        v1 = 0;
                        _invoke_watson(0, 0, 0, 0);
LABEL_f5f0:
                        sub_4030c();
                        v4 = 0;
                        v6 = 0;
                    }
                } while (FindNextFileA(v93, &v34));
                FindClose(v93);
            }
            memcpy(v4, &v7, 4);
            v100 = g_5b820;
            g_5b820 = v4;
            v4 = v100;
            v101 = g_5b828;
            g_5b828 = v5;
            v5 = v101;
            v102 = g_5b830;
            g_5b830 = v6;
            v6 = v102;
            v103 = 2166136261;
            v104 = 0;
            if (g_5b828 != g_5b820)
            {
                do
                {
                    v103 = (*(v104 + (char *)&g_5b820->field_-8) ^ v103) * 16777619;
                    v104 = &(1)[v104];
                } while (v104 < g_5b828 - g_5b820);
            }
            g_5bf2c = v103;
            mp_log("net: this battle belongs to the operation %s: %u files of what earlier battles left behind (%zu bytes) go to a game that joins", &g_5bf40, v7, g_5b828 - g_5b820);
            v105 = v4;
            if (!(v6 - (char *)v105 >= __Eg_my_units) || !(&(&v105->field_0)[-1 * *((long long *)(v105 - 8))] > 31))
                goto LABEL_f5f0;
            v1 = 0;
            _invoke_watson(0, 0, 0, 0);
            goto LABEL_f5da;
        }
LABEL_ef38:
        if (((char)v39 & 1))
        {
            v39 = (unsigned int)v39 & -2;
            v3 = v39;
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v24);
        }
        v15 = 0;
        v59 = &v14;
        if (v16 > 15)
            v59 = v14;
        *((char **)v59) = 0;
LABEL_ef61:
        v61 = &(1)[v61];
        v60 = v21;
        v41 = v61;
        v42 = v60;
    } while (v61 <= v60);
    v62 = v10;
    v63 = (char *)(&v10)[1] - v62 >> 5;
    v65 = (v63 - 5) * 32 + v62;
    v66 = (v63 - 4) * 32 + v62;
    v13 = v66;
    v38 = (v63 - 3) * 32 + v62;
    v67 = v38;
    if (*((long long *)(v38 + 24)) > 15)
        v67 = v38->field_0;
    v68 = *((long long *)(v38 + 16));
    v71 = strlen("*:*");
    v72 = *((long long *)(v38 + 16));
    v73 = v63 * 32;
    v74 = v10;
    v75 = v63 - 3;
    if (!(v75 < v63))
        goto LABEL_f106;
    v76 = v75 * 32;
    goto LABEL_f07a;
}


// v20 RVA 013a48..013bac  (356 bytes)  all_player_led@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    unsigned long long field_-20;
    char padding_-18[24];
    unsigned long long field_0;
    char paddin8[8];
    unsigned long long field_10;
    char field_18;
} struct_2;

typedef struct struct_1 {
    char paddin0[8];
    struct struct_0 *field_8;
    char paddin16[9];
    char field_19;
    char paddin26[2];
    unsigned int field_1c;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
    char paddin8[17];
    char field_19;
    char paddin26[2];
    unsigned int field_1c;
} struct_0;

typedef struct struct_3 {
    char paddin0[64];
    unsigned int field_40;
} struct_3;


int all_player_led@opsave()
{
    char v0;  // [bp-0x58]
    char v1;  // [bp-0x54]
    char v2;  // [bp-0x50]
    char v3;  // [bp-0x40]
    char v4;  // [bp-0x38]
    unsigned long v5;  // [bp-0x8]
    char v6;  // [bp+0x0]
    char v8;  // [bp+0x20]
    struct struct_1 **v9;  // rdx
    unsigned long long *v11;  // rcx
    unsigned long long *v12;  // rdi
    unsigned int v13;  // r15d
    void* v14;  // rsi
    unsigned long long v15;  // r14
    unsigned long long v16;  // rbx
    unsigned long long v17;  // rcx
    unsigned int v18;  // cc_dep1
    unsigned int v19;  // cc_dep2
    struct_0 *v20;  // rcx, Other Possible Types: struct_1 *, unsigned long
    struct_0 *v21;  // rax, Other Possible Types: unsigned long
    unsigned int v22;  // edx
    unsigned long long v23;  // r8
    struct_3 *v24;  // rcx
    unsigned long long v25;  // rdx
    unsigned long long v26;  // rcx, Other Possible Types: unsigned long
    unsigned long long v27;  // rsi
    struct_2 *v28;  // rbx, Other Possible Types: unsigned long
    unsigned long long v29;  // rbp

    v5 = &v6;
    v12 = v11;
    tagged@opsave(&v3, v11);
    v13 = user_side@opsave@opsave(v11, &v3);
    v14 = 0;
    v15 = *((long long *)&v4);
    v16 = *((long long *)&v3);
    if (v16 == v15)
    {
        _Tidy@?$vector@UTagged@opsave@?$vector@UTagged@opsave(&v3);
        v29 = v5;
        return;
    }
    v28 = &(64)[v16];
    do
    {
        if (!(24)[v28] && *((long long *)&(16)[v28]))
        {
            v17 = *((long long *)v28);
            if (!v17 || !v13)
            {
                memcpy(&v0, *((long long *)(v28 - 32)) + *(v12), 4);
                v18 = *((int *)&v0);
                v19 = 1380275029;
            }
            else
            {
                memcpy(&v8, *(v12) + v17, 4);
                v18 = *((int *)&v8);
                v19 = v13;
            }
            if (v18 == v19)
            {
                memcpy(&v1, *(v12) + *((long long *)&(16)[v28]), 4);
                v20 = *(v9);
                v21 = *((long long *)(v20 + 8));
                for (v22 = *((int *)&v1); !(25)[v21]; v21 = *((long long *)v21))
                {
                    if (*((int *)&(28)[v21]) >= v22)
                        v20 = v21;
                    else
                        v21 = &(16)[v21];
                }
                if (!(25)[v20] && v22 >= *((int *)&(28)[v20]))
                {
                    v24 = *((long long *)$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(v23, &v2, v28 - 64));
                    v25 = *((int *)(v24 + 64));
                    *((unsigned long *)(v24 + 64)) = (unsigned int)v25 | 2;
                    v26 = v14 + 1;
                    if (((char)v25 & 2))
                        v26 = v14;
                    v27 = v26;
                }
            }
        }
    } while ((v28 += 96, v28 - 64 != v15));
}


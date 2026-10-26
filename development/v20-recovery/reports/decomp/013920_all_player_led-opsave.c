// v20 RVA 013920..013a48  (296 bytes)  all_player_led@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    unsigned long long field_-20;
    char padding_-18[24];
    unsigned long long field_0;
    unsigned long long field_8;
    char paddin16[8];
    char field_18;
} struct_1;

typedef struct struct_0 {
    char paddin0[64];
    unsigned int field_40;
} struct_0;


int all_player_led@opsave()
{
    char v0;  // [bp-0x68]
    char v1;  // [bp-0x60]
    char v2;  // [bp-0x50]
    char v3;  // [bp-0x48]
    unsigned long v4;  // [bp-0x8]
    char v5;  // [bp+0x0]
    char v7;  // [bp+0x18]
    char v8;  // [bp+0x20]
    unsigned long v9;  // rdx
    unsigned long long *v11;  // rcx
    unsigned long long *v12;  // rdi
    unsigned int v13;  // r15d
    unsigned long long v14;  // r14
    unsigned long long v15;  // rbx
    unsigned long long v16;  // rcx
    unsigned int v17;  // cc_dep1
    unsigned int v18;  // cc_dep2
    struct_0 *v19;  // rcx
    unsigned long long v20;  // rdx
    unsigned long long v21;  // rcx, Other Possible Types: unsigned long
    void* v22;  // rsi, Other Possible Types: unsigned long, unsigned long long
    struct_1 *v23;  // rbx, Other Possible Types: unsigned long
    unsigned long long v24;  // rbp

    v4 = &v5;
    v12 = v11;
    tagged@opsave(&v2, v11);
    v13 = user_side@opsave@opsave(v11, &v2);
    v22 = 0;
    v14 = *((long long *)&v3);
    v15 = *((long long *)&v2);
    if (v15 == v14)
    {
        _Tidy@?$vector@UTagged@opsave@?$vector@UTagged@opsave(&v2);
        v24 = v4;
        return;
    }
    v23 = &(64)[v15];
    do
    {
        if (!(24)[v23] && *((long long *)&(8)[v23]))
        {
            v16 = *((long long *)v23);
            if (!v16 || !v13)
            {
                memcpy(&v8, *((long long *)(v23 - 32)) + *(v12), 4);
                v17 = *((int *)&v8);
                v18 = 1380275029;
            }
            else
            {
                memcpy(&v7, *(v12) + v16, 4);
                v17 = *((int *)&v7);
                v18 = v13;
            }
            if (v17 == v18)
            {
                memcpy(&v0, *((long long *)&(8)[v23]) + *(v12), 4);
                if (((short)*((int *)&v0) & 8208))
                {
                    v19 = *((long long *)$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std$_Try_emplace@AEBV?$basic_string@DU?$char_traits@D@std(v9, &v1, v23 - 64));
                    v20 = *((int *)(v19 + 64));
                    *((unsigned long *)(v19 + 64)) = (unsigned int)v20 | 2;
                    v21 = &(1)[v22];
                    if (((char)v20 & 2))
                        v21 = v22;
                    v22 = v21;
                }
            }
        }
    } while ((v23 += 96, v23 - 64 != v14));
}


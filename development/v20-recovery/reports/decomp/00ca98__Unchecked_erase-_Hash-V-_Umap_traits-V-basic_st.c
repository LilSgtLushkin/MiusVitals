// v20 RVA 00ca98..00cc3e  (422 bytes)  _Unchecked_erase@?$_Hash@V?$_Umap_traits@V?$basic_string@DU?$char_traits@D@std
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[16];
    unsigned long long field_10;
    struct struct_1 *field_18;
    char paddin32[16];
    unsigned long long field_30;
} struct_0;

typedef struct struct_2 {
    struct struct_2 *field_0;
    struct struct_1 *field_8;
    unsigned long long field_10;
    char paddin24[8];
    unsigned long long field_20;
    unsigned long long field_28;
} struct_2;

typedef struct struct_1 {
    unsigned long long field_0;
} struct_1;


long long _Unchecked_erase@?$_Hash@V?$_Umap_traits@V?$basic_string@DU?$char_traits@D@std(struct_0 *a0, struct_2 *a1, unsigned long a2)
{
    unsigned long long *v0;  // [bp-0x68]
    struct struct_1 **v1;  // [bp-0x60]
    unsigned long long *v2;  // [bp-0x58]
    struct_2 *v3;  // [bp-0x50]
    struct_0 *v4;  // [bp+0x8]
    unsigned long v5;  // [bp+0x10]
    unsigned long v6;  // [bp+0x18]
    unsigned long long *v7;  // [bp+0x20]
    struct struct_1 **v9;  // rax
    unsigned long long *v10;  // r13
    unsigned long long *v11;  // r15
    unsigned long long v12;  // rcx, Other Possible Types: unsigned long
    char v13;  // dil
    struct_2 *v14;  // rbx
    struct_2 *v15;  // rsi, Other Possible Types: unsigned long
    struct_0 *v16;  // rbx, Other Possible Types: unsigned long
    unsigned long long v17;  // rax, Other Possible Types: unsigned long
    unsigned long long *v18;  // rax, Other Possible Types: unsigned long
    unsigned long long v19;  // rcx, Other Possible Types: unsigned long
    unsigned long v20;  // r12
    char v21;  // dil
    struct_2 *v22;  // rbx
    unsigned long long *v23;  // rax

    v4 = a0;
    if (a1 == a2)
        return a2;
    v9 = a0 + 8;
    v10 = *(v9);
    v11 = *((long long *)(a0 + 24));
    v1 = v9;
    v0 = *((long long *)(a1 + 8));
    v2 = v0;
    v15 = a1;
    v3 = a1;
    v7 = *((long long *)(a1 + 8));
    v12 = a1 + 16;
    if (*((long long *)(a1 + 40)) > 15)
        v12 = *((long long *)(a1 + 16));
    v20 = (sub_6168(v12, *((long long *)(a1 + 32))) & *((long long *)(a0 + 48))) * 2;
    v5 = *((long long *)((char *)v11 + v20 * 8));
    v17 = *((long long *)((char *)v11 + v20 * 8 + 8));
    v6 = v17;
    do
    {
        v13 = v15 == v17;
        v14 = v15;
        v15 = *((long long *)v15);
        v3 = v15;
        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(v14 + 16);
        sub_4030c(v14, 48);
        v16 = v4;
        *((char **)(v16 + 16)) = *((long long *)(v16 + 16)) - 1;
        if (v13)
        {
            if (v5 == a1)
            {
                *((unsigned long long **)((char *)v11 + v20 * 8)) = v10;
                v18 = v10;
            }
            else
            {
                v18 = v7;
            }
            *((unsigned long *)((char *)v11 + v20 * 8 + 8)) = v18;
            while (true)
            {
LABEL_cc33:
                if (v15 == a2)
                {
                    v23 = v0;
                    *(v23) = v15;
                    *((unsigned long long **)&(8)[v15]) = v23;
                    return a2;
                }
                v19 = &(16)[v15];
                if (*((long long *)&(40)[v15]) > 15)
                    v19 = *((long long *)&(16)[v15]);
                v20 = (sub_6168(v19, *((long long *)&(32)[v15])) & *((long long *)&(48)[v16])) * 2;
                do
                {
                    v21 = v15 == *((long long *)((char *)v11 + v20 * 8 + 8));
                    v22 = v15;
                    v15 = *((long long *)v15);
                    v3 = v15;
                    _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(v22 + 16);
                    sub_4030c(v22, 48);
                    v16 = v4;
                    *((char **)(v16 + 16)) = *((long long *)(v16 + 16)) - 1;
                    if (v21)
                    {
                        *((unsigned long long **)((char *)v11 + v20 * 8)) = v10;
                        *((unsigned long long **)((char *)v11 + v20 * 8 + 8)) = v10;
                        goto LABEL_cc33;
                    }
                } while (v15 != a2);
            }
        }
    } while ((v17 = v6, v15 != a2));
    if (v5 != a1)
    {
        v23 = v0;
        *(v23) = v15;
        *((unsigned long long **)&(8)[v15]) = v23;
        return a2;
    }
    *((unsigned long *)((char *)v11 + v20 * 8)) = v15;
    v23 = v0;
    *(v23) = v15;
    *((unsigned long long **)&(8)[v15]) = v23;
    return a2;
}


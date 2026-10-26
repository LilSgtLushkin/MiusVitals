// v20 RVA 007cb4..007e88  (468 bytes)  $_Try_emplace@V?$basic_string@DU?$char_traits@D@std
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_0;
    char field_8;
} struct_0;

typedef struct struct_1 {
    unsigned int field_0;
    char paddin4[4];
    unsigned long long field_8;
    unsigned long long field_10;
    struct struct_2 *field_18;
    char paddin32[16];
    struct struct_2 *field_30;
    unsigned long long field_38;
} struct_1;

typedef struct struct_4 {
    struct struct_4 *field_0;
    char paddin8[8];
    unsigned long long field_10;
    unsigned long long field_18;
} struct_4;

typedef struct struct_3 {
    char paddin0[8];
    struct struct_2 *field_8;
} struct_3;

typedef struct struct_2 {
    unsigned long long field_0;
} struct_2;


long long $_Try_emplace@V?$basic_string@DU?$char_traits@D@std(struct_1 *a0, struct_0 *a1, struct_4 *a2)
{
    unsigned long long v0[2];  // [bp-0x28]
    int tmp_3;  // tmp #3
    int tmp_5;  // tmp #5
    int tmp_4;  // tmp #4
    void* v1;  // [bp-0x20], Other Possible Types: char
    int v2;  // [bp-0x18], Other Possible Types: unsigned long long (64 bits)[2]
    char v3;  // [bp-0x10]
    struct_0 *v5;  // rbx
    struct_1 *v6;  // rdi
    struct_4 *v7;  // rcx, Other Possible Types: unsigned long
    unsigned long v8;  // r14
    unsigned long long v9;  // rcx
    unsigned long long v11[2];  // rcx
    struct_3 *v12;  // r8
    unsigned long long *v13;  // r9
    unsigned long long *v14;  // rax
    unsigned long v15;  // rdx
    unsigned long long v16;  // r10
    unsigned long long v17;  // rcx
    unsigned long long v18;  // rcx

    v5 = a1;
    v6 = a0;
    v7 = a2;
    if (*((long long *)(a2 + 24)) > 15)
        v7 = a2->field_0;
    v8 = sub_6168(v7, *((long long *)(a2 + 16)));
    $_Find_last@V?$basic_string@DU?$char_traits@D@std(v6, v2, a2, v8);
    v9 = *((long long *)&v3);
    if (v9)
    {
        a1->field_0 = v9;
        *((char **)(a1 + 8)) = 0;
        return a1;
    }
    if (*((long long *)(v6 + 16)) == 384307168202282325)
    {
        ?_Xlength_error@std@@YAXPEBD@Z("unordered_map/set too long");
        [D] Unsupported jumpkind Ijk_SigTRAP at address 32391
    }
    0NodeRef@opsave@opsave(v0, v6 + 8, a2);
    tmp_3 = DivV(tmp_5, tmp_4);
    if ((((char)(CmpF((unsigned long long)tmp_3, v6->field_0) & 69) | (char)((CmpF((unsigned long long)tmp_3, v6->field_0) & 69) >> 6)) & 1) != 1)
    {
        _Forced_rehash@?$_Hash@V?$_Umap_traits@V?$basic_string@DU?$char_traits@D@std@?$_Hash@V?$_Umap_traits@V?$basic_string@DU?$char_traits@D@std(v6, _Desired_grow_bucket_count@?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std(v6));
        *((int128_t *)&v2) = *((int128_t *)$_Find_last@V?$basic_string@DU?$char_traits@D@std(v6, v2, &(16)[*((long long *)&v1)], v8));
    }
    v11 = *((long long *)&v1);
    v1 = 0;
    v12 = v2[0];
    v13 = *((long long *)(v12 + 8));
    *((char **)(v6 + 16)) = &(1)[*((long long *)(v6 + 16))];
    v11[0] = v12;
    *((unsigned long long **)(v11 + 8)) = v13;
    *(v13) = v11;
    *((unsigned long long *[2])(v12 + 8)) = v11;
    v14 = (*((long long *)(v6 + 48)) & v8) * 2;
    v15 = *((long long *)(v6 + 24));
    v16 = *((long long *)(v15 + v14 * 8));
    if (v16 == *((long long *)(v6 + 8)))
    {
        *((unsigned long long *[2])(v15 + v14 * 8)) = v11;
        goto LABEL_7e29;
    }
    else if (v16 == v12)
    {
        *((unsigned long long *[2])(v15 + v14 * 8)) = v11;
    }
    else if (*((long long *)&(8)[(v14 << 3) + v15]) == v13)
    {
LABEL_7e29:
        *((unsigned long long *[2])&(8)[v14 * 8 + v15]) = v11;
    }
    v5->field_0 = v11;
    *((char **)(v5 + 8)) = 1;
    v17 = v1;
    if (v17)
        _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&(16)[v17]);
    v18 = v1;
    if (!v18)
        return v5;
    sub_4030c(v18, 48);
}


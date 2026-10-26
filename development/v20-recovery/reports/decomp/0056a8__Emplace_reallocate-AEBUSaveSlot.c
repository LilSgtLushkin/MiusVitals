// v20 RVA 0056a8..005877  (463 bytes)  $_Emplace_reallocate@AEBUSaveSlot
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    uint128_t field_0;
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    unsigned long long field_50;
} struct_0;


long long $_Emplace_reallocate@AEBUSaveSlot(unsigned long long a0[3], unsigned long a1, struct_0 *a2)
{
    void* v0;  // [bp-0x50], Other Possible Types: struct_0 *
    unsigned long v1;  // [bp-0x48]
    struct_0 *v2;  // [bp-0x40], Other Possible Types: unsigned long
    struct_0 *v3;  // [bp-0x38]
    unsigned long v5;  // r13
    unsigned long long v6[3];  // r14
    unsigned long long v9;  // rcx
    unsigned long long v10;  // rdx
    unsigned long long v11;  // r15
    unsigned long long v12;  // rdx
    unsigned long v13;  // rcx
    unsigned long v14;  // rcx
    unsigned long v15;  // rdi, Other Possible Types: unsigned long long
    struct_0 *v16;  // rbx, Other Possible Types: unsigned long
    struct_0 *v17;  // rax, Other Possible Types: unsigned long
    struct_0 *v18;  // rsi
    struct_0 *v19;  // rbp, Other Possible Types: unsigned long
    unsigned long v20;  // rdx
    unsigned long long v21;  // rcx, Other Possible Types: unsigned long

    v5 = a1;
    v6 = a0;
    v9 = *((long long *)(a0 + 8)) - a0[0];
    v10 = (3353953467947191203 * v9 >> 64 >> 4) + (3353953467947191203 * v9 >> 64 >> 4 >> 63);
    if (v10 == 209622091746699450)
        _Xlength@?$vector@EV?$allocator@E@std(v9, v10, 3353953467947191203, a1 - a0[0]); /* do not return */
    v11 = &(1)[v10];
    v12 = (3353953467947191203 * (*((long long *)(v6 + 16)) - v6[0]) >> 64 >> 4) + (3353953467947191203 * (*((long long *)(v6 + 16)) - v6[0]) >> 64 >> 4 >> 63);
    v13 = v12 / 2;
    if (v12 > 209622091746699450 - v13)
        $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(-0x10);
    v12 = (v11 <= v12 ? v11 : v13 + v12);
    if (v12 > 209622091746699450)
        _Throw_bad_array_new_length@std@std(); /* do not return */
    v14 = v12 * 88;
    v15 = v12;
    if (!v14)
    {
        v16 = 0;
    }
    else
    {
        if (v14 < __Eg_my_units)
            (unsigned int)v17 = sub_402d0();
        else
            v17 = $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(v14);
        v16 = v17;
    }
    v18 = ((3353953467947191203 * (a1 - a0[0]) >> 64 >> 4) + (3353953467947191203 * (a1 - a0[0]) >> 64 >> 4 >> 63)) * 88 + v16;
    v19 = v18 + 88;
    v0 = v16;
    v1 = v12;
    v2 = v19;
    v3 = v19;
    v18->field_0 = a2->field_0;
    *((int128_t *)(v18 + 16)) = *((int128_t *)(a2 + 16));
    *((int128_t *)(v18 + 32)) = *((int128_t *)(a2 + 32));
    *((int128_t *)(v18 + 48)) = *((int128_t *)(a2 + 48));
    *((int128_t *)(v18 + 64)) = *((int128_t *)(a2 + 64));
    *((long long *)(v18 + 80)) = *((long long *)(a2 + 80));
    v2 = v18;
    v20 = *((long long *)(v6 + 8));
    v21 = v6[0];
    if (v5 == v20)
    {
        v19 = v16;
    }
    else
    {
        $_Copy_memmove@PEAEPEAE@std(v21, v5, v16);
        v2 = v16;
        v21 = v5;
        v20 = *((long long *)(v6 + 8));
    }
    $_Copy_memmove@PEAEPEAE@std(v21, v20, v19);
    v0 = 0;
    _Change_array@?$vector@UShotRec@?$vector@UShotRec(v6, v16, v11, v15, v6);
    return v2;
}


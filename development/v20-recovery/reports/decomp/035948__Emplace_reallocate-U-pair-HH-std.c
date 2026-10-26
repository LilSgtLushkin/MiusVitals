// v20 RVA 035948..035ab0  (360 bytes)  $_Emplace_reallocate@U?$pair@HH@std
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long $_Emplace_reallocate@U?$pair@HH@std(unsigned long long a0[3], unsigned long a1, uint128_t a2[2])
{
    void* v0;  // [bp-0x50], Other Possible Types: uint128_t [2]
    unsigned long v1;  // [bp-0x48]
    uint128_t v2[2];  // [bp-0x40], Other Possible Types: unsigned long
    uint128_t v3[2];  // [bp-0x38]
    unsigned long v5;  // r15
    unsigned long long v6[3];  // rsi
    unsigned long long v8;  // rax
    unsigned long long v9;  // r12
    unsigned long long v10;  // rcx
    unsigned long v11;  // rdx
    unsigned long v12;  // rdi, Other Possible Types: unsigned long long
    uint128_t v13[2];  // rbx, Other Possible Types: unsigned long
    unsigned long v14;  // rcx
    uint128_t v15[2];  // rax, Other Possible Types: unsigned long
    uint128_t v16[2];  // r14
    uint128_t v17[2];  // rbp, Other Possible Types: unsigned long
    unsigned long v18;  // rdx
    unsigned long long v19;  // rcx, Other Possible Types: unsigned long

    v5 = a1;
    v6 = a0;
    v8 = *((long long *)(a0 + 8)) - a0[0] >> 5;
    if (v8 == 576460752303423487)
        _Xlength@?$vector@EV?$allocator@E@std(); /* do not return */
    v9 = &(1)[v8];
    v10 = *((long long *)(a0 + 16)) - a0[0] >> 5;
    v11 = v10 / 2;
    if (v10 > 576460752303423487 - v11)
        $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(-0x20);
    v11 = (v9 <= v11 ? v9 : v11 + v10);
    if (v11 > 576460752303423487)
        _Throw_bad_array_new_length@std@std(); /* do not return */
    v14 = v11 * 32;
    v12 = v11;
    if (!v14)
    {
        v13 = 0;
    }
    else
    {
        if (v14 < __Eg_my_units)
            (unsigned int)v15 = sub_402d0();
        else
            v15 = $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(v14);
        v13 = v15;
    }
    v16 = (a1 - a0[0] & -0x20) + v13;
    v17 = v16 + 32;
    *((unsigned long *)&v0[0]) = v13;
    v1 = v11;
    *((uint128_t *[2])&v2[0]) = v17;
    *((uint128_t *[2])&v3[0]) = v17;
    v16[0] = a2[0];
    *((int128_t *)(v16 + 16)) = *((int128_t *)(a2 + 16));
    *((uint128_t *[2])&v2[0]) = v16;
    v18 = *((long long *)(v6 + 8));
    v19 = v6[0];
    if (v5 == v18)
    {
        v17 = v13;
    }
    else
    {
        $_Copy_memmove@PEAEPEAE@std(v19, v5, v13);
        v2 = v13;
        v19 = v5;
        v18 = *((long long *)(v6 + 8));
    }
    $_Copy_memmove@PEAEPEAE@std(v19, v18, v17);
    v0 = 0;
    _Change_array@?$vector@U?$pair@HH@std@?$vector@U?$pair@HH@std(v6, v13, v9, v12, v6);
    return v2;
}


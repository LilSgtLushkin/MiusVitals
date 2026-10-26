// v20 RVA 005b30..005cd2  (418 bytes)  $_Emplace_reallocate@USent
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    uint128_t field_0;
    unsigned long long field_10;
    unsigned long long field_18;
} struct_0;

typedef struct struct_1 {
    char field_0;
    char paddin1[15];
    unsigned long long field_10;
    unsigned long long field_18;
} struct_1;


long long $_Emplace_reallocate@USent(unsigned long long a0[3], unsigned long a1, struct_1 *a2)
{
    char v0;  // [bp-0x58]
    void* v1;  // [bp-0x50], Other Possible Types: struct_0 *
    unsigned long v2;  // [bp-0x48]
    struct_0 *v3;  // [bp-0x40], Other Possible Types: char, unsigned long
    struct_0 *v4;  // [bp-0x38]
    unsigned long long v6;  // r15
    unsigned long long v7[3];  // rdi
    unsigned long long v9;  // rax
    unsigned long long v10;  // r12
    unsigned long long v11;  // rcx
    unsigned long v12;  // rdx
    struct_0 *v14;  // rbx, Other Possible Types: unsigned long
    unsigned long v15;  // rcx
    struct_0 *v16;  // rax, Other Possible Types: unsigned long
    struct_0 *v17;  // r14

    v6 = a1;
    v7 = a0;
    v9 = *((long long *)(a0 + 8)) - a0[0] >> 5;
    if (v9 == 576460752303423487)
        _Xlength@?$vector@EV?$allocator@E@std(); /* do not return */
    v10 = &(1)[v9];
    v11 = *((long long *)(a0 + 16)) - a0[0] >> 5;
    v12 = v11 / 2;
    if (v11 > 576460752303423487 - v12)
        $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(-0x20);
    v12 = (v10 <= v12 ? v10 : v12 + v11);
    if (v12 > 576460752303423487)
        _Throw_bad_array_new_length@std@std(); /* do not return */
    v15 = v12 * 32;
    if (!v15)
    {
        v14 = 0;
    }
    else
    {
        if (v15 < __Eg_my_units)
            (unsigned int)v16 = sub_402d0();
        else
            v16 = $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(v15);
        v14 = v16;
    }
    v17 = (a1 - a0[0] & -0x20) + v14;
    v1 = v14;
    v2 = v12;
    v3 = &(32)[*((long long *)&v3)];
    v4 = v3;
    *((char **)*((long long *)&v3)) = 0;
    *((char **)&(16)[*((long long *)&v3)]) = 0;
    *((char **)&(24)[*((long long *)&v3)]) = 0;
    memcpy(*((long long *)&v3), a2, 32);
    *((char **)(a2 + 16)) = 0;
    *((char **)(a2 + 24)) = 15;
    *((char **)&a2->field_0) = 0;
    v3 = v17;
    if (v6 == *((long long *)(v7 + 8)))
    {
        $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std();
        v1 = 0;
        _Change_array@?$vector@USent@?$vector@USent(v7, v14, v10, v12, v7);
        1_Reallocation_guard@?$vector@USent@?$vector@USent(&v0);
        return *((long long *)&v3);
    }
    $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(v7[0], v6, v14, v7);
    v3 = v14;
}


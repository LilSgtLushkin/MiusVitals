// v20 RVA 039ee0..039ff4  (276 bytes)  $_Resize_reallocate@U_Value_init_tag@std
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_0;
    unsigned int field_8;
    char paddin12[4];
    unsigned long long field_10;
} struct_0;


long long $_Resize_reallocate@U_Value_init_tag@std(unsigned long long a0[3], unsigned long a1, char *a2)
{
    unsigned long long v0[3];  // [bp-0x58]
    void* v1;  // [bp-0x50], Other Possible Types: unsigned long
    unsigned long v2;  // [bp-0x48]
    unsigned long long v3;  // [bp-0x40]
    unsigned long v4;  // [bp-0x38]
    unsigned long v6;  // rbp
    struct_0 *v7;  // r15
    unsigned long long v8;  // r12
    unsigned long long v9;  // rcx
    unsigned long v10;  // rdx
    unsigned long v11;  // rsi
    unsigned long long v12;  // rax, Other Possible Types: unsigned long
    unsigned long long v14;  // rbx

    v6 = a1;
    v7 = a0;
    if (a1 > 9223372036854775807)
        _Xlength@?$vector@EV?$allocator@E@std(); /* do not return */
    v8 = *((long long *)(a0 + 8)) - a0[0];
    v9 = *((long long *)(a0 + 16)) - a0[0];
    v10 = v9 / 2;
    if (v9 <= 9223372036854775807 - v10)
    {
        v11 = v10 + v9;
        if (v11 < v6)
            v11 = v6;
        if (!v11)
        {
LABEL_39f47:
            v0[0] = v7;
            v1 = 0;
            v2 = v11;
            v3 = &(0)[v8];
            v4 = v3;
            memset(v3, *(a2), v6 - v8);
        }
        else if (v11 >= __Eg_my_units)
        {
            $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(v11);
        }
        else
        {
            (unsigned int)v12 = sub_402d0(v11);
            v0[0] = v7;
            v1 = v12;
            v2 = v11;
            v3 = v12 + v8;
            v4 = v3;
            v14 = v6 - v8;
            memset(v3, *(a2), v14);
            v4 = v14 + v3;
            $_Copy_memmove@PEAEPEAE@std(v7->field_0, *((long long *)(v7 + 8)), v12);
            v1 = 0;
            return _Change_array@?$vector@EV?$allocator@E@std(v7, v12, v6, v11);
        }
    }
    else
    {
        v12 = $_Allocate_manually_vector_aligned@U_Default_allocate_traits@std(9223372036854775807);
        goto LABEL_39f47;
    }
}


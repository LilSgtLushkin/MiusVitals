// v20 RVA 035f10..035fb1  (161 bytes)  _Change_array@?$vector@U?$pair@HH@std
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    struct struct_0 *field_0;
    unsigned long long field_8;
    unsigned long long field_10;
} struct_1;

typedef struct struct_0 {
    unsigned long long field_-8;
} struct_0;


long long _Change_array@?$vector@U?$pair@HH@std(struct_1 *a0, unsigned long a1, unsigned long a2, unsigned long a3)
{
    void* v0;  // [bp-0x18]
    struct_1 *v2;  // rsi
    struct_0 *v3;  // rcx

    v2 = a0;
    v3 = a0->field_0;
    if (v3 && (*((long long *)(v2 + 16)) - (char *)v3 & -0x20) >= __Eg_my_units && -8 + (char *)v3 - *((long long *)(v3 - 8)) > 31)
    {
        v0 = 0;
        _invoke_watson(0, 0, 0, 0);
        [D] Unsupported jumpkind Ijk_SigTRAP at address 221104
    }
    v2->field_0 = a1;
    *((unsigned long *)(v2 + 8)) = a2 * 32 + a1;
    *((unsigned long *)(v2 + 16)) = a3 * 32 + a1;
    return sub_4030c();
}


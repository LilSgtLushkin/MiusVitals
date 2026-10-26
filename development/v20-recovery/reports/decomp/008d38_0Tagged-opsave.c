// v20 RVA 008d38..008dc3  (139 bytes)  0Tagged@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    uint128_t field_0;
    unsigned long long field_10;
    unsigned long long field_18;
    unsigned long long field_20;
    unsigned long long field_28;
    unsigned long long field_30;
    unsigned long long field_38;
    unsigned long long field_40;
    unsigned long long field_48;
    unsigned long long field_50;
    char field_58;
} struct_0;

typedef struct struct_1 {
    char paddin0[24];
    unsigned long long field_18;
    unsigned long long field_20;
    unsigned long long field_28;
    unsigned long long field_30;
    unsigned long long field_38;
    unsigned long long field_40;
    unsigned long long field_48;
    unsigned long long field_50;
    char field_58;
} struct_1;


long long 0Tagged@opsave(struct_0 *a0, struct_1 *a1)
{
    struct_0 *v0;  // [bp+0x8]

    v0 = a0;
    *((char **)&a0->field_0) = 0;
    *((char **)(a0 + 16)) = 0;
    *((char **)(a0 + 24)) = 0;
    $_Construct@$01PEBD@?$basic_string@DU?$char_traits@D@std();
    *((long long *)(a0 + 32)) = *((long long *)(a1 + 32));
    *((long long *)(a0 + 40)) = *((long long *)(a1 + 40));
    *((long long *)(a0 + 48)) = *((long long *)(a1 + 48));
    *((long long *)(a0 + 56)) = *((long long *)(a1 + 56));
    *((long long *)(a0 + 64)) = *((long long *)(a1 + 64));
    *((long long *)(a0 + 72)) = *((long long *)(a1 + 72));
    *((long long *)(a0 + 80)) = *((long long *)(a1 + 80));
    *((char *)(a0 + 88)) = *((char *)(a1 + 88));
    return a0;
}


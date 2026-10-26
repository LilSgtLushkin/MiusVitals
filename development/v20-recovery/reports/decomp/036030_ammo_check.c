// v20 RVA 036030..0360c1  (145 bytes)  ammo_check
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    struct struct_0 *field_0;
    struct struct_2 *field_8;
} struct_1;

typedef struct struct_3 {
    char paddin0[8];
    struct struct_2 *field_8;
    unsigned long long field_10;
    struct struct_2 *field_18;
    char paddin32[16];
    unsigned long long field_30;
} struct_3;

typedef struct struct_0 {
    char paddin0[8];
    unsigned long long field_8;
} struct_0;

typedef struct struct_2 {
    unsigned long long field_0;
} struct_2;


long long ammo_check(struct_3 *a0, struct_1 *a1)
{
    struct_1 *v1;  // rdi
    unsigned long long *v2;  // rdx
    unsigned long v3;  // r8
    unsigned long long *v4;  // rax, Other Possible Types: unsigned long
    struct_0 *v5;  // rbx

    v1 = a1;
    v2 = (sub_61cc(a1 + 16) & *((long long *)(a0 + 48))) * 2;
    v3 = *((long long *)(a0 + 24));
    if (*((long long *)&(8)[(v2 << 3) + v3]) == v1)
    {
        if (*((long long *)(v3 + (v2 << 3))) != v1)
        {
            v4 = *((long long *)(v1 + 8));
        }
        else
        {
            v4 = *((long long *)(a0 + 8));
            *((unsigned long long **)(v3 + v2 * 8)) = v4;
        }
        *((unsigned long *)&(8)[v2 * 8 + v3]) = v4;
    }
    else if (*((long long *)(v3 + (v2 << 3))) == v1)
    {
        *((struct struct_0 **)(v3 + v2 * 8)) = v1->field_0;
    }
    v5 = v1->field_0;
    *((char **)(a0 + 16)) = *((long long *)(a0 + 16)) - 1;
    *((struct_0 **)*((long long *)(v1 + 8))) = v5;
    *((long long *)(v5 + 8)) = *((long long *)(v1 + 8));
    sub_4030c(v1, 32);
    return v1->field_0;
}


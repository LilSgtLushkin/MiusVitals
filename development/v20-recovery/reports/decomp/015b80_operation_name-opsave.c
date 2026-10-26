// v20 RVA 015b80..015c75  (245 bytes)  operation_name@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    uint128_t field_0;
    unsigned long long field_10;
    unsigned long long field_18;
} struct_0;


int operation_name@opsave()
{
    unsigned int v0;  // [bp-0x28]
    struct_0 *v1;  // [bp+0x8]
    char v2;  // [bp+0x10]
    char v3;  // [bp+0x18]
    struct_0 *v4;  // rcx
    unsigned long long v5[2];  // rdx
    unsigned long long v6[2];  // rbx
    unsigned long v7;  // r8
    unsigned long long v8;  // rdx
    char *v9;  // rbx

    v1 = v4;
    v6 = v5;
    *((char **)&v0) = 0;
    memcpy(&v3, &(4)[v5[0]], 4);
    memcpy(&v2, &(8)[v5[0]], 4);
    if ((unsigned int)v7 >= *((int *)&v3) || (v8 = (unsigned long long)(unsigned int)*((int *)&v2), (v7 + 1 << 7) + v8 > *((long long *)(v6 + 8)) - v6[0]))
    {
        *((char **)&v4->field_0) = 0;
        *((char **)(v4 + 16)) = 0;
        *((char **)(v4 + 24)) = 0;
        $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(v4, &g_45ce6, strlen(&g_45ce6));
    }
    else
    {
        v9 = v7 * 128 + v6[0] + v8;
        *((char **)&v4->field_0) = 0;
        *((char **)(v4 + 16)) = 0;
        *((char **)(v4 + 24)) = 0;
        $_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std$_Construct@$00PEBD@?$basic_string@DU?$char_traits@D@std(v4, v9, strnlen(v9, 32));
    }
    *((char **)&v0) = 1;
    return;
}


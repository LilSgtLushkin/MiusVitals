// v20 RVA 0345c0..0347c3  (515 bytes)  base_load_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    struct struct_0 *field_-8;
    uint128_t field_0;
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    uint128_t field_50;
    uint128_t field_60;
    uint128_t field_70;
} struct_0;

typedef struct struct_2 {
    uint128_t field_-10;
    uint128_t field_0;
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    unsigned long long field_50;
    unsigned int field_58;
} struct_2;

typedef struct struct_1 {
    uint128_t field_0;
    uint128_t field_10;
    uint128_t field_20;
    uint128_t field_30;
    uint128_t field_40;
    unsigned long long field_50;
    unsigned int field_58;
} struct_1;


long long base_load_hook()
{
    void* v0;  // [bp-0x38]
    struct_0 *v1;  // [bp-0x28]
    struct_0 *v2;  // [bp-0x20], Other Possible Types: unsigned long
    void* v3;  // [bp-0x18], Other Possible Types: unsigned long
    unsigned long v4;  // [bp-0x8]
    char v5;  // [bp+0x0]
    unsigned long v7;  // rax
    uint128_t v8[7];  // rdx
    struct_2 *v9;  // rdx
    struct_1 *v10;  // rcx
    struct_0 *v11;  // rbx, Other Possible Types: unsigned long
    unsigned long long v12;  // rdi, Other Possible Types: unsigned long
    unsigned long long v13;  // rcx
    struct_0 *v14;  // r8, Other Possible Types: unsigned long
    struct_0 *v15;  // rax

    v4 = &v5;
    if (g_5cfa0 == g_5cfa8)
        return g_5cfa8;
    v7 = GetTickCount() - g_5d054;
    if ((unsigned int)v7 < 300)
        return v7;
    v1 = 0;
    v3 = 0;
    v11 = g_5cfa0;
    v12 = g_5cfa8;
    if (g_5cfa0 != g_5cfa8)
    {
        do
        {
            if ((4)[v11] == 8)
            {
                trench_received(v11);
            }
            else
            {
                v8 = (&v1)[1];
                if (v8 != v3)
                {
                    v8[0] = *((int128_t *)v11);
                    *((int128_t *)(v8 + 16)) = *((int128_t *)&(16)[v11]);
                    *((int128_t *)(v8 + 32)) = *((int128_t *)&(32)[v11]);
                    *((int128_t *)(v8 + 48)) = *((int128_t *)&(48)[v11]);
                    *((int128_t *)(v8 + 64)) = *((int128_t *)&(64)[v11]);
                    *((int128_t *)(v8 + 80)) = *((int128_t *)&(80)[v11]);
                    *((int128_t *)(v8 + 96)) = *((int128_t *)&(96)[v11]);
                    v9 = v8 + 164703072086692424;
                    *((int128_t *)(v9 - 16)) = *((int128_t *)&(112)[v11]);
                    v10 = &(128)[v11];
                    v9->field_-10 = v10->field_0;
                    *((int128_t *)(v9 + 16)) = *((int128_t *)(v10 + 16));
                    *((int128_t *)(v9 + 32)) = *((int128_t *)(v10 + 32));
                    *((int128_t *)(v9 + 48)) = *((int128_t *)(v10 + 48));
                    *((int128_t *)(v9 + 64)) = *((int128_t *)(v10 + 64));
                    *((long long *)(v9 + 80)) = *((long long *)(v10 + 80));
                    *((int *)(v9 + 88)) = *((int *)(v10 + 88));
                    v2 = (&v1)[1] + 220;
                }
                else
                {
                    $_Emplace_reallocate@AEBUShotRec(&v1, (&v1)[1], v11);
                }
            }
        } while ((v11 += 220, v11 != g_5cfa8));
        v12 = g_5cfa8;
        v11 = g_5cfa0;
    }
    g_5cfa0 = v1;
    v1 = v11;
    g_5cfa8 = (&v1)[1];
    v2 = v12;
    v13 = g_5cfb0;
    v7 = v3;
    g_5cfb0 = v7;
    v3 = v13;
    v14 = v1;
    if (!v14)
        return v7;
    v15 = v14;
    if (((5366325548715505925 * (v3 - (char *)v1) >> 64 >> 6) + (5366325548715505925 * (v3 - (char *)v1) >> 64 >> 6 >> 63)) * 220 >= __Eg_my_units)
    {
        v14 = *((long long *)(v14 - 8));
        if (-8 + (char *)v15 - v14 > 31)
        {
            v0 = 0;
            _invoke_watson(0, 0, 0, 0);
            [D] Unsupported jumpkind Ijk_SigTRAP at address 214938
        }
    }
    v7 = sub_4030c(v14);
    v1 = 0;
    v3 = 0;
    return v7;
}


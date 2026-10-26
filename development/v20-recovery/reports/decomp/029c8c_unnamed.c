// v20 RVA 029c8c..029df6  (362 bytes)  end_turn_here
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[4892];
    unsigned int field_131c;
    char padding_1320[1584];
    struct struct_2 *field_1950;
} struct_1;

typedef struct struct_2 {
    char paddin0[776];
    unsigned int field_308;
    unsigned int field_30c;
} struct_2;


long long end_turn_here(unsigned long long a0, unsigned int *a1, unsigned int *a2, unsigned int *a3)
{
    unsigned long long v1;  // rsi
    struct_1 *v2;  // rax, Other Possible Types: char, unsigned long, unsigned long long
    unsigned int v3;  // ebx
    struct_1 *v4;  // rdi
    struct struct_0 **v5;  // r14
    unsigned long long v6;  // r15
    unsigned int v7;  // r13d
    void* v8;  // rsi, Other Possible Types: unsigned long
    void* v9;  // rbp, Other Possible Types: unsigned long
    unsigned long long v10;  // rsi, Other Possible Types: unsigned long
    unsigned long long v11;  // r15, Other Possible Types: unsigned long

    v1 = GetModuleHandleA("adv_opgame.codelib");
    (unsigned int)v2 = menu_controller(a0);
    v3 = 0;
    v4 = v2;
    if (v1 && v2)
    {
        (unsigned int)v2 = readable(v2, &g_2af4);
        if ((char)v2)
        {
            (unsigned int)v2 = memcmp(v1 + 419408, &g_4a6a8, 14);
            if (!(unsigned int)v2)
            {
                v6 = v4 + &g_1a74;
                v9 = 0;
                a0 = (!(a0 >> 31 & 1) ? 0 : *((int *)(v4 + &g_131c)));
                if (3 < (unsigned int)a0)
                    a0 = 3;
                v7 = *((int *)(v1 + a0 * 4 + 1263256));
                v8 = 0;
                if (v7)
                {
                    while ((unsigned int)v8 < 30 && (v9 += (unsigned long long)v5(v4, v8 * 132 + v6), v8 = (unsigned long long)((unsigned int)v8 + 1), (unsigned int)v8 < v7))
                    ;
                }
                v10 = &(3960)[v6];
                v11 = 2;
                while (true)
                {
                    v9 += v5(v4, v10);
                    v10 = &(132)[v10];
                    v11 -= 1;
                }
            }
        }
    }
    v2 = 0;
    return v2;
}


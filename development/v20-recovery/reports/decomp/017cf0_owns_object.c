// v20 RVA 017cf0..017de2  (242 bytes)  owns_object
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    char field_0;
} struct_3;

typedef struct struct_0 {
    char paddin0[8];
    unsigned long long field_8;
    unsigned long long field_10;
    struct struct_1 *field_18;
    char paddin32[16];
    struct struct_1 *field_30;
} struct_0;

typedef struct struct_2 {
    char paddin0[8];
    struct struct_2 *field_8;
    unsigned long long field_10;
} struct_2;

typedef struct struct_1 {
    unsigned long long field_0;
} struct_1;


int owns_object()
{
    char v1;  // [bp+0x10]
    void* v2;  // r8
    struct_3 *v3;  // rdx
    struct_3 *v4;  // rcx
    struct_0 *v5;  // rbx
    unsigned long long *v6;  // rax
    unsigned long long *v7;  // rax
    unsigned long long *v9;  // rax
    unsigned long v10;  // rdx
    struct_2 *v11;  // rcx, Other Possible Types: unsigned long
    struct_2 *v16;  // rcx, Other Possible Types: unsigned long

    if (!g_5b619)
        return;
    v2 = 0;
    v3 = &v1;
    if ((char)unit_key())
    {
        v4 = &v1;
        v5 = (g_5b8e0 == 1 ? &g_5b770 : &g_5b6d0);
        v6 = sub_61cc();
        if (!*((long long *)(v5 + 16)))
        {
            v7 = (v6 & g_5b8d0) * 2;
            if (v16 == g_5b8a8)
                return;
            for (v16 = *((long long *)&(8)[v7 * 8 + g_5b8b8]); *((long long *)&v1) != *((long long *)&(16)[v16]); v16 = *((long long *)&(8)[v16]))
            {
                if (v16 == *((long long *)(g_5b8b8 + v7 * 8)))
                    break;
            }
        }
        else
        {
            v9 = *((long long *)(v5 + 24));
            v10 = (*((long long *)(v5 + 48)) & v6) * 2;
            if (v11 == *((long long *)(v5 + 8)))
                return;
            for (v11 = *((long long *)((char *)v9 + v10 * 8 + 8)); *((long long *)&v1) != *((long long *)&(16)[v11]); v11 = *((long long *)&(8)[v11]))
            {
                if (v11 == *((long long *)((char *)v9 + v10 * 8)))
                    break;
            }
        }
    }
}


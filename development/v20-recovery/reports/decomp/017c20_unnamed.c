// v20 RVA 017c20..017cf0  (208 bytes)  menu_controller_of
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_3 {
    char paddin0[920];
    unsigned long long field_398;
    char paddin928[4];
    unsigned int field_3a4;
} struct_3;


long long menu_controller_of(struct struct_1 **a0)
{
    unsigned long v0;  // [bp-0x28]
    int tmp_24;  // tmp #24
    unsigned long v1;  // [bp-0x20]
    struct_2 *v2;  // [bp-0x18]
    void* v3;  // [bp-0x10]
    void* v4;  // [bp+0x8]
    struct_3 *v5;  // rax
    unsigned long long v6;  // rbx
    unsigned long long v7;  // rsi
    void* v8;  // rcx
    unsigned long v9;  // rax
    unsigned long long v10;  // rdi, Other Possible Types: unsigned long
    unsigned int *v11;  // rdx, Other Possible Types: unsigned long

    if (!a0)
        return 4294967295;
    v4 = 0;
    v0 = "i_aicolonel";
    v3 = 0;
    v1 = "aicolonel";
    v2 = &v4;
    (unsigned int)v5 = *((long long *)(*(a0) + 72))();
    if (v5 && g_5be38)
    {
        v6 = *((int *)(v5 + 932));
        v7 = *((long long *)(v5 + 920));
        if (!(unsigned int)v6)
        {
            return 0;
        }
        else if ((unsigned int)v6 <= 64)
        {
            v10 = v6;
            if ((char)readable(v7, v6 * 1584))
            {
                v9 = 0;
                if (!(unsigned int)v6)
                    return 0;
                v11 = &(4)[v7];
                do
                {
                    v8 = 0;
                    tmp_24 = *((int *)v11);
                    v11 = &(1584)[v11];
                    *((char *)&v8) = (int)tmp_24 == g_5be38;
                    v9 += v8;
                    v10 -= 1;
                } while (v6 != 1);
                return v9;
            }
        }
    }
}


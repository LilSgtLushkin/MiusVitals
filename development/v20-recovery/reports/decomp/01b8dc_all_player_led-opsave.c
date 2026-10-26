// v20 RVA 01b8dc..01b9af  (211 bytes)  all_player_led@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    struct struct_0 *field_0;
    char paddin8[16];
    unsigned long long field_18;
} struct_0;


long long all_player_led@opsave(unsigned long a0)
{
    unsigned long long v0;  // [bp-0x38]
    int tmp_17;  // tmp #17
    int tmp_19;  // tmp #19
    int tmp_18;  // tmp #18
    void* v1;  // [bp-0x28]
    char v2;  // [bp+0x0]
    char v4;  // rax, Other Possible Types: unsigned long long, unsigned long
    unsigned int *v5;  // rdi, Other Possible Types: unsigned long
    void* v6;  // xmm0
    int v7;  // xmm0
    unsigned int v8;  // esi
    struct_0 *v9;  // rbx, Other Possible Types: unsigned long
    unsigned int v10;  // xmm0

    v4 = &v2;
    v5 = 0;
    v1 = 0;
    v8 = 0;
    v0 = 0;
    v9 = g_5b738->field_0;
    if (v9 != g_5b738)
    {
        do
        {
            (unsigned int)v4 = owns_object(*((long long *)&(24)[v9]));
            if (!(char)v4)
            {
                (unsigned int)v4 = readable(&(432)[*((long long *)&(24)[v9])], 12);
                if ((char)v4)
                {
                    v4 = 0;
                    do
                    {
                        v6 = 0;
                        *((unsigned int *)&v6) = *((int *)&(432)[v4 * 4 + *((long long *)(v9 + 24))]);
                        v6 = 0;
                        xmm0<16> = (xmm0<16> + Conv(64->128, Load(addr=(stack_base-56 + (rax<8> * 0x8<64>)), size=8, endness=Iend_LE)))
                        *((unsigned long long *)((char *)&v0 + v4 * 8)) = (unsigned long long)v7;
                        v4 = &(1)[v4];
                    } while (v4 < 3);
                    v8 = &(1)[v8];
                }
            }
        } while ((v9 = *((long long *)v9), v9 != g_5b738));
        if (v8)
        {
            do
            {
                tmp_17 = DivV(tmp_19, tmp_18);
                v10 = tmp_17;
                v10 = 0;
                v10 = 0;
                v10 = (int)(double)(v10 >> 64);
                v10 = (int)(double)v10;
                *((int *)(a0 + v5 * 4)) = (int)(double)v10;
                v5 = &(1)[v5];
            } while (v5 < 3);
            v4 = 1;
            return v4;
        }
    }
    *((char *)&v4) = 0;
    return v4;
}


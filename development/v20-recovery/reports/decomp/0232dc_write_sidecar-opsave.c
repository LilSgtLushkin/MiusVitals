// v20 RVA 0232dc..0233c9  (237 bytes)  write_sidecar@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[24];
    unsigned long long field_18;
} struct_1;

typedef struct struct_0 {
    char paddin0[16];
    struct struct_0 *field_10;
    char paddin24[1];
    char field_19;
    char paddin26[30];
    unsigned long long field_38;
} struct_0;


long long write_sidecar@opsave(unsigned long long a0, struct_1 *a1, struct struct_2 **a2)
{
    void* v1;  // [bp+0x20]
    char v2;  // rax, Other Possible Types: unsigned long
    struct_0 *v3;  // rbx, Other Possible Types: unsigned long
    struct_0 *v4;  // rcx
    struct_0 *v5;  // rcx, Other Possible Types: unsigned long
    struct_0 *v6;  // rax, Other Possible Types: unsigned long

    v1 = 0;
    *((unsigned int *)&v2) = fopen_s(&v1, a0, "w");
    if (!v2 && v1)
    {
        fprintf();
        v3 = *(a2)->field_0;
        while (true)
        {
            if (!(25)[v3])
            {
                fprintf(v1, "%s %u\n");
                v4 = *((long long *)&(16)[v3]);
                if (*((char *)(v4 + 25)))
                {
                    for (v6 = *((long long *)&(8)[v3]); !(25)[v6] && v3 == *((long long *)&(16)[v6]); v6 = *((long long *)&(8)[v6]))
                    {
                        v3 = v6;
                    }
                    v3 = v6;
                }
                else
                {
                    v3 = v4;
                    v5 = *((long long *)&v4->paddin0[0]);
                    if (!*((char *)(v5 + 25)))
                    {
                        do
                        {
                            v3 = v5;
                            v5 = *((long long *)v5);
                        } while (!(25)[v5]);
                    }
                }
            }
            else
            {
                (unsigned int)v2 = fclose(v1);
                *((char *)&v2) = 1;
                return v2;
            }
        }
    }
    v2 = 0;
    return v2;
}


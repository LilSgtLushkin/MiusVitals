// v20 RVA 012cc8..012e06  (318 bytes)  all_player_led@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned long long field_-10;
    unsigned long long field_-8;
    unsigned long long field_0;
    char paddin8[8];
    unsigned long long field_10;
    char paddin24[16];
    char field_28;
} struct_0;


int all_player_led@opsave()
{
    char v0;  // [bp-0x58]
    char v1;  // [bp-0x50]
    char v3;  // [bp+0x10]
    char v4;  // [bp+0x18]
    char v5;  // [bp+0x20]
    unsigned long long *v6;  // rcx
    unsigned long long *v7;  // rdi
    unsigned long long v8;  // rax
    unsigned int *v9;  // r9
    unsigned int *v10;  // r8
    unsigned int *v11;  // rdx
    unsigned long long v12;  // rbp
    unsigned long long v13;  // rbx
    unsigned long long v14;  // rcx
    unsigned int v15;  // cc_dep1
    unsigned int v16;  // cc_dep2
    unsigned int v17;  // ecx
    struct_0 *v18;  // rbx, Other Possible Types: unsigned long

    v7 = v6;
    tagged@opsave(&v0, v6);
    (unsigned int)v8 = user_side@opsave@opsave(v6, &v0);
    *((char **)&v9) = 0;
    *((char **)&v10) = 0;
    *((char **)&v11) = 0;
    v12 = *((long long *)&v1);
    v13 = *((long long *)&v0);
    if (v13 == v12)
    {
        _Tidy@?$vector@UTagged@opsave@?$vector@UTagged@opsave(&v0);
        return;
    }
    v18 = &(48)[v13];
    do
    {
        if (!(40)[v18] && *((long long *)(v18 - 8)) && *((long long *)v18))
        {
            v14 = *((long long *)&(16)[v18]);
            if (!v14 || !(unsigned int)v8)
            {
                memcpy(&v4, *((long long *)(v18 - 16)) + *(v7), 4);
                v15 = *((int *)&v4);
                v16 = 1380275029;
            }
            else
            {
                memcpy(&v3, *(v7) + v14, 4);
                v15 = *((int *)&v3);
                v16 = v8;
            }
            if (v15 != v16)
            {
                memcpy(&v5, *((long long *)(v18 - 8)) + *(v7), 4);
                *((char **)&v11) = &(1)[*(v11)];
                v17 = *((int *)&v5);
                *((unsigned long *)&v10) = *(v10) + (v17 & 1);
                *((unsigned long *)&v9) = *(v9) + (v17 / 2 & 1);
            }
        }
    } while ((v18 += 96, v18 - 48 != v12));
}


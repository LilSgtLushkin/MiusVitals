// v20 RVA 012ab8..012cc7  (527 bytes)  launch_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[200];
    unsigned long long field_c8;
    char paddin208[4];
    unsigned int field_d4;
    char paddin216[1304];
    unsigned long long field_5f0;
} struct_1;

typedef struct struct_2 {
    char paddin0[776];
    unsigned int field_308;
} struct_2;


int launch_hook()
{
    unsigned int v0;  // [bp-0x28]
    unsigned long long v2;  // rsi
    unsigned long long v3;  // rcx
    unsigned long long v4;  // r15
    unsigned long long v5;  // rax
    void* v6;  // rbx, Other Possible Types: unsigned long
    struct struct_0 **v7;  // r12
    struct_1 *v8;  // rsi
    struct_2 *v9;  // rbp
    unsigned long long v10;  // rsi
    unsigned long long v11;  // r14
    void* v13;  // rbp, Other Possible Types: unsigned long
    unsigned int v14[210];  // rcx, Other Possible Types: unsigned long
    unsigned long long v15;  // rcx, Other Possible Types: unsigned long
    unsigned long v16;  // rdx
    void* v17;  // rsi, Other Possible Types: unsigned long, unsigned long long
    unsigned int v18[210];  // rcx, Other Possible Types: unsigned long
    unsigned long v19;  // rdx, Other Possible Types: unsigned long long
    void* v20;  // r10, Other Possible Types: unsigned long
    void* v21;  // r8, Other Possible Types: unsigned long, unsigned long long
    unsigned int v22;  // ecx
    unsigned long long v25;  // rax
    unsigned long long v26;  // rcx
    unsigned int v27[210];  // rdi, Other Possible Types: unsigned long

    v2 = GetModuleHandleA("adv_opgame.codelib");
    v4 = menu_controller_of(v3);
    (unsigned int)v5 = operation_on_screen(v3);
    v6 = 0;
    if (!v2 || !v4 || !v5 || !(!memcmp(&(&g_35540)[v2], &g_485e0, 22)))
    {
        v15 = "map: unknown game build, scouting not worked out for this side";
    }
    else
    {
        v8 = is_object(v5);
        v9 = ((char)readable(v8, 1536) == 0 ? *((long long *)(v8 + 1520)) : 0);
        v27 = ((char)readable(v8, 1536) == 0 ? *((long long *)(v8 + 200)) : 0);
        v10 = ((char)readable(v8, 1536) == 0 ? *((int *)(v8 + 212)) : 0);
        if ((char)readable(v9, 784) == 0 || !v27 || !(unsigned int)v10 || !((v11 = v10, (char)readable(v27, v10 * 1776) != 0)))
        {
            v15 = "map: the operation's groups were not found, scouting not worked out";
        }
        else
        {
            v13 = 0;
            v14 = 0;
            if ((unsigned int)v10)
            {
                do
                {
                    if ((unsigned int)v13)
                    {
                        if ((unsigned int)v13 == *((int *)(v9 + 776)))
                        {
                            v17 = 0;
                            v18 = v27;
                            v19 = v11;
                            do
                            {
                                if (*((int *)v18) != (unsigned int)v13)
                                    v17 += *((int *)&(836)[v18]) & 1;
                            } while ((v18 += 1776, v19 -= 1, v11 != 1));
                            v7(v4);
                            v20 = 0;
                            v21 = 0;
                            do
                            {
                                if (*((int *)v27) != (unsigned int)v13)
                                {
                                    v22 = *((int *)&(836)[v27]);
                                    v6 = &(1)[v6];
                                    v25 = v22 & 1;
                                    v26 = v22 / 2 & 1;
                                    v21 += v25;
                                    v20 += v26;
                                }
                            } while ((v18 += 1776, v19 -= 1, v11 != 1));
                            v0 = v20;
                            mp_log("map: this side's own scouting worked out: of the host's %u groups %u seen now (the copy said %u), %u identified", v6);
                            return;
                        }
                        goto LABEL_12c8a;
                    }
                    v16 = v14 * 1776;
                    if (*((int *)(v16 + (char *)v27 + 8)) == 1380275029)
                        v13 = *((int *)(v16 + (char *)&v27[0]));
                    v14 = &(1)[v14];
                } while ((unsigned int)v14 < (unsigned int)v10);
            }
LABEL_12c8a:
            v15 = "map: this map is the host's operation, not this side's: scouting left as the host's copy has it";
        }
    }
    mp_log(v15);
    return;
}


// v20 RVA 035638..03571d  (229 bytes)  safe_str
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_1 {
    char paddin0[240];
    struct struct_2 *field_f0;
    char paddin248[4];
    unsigned int field_fc;
} struct_1;

typedef struct struct_0 {
    char paddin0[4476];
    unsigned int field_117c;
    char padding_1180[4];
    unsigned int field_1184;
    char padding_1188[3220];
    char field_1e1c;
} struct_0;

typedef struct struct_2 {
    struct struct_0 *field_0;
} struct_2;


long long safe_str(struct_1 *a0)
{
    char v0;  // [bp+0x0]
    unsigned long v2;  // rax, Other Possible Types: unsigned long long
    unsigned int v3;  // ebp
    unsigned int v4;  // r12d
    struct_0 *v5;  // rdi
    unsigned int v6;  // ebx
    struct struct_0 **v7;  // r14, Other Possible Types: unsigned long

    v2 = &v0;
    v3 = *((int *)(a0 + 252));
    v6 = 0;
    v4 = g_5d04c;
    v7 = *((long long *)(a0 + 240));
    *((char **)&g_5d04c) = 0;
    if (v3)
    {
        while (v6 < v4)
        {
            v5 = *((long long *)v7);
            (unsigned int)v2 = readable(v5, &g_1e60);
            if ((char)v2)
            {
                (unsigned int)v2 = strncmp(v5 + 8, "i_techn:", 8);
                if (!(unsigned int)v2)
                {
                    (unsigned int)v2 = sub_1bacc(v5);
                    if ((char)v2)
                    {
                        *((char **)(v5 + &g_1e1c)) = 1;
                        *((char **)(v5 + &g_117c)) = 1;
                        *((int *)(v5 + &g_1184)) = 0x3f800000;
                        v2 = mp_log("test: blowup - the crew of %s will blow it up in 1 s", v5 + 8);
                        v6 = &(1)[v6];
                    }
                }
            }
            v7 = &(8)[v7];
            if (1 >= v3)
                break;
        }
        if (v6)
            return v2;
    }
    v2 = mp_log("test: blowup - this player has no vehicle");
    return v2;
}


// v20 RVA 033684..03387c  (504 bytes)  launch_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[56];
    unsigned long long field_38;
    unsigned long long field_40;
    unsigned long long field_48;
    char paddin80[24];
    unsigned int field_68;
} struct_0;


long long launch_hook(unsigned long a0, struct_0 *a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned int a5)
{
    unsigned long v0;  // [bp-0x30], Other Possible Types: unsigned int
    char v1;  // [bp+0x0]
    unsigned long long v3;  // r14
    unsigned long long v4;  // rdi
    struct_0 *v5;  // rbx
    unsigned long long v6;  // rbp
    unsigned long long v7;  // rsi
    unsigned long long v8;  // rax, Other Possible Types: unsigned long
    char v9;  // r15b
    char v10;  // r13b
    unsigned long v11;  // rdx
    unsigned long long v12;  // r12
    unsigned long long v14;  // r9
    unsigned long v15;  // rcx
    struct_0 *v16;  // r8
    unsigned long long v17;  // rcx
    unsigned long long v18;  // rcx
    unsigned long long v19;  // rax, Other Possible Types: unsigned long

    v3 = a3;
    v4 = a2;
    v5 = a1;
    v6 = a0;
    if (((char)a2 & 2) || g_5d024 || GetCurrentThreadId() != g_5d020 || !(char)readable(a1, 144))
    {
        v0 = a5;
    }
    else
    {
        v7 = *((long long *)(a1 + 72));
        g_5d008 = a0;
        if (!g_5a2ec || !(char)sub_19f18() || !sub_1c334())
        {
            v10 = 0;
            goto LABEL_33746;
        }
        else
        {
            v10 = 1;
            if (!v7 || !(char)is_object(v7))
                (char)v8 = sub_1bad4(*((int *)(a1 + 104)));
            else
                (char)v8 = sub_1bacc(v7);
            if ((char)v8)
            {
LABEL_33746:
                v9 = 1;
            }
            else
            {
                v9 = 0;
            }
        }
        v11 = *((long long *)&v1);
        v12 = a4;
        v0 = v11;
        v14 = a2;
        *((char *)&v11) = 76;
        v15 = (!v7 ? "dropped" : "dropped-offmap");
        v16 = v5;
        if (v9)
            v17 = (v7 == 0 ? "own" : "own-offmap");
        if (!v10)
            v18 = "free";
        v19 = log_launch();
        if (!v10)
        {
            g_5d078 = &(1)[g_5d078];
            v0 = a5;
        }
        else if (v9)
        {
            g_5d058 = &(1)[g_5d058];
            if (!v7)
                g_5d090 = &(1)[g_5d090];
            v0 = a5;
            v19 = g_5cfe0(v6, v5, v4, v3, v12);
            if (!*((long long *)(v5 + 56)))
            {
                return v19;
            }
            else if (!*((long long *)(v5 + 64)))
            {
                return v19;
            }
            else
            {
                v19 = queue_shot(v5, v4, v3, v12);
                return v19;
            }
        }
        else
        {
            g_5d060 = &(1)[g_5d060];
            return v19;
        }
    }
    v19 = g_5cfe0(v6, v5, v4, v3);
    return v19;
}


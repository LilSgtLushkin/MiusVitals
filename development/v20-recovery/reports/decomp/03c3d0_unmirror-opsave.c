// v20 RVA 03c3d0..03c58a  (442 bytes)  unmirror@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    char paddin0[8];
    unsigned int field_8;
    unsigned int field_c;
    char paddin16[32];
    char field_30;
    char field_31;
    char field_32;
} struct_3;

typedef struct struct_1 {
    char paddin0[112];
    struct struct_0 *field_70;
    char paddin120[190];
    char field_136;
    char paddin311[2];
    char field_139;
} struct_1;

typedef struct struct_0 {
    char paddin0[12];
    unsigned int field_c;
} struct_0;


long long unmirror@opsave(struct_1 *a0, unsigned long long a1, struct_3 *a2)
{
    char v0;  // [bp-0x78]
    unsigned int v1;  // [bp-0x58]
    unsigned int v2;  // [bp-0x54]
    char v3;  // [bp-0x50]
    int v4;  // [bp-0x4f]
    void* v5;  // [bp-0x3f]
    unsigned int v6;  // [bp-0x37]
    unsigned long long v7;  // [bp-0x30]
    struct_0 *v9;  // rbx
    unsigned int v10;  // eax

    v7 = g_5a380 ^ &v0;
    if (*((char *)(a2 + 50)) && strncmp(1?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(a0), a2 + 52, 28))
    {
        *((char **)&v3) = 0;
        v5 = 0;
        *((char **)&v6) = 0;
        *((char **)&v4) = 0;
        memcpy(&v3, a2 + 52, 28);
        g_5da80(g_5d8f0, a1, &v3);
        g_5dab8 = &(1)[g_5dab8];
    }
    memcpy(a0 + 168, a2 + 4, 4);
    memcpy(a0 + 256, a2 + 16, 4);
    memcpy(a0 + 276, a2 + 20, 4);
    memcpy(a0 + 212, a2 + 24, 4);
    memcpy(a0 + 284, a2 + 28, 12);
    memcpy(a0 + 296, a2 + 40, 8);
    v9 = *((long long *)(a0 + 112));
    *((char *)(a0 + 310)) = *((char *)(a2 + 48));
    *((char *)(a0 + 313)) = *((char *)(a2 + 49));
    if (!*((int *)(a2 + 8)) || !v9 || !(char)readable(v9, 16) || *((int *)(a2 + 12)) >= *((int *)(v9 + 12)) || *((char *)(a2 + 50)) && strncmp(1?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(a0), a2 + 52, 28))
    {
        *((char **)&v1) = 0;
        v10 = -1;
    }
    else
    {
        v1 = *((int *)(a2 + 8));
        v10 = *((int *)(a2 + 12));
    }
    v2 = v10;
    memcpy(a0 + 260, &v1, 4);
    memcpy(a0 + 268, &v2, 4);
    return security_cookie_check(v7 ^ &v0);
}


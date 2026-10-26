// v20 RVA 01b210..01b2d8  (200 bytes)  end_turn_here
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char field_0;
} struct_2;

typedef struct struct_3 {
    char paddin0[3797];
    char field_ed5;
} struct_3;


int end_turn_here()
{
    unsigned long v0;  // [bp-0x28]
    unsigned long v1;  // [bp-0x20]
    struct_2 *v2;  // [bp-0x18]
    void* v3;  // [bp-0x10]
    void* v4;  // [bp+0x8]
    unsigned long long v5;  // rdi
    struct struct_1 **v6;  // rcx
    struct_3 *v7;  // rbx
    char v8;  // dl
    struct struct_0 **v9;  // rdi

    v5 = GetModuleHandleA("adv_interf.codelib");
    v4 = 0;
    v3 = 0;
    v0 = "i_intereff";
    v1 = "intereff";
    v2 = &v4;
    v7 = *((long long *)(*(v6) + 72))(v6, &v0, *((long long *)(*(v6) + 72)));
    if (!v5)
    {
        return;
    }
    else if (!v7)
    {
        return;
    }
    else if (!g_5b618)
    {
        return;
    }
    else if (*((char *)(v7 + 3797)) != 0 == v8)
    {
        return;
    }
    else if (memcmp(&(&g_228c0)[v5], &g_494b0, 16))
    {
        return;
    }
    else
    {
        v9(v7, 1172 CONCAT v8);
        return;
    }
}


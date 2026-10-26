// v20 RVA 002f94..003061  (205 bytes)  on_generate
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_2 {
    char paddin0[96];
    struct struct_3 *field_60;
    char paddin104[1192];
    unsigned long long field_510;
} struct_2;

typedef struct struct_4 {
    char field_0;
} struct_4;

typedef struct struct_3 {
    struct struct_1 *field_0;
} struct_3;

typedef struct struct_1 {
    char paddin0[72];
    struct struct_0 *field_48;
} struct_1;

typedef struct struct_0 {
    struct struct_0 *field_0;
} struct_0;


long long on_generate(struct_2 *a0, unsigned int a1, unsigned long a2, unsigned long long a3)
{
    unsigned long v0;  // [bp-0x28]
    unsigned long v1;  // [bp-0x20]
    struct_4 *v2;  // [bp-0x18]
    void* v3;  // [bp-0x10]
    void* v4;  // [bp+0x8]
    unsigned long v5;  // r8
    struct struct_5 **v6;  // rax, Other Possible Types: unsigned long long, unsigned long
    struct struct_1 **v7;  // rcx
    struct struct_0 **v8;  // r8

    v5 = *((long long *)(a0 + 1296));
    v6 = &(1)[v5];
    *((struct struct_5 ***)(a0 + 1296)) = v6;
    if (v5)
        return &(1)[v5];
    mp_log("first generate (msg=%p)", a1, v5, a3);
    (unsigned int)v6 = strcmp("opgame", "tacgame");
    if (!(unsigned int)v6)
        v6 = map_moved_on();
    if (!g_5a674)
        return v6;
    v7 = *((long long *)(a0 + 96));
    v0 = "i_timer";
    v4 = 0;
    v1 = "timer";
    v2 = &v4;
    v3 = 0;
    v8 = *((long long *)(*(v7) + 72));
    (unsigned int)v6 = v8(v7, &v0, v8);
    if (v6)
    {
        *((long long *)&(24)[*((long long *)v6)])(v6, *((long long *)&(24)[*((long long *)v6)]));
        v6 = mp_log("fixed frame time set on %p", v6, v8, a3);
        return v6;
    }
    return v6;
}


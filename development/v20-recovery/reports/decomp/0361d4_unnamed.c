// v20 RVA 0361d4..0364db  (775 bytes)  apply
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned int field_0;
    unsigned int field_4;
    char paddin8[16];
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    char field_24;
    char field_25;
    char field_26;
    char field_27;
    char paddin40[56];
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
    char field_70;
    char field_71;
    char field_72;
    char paddin115[1];
    unsigned int field_74;
    unsigned int field_78;
    unsigned int field_7c;
    unsigned int field_80;
    unsigned int field_84;
    unsigned int field_88;
} struct_0;

typedef struct struct_3 {
    char paddin0[808];
    unsigned int field_328;
    char paddin812[8];
    unsigned int field_334;
    unsigned int field_338;
    unsigned int field_33c;
    unsigned int field_340;
    char paddin836[124];
    unsigned int field_3c0;
    char paddin964[31];
    char field_3e3;
    char field_3e4;
    char paddin997[10];
    char field_3ef;
} struct_3;


long long apply(struct_0 *a0)
{
    char v0;  // [bp-0xb8]
    int tmp_119;  // tmp #119
    char v1;  // [bp-0x70]
    unsigned long v2;  // [bp-0x68]
    unsigned long v3;  // [bp-0x60]
    unsigned int v4;  // [bp-0x58]
    unsigned int v5;  // [bp-0x54]
    unsigned int v6;  // [bp-0x50]
    unsigned int v7;  // [bp-0x4c]
    unsigned int v8;  // [bp-0x48]
    unsigned int v9;  // [bp-0x44]
    char v10;  // [bp-0x40]
    char v11;  // [bp-0x3f]
    char v12;  // [bp-0x38]
    unsigned long long v13;  // [bp-0x28]
    unsigned long v14;  // [bp-0x8]
    char v15;  // [bp+0x0]
    struct_0 *v17;  // rbx
    struct_3 *v18;  // rdi
    unsigned long long v19;  // rax
    unsigned long long v21;  // r10
    unsigned int v22;  // eax
    unsigned int *v23;  // rcx, Other Possible Types: unsigned long
    unsigned int *v24;  // rdx, Other Possible Types: unsigned long
    unsigned long long v25;  // r9, Other Possible Types: unsigned long
    unsigned int *v26;  // r14, Other Possible Types: unsigned long
    unsigned long long v28;  // rdx
    unsigned long long v30;  // rcx
    unsigned int v31;  // eax
    unsigned long long v32;  // r9
    unsigned long long v33;  // r8
    unsigned long long v34;  // rdx
    unsigned long long v35;  // rcx

    v14 = &v15;
    v13 = g_5a380 ^ &v0;
    v17 = a0;
    v18 = squad_record(g_5d270, a0->field_0);
    if (!v18)
    {
        g_5d298 = &(1)[g_5d298];
        return security_cookie_check(v13 ^ &v0);
    }
    *((int *)(v18 + 832)) = *((int *)(a0 + 24));
    *((int *)(v18 + 808)) = *((int *)(a0 + 28));
    *((char *)(v18 + 995)) = *((char *)(a0 + 36));
    *((char *)(v18 + 996)) = *((char *)(a0 + 37));
    *((char *)(v18 + 1007)) = *((char *)(a0 + 38));
    *((int *)(v18 + 820)) = *((int *)(a0 + 96));
    *((int *)(v18 + 824)) = *((int *)(a0 + 100));
    *((int *)(v18 + 828)) = *((int *)(a0 + 104));
    *((int *)(v18 + 960)) = *((int *)(a0 + 108));
    if (*((char *)(a0 + 39)))
    {
        (unsigned int)v19 = session_unit(g_5d270);
        if (v19)
        {
            v26 = &(304)[v19];
            memcpy(&(432)[v19], a0 + 48, 12);
            v23 = a0 + 60;
            v21 = 3;
            v24 = v26;
            v25 = 3;
            while (true)
            {
                *((int *)(v19 - (char *)v26 - v17 + (char *)v26 + v23 + 180)) = *((int *)v23);
                v22 = *((int *)v23);
                v23 = &(4)[v23];
                *((unsigned int *)v24) = v22;
                v24 = &(16)[v24];
                v25 -= 1;
            }
        }
    }
    *((int *)(g_5d270 + 720)) = *((int *)(v17 + 32));
    memcpy(&v12, v17 + 8, 16);
    v28 = v17->field_0;
    v4 = *((int *)(v17 + 116));
    v5 = *((int *)(v17 + 120));
    v6 = *((int *)(v17 + 124));
    v7 = *((int *)(v17 + 128));
    v8 = *((int *)(v17 + 136));
    v9 = *((int *)(v17 + 132));
    v10 = *((char *)(v17 + 113));
    v11 = *((char *)(v17 + 114));
    tmp_119 = *((char *)(v17 + 112));
    *((char **)&g_5d0e9) = 0;
    *((char **)&g_5d0ea) = 1;
    v30 = -(0 < (tmp_119 & 255)) & &v4;
    v31 = *((int *)(v17 + 4));
    g_5d248 = v30;
    g_5d218(g_5d270, v28, -(0 < v31) & &v12);
    if (g_5d220 && *((char *)(v17 + 112)) && !g_5d0e9)
    {
        v2 = GetModuleHandleA("adv_game.codelib") + 308242;
        v3 = *((long long *)(g_5d270 + 576));
        if (v3)
        {
            v1 = v11;
            g_5d220(g_5d270, &v2, v4, v5, v6, v7, v8, v9, -(0 < v10) & v18);
            g_5d258 = &(1)[g_5d258];
        }
    }
    g_5d290 = &(1)[g_5d290];
    *((char **)&g_5d0ea) = 0;
    g_5d248 = 0;
    if (!g_5d0eb)
        return security_cookie_check(v13 ^ &v0);
    v32 = *((int *)(v17 + 24));
    v33 = *((int *)(v17 + 28));
    v34 = v17->field_0;
    v35 = "trench: squad %08x of the other player dug in here from his values (kind %u, settings %x, tile %u cell %u direction %u): his game %s, this game's dig %s";
    mp_log();
    return security_cookie_check(v13 ^ &v0);
}


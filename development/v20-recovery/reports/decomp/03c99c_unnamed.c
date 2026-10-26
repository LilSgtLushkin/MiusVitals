// v20 RVA 03c99c..03cbd0  (564 bytes)  dig_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[96];
    unsigned int field_60;
    char paddin100[4];
    unsigned long long field_68;
    unsigned long long field_70;
    unsigned long long field_78;
} struct_0;


long long dig_hook(struct struct_1 **a0, unsigned long a1, unsigned long long a2, struct_0 *a3, unsigned int a4, unsigned long a5, unsigned int a6, char a7)
{
    char v0;  // [bp-0x1f8]
    int tmp_5;  // tmp #5
    int tmp_3;  // tmp #3
    int tmp_0;  // tmp #0
    int tmp_9;  // tmp #9
    char v1;  // [bp-0x1d0]
    char v2;  // [bp-0x1c0]
    char v3;  // [bp-0x1b8]
    unsigned int v4;  // [bp-0x150]
    unsigned int v5;  // [bp-0x14c]
    char v6;  // [bp-0x148]
    char v7;  // [bp-0x147]
    char v8;  // [bp-0x138]
    char v9;  // [bp-0x134]
    unsigned int v10;  // [bp-0x130]
    unsigned int v11;  // [bp-0x12c]
    unsigned int v12;  // [bp-0x124]
    unsigned long v13;  // [bp-0x114]
    unsigned long long v14;  // [bp-0x10c]
    unsigned long v15;  // [bp-0x104]
    char v16;  // [bp-0xec]
    unsigned long long v17;  // [bp-0x58]
    unsigned long v18;  // [bp-0x8]
    char v19;  // [bp+0x0]
    unsigned long long v22;  // r14
    unsigned int v23;  // r12d
    unsigned long long v24;  // rsi

    v18 = &v19;
    v17 = g_5a380 ^ &v0;
    v22 = a5;
    v23 = a6;
    v24 = a4;
    if (!(g_5a2f2) || !(sub_1c334()) || !((char)sub_19f18()) || !((char)sub_1d2d4()) || !((char)sub_1e518()) || !((char)readable(a3, 128)) || !((char)sub_1e4e4(a2, &v2)))
    {
LABEL_3cb94:
        v1 = a7;
        a0(a2, a3, v24, v22, v23);
        return security_cookie_check(v17 ^ &v0);
    }
    if (!(char)sub_1bacc(a2))
    {
LABEL_3ca68:
        g_5db48 = &(1)[g_5db48];
        tmp_5 = CasCmpNE(tmp_3, tmp_0);
        if (tmp_5)
            goto LABEL_3ca68;
        return security_cookie_check(v17 ^ &v0);
    }
    memset(&v8, 0, 220);
    memset(&v3, 0, 116);
    memcpy(&v3, a3, 104);
    v7 = a1;
    v4 = v23;
    v6 = v1;
    *((char **)&v9) = 7;
    v12 = v24;
    v5 = *((int *)&(&g_2768)[a2 + (-(0 < (char)a1) & -7172)]);
    v14 = *((long long *)&v2);
    v13 = operation_on_screen(v22);
    v15 = operation_on_screen(*((long long *)(a3 + 120)));
    v10 = 1?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CB_KV?$basic_string@DU?$char_traits@D@std?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CB_KV?$basic_string@DU?$char_traits@D@std(*((long long *)(a3 + 104)));
    v11 = 1?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CB_KV?$basic_string@DU?$char_traits@D@std?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CB_KV?$basic_string@DU?$char_traits@D@std(*((long long *)(a3 + 112)));
    memcpy(&v16, &v3, 116);
    if (GetCurrentThreadId() == g_5db14)
    {
        shots_queue(&v8);
    }
    else
    {
        EnterCriticalSection(&g_5db18.DebugInfo);
        sub_3277c(&g_5dae0, &v8);
        LeaveCriticalSection(&g_5db18.DebugInfo);
    }
LABEL_3cb65:
    g_5db40 = &(1)[g_5db40];
    tmp_9 = CasCmpNE(tmp_3, tmp_0);
    if (tmp_9)
        goto LABEL_3cb65;
    if (g_5db12 == 0)
        goto LABEL_3cb94;
    mp_log("hit: %s struck (kind %u, part %x, ammunition %u), sent", &(8)[a2], v24, *((int *)(a3 + 96)));
    goto LABEL_3cb94;
}


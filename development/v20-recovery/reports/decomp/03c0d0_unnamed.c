// v20 RVA 03c0d0..03c1fc  (300 bytes)  sub_3c0d0
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_3c0d0(char a0[314], char a1[51])
{
    char *v1;  // rbx
    unsigned long long v2;  // rsi
    unsigned long long v3;  // rax, Other Possible Types: unsigned long

    memcpy(a1 + 4, a0 + 168, 4);
    memcpy(a1 + 8, a0 + 260, 4);
    memcpy(a1 + 12, a0 + 268, 4);
    memcpy(a1 + 16, a0 + 256, 4);
    memcpy(a1 + 20, a0 + 276, 4);
    memcpy(a1 + 24, a0 + 212, 4);
    memcpy(a1 + 28, a0 + 284, 12);
    memcpy(a1 + 40, a0 + 296, 8);
    *((char *)(a1 + 48)) = *((char *)(a0 + 310));
    *((char *)(a1 + 49)) = *((char *)(a0 + 313));
    v1 = 1?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std?$_List_node_emplace_op2@V?$allocator@U?$_List_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(a0);
    v2 = strnlen(v1, 28);
    *((char *)(a1 + 50)) = v2 - 1 <= 26;
    v3 = memset(a1 + 52, 0, 28);
    if (*((char *)(a1 + 50)))
    {
        v3 = memcpy(a1 + 52, v1, v2);
        return v3;
    }
    return v3;
}


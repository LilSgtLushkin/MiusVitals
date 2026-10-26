// v20 RVA 01e530..01e653  (291 bytes)  sub_1e530
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


int sub_1e530()
{
    char v0;  // [bp+0x0]
    unsigned long long v2;  // rax
    unsigned long long v3;  // rdi
    unsigned int v4;  // ecx
    char v5;  // dl
    unsigned long long v6;  // rcx
    unsigned long v7;  // rdx

    (char)v2 = ui_blocks_input();
    if ((char)v2)
    {
        v3 = *((long long *)&v0);
        if (!g_5b899 && !(char)$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(v3, &g_229c0, &g_22bb0) && !(char)$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(v3, &g_19d00, &g_19fb0) && v5 != g_5b83f)
        {
            g_5b83f = v5;
            v6 = "clock: this player %s";
            v7 = (v5 ? "no longer asks for a pause" : "asks for a pause");
            mp_log();
        }
    }
    else if (g_5b8e0 != 2 || g_5b618 == (char)v2 || g_5b61b == (char)v2 || g_5b83d == (char)v2)
    {
        v4 = 0;
    }
    else
    {
        v4 = 1;
    }
    goto *((long long *)0x5bf00);
}


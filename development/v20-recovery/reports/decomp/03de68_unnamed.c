// v20 RVA 03de68..03dfc6  (350 bytes)  explode_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[648];
    unsigned int field_288;
} struct_0;


long long explode_hook(struct_0 *a0, unsigned long long a1)
{
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x20]
    unsigned long long v4;  // rax, Other Possible Types: unsigned long
    unsigned long long v5;  // rbp
    unsigned long long v6;  // rax
    unsigned int v7;  // ecx
    unsigned long long v8;  // rcx

    if (!g_5dbd4 && (char)sub_3d19c())
    {
        v5 = $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std(a0, a1);
        (char)v6 = sub_1e510(a0, a1);
        if ((char)v6)
        {
            if (!((unsigned int)v5 == 0))
                goto LABEL_3dec6;
        }
        else
        {
            if (!(g_5dbd5 != (char)v6) && !(sub_1c334() == 1))
            {
LABEL_3dec6:
                v7 = g_5dc20;
                v4 = &(1)[g_5dc20];
                g_5dc20 = v4;
                if (!v7)
                {
                    v4 = mp_log("end: this game's own view that the side %s should leave the battle is not acted on - that is for the game that commands it", xlate_desc(a1));
                    return v4;
                }
                return v4;
            }
        }
        if ((unsigned int)v5 >= 0)
        {
            v4 = g_5db90(a0, a1);
            if ((char)xlate_desc(a0, v5))
                return v4;
            (unsigned int)v4 = xlate_desc(a0, v5);
            if ((char)v4)
            {
                v8 = a1;
                mp_log("end: the side %s leaves the battle%s - the battle ends in %.0f s; the other game is told", xlate_desc(), (g_5dbd5 != 0 ? &g_45ce6 : " (Withdraw pressed)"), *((int *)(a0 + 648)));
                *((char **)&v1) = 0;
                *((char **)&v0) = 0;
                v4 = operation_exists@Joiner@Joiner(1, a1, g_5dbd5);
                return v4;
            }
            return v4;
        }
        goto LABEL_3dec6;
    }
    v4 = g_5db90(a0, a1);
    return v4;
}


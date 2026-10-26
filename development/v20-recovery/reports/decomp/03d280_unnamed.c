// v20 RVA 03d280..03d58b  (779 bytes)  launch_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_3 {
    char paddin0[5];
    char field_5;
    char field_6;
    char paddin7[1];
    unsigned int field_8;
    unsigned int field_c;
    char paddin16[4];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
} struct_3;

typedef struct struct_0 {
    char paddin0[32];
    unsigned int field_20;
} struct_0;

typedef struct struct_4 {
    char paddin0[80];
    unsigned int field_50;
} struct_4;


long long launch_hook(unsigned long a0, struct_3 *a1)
{
    char v0;  // [bp+0x0]
    int tmp_17;  // tmp #17
    unsigned long long v2;  // rax, Other Possible Types: unsigned long
    struct_3 *v3;  // rsi
    unsigned long long v6;  // rdx
    unsigned long long v7;  // rcx
    unsigned long long v8;  // rdx
    unsigned long long v9;  // rdx
    char v10;  // r8b
    unsigned long long v11;  // r8
    unsigned int v12;  // ymm0
    unsigned long long v13;  // rcx
    unsigned long long v14;  // rbx
    unsigned long long v15;  // rcx
    unsigned int v17;  // eax
    struct_0 *v19;  // rax
    unsigned long long v20;  // rdx
    struct_4 *v22;  // rax
    unsigned int v23;  // edx
    void* v25;  // rbx, Other Possible Types: unsigned long
    unsigned long long v26;  // r8
    unsigned long v27;  // rcx
    unsigned long long v28;  // rcx

    v2 = &v0;
    v3 = a1;
    *((char *)&v2) = *((char *)(a1 + 5));
    if (*((char *)(a1 + 5)) == 1)
    {
        (unsigned int)v2 = $_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std$_Erase_tree@V?$allocator@U?$_Tree_node@U?$pair@$$CBIH@std(g_5db88, *((int *)(a1 + 8)));
        if ((unsigned int)v2 < 0)
            return v2;
        (unsigned int)v2 = xlate_desc(g_5db88, v2);
        if ((char)v2)
            return v2;
        v8 = *((int *)(v3 + 8));
        *((char **)&g_5dbd4) = 1;
        g_5db90(g_5db88, v8);
        v12 = *((int *)(v3 + 28));
        *((char **)&g_5dbd4) = 0;
        if ((((char)(CmpF(v12, 0) & 69) | (char)((CmpF(v12, 0) & 69) >> 6)) & 1) != 1)
            *((unsigned int *)(g_5db88 + 648)) = v12;
        v15 = *((int *)(v3 + 8));
        v2 = mp_log("end: the side %s leaves the battle%s, says its game - carried out here; the battle ends in %.0f s", xlate_desc(), (*((int *)(v3 + 12)) != 0 ? &g_45ce6 : " (its player pressed Withdraw)"), *((int *)(g_5db88 + 648)));
        return v2;
    }
    else if ((char)v2 == 2)
    {
        if ((char)sub_1e510())
            g_5dbd7 = *((int *)(a1 + 12)) != 0;
        *((char **)(g_5db88 + 684)) = 0;
        tmp_17 = *((int *)(a1 + 12));
        v9 = *((int *)(a1 + 8));
        v10 = (int)tmp_17 != 0;
        *((char **)&g_5dbd4) = 1;
        g_5db98(g_5db88, v9, v11);
        v13 = *((int *)(v3 + 8));
        v14 = (*((int *)(v3 + 12)) != 0 ? "fights on" : "proposes a ceasefire");
        *((char **)&g_5dbd4) = 0;
        mp_log("end: the side %s %s, says its game", xlate_desc(), v14);
        v2 = session_test_ready(g_5db88);
        return v2;
    }
    else if ((char)v2 == 4)
    {
        v6 = (*((int *)(a1 + 12)) != 0 ? "takes the ceasefire proposal back" : "proposes a ceasefire");
        v7 = "end: the other player %s";
        g_5dbd7 = *((int *)(a1 + 12)) != 0;
        mp_log();
        (unsigned int)v2 = sub_1e510("end: the other player %s", v6);
        if (!(char)v2)
        {
            v2 = input_action_hook(g_5db88);
            return v2;
        }
        return v2;
    }
    else if ((char)v2 != 3)
    {
        return v2;
    }
    else
    {
        (unsigned int)v2 = sub_1c334();
        if ((unsigned int)v2 != 2)
        {
            return v2;
        }
        else if (g_5dbd9)
        {
            return v2;
        }
        else if (*((char *)(g_5db88 + 882)))
        {
            return v2;
        }
        else
        {
            (unsigned int)v2 = sub_1bd5c();
            if ((unsigned int)v2 != 2)
                return v2;
            g_5a2f8 = *((int *)(a1 + 12));
            v17 = *((int *)(a1 + 20)) & 4;
            *((char **)&g_5dbd9) = 1;
            g_5dbdb = v17 != 0;
            (unsigned int)v19 = squad_record(g_5db88);
            if (v19)
            {
                v20 = *((int *)(v19 + 32));
                a1 = 0x2000000 | v20;
                if (!(&(1)[-(-(0 < v17))] & *((int *)(a1 + 20))))
                    a1 = -33554433 & v20;
                *((unsigned int *)(v19 + 32)) = a1;
            }
            (unsigned int)v22 = squad_record(g_5db88);
            if (v22)
            {
                v23 = *((int *)(v22 + 80));
                a1 = 0x2000000 | v23;
                if (!(!(0 < g_5dbdb) + 2 & *((int *)(v3 + 20))))
                    a1 = -33554433 & v23;
                *((unsigned int *)(v22 + 80)) = a1;
            }
            *((int *)(g_5db88 + 1084)) = *((int *)(v3 + 24));
            *((unsigned long *)(g_5db88 + 1088)) = (char)(*((int *)(v3 + 20)) >> 3) & 1;
            if (*((char *)(v3 + 6)))
            {
                v25 = 0;
                do
                {
                    v26 = 32;
                    v27 = (g_5dbdb != 0 ? v25 : (unsigned long long)&(1)[-1 * v25]);
                } while ((a1 = v27 * 32 + 76 + v3, v28 = v25 * 32 + &g_5dbe0, memcpy(), v25 = (unsigned long long)((unsigned int)v25 + 1), (unsigned int)v25 < 2));
                *((char **)&g_5dbd1) = 1;
            }
            *((int *)(g_5db88 + 648)) = 0xbf800000;
            *((char **)(g_5db88 + 1090)) = 0;
            mp_log("end: the host's battle is over (result %u there) - ending this one", *((int *)(v3 + 12)));
            end_turn_here("end: the host's battle is over (result %u there) - ending this one", a1);
            v2 = g_5dba8(g_5db88, 0, 0);
            return v2;
        }
    }
}


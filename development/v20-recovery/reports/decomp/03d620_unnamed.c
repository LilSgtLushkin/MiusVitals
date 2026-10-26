// v20 RVA 03d620..03d73b  (283 bytes)  next_turn_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[680];
    unsigned int field_2a8;
    char paddin684[198];
    char field_372;
} struct_0;


long long next_turn_hook(struct_0 *a0, unsigned long long a1, unsigned long long a2)
{
    unsigned long long v2;  // rax, Other Possible Types: unsigned long
    unsigned long long v3;  // rsi
    char v6;  // cl
    unsigned long long v7;  // rdx
    unsigned long v8;  // r9
    unsigned long v9;  // r8
    unsigned long long v10;  // rcx

    if ((char)sub_3d19c() && !*((char *)(a0 + 882)))
    {
        (unsigned int)v2 = sub_1c334();
        if ((unsigned int)v2 != 2)
        {
            v2 = g_5dba8(a0, a1, a2);
            if (*((char *)(a0 + 882)))
            {
                v3 = *((int *)(a0 + 680));
                *((char **)&g_5dbd8) = 1;
                sub_1e510(a0, 1);
                v6 = (char)xlate_desc(a0, 1) * 2 | (char)xlate_desc(a0, 0);
                v7 = v3;
                v8 = ((v6 & 2) != 0 ? &g_45ce6 : ", the other side left");
                v9 = ((v6 & 1) != 0 ? &g_45ce6 : ", this side left");
                v10 = "end: the battle is over here (result %u%s%s) - the other game is told to end its battle now";
                mp_log();
                v2 = send_table(a0);
                return v2;
            }
            return v2;
        }
        else if (g_5dbda)
        {
            return v2;
        }
        else
        {
            *((char **)&g_5dbda) = 1;
            v2 = mp_log("end: the battle has run out here - it ends when the host's game ends it");
            return v2;
        }
    }
    v2 = g_5dba8(a0, a1, a2);
    return v2;
}


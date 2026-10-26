// v20 RVA 03cc1c..03cd07  (235 bytes)  sub_3cc1c
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_3cc1c()
{
    unsigned long v0;  // [bp-0x18]

    if (!g_5db40 && !g_5db48 && !g_5db50 && !g_5db58 && !g_5db60 && !g_5db68)
        return g_5db68;
    v0 = g_5db68;
    mp_log("hit: 5 s: %lld on this game's units worked out here and sent, %lld of the other game's carried out here, %lld of this game's own on the other player's units dropped, %lld for units not here, %lld failed, %lld not carried out (the part struck could not be found here)", g_5db40, g_5db50, g_5db48, g_5db58, g_5db60);
    g_5db68 = 0;
    g_5db60 = g_5db68;
    g_5db58 = g_5db60;
    g_5db50 = g_5db58;
    g_5db48 = g_5db50;
    g_5db40 = g_5db48;
    return g_5db48;
}


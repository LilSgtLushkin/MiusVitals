// v20 RVA 034d68..034ede  (374 bytes)  sub_34d68
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_34d68()
{
    int tmp_13;  // tmp #13
    int tmp_14;  // tmp #14
    int tmp_11;  // tmp #11
    unsigned long v0;  // [bp-0x10]
    unsigned long long v3;  // rax, Other Possible Types: unsigned long

    if (g_5d058 || g_5d060 || g_5d068 || g_5d070)
    {
        tmp_13 = DivV(tmp_14, tmp_11);
        v0 = g_5d070;
        mp_log("shots: 5 s: %llu fired by this game's units and sent (%.1f KB/s, %llu repeats, %zu waiting for an answer), %llu received and fired here, %llu local ones for the other player's units dropped, %llu refused (ammunition or unit not matched)", g_5d058, tmp_13, g_5d088, g_5cfd8, g_5d068, g_5d060);
    }
    v3 = g_5d0a0;
    g_5d088 = 0;
    g_5d080 = 0;
    g_5d070 = 0;
    g_5d068 = 0;
    g_5d060 = 0;
    g_5d058 = 0;
    if (g_5d090 || g_5d098 || g_5d0a0 || g_5d0a8 || g_5d0b0 || g_5d0b8 || g_5d0c0)
    {
        v0 = g_5d0b8;
        v3 = mp_log("shots: 5 s: %llu of this game's launches were off-map fire; explosions: %llu made here and sent, %llu received, %llu local ones dropped; flares: %llu fired here and sent, %llu received, %llu local ones dropped");
    }
    g_5d0c0 = 0;
    g_5d0b8 = 0;
    g_5d0b0 = 0;
    g_5d0a8 = 0;
    g_5d0a0 = 0;
    g_5d098 = 0;
    g_5d090 = 0;
    return v3;
}


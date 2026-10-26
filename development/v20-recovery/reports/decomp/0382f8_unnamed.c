// v20 RVA 0382f8..03847a  (386 bytes)  sub_382f8
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_382f8()
{
    unsigned long v0;  // [bp-0x18]
    unsigned long v1;  // [bp-0x10]
    unsigned long v3;  // rax

    if (g_5d2c8 || g_5d2d0 || g_5d2d8 || g_5d2e0 || g_5d2e8)
    {
        v1 = g_5d2e0;
        v0 = g_5d2d0;
        mp_log("crater: %llu made here as the host, %llu of the host's made here (%llu more this game had made itself already), %llu of this game's own dropped, %llu from another thread left alone");
    }
    g_5d2e8 = 0;
    g_5d2e0 = 0;
    g_5d2d8 = 0;
    g_5d2d0 = 0;
    g_5d2c8 = 0;
    if (g_5d2a0 || g_5d2a8 || g_5d2b0 || g_5d2b8)
    {
        v0 = g_5d2b8;
        mp_log("trench: outside any dig: %llu stamped here told to the other game, %llu of the other game's stamped here, %llu of them this game had already; %llu of this game's own left out (made on the other game's word before)");
    }
    g_5d2b8 = 0;
    g_5d2b0 = 0;
    g_5d2a8 = 0;
    g_5d2a0 = 0;
    if (!g_5d280 && !g_5d288 && !g_5d290 && !g_5d298)
        return g_5d298;
    v0 = g_5d298;
    mp_log("trench: %llu squads of this player dug in and sent, %llu of the other player dug in here from his values, %llu local digs for his squads dropped, %llu for squads this game does not have");
    v0 = g_5d268;
    v3 = mp_log("trench: of his digs %llu stamped a trench here as in his game (%llu of them where this game's own dig would not have), %llu were kept from stamping one his game did not; %llu trenches stamped outside any dig so far", g_5d250 + g_5d258, g_5d258, g_5d260);
    g_5d298 = 0;
    g_5d290 = 0;
    g_5d288 = 0;
    g_5d280 = 0;
    g_5d260 = 0;
    g_5d258 = 0;
    g_5d250 = 0;
    return v3;
}


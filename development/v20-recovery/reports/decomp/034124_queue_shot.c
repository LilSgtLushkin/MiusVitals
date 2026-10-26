// v20 RVA 034124..034368  (580 bytes)  queue_shot
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long queue_shot(unsigned long long a0[13], unsigned long a1, unsigned long long a2, unsigned long long a3, unsigned int a4)
{
    char v0;  // [bp-0x228]
    int v1;  // [bp-0x208]
    int v2;  // [bp-0x1f8]
    int v3;  // [bp-0x1e8]
    int v4;  // [bp-0x1d8]
    int v5;  // [bp-0x1c8]
    int v6;  // [bp-0x1b8]
    int v7;  // [bp-0x1a8]
    int v8;  // [bp-0x198]
    int v9;  // [bp-0x188]
    int v10;  // [bp-0x178]
    int v11;  // [bp-0x168]
    int v12;  // [bp-0x158]
    int v13;  // [bp-0x148]
    unsigned long long v14;  // [bp-0x138]
    unsigned int v15;  // [bp-0x130]
    unsigned int v16;  // [bp-0x12c]
    char v17;  // [bp-0x128], Other Possible Types: unsigned int
    char v18;  // [bp-0x124]
    char v19;  // [bp-0x123]
    char v20;  // [bp-0x122]
    unsigned int v21;  // [bp-0x120]
    unsigned int v22;  // [bp-0x11c]
    unsigned int v23;  // [bp-0x118]
    unsigned int v24;  // [bp-0x114]
    char v25;  // [bp-0x110]
    char v26;  // [bp-0x10c]
    char v27;  // [bp-0x108]
    unsigned long v28;  // [bp-0x104]
    unsigned long v29;  // [bp-0xfc]
    int v30;  // [bp-0xf8]
    unsigned long v31;  // [bp-0xf4]
    char v32;  // [bp-0xec]
    char v33;  // [bp-0xe8]
    char v34;  // [bp-0xdc]
    char v35;  // [bp-0xd8]
    char v36;  // [bp-0xc8]
    char v37;  // [bp-0xb8]
    char v38;  // [bp-0xa8]
    char v39;  // [bp-0xa4]
    char v40;  // [bp-0x98]
    char v41;  // [bp-0x88]
    char v42;  // [bp-0x7c]
    char v43;  // [bp-0x78]
    char v44;  // [bp-0x68]
    char v45;  // [bp-0x58]
    char v46;  // [bp-0x50]
    unsigned long long v47;  // [bp-0x48]
    unsigned long v48;  // [bp-0x8]
    char v49;  // [bp+0x0]
    unsigned int v52;  // eax
    unsigned long long v53;  // rsi
    unsigned long v54;  // rax, Other Possible Types: unsigned int
    unsigned long v55;  // rax, Other Possible Types: unsigned int
    unsigned int v56;  // ebx

    v48 = &v49;
    v47 = g_5a380 ^ &v0;
    memset(&v17, 0, 220);
    if (!g_5d034)
    {
        v52 = GetTickCount() ^ GetCurrentProcessId() * &g_10000 | 1;
        g_5d034 = v52;
    }
    v19 = a1;
    v53 = *((long long *)(a0 + 56));
    *((char **)&g_5d030) = &(1)[g_5d030];
    v20 = a2 != 0;
    v17 = g_5d030;
    v56 = -1;
    *((char **)&v18) = 0;
    if (!v53)
        v54 = -1;
    else
        v54 = *((long long *)(g_5d000->field_0 + 448))(g_5d000, v53, *((long long *)(g_5d000->field_0 + 448)));
    v21 = v54;
    if (!*((long long *)(a0 + 64)))
        v55 = -1;
    else
        v55 = *((long long *)(g_5d000->field_0 + 448))(g_5d000);
    v22 = v55;
    ammo_check(v53, &v26, &v25);
    if (*((long long *)(a0 + 96)))
        v56 = *((long long *)(g_5d000->field_0 + 448))(g_5d000);
    v23 = v56;
    v28 = 1?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(*((long long *)(a0 + 72)));
    v29 = 1?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(*((long long *)(a0 + 80)));
    v31 = 1?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(a3);
    v24 = a4;
    if (a2)
        memcpy(&v32, a2, 16);
    memcpy(&v34, a0, 144);
    memset(&v39, 0, 32);
    memset(&v42, 0, 8);
    *((int128_t *)&v1) = *((int128_t *)&v17);
    *((int128_t *)&v2) = *((int128_t *)&v23);
    *((int128_t *)&v3) = *((int128_t *)&v27);
    *((int128_t *)&v4) = (int128_t)v30;
    *((int128_t *)&v5) = *((int128_t *)&v33);
    *((int128_t *)&v6) = *((int128_t *)&v35);
    *((int128_t *)&v7) = *((int128_t *)&v36);
    *((int128_t *)&v8) = *((int128_t *)&v37);
    *((int128_t *)&v9) = *((int128_t *)&v38);
    *((int128_t *)&v10) = *((int128_t *)&v40);
    *((int128_t *)&v11) = *((int128_t *)&v41);
    *((int128_t *)&v12) = *((int128_t *)&v43);
    *((int128_t *)&v13) = *((int128_t *)&v44);
    v14 = *((long long *)&v45);
    v15 = *((int *)&v46);
    *((char **)&v16) = 0;
    $_Emplace_back_internal@UPending(&g_5cfb8, &v1);
    return security_cookie_check(v47 ^ &v0);
}


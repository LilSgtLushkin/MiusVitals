// v20 RVA 033db4..033f81  (461 bytes)  queue_shot
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long queue_shot(unsigned long long a0[10], unsigned long a1)
{
    char v0;  // [bp-0x208]
    int v1;  // [bp-0x1e8]
    int v2;  // [bp-0x1d8]
    int v3;  // [bp-0x1c8]
    int v4;  // [bp-0x1b8]
    int v5;  // [bp-0x1a8]
    int v6;  // [bp-0x198]
    int v7;  // [bp-0x188]
    int v8;  // [bp-0x178]
    int v9;  // [bp-0x168]
    int v10;  // [bp-0x158]
    int v11;  // [bp-0x148]
    int v12;  // [bp-0x138]
    int v13;  // [bp-0x128]
    unsigned long long v14;  // [bp-0x118]
    unsigned int v15;  // [bp-0x110]
    unsigned int v16;  // [bp-0x10c]
    char v17;  // [bp-0x108], Other Possible Types: unsigned int
    char v18;  // [bp-0x104]
    unsigned int v19;  // [bp-0x100]
    unsigned int v20;  // [bp-0xfc]
    unsigned int v21;  // [bp-0xf8]
    char v22;  // [bp-0xf0]
    char v23;  // [bp-0xec]
    char v24;  // [bp-0xe8]
    unsigned long v25;  // [bp-0xe4]
    char v26;  // [bp-0xd8]
    char v27;  // [bp-0xc8]
    char v28;  // [bp-0xbc]
    char v29;  // [bp-0xb8]
    char v30;  // [bp-0xa8]
    char v31;  // [bp-0x98]
    char v32;  // [bp-0x88]
    char v33;  // [bp-0x7c]
    char v34;  // [bp-0x78]
    char v35;  // [bp-0x68]
    char v36;  // [bp-0x58]
    char v37;  // [bp-0x48]
    char v38;  // [bp-0x38]
    char v39;  // [bp-0x30]
    unsigned long long v40;  // [bp-0x28]
    unsigned long v41;  // [bp-0x8]
    char v42;  // [bp+0x0]
    unsigned int v46;  // eax
    unsigned long long v47;  // rdx
    unsigned long long v48;  // rbx
    unsigned long v49;  // rax, Other Possible Types: unsigned int
    unsigned long v50;  // rax, Other Possible Types: unsigned int

    v41 = &v42;
    v40 = g_5a380 ^ &v0;
    memset(&v17, 0, 220);
    if (!g_5d034)
    {
        v46 = GetTickCount() ^ GetCurrentProcessId() * &g_10000 | 1;
        g_5d034 = v46;
    }
    v47 = *((long long *)(a0 + 64));
    v48 = *((long long *)(a0 + 72));
    *((char **)&g_5d030) = &(1)[g_5d030];
    v17 = g_5d030;
    *((char **)&v18) = 1;
    if (!v47)
        v49 = -1;
    else
        v49 = *((long long *)(g_5d000->field_0 + 448))(g_5d000);
    v20 = v49;
    if (!v48)
        v50 = -1;
    else
        v50 = *((long long *)(g_5d000->field_0 + 448))(g_5d000, v48, *((long long *)(g_5d000->field_0 + 448)));
    v19 = v50;
    v21 = -1;
    ammo_check(v48, &v23, &v22);
    v25 = 1?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std?$_Tree_temp_node@V?$allocator@U?$_Tree_node@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std(a1);
    memcpy(&v28, a0, 88);
    memset(&v33, 0, 24);
    *((int128_t *)&v1) = *((int128_t *)&v17);
    *((int128_t *)&v2) = *((int128_t *)&v21);
    *((int128_t *)&v3) = *((int128_t *)&v24);
    *((int128_t *)&v4) = *((int128_t *)&v26);
    *((int128_t *)&v5) = *((int128_t *)&v27);
    *((int128_t *)&v6) = *((int128_t *)&v29);
    *((int128_t *)&v7) = *((int128_t *)&v30);
    *((int128_t *)&v8) = *((int128_t *)&v31);
    *((int128_t *)&v9) = *((int128_t *)&v32);
    *((int128_t *)&v10) = *((int128_t *)&v34);
    *((int128_t *)&v11) = *((int128_t *)&v35);
    *((int128_t *)&v12) = *((int128_t *)&v36);
    *((int128_t *)&v13) = *((int128_t *)&v37);
    v14 = *((long long *)&v38);
    v15 = *((int *)&v39);
    *((char **)&v16) = 0;
    $_Emplace_back_internal@UPending(&g_5cfb8, &v1);
    return security_cookie_check(v40 ^ &v0);
}


// v20 RVA 033f84..034124  (416 bytes)  queue_shot
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long queue_shot(unsigned long long a0, unsigned long a1, unsigned long long a2, unsigned long a3)
{
    char v0;  // [bp-0x228]
    unsigned int v1;  // [bp-0x208]
    unsigned int v2;  // [bp-0x200]
    int v3;  // [bp-0x1f8]
    int v4;  // [bp-0x1e8]
    int v5;  // [bp-0x1d8]
    int v6;  // [bp-0x1c8]
    int v7;  // [bp-0x1b8]
    int v8;  // [bp-0x1a8]
    int v9;  // [bp-0x198]
    int v10;  // [bp-0x188]
    int v11;  // [bp-0x178]
    int v12;  // [bp-0x168]
    int v13;  // [bp-0x158]
    int v14;  // [bp-0x148]
    int v15;  // [bp-0x138]
    unsigned long long v16;  // [bp-0x128]
    unsigned int v17;  // [bp-0x120]
    unsigned int v18;  // [bp-0x11c]
    char v19;  // [bp-0x118], Other Possible Types: unsigned int
    char v20;  // [bp-0x114]
    unsigned int v21;  // [bp-0x110]
    unsigned int v22;  // [bp-0x10c]
    unsigned int v23;  // [bp-0x108]
    char v24;  // [bp-0xf8]
    char v25;  // [bp-0xe8]
    char v26;  // [bp-0xdc]
    char v27;  // [bp-0xd8]
    char v28;  // [bp-0xcc]
    char v29;  // [bp-0xc8]
    char v30;  // [bp-0xbc]
    char v31;  // [bp-0xb8]
    char v32;  // [bp-0xb4]
    char v33;  // [bp-0xa8]
    char v34;  // [bp-0x98]
    char v35;  // [bp-0x88]
    char v36;  // [bp-0x78]
    char v37;  // [bp-0x68]
    char v38;  // [bp-0x58]
    char v39;  // [bp-0x48]
    char v40;  // [bp-0x40]
    unsigned long long v41;  // [bp-0x38]
    unsigned long v42;  // [bp-0x8]
    char v43;  // [bp+0x0]
    char v45;  // [bp+0x28]
    unsigned int v47;  // eax

    v42 = &v43;
    v41 = g_5a380 ^ &v0;
    v1 = a1;
    v2 = a3;
    memset(&v19, 0, 220);
    if (!g_5d034)
    {
        v47 = GetTickCount() ^ GetCurrentProcessId() * &g_10000 | 1;
        g_5d034 = v47;
    }
    *((char **)&v20) = 9;
    *((char **)&g_5d030) = &(1)[g_5d030];
    v19 = g_5d030;
    v23 = -1;
    v22 = -1;
    v21 = -1;
    memcpy(&v26, a0, 12);
    memcpy(&v28, a2, 16);
    memcpy(&v30, &a1, 4);
    memcpy(&v31, &a3, 4);
    memcpy(&v32, &v45, 4);
    *((int128_t *)&v3) = *((int128_t *)&v19);
    *((int128_t *)&v4) = *((int128_t *)&v23);
    *((int128_t *)&v5) = *((int128_t *)&v24);
    *((int128_t *)&v6) = *((int128_t *)&v25);
    *((int128_t *)&v7) = *((int128_t *)&v27);
    *((int128_t *)&v8) = *((int128_t *)&v29);
    *((int128_t *)&v9) = *((int128_t *)&v31);
    *((int128_t *)&v10) = *((int128_t *)&v33);
    *((int128_t *)&v11) = *((int128_t *)&v34);
    *((int128_t *)&v12) = *((int128_t *)&v35);
    *((int128_t *)&v13) = *((int128_t *)&v36);
    *((int128_t *)&v14) = *((int128_t *)&v37);
    *((int128_t *)&v15) = *((int128_t *)&v38);
    v16 = *((long long *)&v39);
    v17 = *((int *)&v40);
    *((char **)&v18) = 0;
    $_Emplace_back_internal@UPending(&g_5cfb8, &v3);
    return security_cookie_check(v41 ^ &v0);
}


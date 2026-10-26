// v20 RVA 0297c0..029886  (198 bytes)  ui_start
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long ui_start()
{
    char v0;  // [bp-0x168]
    unsigned long v1;  // [bp-0x148]
    unsigned long v2;  // [bp-0x140]
    void* v3;  // [bp-0x138]
    char v4;  // [bp-0x128]
    unsigned long long v5;  // [bp-0x18]
    unsigned long long v7;  // rcx

    v5 = g_5a380 ^ &v0;
    $sprintf_s@$0BAE$sprintf_s@$0BAE(&v4, "%s\\users\\mp_last_host.txt", &g_5c6d0);
    v3 = 0;
    if (fopen_s(&v3, &v4, "w"))
        return security_cookie_check(v5 ^ &v0);
    v7 = v3;
    if (v7)
    {
        *((int *)&v2) = (g_5c469 != 0 ? &g_45ce6 : "upnp");
        v1 = &g_5c8a8;
        fprintf(v7, "%s\n%s\n%s\n%s\n", &g_5c690, &g_5c8b0);
        fclose(v3);
        return security_cookie_check(v5 ^ &v0);
    }
    return security_cookie_check(v5 ^ &v0);
}


// v20 RVA 02a09c..02a12f  (147 bytes)  operation_exists@Joiner
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long operation_exists@Joiner(unsigned long long a0)
{
    char v0;  // [bp-0xb8]
    char v1;  // [bp-0x98]
    unsigned long long v2;  // [bp-0x18]
    char *v4;  // rax

    v2 = g_5a380 ^ &v0;
    strncpy_s(&v1, 128, a0, 127);
    *((char **)&g_5c8b0) = 0;
    (unsigned int)v4 = strrchr(&v1, 58);
    if (v4)
    {
        *(&v4) = 0;
        0bad_array_new_length@std@std(&g_5c8b0, v4 + 1);
    }
    strncpy_s(&g_5c690, 64, &v1, 63);
    return security_cookie_check(v2 ^ &v0);
}


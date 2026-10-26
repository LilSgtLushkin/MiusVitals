// v20 RVA 03fa80..03fb9e  (286 bytes)  mp_log
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct sockaddr {
    unsigned short sin_family;
    char sa_data[14];
} sockaddr;


long long mp_log(unsigned long long *a0, void* a1, unsigned long a2)
{
    char v0;  // [bp-0x78]
    unsigned int v1;  // [bp-0x48]
    unsigned int v2;  // [bp-0x44]
    void* v3;  // [bp-0x40]
    sockaddr v4;  // [bp-0x38]
    char v5;  // [bp-0x36]
    unsigned long long v6;  // [bp-0x28]

    v6 = g_5a380 ^ &v0;
    if ((char)steam_init())
    {
        if (!g_5df68)
        {
            *((char **)&v1) = 0;
            if ((char)g_5de40(g_5de78, &v1, 0))
            {
                v3 = 0;
                if ((char)g_5de48(g_5de78, a1, a2, &v1, &v3) && v1)
                {
                    g_5df28 = &(1)[g_5df28];
                    *(a0) = v3;
                    return security_cookie_check(v6 ^ &v0);
                }
            }
        }
        else
        {
            *((char **)&v2) = 16;
            if ((int)recvfrom(g_5a300, a1, a2, 0, &v4, &v2) > 0)
            {
                g_5df28 = &(1)[g_5df28];
                *(a0) = ntohs(*((short *)&v5)) - &g_9c40;
                return security_cookie_check(v6 ^ &v0);
            }
        }
    }
    return security_cookie_check(v6 ^ &v0);
}


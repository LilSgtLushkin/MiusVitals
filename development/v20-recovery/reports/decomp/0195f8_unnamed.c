// v20 RVA 0195f8..0196bc  (196 bytes)  state_load
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char paddin12[4];
    char field_10;
} struct_0;


long long state_load(unsigned long long a0, unsigned long a1, struct_0 *a2)
{
    char v0;  // [bp-0x88]
    char v1;  // [bp-0x68]
    unsigned int v2;  // [bp-0x64]
    unsigned int v3;  // [bp-0x60]
    unsigned long v4;  // [bp-0x58]
    char v5;  // [bp-0x50]
    char v6;  // [bp-0x40]
    unsigned long long v7;  // [bp-0x20]
    unsigned int v9;  // edi

    v7 = g_5a380 ^ &v0;
    v4 = a0;
    v9 = 0;
    *((char **)&v3) = 0;
    *((char **)&v2) = 0;
    0?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std?$_Hash@V?$_Umap_traits@IIV?$_Uhash_compare@IU?$hash@I@std();
    *((char **)&v3) = 1;
    if (!*((char *)(a2 + 16)))
    {
        return security_cookie_check(v7 ^ &v0);
    }
    else if (a2->field_0 != 2)
    {
        return security_cookie_check(v7 ^ &v0);
    }
    else if (!*((int *)(a2 + 4)))
    {
        return security_cookie_check(v7 ^ &v0);
    }
    else if (*((int *)(a2 + 8)) > 0)
    {
        do
        {
            $_Try_emplace@V?$basic_string@DU?$char_traits@D@std$_Try_emplace@V?$basic_string@DU?$char_traits@D@std(a0, &v5, $?HDU?$char_traits@D@std$?HDU?$char_traits@D@std(a0, &v2, &v1));
            _Tidy_deallocate@?$basic_string@DU?$char_traits@D@std(&v6);
            v9 = &(1)[v9];
        } while (v9 < *((int *)(a2 + 8)));
        return security_cookie_check(v7 ^ &v0);
    }
    else
    {
        return security_cookie_check(v7 ^ &v0);
    }
}


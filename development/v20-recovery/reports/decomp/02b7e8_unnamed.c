// v20 RVA 02b7e8..02bac1  (729 bytes)  type_address
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char field_0;
} struct_0;


long long type_address(unsigned long a0, unsigned long long a1, unsigned long a2)
{
    char v0;  // [bp-0xf8]
    unsigned short v1;  // [bp-0xd8]
    unsigned short v2;  // [bp-0xd6]
    char v3;  // [bp-0xb8]
    unsigned long long v4;  // [bp-0x48]
    char *v6;  // rdi, Other Possible Types: unsigned long
    unsigned short v7;  // ax
    unsigned long long v8;  // rbx, Other Possible Types: unsigned long
    unsigned long v10;  // rbx, Other Possible Types: unsigned long long
    char v11;  // r15b
    char v12;  // bpl
    unsigned long long v13;  // rbx, Other Possible Types: unsigned long
    void* v14;  // rbp
    char *v15;  // rbx, Other Possible Types: unsigned long
    struct_0 *v16;  // rdi, Other Possible Types: unsigned long long, unsigned long
    unsigned long long v17;  // r15

    v4 = g_5a380 ^ &v0;
    v6 = strlen();
    v7 = GetAsyncKeyState(17) >> 15;
    v1 = v7;
    v8 = 48;
    v2 = GetAsyncKeyState(16) >> 15;
    do
    {
        if ((char)key_pressed(v8) && !(char)v2 && v6 < a1)
        {
            *((char *)(a0 + v6)) = v8;
            v6 = &(1)[v6];
            *((char *)(v6 + a0)) = v2;
        }
    } while ((v8 += 1, (unsigned int)v8 <= 57));
    v10 = 96;
    do
    {
        if ((char)key_pressed(v10) && v6 < a1)
        {
            *((char **)(a0 + v6)) = v10 - 48;
            v6 = &(1)[v6];
            *((char **)(v6 + a0)) = 0;
        }
    } while ((v8 += 1, (unsigned int)v8 <= 57));
    v11 = 0;
    v12 = 0;
    v13 = 65;
    while (true)
    {
        if ((char)key_pressed(v13))
        {
            if (!v1)
            {
                if (!(char)a2 && v6 < a1)
                {
                    *((char **)(a0 + v6)) = &(32)[v13];
                    v6 = &(1)[v6];
                    *((char **)(v6 + a0)) = 0;
                }
            }
            else
            {
                if ((unsigned int)v13 == 86)
                {
                    v11 = 1;
                    v13 = &(1)[v13];
                    continue;
                }
            }
        }
        v13 = &(1)[v13];
        if ((unsigned int)v13 > 90)
            break;
    }
    if ((!(!(char)key_pressed(190)) || !(!(char)key_pressed(110))) && !(char)a2 && v6 < a1)
    {
        *((char **)(a0 + v6)) = 46;
        v6 = &(1)[v6];
        *((char **)(v6 + a0)) = 0;
    }
    if ((char)key_pressed(189) && !(char)a2 && v6 < a1)
    {
        *((char **)(a0 + v6)) = 45;
        v6 = &(1)[v6];
        *((char **)(v6 + a0)) = 0;
    }
    if ((char)key_pressed(186) && (char)v2)
        v12 = (!(char)a2 ? 0 : 1);
    if ((char)key_pressed(8) && v6)
        *((char **)(a0 + v6 - 1)) = 0;
    if ((char)key_pressed(46))
        *((char **)a0) = 0;
    if (v12)
        *((char **)&g_5c7dc) = 1;
    if (!v11)
    {
        return security_cookie_check(v4 ^ &v0);
    }
    else if (OpenClipboard(0))
    {
        v14 = GetClipboardData(1);
        if (v14)
        {
            v15 = GlobalLock(v14);
            if (v15)
            {
                v16 = 0;
                if (*(v15))
                {
                    while (true)
                    {
                        v17 = &(1)[v16];
                        if (v17 >= 100)
                            break;
                        a0 = *((char *)v15);
                        if ((char)a2)
                        {
                            if (isdigit(a0) == 0)
                                goto LABEL_2ba48;
                        }
                        else
                        {
                            if (!(isalnum(a0)) && !(*((char *)v15) == 46) && !((char)v7 == 58) && !((char)v7 == 45))
                                goto LABEL_2ba48;
                        }
                        (&v3)[v16] = *((char *)v15);
                        v16 = v17;
LABEL_2ba48:
                        v15 = &(1)[v15];
                        if (!*((char *)v15))
                            break;
                    }
                    if (v16 >= 100)
                    {
                        sub_4078c();
                        [D] Unsupported jumpkind Ijk_SigTRAP at address 178880
                    }
                }
                *((char **)&(&v3)[v16]) = 0;
                if (!(char)a2)
                    operation_exists@Joiner@Joiner(&v3);
                else
                    strncpy_s(a0, a1, &v3, a1 - 1);
                GlobalUnlock(v14);
            }
        }
        CloseClipboard();
        return security_cookie_check(v4 ^ &v0);
    }
    else
    {
        return security_cookie_check(v4 ^ &v0);
    }
}


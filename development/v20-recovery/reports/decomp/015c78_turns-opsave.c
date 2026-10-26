// v20 RVA 015c78..015d23  (171 bytes)  turns@opsave
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


int turns@opsave()
{
    unsigned long v0;  // [bp+0x8]
    char v1;  // [bp+0x18]
    unsigned long v2;  // rdx
    unsigned long long v4[2];  // rcx, Other Possible Types: unsigned long
    unsigned long long v6;  // rdx
    unsigned long long v7;  // rdi
    void* v8;  // rbx, Other Possible Types: unsigned long

    memcpy(&v0, &(4)[v4[0]], 4);
    memcpy(&v1, &(8)[v4[0]], 4);
    v8 = 0;
    if ((int)v0 > 0)
    {
        while (true)
        {
            v6 = v4[0];
            v7 = v8 * 128 + *((int *)&v1);
            if (v7 + 128 > *((long long *)(v4 + 8)) - v6)
                break;
            v4 = v6 + v7;
            if (!strncmp(v4, v2, 32))
                return;
            v8 = &(1)[v8];
            if ((unsigned int)v8 >= (int)v0)
                break;
        }
    }
    return;
}


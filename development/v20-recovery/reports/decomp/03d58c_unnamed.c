// v20 RVA 03d58c..03d61e  (146 bytes)  input_action_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[660];
    char field_294;
    char paddin661[23];
    unsigned int field_2ac;
} struct_0;


long long input_action_hook(struct_0 *a0)
{
    int tmp_7;  // tmp #7
    char v1;  // al

    if (!g_5dbd6 || !g_5dbd7)
    {
        tmp_7 = *((char *)(a0 + 660)) & 1;
        *((char **)&g_5dbd4) = 1;
        if ((char)tmp_7)
            g_5dba0();
    }
    else
    {
        v1 = *((int *)(a0 + 660)) & 3;
        *((char **)&g_5dbd4) = 1;
        if (v1 != 3)
        {
            *((char **)(a0 + 684)) = 0;
            g_5dba0();
            g_5db98(a0);
        }
    }
    *((char **)&g_5dbd4) = 0;
    return session_test_ready(a0);
}


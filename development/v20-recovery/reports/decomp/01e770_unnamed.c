// v20 RVA 01e770..01e825  (181 bytes)  box_result_hook
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long box_result_hook(unsigned long a0)
{
    int v1;  // xmm1
    int v2;  // xmm6

    v2 = v1;
    if ((char)ui_blocks_input())
    {
        if (((char)((CmpF((unsigned int)v2, g_5a218) & 69) >> 2) & 1) || (CmpF((unsigned int)v2, g_5a218) & 64))
        {
            g_5a218 = (unsigned int)v2;
            mp_log("clock: this player asks for speed x%.2f", (unsigned int)v2);
        }
        if (g_5b8e0 != 2)
            sub_d590();
    }
    goto *((long long *)376568);
}


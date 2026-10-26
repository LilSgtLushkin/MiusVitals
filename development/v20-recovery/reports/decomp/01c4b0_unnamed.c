// v20 RVA 01c4b0..01c544  (148 bytes)  hook_GetRawInputData
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


int hook_GetRawInputData()
{
    char v1;  // cl

    if (!v1)
    {
        if (g_5b61d && g_5b900 && g_5b904 == 2)
            CloseHandle(CreateThread(0, 0, find_local_ip, g_5b900, 0, 0));
        *((char **)&g_5b900) = 0;
        *((char **)&g_5b904) = 0;
    }
    g_5b61d = v1;
    SetEnvironmentVariableA("MP_UPNP", -(0 < v1) & "1");
    return;
}


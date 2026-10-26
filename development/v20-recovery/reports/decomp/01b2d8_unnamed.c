// v20 RVA 01b2d8..01b380  (168 bytes)  sub_1b2d8
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long sub_1b2d8(unsigned long a0)
{
    char *v1;  // rdx, Other Possible Types: unsigned long

    sub_1e2c4();
    explode_hook(0);
    *((char **)&g_5b8e0) = 1;
    *((unsigned int *)&g_5b8e4) = a0;
    SetEnvironmentVariableA("MP_ROLE", "host");
    v1 = ((unsigned int)a0 == 3 ? "skirmish" : "mission");
    if ((unsigned int)a0 == 2)
        v1 = "operation";
    SetEnvironmentVariableA("MP_HOST_MODE", v1);
    *((char **)&g_5b61f) = 0;
    SetEnvironmentVariableA("MP_SWAP", "0");
    SetEnvironmentVariableA("MP_PEER_NAME", 0);
    *((char **)&g_5bbb0) = 0;
    find_local_ip();
    return host_listen();
}


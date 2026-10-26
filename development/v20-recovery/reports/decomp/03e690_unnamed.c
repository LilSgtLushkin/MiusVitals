// v20 RVA 03e690..03e783  (243 bytes)  end_turn_here
// pseudo-C from angr; strings are exact, control flow is structured, names are injected


long long end_turn_here(unsigned long long a0, unsigned long long a1)
{
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x10]
    char v3;  // bl
    void* v4;  // rdi
    unsigned long long v5;  // rcx
    unsigned long long v6;  // rdx
    unsigned long long v7;  // rax, Other Possible Types: unsigned long

    v3 = a1;
    v4 = a0;
    if (!g_5dbd4 && (char)sub_3d19c())
    {
        g_5dbd6 = (char)a1;
        v5 = "end: this player %s";
        v6 = ((char)a1 ? "takes the ceasefire proposal back" : "proposes a ceasefire");
        mp_log();
        if (!(char)sub_1e510("end: this player %s", v6))
        {
            *((char **)&v1) = 0;
            *((char **)&v0) = 0;
            operation_exists@Joiner@Joiner(4, 0, v3);
            v7 = input_action_hook(v4);
            return v7;
        }
        *((char *)&v6) = v3;
        g_5dba0(v4, v6);
        *((char **)&v1) = 0;
        *((char **)&v0) = 0;
        operation_exists@Joiner@Joiner(2);
        v7 = session_test_ready(v4);
        return v7;
    }
    v7 = g_5dba0(a0, a1);
    return v7;
}


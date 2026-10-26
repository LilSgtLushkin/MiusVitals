// v20 RVA 023174..0232d9  (357 bytes)  button
// pseudo-C from angr; strings are exact, control flow is structured, names are injected

typedef struct struct_0 {
    char paddin0[312];
    unsigned long long field_138;
} struct_0;


char button(struct_0 *a0)
{
    unsigned long long v1;  // rax

    (char)v1 = readable(*((long long *)(a0 + 312)), 496);
    if (!(char)v1)
        return v1;
    return v1;
}


#include "env_value.h"
#include <stdio.h>
#include <string.h>

static int checks, failed;
static void check(bool ok, const char *label) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", label); }
}
int main()
{
    const char *name = "MIUS_ENV_VALUE_SELFTEST";
    char text[8] = "old";
    int value = 42;
    SetEnvironmentVariableA(name, nullptr);
    check(!env_text(name, text, sizeof text) && !text[0], "missing clears buffer");
    check(!env_integer(name, &value) && value == 42, "missing preserves default");
    SetEnvironmentVariableA(name, "1234567");
    check(env_text(name, text, sizeof text) && !strcmp(text, "1234567"), "longest fitting text");
    SetEnvironmentVariableA(name, "12345678");
    check(!env_text(name, text, sizeof text) && !text[0], "oversized is not parsed");
    SetEnvironmentVariableA(name, "-37");
    check(env_integer(name, &value) && value == -37, "signed integer");
    SetEnvironmentVariableA(name, "1garbage");
    check(!env_integer(name, &value) && value == -37, "trailing garbage preserves default");
    SetEnvironmentVariableA(name, "9999999999999999999999999999999999999999999");
    check(!env_integer(name, &value) && value == -37, "overlong integer preserves default");
    SetEnvironmentVariableA(name, "2147483648");
    check(!env_integer(name, &value) && value == -37, "out of int range preserves default");
    SetEnvironmentVariableA(name, "0");
    check(env_integer(name, &value) && value == 0, "explicit zero");
    SetEnvironmentVariableA(name, nullptr);
    printf("environment selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

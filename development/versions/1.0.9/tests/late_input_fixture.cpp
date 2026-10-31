// Test-only DLL loaded after the first input scan. It imports user32 just like a game module.
#include <windows.h>
extern "C" __declspec(dllexport) SHORT late_input_probe() { return GetAsyncKeyState(VK_F12); }

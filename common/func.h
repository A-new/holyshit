#pragma once
#include <windows.h>

void LoadMap();

bool HasDebuggee();
BOOL InjectIt(HANDLE hrp, LPCSTR DllPath/*, const DWORD dwRemoteProcessld*/)/*×¢ÈëÖ÷º¯Êý */;
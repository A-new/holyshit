#pragma once
#include <windows.h>
#include <string>

extern HWND g_ollyWnd;

void about();
void LoadMap();

// call od functions
bool HasDebuggee();


BOOL InjectIt(HANDLE hrp, LPCSTR DllPath/*, const DWORD dwRemoteProcessld*/)/*×¢ÈëÖ÷º¯Êý */;


bool IsSysFile(const TCHAR* DllPath);
std::string wstring2string(const std::wstring & rwString, UINT codepage);

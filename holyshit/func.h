#pragma once
#include <windows.h>
#include <string>
#include "define.h"

extern HWND g_ollyWnd;

void about();
void LoadMap();

// call od functions
bool HasDebuggee();

std::tstring GetDebugeedExePath();

bool IsSysFile(const TCHAR* DllPath);
std::string wstring2string(const std::wstring & rwString, UINT codepage);

#pragma once
#include <windows.h>

void hook_DRAWFUNC_cpudasm();

//void hook_CreateProcessInternalW();
void hook_DllCheck();

// ÎªODÌí¼ÓÓ²¼þ
void hook_0040869C();
void hook_00439191();

void hook_Sethardwarebreakpoint();
void hook( PVOID *ppPointer, PVOID pDetour );
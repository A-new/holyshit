#pragma once
#include <windows.h>

void hook( PVOID *ppPointer, PVOID pDetour );

//void hook_CreateProcessInternalW();

// ΪOD���Ӳ��
void hook_0040869C();
void hook_00439191();
void hook_Sethardwarebreakpoint();

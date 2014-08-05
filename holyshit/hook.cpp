#include "hook.h"
#include <windows.h>
#include "detours.h"
#include "../sdk/sdk.h"
#pragma comment(lib, "detours_x86.lib")
void hook( PVOID *ppPointer, PVOID pDetour )
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(ppPointer, pDetour);
    DetourTransactionCommit();
}

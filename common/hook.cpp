#include "hook.h"
#include <windows.h>
#include "detours.h"
#include "../sdk/sdk.h"

void hook( PVOID *ppPointer, PVOID pDetour )
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(ppPointer, pDetour);
    DetourTransactionCommit();
}

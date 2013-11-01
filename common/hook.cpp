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

static DWORD g_value;
static DWORD g_set;
PVOID Org0040869C = (PVOID)0x0040869C; // od1.10
void __declspec(naked) My0040869C()
{
    /*Gettextxy*/
    __asm
    {
        jmp Org0040869C;
    }
}
void hook_0040869C()
{
    hook(&(PVOID&)Org0040869C, My0040869C);

}


PVOID Org00439191 = (PVOID)0x00439191; // od1.10
void __declspec(naked) My00439191()
{
    __asm
    {
        jmp Org00439191;
    }
}
void hook_00439191()
{
    hook(&(PVOID&)Org00439191, My00439191);

}

#ifdef HOLYSHIT_EXPORTS // od1
typedef int (cdecl *SETHARDWAREBREAKPOINT)(ulong addr,int size,int type);

SETHARDWAREBREAKPOINT Sethardwarebreakpoint_Org = Sethardwarebreakpoint;
int cdecl Sethardwarebreakpoint_hook(ulong addr,int size,int type)
{
    return Sethardwarebreakpoint_Org(addr, size, type);
}
void hook_Sethardwarebreakpoint()
{
    hook(&(PVOID&)Sethardwarebreakpoint_Org, Sethardwarebreakpoint_hook);
}
#endif

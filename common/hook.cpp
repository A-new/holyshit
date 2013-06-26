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


typedef
BOOL
(WINAPI
*CREATEPROCESSINTERNALW)(HANDLE hToken,
                       LPCWSTR lpApplicationName,
                       LPWSTR lpCommandLine,
                       LPSECURITY_ATTRIBUTES lpProcessAttributes,
                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                       BOOL bInheritHandles,
                       DWORD dwCreationFlags,
                       LPVOID lpEnvironment,
                       LPCWSTR lpCurrentDirectory,
                       LPSTARTUPINFOW lpStartupInfo,
                       LPPROCESS_INFORMATION lpProcessInformation,
                       PHANDLE hNewToken);

CREATEPROCESSINTERNALW CreateProcessInternalW = NULL;

#include <Shellapi.h>
#include <shlwapi.h>
//#pragma comment(lib, "Shlapi.lib") // CommandLineToArgvW

#pragma comment(lib, "shlwapi.lib") // CommandLineToArgvW
BOOL
WINAPI
 MyCreateProcessInternalW(HANDLE hToken,
 LPCWSTR lpApplicationName,
 LPWSTR lpCommandLine,
 LPSECURITY_ATTRIBUTES lpProcessAttributes,
 LPSECURITY_ATTRIBUTES lpThreadAttributes,
 BOOL bInheritHandles,
 DWORD dwCreationFlags,
 LPVOID lpEnvironment,
 LPCWSTR lpCurrentDirectory,
 LPSTARTUPINFOW lpStartupInfo,
 LPPROCESS_INFORMATION lpProcessInformation,
 PHANDLE hNewToken)
{
    //return CreateProcessInternalW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles
    //    , dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);

    if (!lpApplicationName && lpCommandLine)
    {
        LPWSTR *szArglist;
        int nArgs;

        szArglist = CommandLineToArgvW(lpCommandLine, &nArgs);
        if(szArglist )
        {
            if (nArgs == 2 && StrStrIW(szArglist[0], L"loaddll.exe"))
            {
                dwCreationFlags |= CREATE_SUSPENDED;
                BOOL bRet = CreateProcessInternalW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles
                    , dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);

                if (bRet)
                {
                    LPVOID lpAddr = VirtualAllocEx(lpProcessInformation->hProcess, 0, 1000,MEM_COMMIT, PAGE_EXECUTE_READWRITE);
                    if (lpAddr)
                    {
                        CONTEXT ct;
                        ct.ContextFlags = CONTEXT_CONTROL; // eip
                        GetThreadContext(lpProcessInformation->hThread, &ct);
                        //*((DWORD*)lpAddr) = ct.Eip;
                        WriteProcessMemory(lpProcessInformation->hProcess, lpAddr, &ct.Eip, 4, 0);
                        HMODULE hKer = GetModuleHandleW(L"kernel32.dll");
                        PROC pp = GetProcAddress(hKer, "LoadLibraryA");
                        //*((DWORD*)lpAddr + 1) = GetProcAddress(hKer, "LoadLibraryA");
                        WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)((DWORD)lpAddr+4), &pp, 4, 0);
                        char loadsys[MAX_PATH] = {0};
                        GetModuleFileNameA(NULL, loadsys, MAX_PATH);
                        char* p = strrchr(loadsys, '\\');
                        if (p)
                        {
                            *(p + 1) = 0;
                            strcat(loadsys, "loadsys.dll");
                        }
                        WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)((DWORD)lpAddr+8), loadsys, MAX_PATH, 0);

                        const char shell[] = {0x6A, 0x00, 0x60, 0x9C, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x81, 0xE6, 0x00, 0x00, 0xFF, 0xFF, 0xAD, 0x89, 0x44, 0x24, 0x24, 0xAD, 0x56, 0xFF, 0xD0, 0x9D, 0x61, 0xC3};
                        WriteProcessMemory(lpProcessInformation->hProcess, (LPVOID)((DWORD)lpAddr+8 + MAX_PATH), shell, MAX_PATH, 0);
                        ct.Eip = (DWORD)lpAddr+8 + MAX_PATH;
                        SetThreadContext(lpProcessInformation->hThread, &ct);
                        ResumeThread(lpProcessInformation->hThread);
                        //VirtualFreeEx(lpProcessInformation->hProcess, lpAddr, 0, MEM_RELEASE);
                    }

                }
                LocalFree(szArglist);
                return bRet;
            }
            LocalFree(szArglist);
        }
    }
    return CreateProcessInternalW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles
        , dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
}

void hook_CreateProcessInternalW()
{
    HMODULE hMod = GetModuleHandleW(L"kernel32.dll");
    if (hMod)
    {
        CreateProcessInternalW = (CREATEPROCESSINTERNALW)GetProcAddress(hMod, "CreateProcessInternalW");
        if (CreateProcessInternalW)
        {
            hook(&(PVOID&)CreateProcessInternalW, MyCreateProcessInternalW);
        }
    }
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

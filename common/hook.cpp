#include "hook.h"
#include <windows.h>
#include "detours.h"
#include "../sdk/sdk.h"


LONG WINAPI hook( PVOID *ppPointer, PVOID pDetour )
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(ppPointer, pDetour);
    DetourTransactionCommit();
    return 0;
}


DRAWFUNC *g_func;

#define COLUMN_LABELS 4
int DRAWFUNC_cpudasm( TCHAR *s, uchar *mask, int *select, t_table *pt, t_sortheader *ps,int column, void * cache )
{
    if (column == COLUMN_LABELS)
    {
        t_dump *p = sdk_Getcpudisasmdump();
        ulong addr = 0;

#ifdef HOLYSHIT_EXPORTS
        addr = p->lastaddr;
#else
        addr = p->addr;
        addr += *((ulong*)((char*)cache + 0x20)); // fuck! cache是自定义结构，但0x20应该不会变化太大
        //t_drawheader *td = (t_drawheader*)ps;
        //addr = td->nextaddr; // 下一行的地址,shit
#endif

        if (addr)
        {
            int len = Findname(addr, NM_LABEL, s);
            if (len)
            {
                memset(mask, DRAW_HILITE, len);
                *select = DRAW_MASK | DRAW_VARWIDTH;
            }
            return len;
        }
        return 0;
    }
    else
    {
#ifdef HOLYSHIT_EXPORTS
        return g_func(s, (char*)mask, select, ps, column);
#else
        return g_func(s, mask, select, pt, ps, column, cache);
#endif
    }
}

#ifdef HOLYSHIT_EXPORTS
int MyDRAWFUNC(char *s,char *mask,int *select,t_sortheader *ps,int column)//(char *,char *,int *,t_sortheader *,int)
{
    return DRAWFUNC_cpudasm(s, (uchar*)mask, select, 0, ps, column, 0);
}
#endif


class CHookOnce
{
public:
    CHookOnce()
    {
        t_dump *td = sdk_Getcpudisasmdump();
        t_table *p = &td->table;
        if (p)
        {
            t_bar* tb = &p->bar;
            tb->nbar = 5;

            tb->name[COLUMN_LABELS] = _T("Label");
            tb->mode[COLUMN_LABELS] = BAR_NOSORT;
            tb->defdx[COLUMN_LABELS] = 100;
            tb->dx[COLUMN_LABELS] = 100;

            tb->defdx[3] = 200;
            tb->dx[3] = 200;

            g_func = p->drawfunc;

#ifdef HOLYSHIT_EXPORTS
            hook(&(PVOID&)g_func, MyDRAWFUNC);
#else // od2
            hook(&(PVOID&)g_func, DRAWFUNC_cpudasm);
#endif

            InvalidateRect(p->hw, NULL, TRUE);

        }
    }
};

void hook_DRAWFUNC_cpudasm()
{
    static CHookOnce a;
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

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
PVOID Org00477754 = (PVOID)0x00477754; // od1.10
void __declspec(naked) My00477754()
{
    static const char* ebp08;
    static DWORD* change;
    __asm{
        push eax
        mov eax, dword ptr [ebp + 0x08]
        mov ebp08, eax
        lea eax, dword ptr [ebp - 0x8]
        mov change, eax
        pop eax
        pushad
        pushfd
    }
    static const char* p;
    p = strrchr(ebp08, '.');
    if (p)
    {
        if (0 == StrCmpNIA(p, ".sys", 4))
        {
            *change =  1;
        }
    }

    p = strrchr(ebp08, '\\') ;
    if (p)
    {
        p += 1;
        if (0 == stricmp(p, "ntoskrnl.exe"))
        {
            *change =  1;
        }
    }

    __asm{
        popfd
        popad
        jmp Org00477754
    }
}

void hook_00477754()
{
    hook(&(PVOID&)Org00477754, My00477754);
}
#include "hook.h"
#include <windows.h>
#include "detours.h"
#include "../sdk/sdk.h"
#include <boost/thread/mutex.hpp>

int width_label;
int width_comment;

void hook( PVOID *ppPointer, PVOID pDetour )
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(ppPointer, pDetour);
    DetourTransactionCommit();
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

//typedef void    (cdecl *DEFAULTBAR)(t_bar *pb);
//DEFAULTBAR OrgDefaultbar;
//void cdecl MyDefaultbar(t_bar *pb)
//{
//    OrgDefaultbar(pb);
//}
//void hook_Defaultbar()
//{
//    HMODULE hMod = GetModuleHandle(NULL);
//    //int temp = (int)&;
//    OrgDefaultbar = (DEFAULTBAR)GetProcAddress(hMod, "_Defaultbar");
//    hook(&(PVOID&)OrgDefaultbar, MyDefaultbar);
//}

extern int width_label;
extern int width_comment;

void hook_DRAWFUNC_cpudasm()
{
    static boost::mutex mu;
    static bool hooked = false;
    if (!hooked)
    {
        boost::mutex::scoped_lock lLock(mu);
        if (!hooked)
        {
            t_dump *td = sdk_Getcpudisasmdump();
            t_table *p = &td->table;
            if (p && p->hw)
            {
                t_bar* tb = &p->bar;
                tb->nbar = 5;

                tb->name[COLUMN_LABELS] = _T("Label");
                tb->mode[COLUMN_LABELS] = BAR_NOSORT;

                int size_font = tb->dx[2] / tb->defdx[2];
                tb->defdx[COLUMN_LABELS] = width_label / size_font; // 字符串长度，需要重新计算，否则后面调用Defaultbar会显示不正常
                tb->dx[COLUMN_LABELS] = width_label;

                tb->defdx[3] = width_comment / size_font;
                tb->dx[3] = width_comment;

                g_func = p->drawfunc;

#ifdef HOLYSHIT_EXPORTS
                hook(&(PVOID&)g_func, MyDRAWFUNC);
#else // od2
                hook(&(PVOID&)g_func, DRAWFUNC_cpudasm);
#endif

                InvalidateRect(p->hw, NULL, TRUE);
                hooked = true;

                //hook_Defaultbar();
            }
        }
    }

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
#ifdef HOLYSHIT_EXPORTS
PVOID OrgDllCheck = (PVOID)0x00477754; // od1.10
#else
PVOID OrgDllCheck = (PVOID)0x0044BFCE; // od2
#endif
PVOID OrgDllCheck2 = (PVOID)0x00458257; // only for od2


bool IsSysFile(const TCHAR* DllPath)
{
    const TCHAR* p;
    p = _tcsrchr(DllPath, '.');
    if (p)
    {
        if (0 == StrCmpNI(p, _T(".sys"), 4))
        {
            return true;
            // *change =  1;
        }
    }

    p = _tcsrchr(DllPath, _T('\\')) ;
    if (p)
    {
        p += 1;

        if (0 == _tcsicmp(p, _T("ntoskrnl.exe")))
        {
            return true;
            //*change =  1;
        }
    }
    return false;
}
void __declspec(naked) MyDllCheck()
{
    static const TCHAR* DllPath;
    static DWORD* change;
    __asm{
        push eax
        mov eax, dword ptr [ebp + 0x08]
        mov DllPath, eax
#ifdef HOLYSHIT_EXPORTS
        lea eax, dword ptr [ebp - 0x08]
#else
        lea eax, dword ptr [ebp - 0x0C]
#endif
        mov change, eax
        pop eax
        pushad
        pushfd
    }
    if (IsSysFile(DllPath))
    {
        *change = 1;
    }

    __asm{
        popfd
        popad
        jmp OrgDllCheck
    }
}

BOOL __cdecl Mycompare(const TCHAR* DllPath, const TCHAR* r)
{
    if(IsSysFile(DllPath))
    {
        return 1;
    }
    return 0;
}
void __declspec(naked) MyDllCheck2()
{
    __asm
    {
        call Mycompare;
        test eax,eax;
        je l1
        ADD DWORD PTR [ESP],0x2 // 跟原call一样，居然修改参数
l1:
        jmp OrgDllCheck2;
    }
    //static const TCHAR* DllPath;
    //
    //__asm{
    //    push eax
    //    mov eax, dword ptr [esp + 0x4]
    //    mov DllPath, eax
    //    pop eax
    //    pushad
    //    pushfd
    //}

    //if (IsSysFile(DllPath))
    //{
    //    __asm{
    //        mov eax, 1
    //    }
    //}

    //__asm{
    //    popfd
    //    popad
    //    jmp OrgDllCheck2
    //}
}

void hook_DllCheck()
{
    hook(&(PVOID&)OrgDllCheck, MyDllCheck);
#ifndef HOLYSHIT_EXPORTS // only for od2
    hook(&(PVOID&)OrgDllCheck2, MyDllCheck2);
#endif
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

int get_width_label()
{
    t_dump *td = sdk_Getcpudisasmdump();
    if (td)
    {
        t_table *p = &td->table;
        if (p)
        {
            t_bar* tb = &p->bar;
            if (tb && tb->nbar == 5)
            {
                return tb->dx[COLUMN_LABELS];
            }
        }
    }
    return 0;
}

int get_width_comment()
{
    t_dump *td = sdk_Getcpudisasmdump();
    if (td)
    {
        t_table *p = &td->table;
        if (p)
        {
            t_bar* tb = &p->bar;
            if (tb && tb->nbar == 5)
            {
                return tb->dx[3];
            }
        }
    }
    return 0;
}

void set_width_label(int i)
{
    width_label = i;
}

void set_width_comment(int i)
{
    width_comment = i;
}


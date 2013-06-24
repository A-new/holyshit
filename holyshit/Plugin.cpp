#include <windows.h>
#include "../sdk/sdk.h"
#include "../common/func.h"
#include "../common/hook.h"
#include "../common/toolbar.h"

extc int  _export cdecl ODBG_Plugindata(char shortname[32])
{
    strcpy(shortname, "HolyShit");
    return PLUGIN_VERSION;
}


HWND g_ollyWnd;
extc int  _export cdecl ODBG_Plugininit(int ollydbgversion,HWND hw,
                                        ulong *features)
{
    g_ollyWnd = hw;
    if(CToolbar_Global.init("D:\\src\\vc\\holyshit\\common\\test.ini"))
        CToolbar_Global.attach((HWND)Plugingetvalue(VAL_HWMAIN));

    // 在此处CPUDASM还没有创建起来


    //hook_CreateProcessInternalW();
    hook_DllCheck();

    //hook_Sethardwarebreakpoint();
    //hook_0040869C();
    //hook_00439191();
    return 0;
}




int ODBG_Pluginmenu(int origin,char data[4096],void *item)
{
    if(origin == PM_MAIN)
    {
        strcpy(data,"0 Load map files, 1 About");
        return 1;
    }
    return 0;
}

void ODBG_Pluginaction(int origin,int action,void *item)
{
    static bool IsFirst = TRUE;

    switch(action)
    {
    case 0:
        MessageBoxA(g_ollyWnd, "sorry, not done yet:(", "...", MB_ICONINFORMATION);
        //LoadMap();
        break;

    case 1:
        {	
            MessageBoxA(g_ollyWnd,"HolyShit v0.2\r\nCopyright (C) 2013 lynnux\r\nlynnux@qq.com"
                "\r\n\r\nSpecial thanks: 疯子,zclyj,the author of IDAFicator"
                ,"About",MB_ICONINFORMATION);		
        }
        break;
    default:
        break;
    }

}
//
//int ODBG_Pluginuddrecord(t_module *pmod,int ismainmodule,
//                                             ulong tag,ulong size,void *data)
//{
//    if (ismainmodule)
//    {
//        char* p = (pmod->name);
//        p = 0;
//    }
//
//    return 0;
//}


//int ODBG_Pausedex(int reason, int extdata, t_reg *reg, DEBUG_EVENT *debugevent)
//{
//
//}

HMODULE g_hModule;

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
class CInjectOnce
{
public:
    CInjectOnce()
    {

    }
protected:
private:
};

bool bInjected = false;

void  ODBG_Pluginmainloop(DEBUG_EVENT *debugevent) 
{
    hook_DRAWFUNC_cpudasm();
    if (debugevent && debugevent->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
    {
        bInjected = false;
    }

    //if (debugevent)
    //{
    //    DWORD dw = debugevent->dwDebugEventCode;
    //    dw = 2;
    //}
}

int  ODBG_Paused(int reason,t_reg *reg)
{
    if (reason == PP_TERMINATED)
    {
        bInjected = false;
    }
 
    if (!bInjected)
    {
        if (reason == PP_EVENT)
        {
            const char* file = (const char*)Plugingetvalue(VAL_EXEFILENAME);
            if (StrRStrI(file, 0, TEXT(".sys"))
                || StrRStrI(file, 0, TEXT("ntoskrnl.exe")))
            {
                HANDLE hProcess = (HANDLE)Plugingetvalue(VAL_HPROCESS);
                HANDLE hThread = (HANDLE)Plugingetvalue(VAL_HMAINTHREAD);
                if (hProcess && hThread)
                {

                    LPVOID lpAddr = VirtualAllocEx(hProcess, 0, 1000,MEM_COMMIT, PAGE_EXECUTE_READWRITE);
                    if (lpAddr)
                    {
                        CONTEXT ct;
                        ct.ContextFlags = CONTEXT_CONTROL; // eip
                        GetThreadContext(hThread, &ct);
                        //*((DWORD*)lpAddr) = ct.Eip;
                        WriteProcessMemory(hProcess, lpAddr, &ct.Eip, 4, 0);
                        HMODULE hKer = GetModuleHandleW(L"kernel32.dll");
                        PROC pp = GetProcAddress(hKer, "LoadLibraryA");
                        //*((DWORD*)lpAddr + 1) = GetProcAddress(hKer, "LoadLibraryA");
                        WriteProcessMemory(hProcess, (LPVOID)((DWORD)lpAddr+4), &pp, 4, 0);

                        CHAR loadsys[MAX_PATH];
                        GetModuleFileNameA(g_hModule, loadsys, MAX_PATH);
                        LPSTR pFind = StrRChrA(loadsys, 0, '\\');
                        if (pFind)
                        {
                            pFind += 1;
                            *pFind = 0;
                            lstrcatA(loadsys, "loadsys.dll");
                        }
                        else
                        {
                            lstrcpyA(loadsys, "loadsys.dll");
                        }
                        WriteProcessMemory(hProcess, (LPVOID)((DWORD)lpAddr+8), loadsys, MAX_PATH, 0);

                        const char shell[] = {0x6A, 0x00, 0x60, 0x9C, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x81, 0xE6, 0x00, 0x00, 0xFF, 0xFF, 0xAD, 0x89, 0x44, 0x24, 0x24, 0xAD, 0x56, 0xFF, 0xD0, 0x9D, 0x61, 0xC3};
                        WriteProcessMemory(hProcess, (LPVOID)((DWORD)lpAddr+8 + MAX_PATH), shell, MAX_PATH, 0);
                        ct.Eip = (DWORD)lpAddr+8 + MAX_PATH;

                        reg->modified = TRUE;

                        //Setbreakpoint(reg->ip, TY_ONESHOT, 0);
                        reg->ip = ct.Eip;

                        Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 1, VK_F9); 
                        //Go(GetThreadId(hThread), reg->ip, STEP_RUN, 0, 0);

                        //2.Setdisasm(dw,1,CPU_ASMHIST);

                        //SetThreadContext(hThread, &ct);
                        bInjected = true;
                        //ResumeThread(hThread);
                    }

                    //if (hProcess)
                    //{
                    //    CHAR szFile[MAX_PATH];
                    //    GetModuleFileNameA(g_hModule, szFile, MAX_PATH);
                    //    LPSTR pFind = StrRChrA(szFile, 0, '\\');
                    //    if (pFind)
                    //    {
                    //        pFind += 1;
                    //        *pFind = 0;
                    //        lstrcatA(szFile, "loadsys.dll");
                    //    }
                    //    else
                    //    {
                    //        lstrcpyA(szFile, "loadsys.dll");
                    //    }
                    //    if(InjectIt(hProcess, szFile)
                    //        )
                    //        bInjected = true;
                    //}
                }
            }
        }
    }
    return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;
    }
    return TRUE; 
}
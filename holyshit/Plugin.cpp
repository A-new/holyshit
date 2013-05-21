#include <windows.h>
#include "../sdk/sdk.h"
#include "../common/func.h"
#include "../common/hook.h"

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

    // 在此处CPUDASM还没有创建起来


    //hook_CreateProcessInternalW();
    hook_00477754();
    return 0;
}


//void  ODBG_Pluginmainloop(DEBUG_EVENT *debugevent) 
//{
//    hook_DRAWFUNC_cpudasm();
//    if (debugevent)
//    {
//        DWORD dw = debugevent->dwDebugEventCode;
//        dw = 2;
//    }
//}

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
                "\r\n\r\nSpecial thanks: 疯子"
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
int  ODBG_Paused(int reason,t_reg *reg)
{
    //reason = 1;
    if (reason == PP_EVENT)
    {
        if (!bInjected)
        {
            const char* file = (const char*)Plugingetvalue(VAL_EXEFILENAME);
            if (StrRStrI(file, 0, TEXT(".sys"))
                || StrRStrI(file, 0, TEXT("ntoskrnl.exe")))
            {
                HANDLE hProcess = (HANDLE)Plugingetvalue(VAL_HPROCESS);
                if (hProcess)
                {
                    CHAR szFile[MAX_PATH];
                    GetModuleFileNameA(g_hModule, szFile, MAX_PATH);
                    LPSTR pFind = StrRChrA(szFile, 0, '\\');
                    if (pFind)
                    {
                        pFind += 1;
                        *pFind = 0;
                        lstrcatA(szFile, "loadsys.dll");
                    }
                    else
                    {
                        lstrcpyA(szFile, "loadsys.dll");
                    }
                    if(InjectIt(hProcess, szFile)
                        )
                        bInjected = true;
                }
            }
        }
    }
    else if (reason == PP_TERMINATED)
    {
        bInjected = false;
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
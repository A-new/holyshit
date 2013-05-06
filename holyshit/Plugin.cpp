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
    return 0;
}


void  ODBG_Pluginmainloop(DEBUG_EVENT *debugevent) 
{
    hook_DRAWFUNC_cpudasm();
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
        LoadMap();
        break;

    case 1:
        {	
            MessageBoxA(g_ollyWnd,"HolyShit v0.1\r\nCopyright (C) 2013 lynnux\r\nlynnux@qq.com","About",MB_ICONINFORMATION);		
        }
        break;
    default:
        break;
    }

}

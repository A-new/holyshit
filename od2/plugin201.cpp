#include "../sdk/sdk.h"
#include "../common/hook.h"
#include "../common/func.h"
#include "../common/toolbar.h"
#include "../common/loadsys.h"
#include "../common/label.h"
#include "../common/define.h"
#include <Shlwapi.h>
HMODULE g_hModule = NULL;
extc int __cdecl ODBG2_Pluginquery(int ollydbgversion,ulong *features,
                                   wchar_t pluginname[SHORTNAME],wchar_t pluginversion[SHORTNAME]) 
{
    if (ollydbgversion != 201)
        return 0;

    wcscpy(pluginname, PLUGIN_NAME);       // Name of plugin
    wcscpy(pluginversion, L"003");       // Version of plugin
    return PLUGIN_VERSION;               // Expected API version
};



extc int __cdecl ODBG2_Plugininit(void) 
{
    g_ollyWnd = hwollymain;

    // label相关初始化
    int width_label = DEFAULT_WIDTH_LABEL;
    int width_comment = DEFAULT_WIDTH_COMMENT;
    Getfromini(NULL, PLUGIN_NAME, WIDTH_LABEL, L"%i", &width_label);
    Getfromini(NULL, PLUGIN_NAME, WIDTH_COMMENT, L"%i", &width_comment);
    set_width_label(width_label);
    set_width_comment(width_comment);
    hook_label_functions(); // 如果之前退出OD时关闭了汇编窗口，下次启动时有时窗口仍然没有创建，也跟OD1一样，需要在mainloop里加

    // loadsys相关初始化
    hook_loadsys_functions();

    // toolbar相关初始化
    WCHAR szTB[MAX_PATH];
    Getfromini(NULL, PLUGIN_NAME, INI_PATH, L"%s", &szTB);
    if (PathFileExistsW(szTB))
    {
        std::string path = wstring2string(szTB, CP_ACP);
        if(CToolbar_Global.init(path))//"D:\\src\\vc\\holyshit\\common\\test.ini"
            CToolbar_Global.attach(hwollymain);
    }
    else
    {
        szTB[0] = 0;
        Writetoini(NULL, PLUGIN_NAME, INI_PATH, L"%s", szTB);
    }

    return 0;
}

void ODBG2_Plugindestroy(void)
{
    int i = get_width_label();
    if(i)
        Writetoini(NULL, PLUGIN_NAME, WIDTH_LABEL, L"%i", i);
    i = get_width_comment();
    if(i)
        Writetoini(NULL, PLUGIN_NAME, WIDTH_COMMENT, L"%i", i);
}
// ODBG2_Pluginclose是可以被撒消的
int ODBG2_Pluginclose(void)
{
    return 0;
}

static int MloadMap(t_table *pt,wchar_t *name,ulong index,int mode)
{
    if (mode==MENU_VERIFY)
        return MENU_NORMAL;                // Always available
    else if (mode==MENU_EXECUTE) 
    {
        LoadMap();
        return MENU_REDRAW;
    };
    return MENU_ABSENT;
}

static int MAbout(t_table *pt,wchar_t *name,ulong index,int mode)
{
    if (mode==MENU_VERIFY)
        return MENU_NORMAL;                // Always available
    else if (mode==MENU_EXECUTE) 
    {
        about();
        return MENU_REDRAW;
    };
    return MENU_ABSENT;
}


static t_menu mainmenu[] = {
    //{ L"load map file..",
    //L"load map file ,which maybe from IDA or dede",
    //K_NONE, MloadMap, NULL, 0 },
    { L"About",
    L"about holyshit",
    K_NONE, MAbout, NULL, 0 },
    { L"nothing more..",
    L"about holyshit",
    K_NONE, MAbout, NULL, 0 },
    { NULL, NULL, K_NONE, NULL, NULL, 0 }
};
extc t_menu *ODBG2_Pluginmenu(wchar_t *type)
{
    if (wcscmp(type,PWM_MAIN)==0)
        // Main menu.
        return mainmenu;
    return NULL;
}

bool bInjected = false;
void ODBG2_Pluginmainloop(DEBUG_EVENT *debugevent)
{
    hook_label_functions(); // 否则有可能遗漏
    //if (debugevent && debugevent->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
    //{
    //    bInjected = false;
    //}
}
void ODBG2_Pluginreset(void)
{
    bInjected = false;
}
void  ODBG2_Pluginnotify(int code,void *data,
                         ulong parm1,ulong parm2)
{
    if (code == PN_STATUS)
    {
        if (parm1 == STAT_CLOSING
            || parm1 == STAT_FINISHED)
        {
            bInjected = false;
        }

        if(parm1 == STAT_PAUSED)
        {
            if (!bInjected)
            {
                if (rundll 
                    && IsSysFile(executable))
                {
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
                    if(InjectIt(process, loadsys))
                    {
                        bInjected = true;
                        //wchar_t* p = StrRChrW(executable, 0 ,L'\\') + 1;
                        //t_module* tm = Findmodulebyname(p);
                        //Setcpu(0,tm->entry,0,0,0,
                        //    CPU_ASMHIST|CPU_ASMCENTER|CPU_ASMFOCUS);
                        Run(STAT_RUNNING, 0); // sendmessage不起作用
                    }
                }
            }
        }
    }
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



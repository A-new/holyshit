#include "../sdk/sdk.h"
#include "../common/hook.h"
#include "../common/func.h"
#include "../common/toolbar.h"

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
    int width_label = DEFAULT_WIDTH_LABEL;
    int width_comment = DEFAULT_WIDTH_COMMENT;
    Getfromini(NULL, PLUGIN_NAME, WIDTH_LABEL, L"%i", &width_label);
    Getfromini(NULL, PLUGIN_NAME, WIDTH_COMMENT, L"%i", &width_comment);
    set_width_label(width_label);
    set_width_comment(width_comment);

    hook_DRAWFUNC_cpudasm(); // 如果之前退出OD时关闭了汇编窗口，下次启动时有时窗口仍然没有创建，也跟OD1一样，需要在mainloop里加
    hook_DllCheck();

    if(CToolbar_Global.init("D:\\src\\vc\\holyshit\\common\\test.ini"))
        CToolbar_Global.attach(hwollymain);

    return 0;
}

int ODBG2_Pluginclose(void)
{
    int i = get_width_label();
    if(i)
        Writetoini(NULL, PLUGIN_NAME, WIDTH_LABEL, L"%i", i);
    i = get_width_comment();
    if(i)
        Writetoini(NULL, PLUGIN_NAME, WIDTH_COMMENT, L"%i", i);
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

static t_menu mainmenu[] = {
    { L"load map file..",
    L"load map file ,which maybe from IDA or dede",
    K_NONE, MloadMap, NULL, 0 },
    { L"odhs plugin v0.1",
    L"About odhs plugin",
    K_NONE, MloadMap, NULL, 0 },
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
    hook_DRAWFUNC_cpudasm();
    if (debugevent && debugevent->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
    {
        bInjected = false;
    }
}

void  ODBG2_Pluginanalyse(t_module *pmod)
{
    //Run(STAT_RUNNING, 0);
     Resumeallthreads();
    //Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 1, VK_F9); 
    if (pmod)
    {
    wchar_t* p = pmod->modname; 
    p = pmod->path;
    }
}
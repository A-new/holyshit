#include "command_OD.h"
#include "func.h"
#include "../sdk/sdk.h"

static void Explorer_Open(ARG_LIST_PTR)
{
    if (HasDebuggee())
    {
        std::tstring path = GetDebugeedExePath();
        path.insert(0, TEXT("/select, "));
        //CString str = _T("/select, E:\\TestDir\\test.txt");
        ShellExecute(NULL, TEXT("open"), TEXT("explorer.exe"), path.c_str(), NULL, SW_SHOWNORMAL );
    }
}

#ifndef HOLYSHIT_EXPORTS
typedef int (__cdecl* MSEARCH)(t_table *pt,wchar_t *name,ulong index,int mode);
static void OD2_SearchString(ARG_LIST_PTR)
{
    if (HasDebuggee())
    {
        MSEARCH pSearch = (MSEARCH)*((DWORD**) HARDCODE(0x00552AC4));
        MSEARCH pInit = (MSEARCH)(*(DWORD**) HARDCODE(0x005532D4));
        if (pSearch&& pInit)
        {
            t_dump *td = sdk_Getcpudisasmdump();
            if (td)
            {
                // 相当于初始化，这个很容易跟踪到
                pInit(&td->table, L"$", 0, 0);

                // 参考004253CF
                Suspendallthreads();
                pSearch(&td->table, L"", 0, 1);
                Resumeallthreads();
            }
        }
    }
}
#endif

static void Pluginmenu(ARG_LIST_PTR args)
{
    HMODULE hMod = GetModuleHandleA(args->at(0).c_str());
    if (hMod)
    {

#ifdef HOLYSHIT_EXPORTS
        typedef void (cdecl *ODBG_PLUGINACTION)(int origin,int action,void *item); 
        ODBG_PLUGINACTION call = (ODBG_PLUGINACTION)GetProcAddress(hMod, "_ODBG_Pluginaction");
        if (!call)
        {
            return;
        }
        int origin = 0;
        if (0 == stricmp(args->at(1).c_str(), "MAIN"))
        {
            origin = PM_MAIN;
        }
        else if(0 == stricmp(args->at(1).c_str(), "DISASM"))
        {
            origin = PM_DISASM;
        }
        else
            return;

        int action = atoi(args->at(2).c_str());
        call(origin, action, NULL);
#else
    typedef t_menu* (__cdecl *ODBG2_PLUGINMENU)(wchar_t *type);
    ODBG2_PLUGINMENU menu_get = (ODBG2_PLUGINMENU)GetProcAddress(hMod, "ODBG2_Pluginmenu");
    if(menu_get)
    {
        t_menu * menu = NULL;
        if (0 == stricmp(args->at(1).c_str(), "MAIN"))
        {
            menu = menu_get(PWM_MAIN);
        }
        else if(0 == stricmp(args->at(1).c_str(), "DISASM"))
        {
            menu = menu_get(PWM_DISASM);
        }

        if (menu)
        {
            int action = atoi(args->at(2).c_str());
            menu += action;
            menu->menufunc(NULL, NULL, 0, MENU_EXECUTE);
        }
    }
#endif

    }

}
namespace Command
{
    void RegisterBultinCommand()
    {
        CCommand_Single.Register("explorer", Explorer_Open);
        CCommand_Single.Register("Pluginmenu", Pluginmenu);
#ifndef HOLYSHIT_EXPORTS
        CCommand_Single.Register("SearchString", OD2_SearchString);
#endif
    }
}

#include "ustrref.h"
#include "../sdk/sdk.h"
#include "func.h"
#include "command.h"

static int MSearch(t_table *pt,wchar_t *name,ulong index,int mode)
{
    if (mode==MENU_VERIFY)
        return MENU_NORMAL;                // Always available
    else if (mode==MENU_EXECUTE) 
    {
        CCommand_Single.Invoke("SearchString", 0);
        return MENU_REDRAW;
    };
    return MENU_ABSENT;
}

static t_menu mainmenu1[] = {
    { L"ËÑË÷ËùÓÐ×Ö·û´®",
    L"ËÑË÷ËùÓÐ½»²æ×Ö·û´®",
    K_NONE, MSearch, NULL, 0 },
    { NULL, NULL, K_NONE, NULL, NULL, 0 }
};

t_menu * UStrRef::ODBG2_Pluginmenu( wchar_t *type )
{
    if (wcscmp(type,PWM_DISASM)==0)
    {
        if (!HasDebuggee())
        {
            return NULL;
        }
        return mainmenu1;
    }
    return NULL;
}
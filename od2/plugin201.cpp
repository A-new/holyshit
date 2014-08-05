#include <Shlwapi.h>
#include <algorithm>
#include <boost/bind.hpp>

#include "holyshit.cpp"
#include "ustrref.cpp"
//#include "StrFinder.cpp"
#include "OD2str.cpp"

/*
OD2�������ã�
���ޣ�Ĭ��unicode���룩
TODO:
���м���cpp��release����ʱʹ���˽�ֹ�Ż��������뷽��ȥ��
*/

typedef std::vector<IPlugin201*> IPLUGIN_LIST;
IPLUGIN_LIST plugins_all;

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
    CConfig* pCConfig = new CConfig;
    pCConfig->set_mod(g_hModule);

    // label
    Label* pLabel = new Label(pCConfig);
    plugins_all.push_back(pLabel);
    pCConfig->setILabelForConfig(pLabel);

    // jmpstack
    plugins_all.push_back(new JmpStack(pCConfig));

    // loadsys
    plugins_all.push_back(new LoadSys());

    // command
    plugins_all.push_back(new CCommand_OD());

    // toolbar
    plugins_all.push_back(new Toolbar(pCConfig));

    //ustrref
    plugins_all.push_back(new UStrRef());

    // str_patch
    plugins_all.push_back(new str_patch(pCConfig));

    // configֻ�������
    pCConfig->loadall();
    plugins_all.push_back(pCConfig);

    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin201::ODBG2_Plugininit,
        _1));

    return 0;
}

void ODBG2_Plugindestroy(void)
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin201::ODBG2_Plugindestroy,
        _1));

    IPLUGIN_LIST::iterator i = plugins_all.begin();
    for (; i != plugins_all.end(); ++i)
    {
        delete *i;
    }
    plugins_all.clear();
}

// ODBG2_Pluginclose�ǿ��Ա�������
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

// OD1��������ţ��޷��ﵽ���������
extc t_menu *ODBG2_Pluginmenu(wchar_t *type)
{
    static std::vector<t_menu> all_menu;
    all_menu.clear();

    for (IPLUGIN_LIST::iterator i = plugins_all.begin(); i != plugins_all.end(); ++i)
    {
        t_menu *p =  (*i)->ODBG2_Pluginmenu(type);
        if(p)
        {
            while(p->name)
            {
                all_menu.push_back(*p);
                ++p;
            }
        }
    }

    // ĩβ��null menu�������MAIN�Ļ���Ҫ������ģ����Ϣ
    if (wcscmp(type,PWM_MAIN)==0)
    {
        t_menu* p = mainmenu;
        for (size_t t = 0; t < sizeof(mainmenu)/sizeof(t_menu); ++t, ++p)
        {
            all_menu.push_back(*p);
        }
    }
    else
    {
        all_menu.push_back(mainmenu[sizeof(mainmenu)/sizeof(t_menu) - 1]);
    }

    if (all_menu.empty())
    {
        return NULL;
    }
    else
    {
        return &all_menu[0];
    }
}

void ODBG2_Pluginmainloop(DEBUG_EVENT *debugevent)
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin201::ODBG2_Pluginmainloop,
        _1, debugevent));
}
void ODBG2_Pluginreset(void)
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin201::ODBG2_Pluginreset,
        _1));
}
void  ODBG2_Pluginnotify(int code,void *data,
                         ulong parm1,ulong parm2)
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin201::ODBG2_Pluginnotify,
        _1, code, data, parm1, parm2));
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



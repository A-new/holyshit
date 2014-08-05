#include <windows.h>
#include <vector>
#include <algorithm>
#include <boost/bind.hpp>

#include "holyshit.cpp"
/*
编译OD1特殊设置：
多字节
/J 默认char无符号
/Zp1 结构对齐1字节
TODO:
还有几个cpp在release编译时使用了禁止优化，必须想方法去除
*/

typedef std::vector<IPlugin110*> IPLUGIN_LIST;
IPLUGIN_LIST plugins_all;

extc int  _export cdecl ODBG_Plugindata(char shortname[32])
{
    strcpy(shortname, PLUGIN_NAME);
    return PLUGIN_VERSION;
}

HMODULE g_hModule;

extc int  _export cdecl ODBG_Plugininit(int ollydbgversion,HWND hw,
                                        ulong *features)
{
    if (ollydbgversion != 110)
    {
        return -1;
    }
    g_ollyWnd = hw;

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

    pCConfig->loadall();
    plugins_all.push_back(pCConfig); // 配置也算是plugin，只是ODBG_Plugininit里的代码必须在这里写

    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin110::_ODBG_Plugininit,
        _1, ollydbgversion, hw, features));

    return 0;
}


void cdecl ODBG_Plugindestroy(void)
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin110::_ODBG_Plugindestroy, _1));

    IPLUGIN_LIST::iterator i = plugins_all.begin();
    for (; i != plugins_all.end(); ++i)
    {
        delete *i;
    }
    plugins_all.clear();
}

// ODBG_Pluginclose是可以被撒消的
int cdecl ODBG_Pluginclose(void)
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin110::_ODBG_Pluginclose, _1));
    return 0;
}


int ODBG_Pluginmenu(int origin,char data[4096],void *item)
{
    if(origin == PM_MAIN)
    {
        strcpy(data,"0 Load map files, 1 About");
        //strcpy(data,"1 About");
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
        {

        }
        break;

    case 1:
        {	
            about();	
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


bool bInjected = false;
void  ODBG_Pluginmainloop(DEBUG_EVENT *debugevent) 
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin110::_ODBG_Pluginmainloop, _1, debugevent));
}

// 新进程创建之前调用，只能做一般的内部变量清理工作，获取不到handle of process
void ODBG_Pluginreset(void)
{
    bInjected = false;
    // od1和od2里获取到的exefile都是错的
}
int  ODBG_Paused(int reason,t_reg *reg)
{
    return 0;
}

extc void _export cdecl ODBG_Pluginsaveudd(t_module *pmod,int ismainmodule)
{
}

// 只有当OD不认的tag才会调用，所以一般断不下来
extc int  _export cdecl ODBG_Pluginuddrecord(t_module *pmod,int ismainmodule,
                                             ulong tag,ulong size,void *data)
{
    size = 0;
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
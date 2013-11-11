#ifndef IPLUGIN110_H
#define IPLUGIN110_H

#include <windows.h>

struct t_module;
struct t_reg;

#ifndef PLUGIN_VERSION
#define PLUGIN_VERSION 110 
#endif
typedef unsigned long  ulong;

class IPlugin110
{
public:
    virtual ~IPlugin110(){}
    virtual int  _ODBG_Plugindata(char shortname[32]){return PLUGIN_VERSION;}
    virtual int  _ODBG_Plugininit(int ollydbgversion,HWND hw,
        ulong *features){return 0;}
    virtual void _ODBG_Pluginmainloop(DEBUG_EVENT *debugevent){}
    virtual void _ODBG_Pluginsaveudd(t_module *pmod,int ismainmodule){}
    virtual int  _ODBG_Pluginuddrecord(t_module *pmod,int ismainmodule,
        ulong tag,ulong size,void *data){return 0;}
    virtual int  _ODBG_Pluginmenu(int origin,char data[4096],void *item){return 0;}
    virtual void _ODBG_Pluginaction(int origin,int action,void *item){}
    virtual int  _ODBG_Pluginshortcut(
        int origin,int ctrl,int alt,int shift,int key,
        void *item){return 0;}
    virtual void _ODBG_Pluginreset(void){}
    virtual int  _ODBG_Pluginclose(void){return 0;}
    virtual void _ODBG_Plugindestroy(void){}
    virtual int  _ODBG_Paused(int reason,t_reg *reg){return 0;}
    virtual int  _ODBG_Pausedex(int reasonex,int dummy,t_reg *reg,
        DEBUG_EVENT *debugevent){return 0;}
    virtual int  _ODBG_Plugincmd(int reason,t_reg *reg,char *cmd){return 0;}

};

#endif // IPLUGIN110_H

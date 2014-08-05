#ifndef IPLUGIN201_H
#define IPLUGIN201_H

#include <windows.h>

struct t_module;
struct t_run;
struct t_disasm;
struct t_thread;
struct t_reg;
struct t_dump;
struct t_menu;
struct t_control;
struct t_uddsave;
#ifndef SHORTNAME
#define SHORTNAME      32
#endif
//#ifndef PLUGIN_VERSION
//#define PLUGIN_VERSION 0x02010001
//#endif
typedef unsigned char  uchar;
typedef unsigned long  ulong;          // Unsigned long

class IPlugin201
{
public:
    virtual ~IPlugin201(){}
    //virtual int ODBG2_Pluginquery(int ollydbgversion,ulong *features,
    //    wchar_t pluginname[SHORTNAME],
    //    wchar_t pluginversion[SHORTNAME]){return PLUGIN_VERSION;}
    virtual int ODBG2_Plugininit(void){return 0;}
    virtual void ODBG2_Pluginanalyse(t_module *pmod){}
    virtual void ODBG2_Pluginmainloop(DEBUG_EVENT *debugevent){}
    virtual int ODBG2_Pluginexception(t_run *prun,const t_disasm *da,
        t_thread *pthr,t_reg *preg,wchar_t *message){return 0;}
    virtual void ODBG2_Plugintempbreakpoint(ulong addr,
        const t_disasm *da,t_thread *pthr,t_reg *preg){}
    virtual void ODBG2_Pluginnotify(int code,void *data,
        ulong parm1,ulong parm2){}
    virtual int ODBG2_Plugindump(t_dump *pd,wchar_t *s,uchar *mask,
        int n,int *select,ulong addr,int column){return 0;}
    virtual t_menu * ODBG2_Pluginmenu(wchar_t *type){return 0;}
    virtual t_control * ODBG2_Pluginoptions(UINT msg,WPARAM wp,LPARAM lp){return 0;}
    virtual void ODBG2_Pluginsaveudd(t_uddsave *psave,t_module *pmod,
        int ismainmodule){}
    virtual void ODBG2_Pluginuddrecord(t_module *pmod,int ismainmodule,
        ulong tag,ulong size,void *data){}
    virtual void ODBG2_Pluginreset(void){}
    virtual int ODBG2_Pluginclose(void){return 0;}
    virtual void ODBG2_Plugindestroy(void){}

};
#endif // IPLUGIN201_H

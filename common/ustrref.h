#pragma once
#include "IPlugin201.h"

class UStrRef
    : public IPlugin201
{
public:
    virtual int ODBG2_Plugininit(void);
    virtual void ODBG2_Pluginreset(void);
    virtual void ODBG2_Plugindestroy(void);
    virtual void ODBG2_Pluginnotify(int code,void *data,
        ulong parm1,ulong parm2);
    virtual t_menu *ODBG2_Pluginmenu(wchar_t *type);
protected:
private:
};
#pragma once
#include "IPlugin201.h"

class UStrRef
    : public IPlugin201
{
public:
    virtual t_menu *ODBG2_Pluginmenu(wchar_t *type);
protected:
private:
};
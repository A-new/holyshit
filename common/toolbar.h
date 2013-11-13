#pragma once
#include <windows.h>
#include <string>
#include "define.h"
#include "IPlugin110.h"
#include "IPlugin201.h"

#define INI_PATH TEXT("toolbar_ini")

class IConfigForToolbar
{
public:
    virtual std::tstring get_ini_path() = 0;
    //virtual void set_ini_path(const std::tstring& path) = 0;
};
class Toolbar
    : public IPlugin110
    , public IPlugin201
{
public:
    Toolbar(IConfigForToolbar*);
    // IPlugin110
    virtual int  _ODBG_Plugininit(int ollydbgversion,HWND hw,
        ulong *features);

    // IPlugin201
    virtual int ODBG2_Plugininit(void);
protected:
private:
    IConfigForToolbar* m_IConfigForToolbar;
};
#pragma once
#include "IPlugin110.h"
#include "IPlugin201.h"

void hook_loadsys_functions();
class LoadSys
    : public IPlugin110
    , public IPlugin201
{
public:
    // IPlugin110
    virtual int  _ODBG_Plugininit(int ollydbgversion,HWND hw,
        ulong *features);

    // IPlugin201
    int ODBG2_Plugininit(void);
protected:
private:
};
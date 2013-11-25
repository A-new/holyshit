#pragma once
#include "IPlugin110.h"
#include "IPlugin201.h"

namespace Command
{
    void RegisterBultinCommand(); // ÃÌº”≥£”√√¸¡Ó
}

class CCommand_OD
    : public IPlugin110
    , public IPlugin201
{
public:
    virtual int _ODBG_Plugininit( int ollydbgversion,HWND hw, ulong *features )
    {
        Command::RegisterBultinCommand();
        return 0;
    }

    virtual int ODBG2_Plugininit( void )
    {
        Command::RegisterBultinCommand();
        return 0;
    }
};
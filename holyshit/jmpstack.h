#pragma once
#include <Windows.h>
#include <tchar.h>
#include "../sdk/sdk.h"

#include <atlbase.h>
#include <atlwin.h>
template <class T, class TBase = CWindow, class TWinTraits = ATL::CControlWinTraits>
class CJmpStack_base: public ATL::CWindowImpl< T, TBase, TWinTraits>
{
public:
    //BOOL SubclassWindow(HWND hWnd);

protected:
    BEGIN_MSG_MAP(CJmpStack_base)
        //MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_KEYUP, OnKeyup)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeydown)
    END_MSG_MAP()

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (wParam == VK_RETURN)
        {
            ::MessageBoxA(0, 0, 0, 0);
        }
        return 0;
    }
    virtual LRESULT OnKeyup(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled){ return 0;}
    virtual LRESULT OnKeydown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled){return 0;}
private:
};

class CJmpStack_ACPUASM : public CJmpStack_base<CJmpStack_ACPUASM>
{
public:
    virtual LRESULT OnKeyup(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnKeydown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
protected:
private:
};

class IConfigForJmpStack
{
public:
    virtual bool jmp_enabled() = 0;
};

#include "IPlugin110.h"
#include "IPlugin201.h"
class JmpStack 
    : public IPlugin110
    , public IPlugin201
{
public:
    JmpStack(IConfigForJmpStack*);

    // IPlugin110
    virtual void  _ODBG_Pluginmainloop(DEBUG_EVENT *debugevent);

    // IPlugin201
    virtual int ODBG2_Plugininit(void);
    virtual void ODBG2_Pluginmainloop(DEBUG_EVENT *debugevent);
protected:
private:
    IConfigForJmpStack* m_IConfigForJmpStack;
};

#define CJmpStack_Single (CJmpStack::getInstance())
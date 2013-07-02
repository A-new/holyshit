#pragma once
#include <Windows.h>
#include <tchar.h>
#include "../sdk/sdk.h"
//#pragma pack(1)
// 以此为基类，不同窗口可能会派生不同的 ?? 
// 直接拿ATL来用! 因为这样派生，最不好处理的就是WindowProc，因为它是static的！
class CJmpStack
{
public:
    static HWND FindACPUASM(const TCHAR* clsName);
    static CJmpStack& getInstance();

    void Attach(HWND);
protected:
    static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    CJmpStack();
    ~CJmpStack();
private:
    LONG_PTR OldProc_;
};

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
    LRESULT OnKeyup(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if (wParam == VK_RETURN)
        {
            //::MessageBoxA(0, 0, 0, 0);
        }
        bHandled = FALSE; // 一定不要忘了！
        return 0;
    }
    LRESULT OnKeydown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if (wParam == VK_END)
        {
//#define VK_EXECUTE        0x2B
//#define VK_INSERT         0x2D
            //Sendshortcut(PM_DISASM, 0, WM_CHAR, 0, 0, VK_INSERT); // esc
            //Sendshortcut(PM_DISASM, 0, WM_CHAR, 0, 0, VK_EXECUTE); // `
            //::MessageBoxA(0, 0, 0, 0);
        }
        else if(wParam == VK_HOME)
        {
            //Sendshortcut(PM_DISASM, 0, WM_CHAR, 0, 0, VK_EXECUTE);
        }
        else
            bHandled = FALSE;
        return 0;
    }
    
private:
};

class CJmpStack_ACPUASM : public CJmpStack_base<CJmpStack_ACPUASM>
{
public:
protected:
private:
};

HWND GetACPUASM_WND();

void hook_jmpstack_functions();


#define CJmpStack_Single (CJmpStack::getInstance())
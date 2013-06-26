#pragma once
#include <Windows.h>
#include <tchar.h>

// �Դ�Ϊ���࣬��ͬ���ڿ��ܻ�������ͬ�� ?? 
// ֱ����ATL����! ��Ϊ������������ô���ľ���WindowProc����Ϊ����static�ģ�
class CJmpStack
{
public:
    static HWND FindACPUASM(const TCHAR* clsName);
    static CJmpStack& getInstance()
    {
        static CJmpStack a;
        return a;
    }
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
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    END_MSG_MAP()

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (wParam == VK_RETURN)
        {
            MessageBox(0, 0, 0, 0);
        }
    }

private:
};

HWND GetACPUASM_WND();

void hook_jmpstack_functions();


#define CJmpStack_Single (CJmpStack::getInstance())
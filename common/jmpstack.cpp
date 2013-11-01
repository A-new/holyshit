#include "jmpstack.h"
#include "func.h"
#include "../sdk/sdk.h"
#include "hook.h"

static HWND GetACPUASM_WND()
{
    t_dump *td = sdk_Getcpudisasmdump();
    if (td)
    {
        return td->table.hw;
    }
    return 0;
}

#include <boost/thread/mutex.hpp>
void hook_jmpstack_functions()
{
    static boost::mutex mu;
    static bool hooked = false;
    if (!hooked)
    {
        boost::mutex::scoped_lock lLock(mu);
        if (!hooked)
        {
            HWND hwnd = GetACPUASM_WND();
            if (hwnd)
            {
                hooked = true;
                static CJmpStack_ACPUASM a;
                a.SubclassWindow(GetACPUASM_WND());
            }

        }
    }
}

LRESULT CJmpStack_ACPUASM::OnKeyup( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
{
    if (wParam == VK_RETURN)
    {
        //::MessageBoxA(0, 0, 0, 0);
    }
    bHandled = FALSE; // 一定不要忘了！
    return 0;
}

LRESULT CJmpStack_ACPUASM::OnKeydown( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
{   
    bHandled = TRUE;
    if (wParam == VK_ESCAPE)
    {
        //#define VK_EXECUTE        0x2B
        //#define VK_INSERT         0x2D
        PostMessage(/*g_ollyWnd,*/ WM_KEYDOWN, VK_SUBTRACT, 0);
    }
    else if(wParam == VK_OEM_3)
    {
        PostMessage(/*g_ollyWnd,*/ WM_KEYDOWN, VK_ADD, 0);
    }
    else
        bHandled = FALSE;
    return 0;
}
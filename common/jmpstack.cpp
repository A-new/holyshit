#include "jmpstack.h"
#include "func.h"
#include "../sdk/sdk.h"
#include "hook.h"
//#include <boost/>

// 0x4CDA2D 表示鼠标点击的当前地址
// 还有个是点击外的下一行地址
struct Mydata
{
    HWND hFind;
    const TCHAR* Name;
};
static BOOL CALLBACK EnumChildWindowProc(HWND hwnd,LPARAM lParam)
{
    Mydata* d =  reinterpret_cast<Mydata*>(lParam);
    if(!IsWindowVisible(hwnd)) return TRUE;
    TCHAR szName[MAX_PATH];
    if(GetClassName(hwnd, szName, MAX_PATH))
    {
        if(0 == _tcsicmp(szName, d->Name))
        {
            d->hFind = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

HWND GetACPUASM_WND()
{
    t_dump *td = sdk_Getcpudisasmdump();
    if (td)
    {
        return td->table.hw;
    }
    return 0;
}
typedef
BOOL
(WINAPI
*CONTINUEDEBUGEVENT)(
                   __in DWORD dwProcessId,
                   __in DWORD dwThreadId,
                   __in DWORD dwContinueStatus
                   );
CONTINUEDEBUGEVENT OldContinueDebugEvent;
BOOL
WINAPI
 HookContinueDebugEvent(
 __in DWORD dwProcessId,
 __in DWORD dwThreadId,
 __in DWORD dwContinueStatus
 )
{
    return OldContinueDebugEvent(dwProcessId, dwThreadId, dwContinueStatus);
}
void hook_jmpstack_functions()
{
    static bool bHooked = false;
    if (!bHooked)
    {
        bHooked = true;

        HMODULE hMod = GetModuleHandleW(L"Kernel32");
        OldContinueDebugEvent = (CONTINUEDEBUGEVENT)GetProcAddress(hMod, "ContinueDebugEvent");
        //hook(&(PVOID&)OldContinueDebugEvent, HookContinueDebugEvent);

        static CJmpStack_ACPUASM a;
        a.SubclassWindow(GetACPUASM_WND());
    }
}


HWND CJmpStack::FindACPUASM(const TCHAR* clsName)
{
    Mydata d;
    d.Name = clsName;//TEXT("ACPUASM");
    d.hFind = 0;
    EnumChildWindows(g_ollyWnd, EnumChildWindowProc, (LPARAM)&d);
    return d.hFind;
}

LRESULT CALLBACK CJmpStack::WindowProc(          HWND hwnd,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam
                            )
{
    if (uMsg == WM_KEYDOWN)
    {
        if (wParam == VK_RETURN)
        {

        }
    }

    return CallWindowProc((WNDPROC)CJmpStack_Single.OldProc_, hwnd, uMsg, wParam, lParam);
}

void CJmpStack::Attach( HWND hWnd)
{
    OldProc_ = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
}

CJmpStack::CJmpStack()
: OldProc_(0)
{

}

CJmpStack::~CJmpStack()
{

}

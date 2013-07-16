#include "jmpstack.h"
#include "func.h"
#include "../sdk/sdk.h"
#include "hook.h"
//#include <boost/>

//#pragma pack(1)
// 以此为基类，不同窗口可能会派生不同的 ?? 
// 直接拿ATL来用! 因为这样派生，最不好处理的就是WindowProc，因为它是static的！
//class CJmpStack
//{
//public:
//    static HWND FindACPUASM(const TCHAR* clsName);
//    static CJmpStack& getInstance();
//
//    void Attach(HWND);
//protected:
//    static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//    CJmpStack();
//    ~CJmpStack();
//private:
//    LONG_PTR OldProc_;
//};
//
//// 0x4CDA2D 表示鼠标点击的当前地址
//// 还有个是点击外的下一行地址
//struct Mydata
//{
//    HWND hFind;
//    const TCHAR* Name;
//};
//static BOOL CALLBACK EnumChildWindowProc(HWND hwnd,LPARAM lParam)
//{
//    Mydata* d =  reinterpret_cast<Mydata*>(lParam);
//    if(!IsWindowVisible(hwnd)) return TRUE;
//    TCHAR szName[MAX_PATH];
//    if(GetClassName(hwnd, szName, MAX_PATH))
//    {
//        if(0 == _tcsicmp(szName, d->Name))
//        {
//            d->hFind = hwnd;
//            return FALSE;
//        }
//    }
//    return TRUE;
//}

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

            //HMODULE hMod = GetModuleHandleW(L"Kernel32");
            //OldContinueDebugEvent = (CONTINUEDEBUGEVENT)GetProcAddress(hMod, "ContinueDebugEvent");
            //hook(&(PVOID&)OldContinueDebugEvent, HookContinueDebugEvent);
        }
    }
}

//
//HWND CJmpStack::FindACPUASM(const TCHAR* clsName)
//{
//    Mydata d;
//    d.Name = clsName;//TEXT("ACPUASM");
//    d.hFind = 0;
//    EnumChildWindows(g_ollyWnd, EnumChildWindowProc, (LPARAM)&d);
//    return d.hFind;
//}
//
//LRESULT CALLBACK CJmpStack::WindowProc(          HWND hwnd,
//                            UINT uMsg,
//                            WPARAM wParam,
//                            LPARAM lParam
//                            )
//{
//    if (uMsg == WM_KEYDOWN)
//    {
//        if (wParam == VK_RETURN)
//        {
//
//        }
//    }
//
//    return CallWindowProc((WNDPROC)CJmpStack_Single.OldProc_, hwnd, uMsg, wParam, lParam);
//}
//
//void CJmpStack::Attach( HWND hWnd)
//{
//    OldProc_ = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
//}
//
//CJmpStack::CJmpStack()
//: OldProc_(0)
//{
//
//}
//
//CJmpStack::~CJmpStack()
//{
//
//}
//
//CJmpStack& CJmpStack::getInstance()
//{
//    static CJmpStack a;
//    return a;
//}

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
#include <windows.h>
#include <vector>
#include <boost/bind.hpp>
#include "../sdk/sdk.h"
#include "../common/define.h"
#include "../common/func.h"
#include "../common/hook.h"
#include "../common/toolbar.h"
#include "../common/loadsys.h"
#include "../common/label.h"
#include "../common/config.h"
#include "../common/jmpstack.h"
#include "../common/command_OD.h"

#include <Shlwapi.h>

typedef std::vector<IPlugin110*> IPLUGIN_LIST;
IPLUGIN_LIST plugins_all;

extc int  _export cdecl ODBG_Plugindata(char shortname[32])
{
    strcpy(shortname, PLUGIN_NAME);
    return PLUGIN_VERSION;
}

HMODULE g_hModule;

extc int  _export cdecl ODBG_Plugininit(int ollydbgversion,HWND hw,
                                        ulong *features)
{
    if (ollydbgversion != 110)
    {
        return -1;
    }
    g_ollyWnd = hw;
    CConfig_Single.set_mod(g_hModule);
    plugins_all.push_back(new Label(reinterpret_cast<IConfigForLabel *>(&CConfig_Single)));

    CConfig_Single.loadall();

    //hook_DRAWFUNC_cpudasm放到mainloop里去做，因为在此处CPUDASM还没有创建起来
    // loadsys相关初始化
    hook_loadsys_functions();

    // toolbar相关初始化
    std::tstring szTB = CConfig_Single.get_ini_path();
    if (PathFileExistsA(szTB.c_str()))
    {
        if(CToolbar_Global.init(szTB.c_str()))//"D:\\src\\vc\\holyshit\\common\\test.ini"
        {
            Command::RegisterBultinCommand();
            CToolbar_Global.attach((HWND)Plugingetvalue(VAL_HWMAIN));
        }
    }
    return 0;
}


void cdecl ODBG_Plugindestroy(void)
{
    CConfig_Single.saveall(true);
}

// ODBG_Pluginclose是可以被撒消的
int cdecl ODBG_Pluginclose(void)
{
    return 0;
}


int ODBG_Pluginmenu(int origin,char data[4096],void *item)
{
    if(origin == PM_MAIN)
    {
        strcpy(data,"0 Load map files, 1 About");
        //strcpy(data,"1 About");
        return 1;
    }
    return 0;
}

void ODBG_Pluginaction(int origin,int action,void *item)
{
    static bool IsFirst = TRUE;

    switch(action)
    {
    case 0:
        //MessageBoxA(g_ollyWnd, "sorry, not done yet:(", "...", MB_ICONINFORMATION);
        {
             
            //LoadMap();
            
            //DWORD endaddr, addr;
            ////Findmodulebyname("")
            //HMODULE hmod= GetModuleHandleA("sample.sys");
            //t_memory* tmem = Findmemory((ulong)hmod) + 1000);
            //char cmd[MAXCMDSIZE]={0};
            //do
            //{
            //    addr = Disassembleforward(0, tmem->base, tmem->size, addr, 1, 1);
            //    endaddr = Disassembleforward(0, tmem->base, tmem->size, addr, 1, 1);
            //    size = Readcommand(addr, cmd);
            //    if(addr == tmem->base + tmem->size)
            //        size = 0;
            //    if(size)
            //    {
            //        t_disasm dasm;
            //        ::memset(&dasm,0,sizeof(t_disasm));
            //        DWORD dsize=0;
            //        unsigned char* bytes=Finddecode(addr,&dsize);
            //        ::Disasm((uchar*)cmd,size,addr,bytes,&dasm,DISASM_CODE,NULL);
            //    }
            //}
            /*
            
            反汇编代码搜索方法的实现：
            virtual IDispatch* __stdcall FindDisasm2(DWORD addr,IDispatch* condition)
            {
            //判断参数是否条件表达式，因为没有定义新接口，所以不能用QueryInterface来确定。
            BOOL r=DispatchDriver::GetProperty(condition,L"IsCondition",Type2Type<BOOL>());
            if(!r)
            return NULL;
            Condition* pv=static_cast<Condition*>(condition);
            ConditionExpression* pCE=pv->GetCondition();
            pCE->AddRef();//引用计数加1
            DWORD endaddr;
            int ct=0;
            int size = 0;
            t_memory* tmem = Findmemory(addr);
            char cmd[MAXCMDSIZE]={0};
            do
            {
            addr = Disassembleforward(0, tmem->base, tmem->size, addr, 1, 1);
            endaddr = Disassembleforward(0, tmem->base, tmem->size, addr, 1, 1);
            size = Readcommand(addr, cmd);
            if(addr == tmem->base + tmem->size)
            size = 0;
            if(size)
            {
            t_disasm dasm;
            ::memset(&dasm,0,sizeof(t_disasm));
            DWORD dsize=0;
            unsigned char* bytes=Finddecode(addr,&dsize);
            ::Disasm((uchar*)cmd,size,addr,bytes,&dasm,DISASM_CODE,NULL);
            //应用LAMBDA表达式执行计算来得到判断
            if((*pCE)(&dasm))
            {
            pCE->Release();//引用计数减1
            DisasmInfo* p=DisasmInfo::CreateDispatch();
            p->Init(&dasm);
            return p;
            }
            }
            if(ct % EXTERN_EVENT_INTERVAL == 0)
            {
            dlg.DoMessageLoopOnce();
            }
            ct++;
            } while(size != 0);
            pCE->Release();//引用计数减1
            return NULL;
            }
            */
        }
        break;

    case 1:
        {	
            about();	
        }
        break;
    default:
        break;
    }

}
//
//int ODBG_Pluginuddrecord(t_module *pmod,int ismainmodule,
//                                             ulong tag,ulong size,void *data)
//{
//    if (ismainmodule)
//    {
//        char* p = (pmod->name);
//        p = 0;
//    }
//
//    return 0;
//}


//int ODBG_Pausedex(int reason, int extdata, t_reg *reg, DEBUG_EVENT *debugevent)
//{
//
//}


bool bInjected = false;
void  ODBG_Pluginmainloop(DEBUG_EVENT *debugevent) 
{
    std::for_each(plugins_all.begin(), plugins_all.end(), boost::bind(&IPlugin110::_ODBG_Pluginmainloop, _1, debugevent));
    //hook_label_functions();
    if (CConfig_Single.jmp_enabled())
    {
        hook_jmpstack_functions();
    }

    //if (debugevent && debugevent->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
    //{
    //    bInjected = false;
    //}
}

// 新进程创建之前调用，只能做一般的内部变量清理工作，获取不到handle of process
void ODBG_Pluginreset(void)
{
    bInjected = false;
    // od1和od2里获取到的exefile都是错的
}
int  ODBG_Paused(int reason,t_reg *reg)
{
    //if (reason == PP_TERMINATED)
    //{
    //    bInjected = false;
    //}
 
    //if (!bInjected)
    //{
    //    if (reason == PP_EVENT)
    //    {
    //        const char* file = (const char*)Plugingetvalue(VAL_EXEFILENAME);
    //        //if (StrRStrI(file, 0, TEXT(".sys"))
    //        //    || StrRStrI(file, 0, TEXT("ntoskrnl.exe")))
    //        if(IsSysFile(file))
    //        {
    //            HANDLE hProcess = (HANDLE)Plugingetvalue(VAL_HPROCESS);
    //            HANDLE hThread = (HANDLE)Plugingetvalue(VAL_HMAINTHREAD);
    //            if (hProcess && hThread)
    //            {

    //                LPVOID lpAddr = VirtualAllocEx(hProcess, 0, 1000,MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    //                if (lpAddr)
    //                {
    //                    CONTEXT ct;
    //                    ct.ContextFlags = CONTEXT_CONTROL; // eip
    //                    GetThreadContext(hThread, &ct);
    //                    //*((DWORD*)lpAddr) = ct.Eip;
    //                    WriteProcessMemory(hProcess, lpAddr, &ct.Eip, 4, 0);
    //                    HMODULE hKer = GetModuleHandleW(L"kernel32.dll");
    //                    PROC pp = GetProcAddress(hKer, "LoadLibraryA");
    //                    //*((DWORD*)lpAddr + 1) = GetProcAddress(hKer, "LoadLibraryA");
    //                    WriteProcessMemory(hProcess, (LPVOID)((DWORD)lpAddr+4), &pp, 4, 0);

    //                    CHAR loadsys[MAX_PATH];
    //                    GetModuleFileNameA(g_hModule, loadsys, MAX_PATH);
    //                    LPSTR pFind = StrRChrA(loadsys, 0, '\\');
    //                    if (pFind)
    //                    {
    //                        pFind += 1;
    //                        *pFind = 0;
    //                        lstrcatA(loadsys, "loadsys.dll");
    //                    }
    //                    else
    //                    {
    //                        lstrcpyA(loadsys, "loadsys.dll");
    //                    }
    //                    WriteProcessMemory(hProcess, (LPVOID)((DWORD)lpAddr+8), loadsys, MAX_PATH, 0);

    //                    const char shell[] = {0x6A, 0x00, 0x60, 0x9C, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x81, 0xE6, 0x00, 0x00, 0xFF, 0xFF, 0xAD, 0x89, 0x44, 0x24, 0x24, 0xAD, 0x56, 0xFF, 0xD0, 0x9D, 0x61, 0xC3};
    //                    WriteProcessMemory(hProcess, (LPVOID)((DWORD)lpAddr+8 + MAX_PATH), shell, MAX_PATH, 0);
    //                    ct.Eip = (DWORD)lpAddr+8 + MAX_PATH;

    //                    reg->modified = TRUE;

    //                    //Setbreakpoint(reg->ip, TY_ONESHOT, 0);
    //                    reg->ip = ct.Eip;

    //                    Sendshortcut(PM_MAIN, 0, WM_KEYDOWN, 0, 1, VK_F9); 
    //                    //Go(GetThreadId(hThread), reg->ip, STEP_RUN, 0, 0);

    //                    //2.Setdisasm(dw,1,CPU_ASMHIST);

    //                    //SetThreadContext(hThread, &ct);
    //                    bInjected = true;
    //                    //ResumeThread(hThread);
    //                }
    //            }
    //        }
    //    }
    //}
    return 0;
}

extc void _export cdecl ODBG_Pluginsaveudd(t_module *pmod,int ismainmodule)
{
    ismainmodule = 0;
    // Pluginsaverecord
}

// 只有当OD不认的tag才会调用，所以一般断不下来
extc int  _export cdecl ODBG_Pluginuddrecord(t_module *pmod,int ismainmodule,
                                             ulong tag,ulong size,void *data)
{
    size = 0;
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;
    }
    return TRUE; 
}
#include "func.h"
#include "../sdk/sdk.h"
#include <shlwapi.h>

//#include "map.h"
HWND g_ollyWnd = NULL;
DWORD GetCurrentEIP(void)
{
    t_thread* t2;// t_thread

    t2 = Findthread(Getcputhreadid());
    return t2->reg.ip;
}

void LoadMap()
{
    //Browsefilename(char *title,char *name,char *defext,
    //    int mode);
    //Browsefilename(wchar_t *title,wchar_t *name,wchar_t *args,
    //    wchar_t *currdir,wchar_t *defext,HWND hwnd,int mode);

    if (!HasDebuggee())
    {
        return;
    }


    TCHAR path[MAX_PATH];
    memset(path, 0, sizeof(path));
    if(0 == Browsefilename(_T("Select map file"), path, _T(".map"), 0))
        return;
    
    if (_tcsicmp(path, _T(".\\")) == 0)        //just a litle check
        return;
    
    //CMap c(path);
    //CMap::line_iterator ci = c.begin();

    //for (; ci != c.end(); ++ci)
    //{
    //    std::tstring s = *ci;
    //    s = _T("123");
    //}

        //if (what == 0) {
        //    Addtolist(0, 0, _T("MapConv: OK: Map file successfuly imported - labels updated"));
        //}
        //else  {
        //    Addtolist(0, 0, _T("MapConv: OK: Map file successfuly imported - comments updated"));
        //}

    //Setcpu(0,0,0,0,CPU_ASMFOCUS);

}

bool HasDebuggee()
{
    HANDLE procHandle = NULL;
#ifdef HOLYSHIT_EXPORTS
    procHandle = (HANDLE)Plugingetvalue(VAL_HPROCESS);
#else
    procHandle = process;
#endif
    return (procHandle != NULL);
}



BOOL InjectIt(HANDLE hrp, LPCSTR DllPath/*, const DWORD dwRemoteProcessld*/)//注入主函数
{
    //HANDLE hrp = NULL;

    //if((hrp=OpenProcess(PROCESS_CREATE_THREAD|//允许远程创建线程
    //    PROCESS_VM_OPERATION|//允许远程VM操作
    //    PROCESS_VM_WRITE,//允许远程VM写
    //    FALSE,dwRemoteProcessld)) == NULL)
    //{
    //    // OpenProcess Error
    //    return FALSE;
    //}

    LPTSTR psLibFileRemote = NULL;

    //使用VirtualAllocEx函数在远程进程的内存地址空间分配DLL文件名缓冲
    psLibFileRemote=(LPTSTR)VirtualAllocEx(hrp, NULL, lstrlenA(DllPath)+1,
        MEM_COMMIT, PAGE_READWRITE);

    if(psLibFileRemote == NULL)
    {
        // VirtualAllocEx Error
        return FALSE;
    }

    //使用WriteProcessMemory函数将DLL的路径名复制到远程的内存空间
    if(WriteProcessMemory(hrp, psLibFileRemote, (void *)DllPath, lstrlenA(DllPath)+1, NULL) == 0)
    {
        // WriteProcessMemory Error
        return FALSE;
    }

    //计算LoadLibraryA的入口地址
    PTHREAD_START_ROUTINE pfnStartAddr=(PTHREAD_START_ROUTINE)
        GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");

    if(pfnStartAddr == NULL)
    {
        // GetProcAddress Error!
        return FALSE;
    }
    //pfnStartAddr地址就是LoadLibraryA的入口地址


    HANDLE hrt = NULL;

    if((hrt = CreateRemoteThread(hrp,
        NULL,
        0,
        pfnStartAddr,
        psLibFileRemote,
        0,
        NULL)) == NULL)
    {
        // CreateRemote Error
        return FALSE;
    }
    CloseHandle(hrt);
    //WaitForSingleObject(hrt, INFINITE);
    return TRUE;
}

void about()
{
    MessageBoxA(g_ollyWnd,"HolyShit v0.2\r\nCopyright (C) 2013 lynnux\r\nlynnux@qq.com"
        "\r\n\r\nSpecial thanks: 疯子,zclyj,the author of IDAFicator"
        "\r\n\r\nall functions:show label, let od can load sys, toolbar, jmp stack"
        ,"About",MB_ICONINFORMATION);
}

#include <vector>
std::wstring string2wstring(const std::string & rString, UINT codepage)
{
    int len = MultiByteToWideChar(codepage, 0, rString.c_str(), -1, NULL, 0);
    if(len > 0)
    {		
        std::vector<wchar_t> vw(len);
        MultiByteToWideChar(codepage, 0, rString.c_str(), -1, &vw[0], len);
        return &vw[0];
    }
    else
        return L"";
}

std::string wstring2string(const std::wstring & rwString, UINT codepage)
{
    int len = WideCharToMultiByte(codepage, 0, rwString.c_str(), -1, NULL, 0, NULL, NULL);
    if(len > 0)
    {		
        std::vector<char> vw(len);
        WideCharToMultiByte(codepage, 0, rwString.c_str(), -1, &vw[0], len, NULL, NULL);
        return &vw[0];
    }
    else
        return "";
}


bool IsSysFile(const TCHAR* DllPath)
{
    const TCHAR* p;
    p = _tcsrchr(DllPath, '.');
    if (p)
    {
        if (0 == StrCmpNI(p, _T(".sys"), 4))
        {
            return true;
            // *change =  1;
        }
    }

    p = _tcsrchr(DllPath, _T('\\')) ;
    if (p)
    {
        p += 1;

        if (0 == _tcsicmp(p, _T("ntoskrnl.exe")))
        {
            return true;
            //*change =  1;
        }
    }
    return false;
}
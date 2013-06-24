#include "func.h"
#include "../sdk/sdk.h"
//#include "map.h"

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



BOOL InjectIt(HANDLE hrp, LPCSTR DllPath/*, const DWORD dwRemoteProcessld*/)//ע��������
{
    //HANDLE hrp = NULL;

    //if((hrp=OpenProcess(PROCESS_CREATE_THREAD|//����Զ�̴����߳�
    //    PROCESS_VM_OPERATION|//����Զ��VM����
    //    PROCESS_VM_WRITE,//����Զ��VMд
    //    FALSE,dwRemoteProcessld)) == NULL)
    //{
    //    // OpenProcess Error
    //    return FALSE;
    //}

    LPTSTR psLibFileRemote = NULL;

    //ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ�������
    psLibFileRemote=(LPTSTR)VirtualAllocEx(hrp, NULL, lstrlenA(DllPath)+1,
        MEM_COMMIT, PAGE_READWRITE);

    if(psLibFileRemote == NULL)
    {
        // VirtualAllocEx Error
        return FALSE;
    }

    //ʹ��WriteProcessMemory������DLL��·�������Ƶ�Զ�̵��ڴ�ռ�
    if(WriteProcessMemory(hrp, psLibFileRemote, (void *)DllPath, lstrlenA(DllPath)+1, NULL) == 0)
    {
        // WriteProcessMemory Error
        return FALSE;
    }

    //����LoadLibraryA����ڵ�ַ
    PTHREAD_START_ROUTINE pfnStartAddr=(PTHREAD_START_ROUTINE)
        GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");

    if(pfnStartAddr == NULL)
    {
        // GetProcAddress Error!
        return FALSE;
    }
    //pfnStartAddr��ַ����LoadLibraryA����ڵ�ַ


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
    return TRUE;
}
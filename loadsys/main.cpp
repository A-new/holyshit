#include <windows.h>
#include "myntdll.h"
#pragma comment(lib, "ntdll.lib")

//#pragma comment(linker,"/entry:DllMain")
//#pragma comment(linker, "/MERGE:.rdata=.text") 
//#pragma comment(linker, "/MERGE:.data=.text") 
//#pragma comment(linker, "/MERGE:code=.text") 
//#pragma comment(linker,"/SECTION:.text,RWE")
//#pragma optimize("gsy", on)
//#ifdef _MANAGED
//#pragma managed(push, off)
//#endif

//#pragma data_seg(".SharedDataName")
//HHOOK g_hk = NULL;
//HWND g_hDstWnd = NULL;
//BOOL g_bOK = FALSE;
//char g_dllname[MAX_PATH] = {0};
//char g_hwndinfo[NUM_WNDINFO] = {0};
//#pragma data_seg()
//#pragma comment(linker,"/section:.SharedDataName,rws")

void
InitializeListHead(
                   IN PLIST_ENTRY ListHead
                   )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}


void
InsertTailList(
               IN PLIST_ENTRY ListHead,
               IN PLIST_ENTRY Entry
               )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

#pragma pack(push)
#pragma pack(1)

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
    HANDLE Section;                 // Not filled in
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR FullPathName[ 256 ];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[ 1 ];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

#pragma pack(pop)


PVOID QuerySystemInfo(SYSTEM_INFORMATION_CLASS SystemInformationClass) {
    PVOID p = NULL;
    NTSTATUS statu;
    LONG size = 0x40000;

    do {
        if (p) {
            free(p);
            size *= 2;
        }

        p = malloc(size);
        if (p == NULL)
            return NULL;

        statu = ZwQuerySystemInformation(SystemInformationClass,
            p,
            size,
            NULL);
    } while (statu == STATUS_INFO_LENGTH_MISMATCH);

    if (!NT_SUCCESS(statu)) {
        free(p);
        return NULL;
    }

    return p;
}

#include <vector>
typedef std::vector<const char*> LIST_DLLS;
void getModule(HMODULE hMod, LIST_DLLS& list)
{
    //InitializeListHead(&list);

    IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)hMod;
    IMAGE_NT_HEADERS *pNtHeader = (IMAGE_NT_HEADERS*)((BYTE *)hMod + pDosHeader->e_lfanew);
    IMAGE_OPTIONAL_HEADER *pOptHeader = &pNtHeader->OptionalHeader;
    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *) ((BYTE *)hMod + 
        pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);


    while(pImportDesc->FirstThunk)  
    {  
        const char *pszDllName = (const char *)((BYTE *)hMod + pImportDesc->Name);
        //DWORD dw = GetFileAttributesA(pszDllName);
        //if (dw & FILE_ATTRIBUTE_REPARSE_POINT)        
        //{
        //    
        //}
        //dw = GetLastError();
        list.push_back(pszDllName);

        pImportDesc++;  
    }

}

#include <Tlhelp32.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
HMODULE getSysMod()
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (hSnap)
    {
        MODULEENTRY32 me;
        me.dwSize = sizeof MODULEENTRY32;
        if (Module32First(hSnap, &me))
        {
            do 
            {
                if (StrRStrI(me.szExePath, 0, TEXT(".sys")))
                {
                    return me.hModule;
                }
                else if(StrRStrI(me.szExePath, 0, TEXT("ntoskrnl.exe")))
                {
                    return me.hModule;
                }
                me.dwSize = sizeof MODULEENTRY32;
            } while (Module32Next(hSnap, &me));
        }

        DWORD dw = GetLastError();
        CloseHandle(hSnap);
    }
    return NULL;
}

LIST_ENTRY*			g_entry = NULL;

bool FindImport(PRTL_PROCESS_MODULE_INFORMATION pModuleInfo,const LIST_DLLS& dlls)
{
    const char* pModName = (const char*)pModuleInfo->FullPathName + pModuleInfo->OffsetToFileName;

    LIST_DLLS::const_iterator ci = dlls.begin();
    for (ci; ci!=dlls.end(); ++ci)
    {
        if (0 == stricmp(*ci, pModName))
        {
            return true;
        }
    }
    return false;
}

#include <string>
void FixImport(void* base, IMAGE_NT_HEADERS* nt) {
    PIMAGE_IMPORT_DESCRIPTOR iid =  (PIMAGE_IMPORT_DESCRIPTOR)(nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress + (char*)base);
    while (iid->Name) {
        //CHAR ss[MAX_PATH];
        //wsprintfA(ss, "module: %s\r\n", (char*)base+iid->Name);
        //OutputDebugStringA(ss);
        HMODULE hModule = GetModuleHandleA((char*)base+iid->Name);
        PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((iid->OriginalFirstThunk ? iid->OriginalFirstThunk : iid->FirstThunk)+(char*)base);
        PIMAGE_THUNK_DATA fill = (PIMAGE_THUNK_DATA)(iid->FirstThunk+(char*)base);
        while (thunk->u1.Ordinal) {

            HMODULE hFinal = hModule;

            // 特别处理hal中的ExAcquireFastMutex, ExReleaseFastMutex, ExTryToAcquireFastMutex
            std::string FunName = (const char*)((PIMAGE_IMPORT_BY_NAME)(thunk->u1.Ordinal+(char*)base))->Name;
            if (0 == stricmp(FunName.c_str(), "ExAcquireFastMutex")
                || 0 == stricmp(FunName.c_str(), "ExReleaseFastMutex")
                || 0 == stricmp(FunName.c_str(), "ExTryToAcquireFastMutex"))
            {
                hFinal = GetModuleHandleA("ntoskrnl.exe");
                FunName.insert(2, "i");
            }

            //wsprintfA(ss, "fun: %s\r\n", FunName.c_str());
            //OutputDebugStringA(ss);
            
            // ntoskrnl.exe的IAT在text段，是只读的
            DWORD dwold;
            VirtualProtect(fill, 4, PAGE_READWRITE, &dwold);

            fill->u1.Ordinal = (DWORD)GetProcAddress(hFinal, FunName.c_str()/*(char*)((PIMAGE_IMPORT_BY_NAME)(thunk->u1.Ordinal+(char*)base))->Name*/);
            fill++;
            thunk++;
        }
        iid++;
    }
}

void FixReloc(void* base, IMAGE_NT_HEADERS* nt) {
    PIMAGE_NT_HEADERS   ntheader;
    PIMAGE_BASE_RELOCATION pReloc;
    ULONG               count;
    PUSHORT             pData;
    ULONG               i;
    PULONG              p;


    ntheader  = nt;
    ULONG ImageBase = (ULONG)base;
    pReloc    = (PIMAGE_BASE_RELOCATION)(ntheader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress + ImageBase);

    if (pReloc == base) {
        return;
    }

    do {
        count = (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))/sizeof(WORD);
        pData = (PUSHORT)((PUCHAR)pReloc + sizeof(IMAGE_BASE_RELOCATION));
        for (i = 0; i < count; i++) {
            if ((pData[i]>>12) == IMAGE_REL_BASED_HIGHLOW) {
                p = (PULONG)((pData[i]&0xFFF) + pReloc->VirtualAddress + (ULONG)ImageBase);

                // ntoskrnl.exe所有段都是只读的
                DWORD dwold;
                VirtualProtect(p, 4, PAGE_READWRITE, &dwold);
                *p = *p - ntheader->OptionalHeader.ImageBase + ImageBase;
            }
        }
        pReloc = (PIMAGE_BASE_RELOCATION)((PUCHAR)pReloc + pReloc->SizeOfBlock);
    } while (pReloc->VirtualAddress && pReloc->SizeOfBlock);
}

void BuildDriverList() 
{
    HMODULE base = getSysMod();
    if (!base)
    {
        return;
    }

    LIST_DLLS list;
    getModule(base, list);

    LIST_DLLS::const_iterator ci = list.begin();
    for (; ci!= list.end(); ++ci)
    {
        LoadLibraryExA(*ci, 0, DONT_RESOLVE_DLL_REFERENCES);
    }

    // 这段是给DriverEntry用的
    //PRTL_PROCESS_MODULES pModules = NULL;
    //PRTL_PROCESS_MODULE_INFORMATION pModuleInfo = NULL;

    //// 获取已经加载驱动列表
    //pModules = (PRTL_PROCESS_MODULES)QuerySystemInfo(SystemModuleInformation);
    //if (pModules) 
    //{
    //    pModuleInfo = &pModules->Modules[0];

    //    for (ULONG i = 0 ; i < pModules->NumberOfModules; ++i, pModuleInfo++) 
    //    {
    //        if (FindImport(pModuleInfo, list) 
    //            || pModuleInfo->LoadOrderIndex == 0
    //            || pModuleInfo->LoadOrderIndex == 1 // hal.dll有不同的名字
    //            )
    //        {
    //            LDR_DATA_TABLE_ENTRY* cur = (LDR_DATA_TABLE_ENTRY*)malloc(sizeof(LDR_DATA_TABLE_ENTRY));
    //            memset(cur, 0, sizeof(LDR_DATA_TABLE_ENTRY));

    //            cur->Flags			= pModuleInfo->Flags;
    //            cur->DllBase		= pModuleInfo->ImageBase;
    //            cur->SizeOfImage	= pModuleInfo->ImageSize;
    //            cur->LoadCount		= pModuleInfo->LoadCount;


    //            int len = MultiByteToWideChar(CP_ACP, 0, (char*)&pModuleInfo->FullPathName, strlen((char*)pModuleInfo->FullPathName), NULL, 0);
    //            WCHAR* p = (WCHAR*)malloc((len+1) * sizeof(WCHAR));
    //            MultiByteToWideChar(CP_ACP, 0, (char*)&pModuleInfo->FullPathName, len, p, len*sizeof(WCHAR));
    //            p[len] = 0;
    //            RtlInitUnicodeString(&cur->FullDllName, p);
    //            RtlInitUnicodeString(&cur->BaseDllName, p + pModuleInfo->OffsetToFileName);


    //            char* name = (char*)&pModuleInfo->FullPathName + pModuleInfo->OffsetToFileName;
    //            if (pModuleInfo->LoadOrderIndex == 0) {

    //                g_entry = &cur->InLoadOrderLinks; // 实际起作用的是LoadLibraryExA
    //                InitializeListHead(g_entry);

    //                //wcscpy(p + pModuleInfo->OffsetToFileName, L"ntoskrnl.exe");
    //                cur->DllBase = LoadLibraryExA("ntoskrnl.exe", 0, DONT_RESOLVE_DLL_REFERENCES);
    //            }
    //            else if (pModuleInfo->LoadOrderIndex == 1)
    //            {
    //                cur->DllBase = LoadLibraryExA("hal.dll", 0, DONT_RESOLVE_DLL_REFERENCES);
    //            }
    //            else
    //            {
    //                cur->DllBase = LoadLibraryExA(name, 0, DONT_RESOLVE_DLL_REFERENCES);
    //            }
    //            //else if (stricmp(name, "hal.dll") == 0) {
    //            //    
    //            //}
    //            //else if (stricmp(name, "kdcom.dll") == 0) {
    //            //    cur->DllBase = LoadLibraryExW(L"kdcom.dll", 0, DONT_RESOLVE_DLL_REFERENCES);
    //            //}

    //            //printf("%s %08X\r\n", name, cur->DllBase);

    //            InsertTailList(g_entry, &cur->InLoadOrderLinks);
    //        }

    //    }
    //    free(pModules);
    //}

    IMAGE_NT_HEADERS* nt = NULL;
    nt = (IMAGE_NT_HEADERS*)(((IMAGE_DOS_HEADER*)base)->e_lfanew + (char*)base);
    //oep = nt->OptionalHeader.AddressOfEntryPoint;

    FixImport(base, nt);
    FixReloc(base, nt);
}

DWORD WINAPI ThreadProc(
                        __in  LPVOID lpParameter
                        )
{
    //Sleep(2000);
    //WaitForInputIdle(GetCurrentProcess(), 3000); // console!

    //__asm{int 3}
    BuildDriverList();
    
    return 0;
}
//
//int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
//{
//        WaitForSingleObject(CreateThread(NULL, 0, ThreadProc, NULL, 0, 0), INFINITE);
//        return 0;
//}
BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        TCHAR szFile[MAX_PATH];
        GetModuleFileName(NULL, szFile, MAX_PATH);
        OutputDebugString(szFile);
        if (!StrRStrI(szFile, 0, TEXT("loaddll.exe")))
        {
            return TRUE;
        }
        ThreadProc(0);
        //CloseHandle(CreateThread(NULL, 0, ThreadProc, NULL, 0, 0));
        //DisableThreadLibraryCalls(hModule);
    }
    return TRUE; 
}
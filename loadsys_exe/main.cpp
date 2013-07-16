#include <windows.h>
#include "myntdll.h"
//#pragma comment(lib, "ntdll.lib") // 跟libcmt冲突，只好放在工程属性里去
#include <shellapi.h>

typedef struct _DRIVER_EXTENSION {

    //
    // Back pointer to Driver Object
    //

    struct _DRIVER_OBJECT *DriverObject;

    //
    // The AddDevice entry point is called by the Plug & Play manager
    // to inform the driver when a new device instance arrives that this
    // driver must control.
    //

    void* AddDevice;

    //
    // The count field is used to count the number of times the driver has
    // had its registered reinitialization routine invoked.
    //

    ULONG Count;

    //
    // The service name field is used by the pnp manager to determine
    // where the driver related info is stored in the registry.
    //

    UNICODE_STRING ServiceKeyName;

    //
    // Note: any new shared fields get added here.
    //


} DRIVER_EXTENSION, *PDRIVER_EXTENSION;

typedef struct _DRIVER_OBJECT {
    WORD Type;
    WORD Size;

    //
    // The following links all of the devices created by a single driver
    // together on a list, and the Flags word provides an extensible flag
    // location for driver objects.
    //

    void* DeviceObject;
    ULONG Flags;

    //
    // The following section describes where the driver is loaded.  The count
    // field is used to count the number of times the driver has had its
    // registered reinitialization routine invoked.
    //

    PVOID DriverStart;
    ULONG DriverSize;
    PVOID DriverSection;
    PDRIVER_EXTENSION DriverExtension;

    //
    // The driver name field is used by the error log thread
    // determine the name of the driver that an I/O request is/was bound.
    //

    UNICODE_STRING DriverName;

    //
    // The following section is for registry support.  Thise is a pointer
    // to the path to the hardware information in the registry
    //

    PUNICODE_STRING HardwareDatabase;

    //
    // The following section contains the optional pointer to an array of
    // alternate entry points to a driver for "fast I/O" support.  Fast I/O
    // is performed by invoking the driver routine directly with separate
    // parameters, rather than using the standard IRP call mechanism.  Note
    // that these functions may only be used for synchronous I/O, and when
    // the file is cached.
    //

    void* FastIoDispatch;

    //
    // The following section describes the entry points to this particular
    // driver.  Note that the major function dispatch table must be the last
    // field in the object so that it remains extensible.
    //

    void* DriverInit;
    void* DriverStartIo;
    void* DriverUnload;
    void* MajorFunction[0x1b + 1];

} DRIVER_OBJECT,*PDRIVER_OBJECT;

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

void Fix(HMODULE base) 
{
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
    IMAGE_NT_HEADERS* nt = NULL;
    nt = (IMAGE_NT_HEADERS*)(((IMAGE_DOS_HEADER*)base)->e_lfanew + (char*)base);
    //oep = nt->OptionalHeader.AddressOfEntryPoint;

    FixImport(base, nt);
    FixReloc(base, nt);
}
PVOID
__stdcall
ExAllocatePoolWithTag(
                      POOL_TYPE  PoolType,
                      SIZE_T  NumberOfBytes,
                      ULONG  Tag

                      )
{
    return VirtualAlloc(NULL, NumberOfBytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

VOID
__stdcall
ExFreePoolWithTag(
                  PVOID P,
                  ULONG Tag
                  ) 
{
    VirtualFree(P, 0, MEM_DECOMMIT);
}

FARPROC ExAllocatePool_Real = NULL;
//FARPROC ExFreePool_Real = NULL;
FARPROC ExFreePoolWithTag_Real = NULL;

#include "../common/detours.h"

void hook( PVOID *ppPointer, PVOID pDetour )
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(ppPointer, pDetour);
    DetourTransactionCommit();
}

void ReplaceDriverFunction() {
    HMODULE hModule = GetModuleHandleA("ntoskrnl.exe");
    ExAllocatePool_Real = GetProcAddress(hModule, "ExAllocatePoolWithTag");
    hook(&(PVOID&)ExAllocatePool_Real, ExAllocatePoolWithTag);

    FARPROC ExFreePoolWithTag_Real = GetProcAddress(hModule, "ExFreePoolWithTag");
    hook(&(PVOID&)ExFreePoolWithTag_Real, ExFreePoolWithTag);
}


VOID
InitializeListHead(
                   IN PLIST_ENTRY ListHead
                   )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}


VOID
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

LIST_ENTRY* g_entry = NULL;
void ssss()
{
    // 这段是给DriverEntry用的
    PRTL_PROCESS_MODULES pModules = NULL;
    PRTL_PROCESS_MODULE_INFORMATION pModuleInfo = NULL;

    // 获取已经加载驱动列表
    pModules = (PRTL_PROCESS_MODULES)QuerySystemInfo(SystemModuleInformation);
    if (pModules) 
    {
        pModuleInfo = &pModules->Modules[0];

        for (ULONG i = 0 ; i < 3/*pModules->NumberOfModules*/; ++i, pModuleInfo++) 
        {
            //if (/*FindImport(pModuleInfo, list) */
            //    /*||*/ pModuleInfo->LoadOrderIndex == 0 // ntoskrnl.exe
            //    || pModuleInfo->LoadOrderIndex == 1 // hal.dll有不同的名字
            //    )
            //{
                LDR_DATA_TABLE_ENTRY* cur = (LDR_DATA_TABLE_ENTRY*)malloc(sizeof(LDR_DATA_TABLE_ENTRY));
                memset(cur, 0, sizeof(LDR_DATA_TABLE_ENTRY));

                cur->Flags			= pModuleInfo->Flags;
                cur->DllBase		= pModuleInfo->ImageBase;
                cur->SizeOfImage	= pModuleInfo->ImageSize;
                cur->LoadCount		= pModuleInfo->LoadCount;


                int len = MultiByteToWideChar(CP_ACP, 0, (char*)&pModuleInfo->FullPathName, strlen((char*)pModuleInfo->FullPathName), NULL, 0);
                WCHAR* p = (WCHAR*)malloc((len+1) * sizeof(WCHAR));
                MultiByteToWideChar(CP_ACP, 0, (char*)&pModuleInfo->FullPathName, len, p, len*sizeof(WCHAR));
                p[len] = 0;
                RtlInitUnicodeString(&cur->FullDllName, p);
                RtlInitUnicodeString(&cur->BaseDllName, p + pModuleInfo->OffsetToFileName);


                char* name = (char*)&pModuleInfo->FullPathName + pModuleInfo->OffsetToFileName;
                if (pModuleInfo->LoadOrderIndex == 0) {

                    g_entry = &cur->InLoadOrderLinks; // 实际起作用的是LoadLibraryExA
                    InitializeListHead(g_entry);

                    //wcscpy(p + pModuleInfo->OffsetToFileName, L"ntoskrnl.exe");
                    cur->DllBase = LoadLibraryExA("ntoskrnl.exe", 0, DONT_RESOLVE_DLL_REFERENCES);
                }
                else if (pModuleInfo->LoadOrderIndex == 1)
                {
                    cur->DllBase = LoadLibraryExA("hal.dll", 0, DONT_RESOLVE_DLL_REFERENCES);
                }
                else
                {
                    cur->DllBase = LoadLibraryExA(name, 0, DONT_RESOLVE_DLL_REFERENCES);
                }
                //else if (stricmp(name, "hal.dll") == 0) {
                //    
                //}
                //else if (stricmp(name, "kdcom.dll") == 0) {
                //    cur->DllBase = LoadLibraryExW(L"kdcom.dll", 0, DONT_RESOLVE_DLL_REFERENCES);
                //}

                //printf("%s %08X\r\n", name, cur->DllBase);

                InsertTailList(g_entry, &cur->InLoadOrderLinks);
            //}

        }
        free(pModules);
    }
}

__declspec(dllexport) void __cdecl  DriverEntry()
{
    
}

typedef long (__stdcall *DriverEntry_PTR)( 
    IN PDRIVER_OBJECT  DriverObject, 
    IN PUNICODE_STRING  RegistryPath 
    );

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{

    LPWSTR *szArglist;
    int nArgs;
    int i;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if( NULL == szArglist )
    {
        //wprintf(L"CommandLineToArgvW failed\n");
        return 0;
    }
    else
    {
        WCHAR szDll[MAX_PATH];
        lstrcpyW(szDll, szArglist[1]);
        LocalFree(szArglist);

        HMODULE hmod = LoadLibraryExW(szDll, 0, DONT_RESOLVE_DLL_REFERENCES);
        if (hmod)
        {
            Fix(hmod);
            ReplaceDriverFunction();

            IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(((IMAGE_DOS_HEADER*)hmod)->e_lfanew + (char*)hmod);
            DWORD oep = nt->OptionalHeader.AddressOfEntryPoint;

            DriverEntry_PTR entry = (DriverEntry_PTR)((char*)hmod + oep);

            DRIVER_OBJECT ob;
            ob.DriverStart = (void*)hmod;
            ob.DriverSize  = nt->OptionalHeader.SizeOfImage;
            ob.DriverSection = NULL;
            ob.Type = 4;
            ob.Flags = 2;
            ob.DriverInit = entry;

            ssss();
            LDR_DATA_TABLE_ENTRY* cur = (LDR_DATA_TABLE_ENTRY*)malloc(sizeof(LDR_DATA_TABLE_ENTRY));
            memset(cur, 0, sizeof(LDR_DATA_TABLE_ENTRY));

            cur->DllBase		= (void*)hmod;
            cur->SizeOfImage	= nt->OptionalHeader.SizeOfImage;
            cur->LoadCount		= 1;
            WCHAR szTemp[MAX_PATH];
            wsprintfW(szTemp, L"\\??\\%s", szDll);
            RtlInitUnicodeString(&cur->FullDllName, szTemp);
            {
                WCHAR *p = StrRChrW(szDll, 0, L'\\') + 1;
                WCHAR *p2 = StrRChrW(szDll, 0, L'.');
                size_t size = p2 - p;
                wcsncpy(szTemp, p, size);
                RtlInitUnicodeString(&cur->BaseDllName, szTemp);
            }
            InsertTailList(g_entry, &cur->InLoadOrderLinks);
            ob.DriverSection = &cur;
            
            UNICODE_STRING path;
            lstrcpyW(szTemp, L"\\Registry\\Machine\\System\\ControlSet001\\Services\\");
            {
                WCHAR *p = StrRChrW(szDll, 0, L'\\') + 1;
                WCHAR *p2 = StrRChrW(szDll, 0, L'.');
                size_t size = p2 - p;
                wcsncat(szTemp, p, size);
                RtlInitUnicodeString(&path, szTemp);
            }

            entry(&ob, &path);
            // shit strongOD的还是无法脱，有处IN3错误，进去全是VM，NB
        }
       

    }
    return(1);
}

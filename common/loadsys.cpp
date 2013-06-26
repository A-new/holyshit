#include "hook.h"
#include "loadsys.h"
#include "func.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#ifdef HOLYSHIT_EXPORTS
PVOID OrgDllCheck = (PVOID)0x00477754; // od1.10
#else
PVOID OrgDllCheck = (PVOID)0x0044BFCE; // od2
#endif
PVOID OrgDllCheck2 = (PVOID)0x00458257; // only for od2


void __declspec(naked) MyDllCheck()
{
    static const TCHAR* DllPath;
    static DWORD* change;
    __asm{
        push eax
            mov eax, dword ptr [ebp + 0x08]
        mov DllPath, eax
#ifdef HOLYSHIT_EXPORTS
            lea eax, dword ptr [ebp - 0x08]
#else
            lea eax, dword ptr [ebp - 0x0C]
#endif
        mov change, eax
            pop eax
            pushad
            pushfd
    }
    if (IsSysFile(DllPath))
    {
        *change = 1;
    }

    __asm{
        popfd
            popad
            jmp OrgDllCheck
    }
}

BOOL __cdecl Mycompare(const TCHAR* DllPath, const TCHAR* r)
{
    if(IsSysFile(DllPath))
    {
        return 1;
    }
    return 0;
}
void __declspec(naked) MyDllCheck2()
{
    __asm
    {
        call Mycompare;
        test eax,eax;
        je l1
            ADD DWORD PTR [ESP],0x2 // 跟原call一样，居然修改参数
l1:
        jmp OrgDllCheck2;
    }
    //static const TCHAR* DllPath;
    //
    //__asm{
    //    push eax
    //    mov eax, dword ptr [esp + 0x4]
    //    mov DllPath, eax
    //    pop eax
    //    pushad
    //    pushfd
    //}

    //if (IsSysFile(DllPath))
    //{
    //    __asm{
    //        mov eax, 1
    //    }
    //}

    //__asm{
    //    popfd
    //    popad
    //    jmp OrgDllCheck2
    //}
}

void hook_loadsys_functions()
{
    hook(&(PVOID&)OrgDllCheck, MyDllCheck);
#ifndef HOLYSHIT_EXPORTS // only for od2
    hook(&(PVOID&)OrgDllCheck2, MyDllCheck2);
#endif
}

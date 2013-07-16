#include "hook.h"
#include "loadsys.h"
#include "func.h"
#include "../sdk/sdk.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#ifdef HOLYSHIT_EXPORTS
PVOID OrgDllCheck = (PVOID)0x00477754; // od1.10
PVOID OrgDllCheck3 = (PVOID)0x004778D4; // wsprint，改LOADDLL.exe为LOADSYS.exe
PVOID OrgDllCheck4 = (PVOID)0x0042F9D2;
PVOID OrgDllCheck5 = (PVOID)0x0042F9A3;
#else // od2
PVOID OrgDllCheck = (PVOID)0x0044BFCE; 
PVOID OrgDllCheck2 = (PVOID)0x00458257; // only for od2，第一个版本需要，现在有loadsys.exe了，不patch这个地方了
PVOID OrgDllCheck3 = (PVOID)0x0044C1D0;
PVOID OrgDllCheck6 = (PVOID)0x004D0ED4;
PVOID OrgDllCheck7 = (PVOID)0x004542E4;
#endif


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



//004778D4  |. E8 53F30200    CALL OLLYDBG.004A6C2C                    ; \OLLYDBG.004A6C2C
//0012C138   0012C5D0  |Arg1 = 0012C5D0
//0012C13C   004C2044  |Arg2 = 004C2044 ASCII ""%s\LOADDLL.EXE" %s"
//0012C140   004D3868  |Arg3 = 004D3868 ASCII "D:\green\odbg110"
//0012C144   0012DD60  \Arg4 = 0012DD60 ASCII "D:\src\crack\unvm\sample.sys"
// od2 0044C1D0  |. E8 9BD70900    CALL ollydbg.004E9970                    ; \ollydbg.004E9970
void __cdecl Mycompare3(TCHAR* buf, TCHAR* format, const TCHAR* dir,const TCHAR* DllPath)
{
    if (0 == lstrcmpi(format, TEXT("\"%s\\LOADDLL.EXE\" %s"))
        && IsSysFile(DllPath))
    {
        format = TEXT("\"%s\\LOADSYS.EXE\" %s");
    }
}

void __declspec(naked) MyDllCheck3()
{
    __asm
    {
        call Mycompare3;
        jmp OrgDllCheck3;
    }
}

/*
0042F9C2  |. 6A 00          PUSH 0x0                                 ; /Arg4 = 00000000
0042F9C4  |. 6A 00          PUSH 0x0                                 ; |Arg3 = 00000000
0042F9C6  |. 68 80480000    PUSH 0x4880                              ; |Arg2 = 00004880
0042F9CB  |. 8B4D AC        MOV ECX,[LOCAL.21]                       ; |
0042F9CE  |. 8B41 28        MOV EAX,DWORD PTR DS:[ECX+0x28]          ; |
0042F9D1  |. 50             PUSH EAX                                 ; |Arg1
0042F9D2  |. E8 899BFEFF    CALL OLLYDBG._Setbreakpointext           ; \_Setbreakpointext

0012DA54   00271433  |Arg1 = 00271433 // ep地址往后设置就可以了
0012DA58   00004880  |Arg2 = 00004880
0012DA5C   00000000  |Arg3 = 00000000
0012DA60   00000000  \Arg4 = 00000000
*/

void __cdecl Mycompare4(DWORD* ep, DWORD, DWORD, DWORD
#ifndef HOLYSHIT_EXPORTS
        , DWORD // OD2
#endif
                        )
{
    std::tstring str = GetDebugeedExePath();

    if (IsSysFile(str.c_str()))
    {
        static DWORD delta = 0;
        if (delta == 0)
        {
            delta = -1;
            HMODULE hMod = LoadLibraryExA("loadsys.exe", 0, DONT_RESOLVE_DLL_REFERENCES);
            if (hMod)
            {
                PROC p = GetProcAddress(hMod, "DriverEntry");
                delta = (DWORD)p - (DWORD)hMod;
                FreeLibrary(hMod);
            }
        }

        t_module* tm = Findmodule((ulong)ep);
        ep = (DWORD*)((DWORD)tm->base + delta);
        
    }
}

#ifdef HOLYSHIT_EXPORTS
void __declspec(naked) MyDllCheck4()
{
    __asm
    {
        call Mycompare4;
        jmp OrgDllCheck4;
    }
}

void __declspec(naked) MyDllCheck5()
{
    __asm
    {
        call Mycompare4;
        jmp OrgDllCheck5;
    }
}
#else
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

//0042F9A3  |. E8 B89BFEFF    CALL OLLYDBG._Setbreakpointext           ; \_Setbreakpointext


// 关键地址，00454079  |. E8 56CE0700    |CALL ollydbg.004D0ED4
/*
004D0ED4  /$ 55             PUSH EBP
*/
BOOL __cdecl Mycompare6()
{
    std::tstring str = GetDebugeedExePath();
    return IsSysFile(str.c_str());
}

//void __declspec(naked) MyDllCheck6()
//{
//    __asm
//    {
//        call Mycompare6;
//        test eax, eax;
//        je l1;
//l1:
//        jmp OrgDllCheck6;
//    }
//}


BOOL MyDllCheck6()
{
    if (Mycompare6())
    {
        return 0;
    }
    else
    {
        typedef BOOL (__stdcall *nothing)();
        nothing n1 = (nothing)OrgDllCheck6;
        return n1();
    }
}


/* OD2设置EP用的硬件断点，_sethardware...
004542D2  |. FF35 24475C00  |PUSH DWORD PTR DS:[zwcontinue]          ; /Arg5 = 777A4650
004542D8  |. 6A 00          |PUSH 0x0                                ; |Arg4 = 00000000
004542DA  |. 6A 00          |PUSH 0x0                                ; |Arg3 = 00000000
004542DC  |. 68 00200000    |PUSH 0x2000                             ; |Arg2 = 00002000
004542E1  |. FF75 9C        |PUSH [LOCAL.25]                         ; |Arg1 // 改这个值即可
004542E4  |. E8 8FD0FFFF    |CALL ollydbg.00451378                   ; \ollydbg.00451378
*/
void __declspec(naked) MyDllCheck7()
{
    __asm
    {
        call Mycompare4;
        jmp OrgDllCheck7;
    }
}
#endif

void hook_loadsys_functions()
{
    hook(&(PVOID&)OrgDllCheck, MyDllCheck); // 让sys后辍的可以加载
    hook(&(PVOID&)OrgDllCheck3, MyDllCheck3); // 更改loaddll为loadsys

#ifdef HOLYSHIT_EXPORTS
    hook(&(PVOID&)OrgDllCheck4, MyDllCheck4); // 设置ep为永远达不到的地址
    hook(&(PVOID&)OrgDllCheck5, MyDllCheck5); // 设置ep为永远达不到的地址
#else // OD2
    //hook(&(PVOID&)OrgDllCheck2, MyDllCheck2); // 判断是否是主程序 only for OD2，第一个版本需要，现在有loadsys.exe了，不patch这个地方了
    hook(&(PVOID&)OrgDllCheck6, MyDllCheck6); // 去掉无法连接loaddll提示
    hook(&(PVOID&)OrgDllCheck7, MyDllCheck7); // 设置ep为永远达不到的地址
#endif


}

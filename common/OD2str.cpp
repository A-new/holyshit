#include "OD2str.h"
#include "hook.h"
#include "StrFinder.h"
#include "../sdk/sdk.h"

/*
shit！OD对lea指令不作处理？lea eax,dword ptr[xxx]如果xxx是字符串，是不会去找的
搜索字符串跟分析有关，每次动态改了代码后需要C-A*/

str_patch::str_patch( IConfigForStrpatch* i)
: m_IConfigForStrpatch(i)
{

}
typedef int (__cdecl* ISTEXTA)(int);
typedef int (__cdecl* ISTEXTW)(int);
typedef int (__cdecl* ASCIITOUNICODE)(const char *s,int ns,wchar_t *w,int nw);
typedef int (__cdecl* UTFTOUNICODE)(const char *t,int nt,wchar_t *w,int nw);
typedef int (__cdecl *ISSTRING)(ulong addr,int isstatic,wchar_t *symb,int nsymb);
static ISTEXTA org_IsTextA = NULL;
static ISTEXTW org_IsTextW = NULL;
static ASCIITOUNICODE org_Asciitounicode = NULL;
static UTFTOUNICODE org_Utftounicode = NULL;
static ISSTRING org_Isstring = NULL;

static int IsSimplifiedCH(unsigned short &dch)
{
    unsigned char highbyte;
    unsigned char lowbyte;

    highbyte = dch & 0x00ff;
    lowbyte  = dch / 0x0100;

    if (highbyte==0 || lowbyte==0)
        return 0;

    //    区名    码位范围   码位数  字符数 字符类型
    // 双字节2区 B0A1—F7FE   6768    6763    汉字
    if ((highbyte >= 0xb0 && highbyte <= 0xf7) && (lowbyte  >= 0xa1 && lowbyte  <= 0xfe))
        return 1;

    char GBt[3];
    char GBs[3];
    memset(GBt,0,3);
    memcpy(GBt,&dch,2);
    memset(GBs,0,3);
    if (LCMapStringA(0x0804,LCMAP_SIMPLIFIED_CHINESE, GBt, 2, GBs, 2))
    {
        highbyte=GBs[0];
        lowbyte=GBs[1];
        if (highbyte==0 || lowbyte==0)
            return 0;
        if ((highbyte >= 0xb0 && highbyte <= 0xf7) && (lowbyte  >= 0xa1 && lowbyte  <= 0xfe))
        {
            memcpy(&dch,GBs,2);
            return 1;
        }
    }

    return 0;
}

static int IsGraphicCH(const unsigned short dch)
{
    int nRet;
    unsigned char highbyte;
    unsigned char lowbyte;

    highbyte = dch & 0x00ff;
    lowbyte  = dch / 0x0100;

    //    区名    码位范围   码位数  字符数 字符类型
    // 双字节1区 A1A1—A9FE    846     718  图形符号
    if (
        (highbyte >= 0xa1 && highbyte <= 0xa9) &&
        (lowbyte  >= 0xa1 && lowbyte  <= 0xfe)
        )
        nRet = 1;
    //    区名    码位范围   码位数  字符数 字符类型
    // 双字节5区 A840—A9A0    192     166  图形符号
    else if (
        (highbyte >= 0xa8 && highbyte <= 0xa9) &&
        (lowbyte  >= 0x40 && lowbyte  <= 0xa0)
        )
        nRet = 1;
    else
        nRet = 0;

    return nRet;
}
bool isSimGra(const unsigned short dch)
{
    unsigned char g_chrRet[2];
    g_chrRet[0]=0;
    g_chrRet[1]=0;

    memcpy(g_chrRet,&dch,2);

    if (IsSimplifiedCH(*(unsigned short *)&g_chrRet) || IsGraphicCH(*(unsigned short *)&g_chrRet))
        return true;
    else
        return false;
}

/*
in _Isstring

00487720   8B85 E0FEFFFF    MOV EAX,DWORD PTR SS:[EBP-0x120]         ; 首字符
00487726   50               PUSH EAX
00487727   E8 94B2F7FF      CALL ollydbg._IstextA
0048772C   59               POP ECX
0048772D   8945 F0          MOV DWORD PTR SS:[EBP-0x10],EAX
00487730   8D9D E1FEFFFF    LEA EBX,DWORD PTR SS:[EBP-0x11F]
00487736   33F6             XOR ESI,ESI
00487738   46               INC ESI
00487739   837D F0 02       CMP DWORD PTR SS:[EBP-0x10],0x2
0048773D   75 05            JNZ SHORT ollydbg.00487744
0048773F   43               INC EBX
00487740   46               INC ESI
00487741   90               NOP
00487742   90               NOP
00487743   90               NOP
00487744   33C9             XOR ECX,ECX
00487746   8A8D E0FEFFFF    MOV CL,BYTE PTR SS:[EBP-0x120]
0048774C   3BFE             CMP EDI,ESI
0048774E   894D FC          MOV DWORD PTR SS:[EBP-0x4],ECX           ; 取首个字符
00487751   90               NOP                                      ; 第二个字符
00487752   90               NOP
00487753   90               NOP
00487754   90               NOP
00487755   90               NOP
00487756   90               NOP
00487757   7E 21            JLE SHORT ollydbg.0048777A
00487759   8B03             MOV EAX,DWORD PTR DS:[EBX]
0048775B   50               PUSH EAX
0048775C   E8 5FB2F7FF      CALL ollydbg._IstextA
00487761   59               POP ECX
00487762   8945 F8          MOV DWORD PTR SS:[EBP-0x8],EAX
00487765   837D F8 00       CMP DWORD PTR SS:[EBP-0x8],0x0
00487769   74 0F            JE SHORT ollydbg.0048777A
0048776B   837D F8 02       CMP DWORD PTR SS:[EBP-0x8],0x2
0048776F   75 03            JNZ SHORT ollydbg.00487774
00487771   43               INC EBX
00487772   46               INC ESI
00487773   90               NOP
00487774   46               INC ESI
00487775   43               INC EBX
00487776   3BFE             CMP EDI,ESI
00487778  ^7F DF            JG SHORT ollydbg.00487759


*/

// 用__stdcall省点力
static int __stdcall IsTextA_check(int a)
{
    return isSimGra(a) ? 2 : 0;
}

//0x004A335D
// 中文字符判断暂时不支持繁体(见“中文搜索源码”)
static void __declspec(naked) __cdecl hook_IsTextA()
{
    __asm{
        mov eax, dword ptr[esp + 4];
        push eax;
        call org_IsTextA;
        add esp, 4;
        cmp eax, 1;
        je back;
        cmp dword ptr [esp], 0x0048772C;
        je check;
        cmp dword ptr [esp], 0x00487761;
        je check;
        jmp back;
check:
        mov eax, dword ptr[esp + 4];
        push eax;
        call IsTextA_check;
        jmp back;
back:
        retn
    }
}


static void UnicodeToGB2312(unsigned char* pOut,unsigned short uData)   
{   
    WideCharToMultiByte(CP_ACP,NULL,(LPCWSTR)&uData,1,(LPSTR)pOut,sizeof(unsigned short),NULL,NULL);   
    return;   
}
static int __cdecl hook_IsTextW(int a)
{
    int ret = org_IsTextW(a);
    if (ret == 0)
    {
        unsigned char  g_chrRet[2];
        UnicodeToGB2312(g_chrRet,a);
        if (IsGraphicCH(*(USHORT *)&g_chrRet) 
            ||
            IsSimplifiedCH(*(USHORT *)&g_chrRet))
        {
            return 1;
        }
    }
    return ret;
}

int __cdecl hook_Asciitounicode(const char *s,int ns,wchar_t *w,int nw)
{
    // ns的长度是缓冲区长度
    int ret = MultiByteToWideChar(CP_ACP, 0, s, -1, w, nw);//org_Asciitounicode(s, ns, w, nw);
    return  ret - 1; // 多了个NULL
}

int __cdecl hook_Utftounicode(const char *s,int ns,wchar_t *w,int nw)
{
    int ret = org_Utftounicode(s, ns, w,nw);
    if(ret == 0)
    {
        return hook_Asciitounicode(s, ns, w, nw);
    }
    return ret;
}

/*
004A335D   8B45 C8               ||/MOV EAX,DWORD PTR SS:[EBP-0x38]
004A3360   F640 01 02            |||TEST BYTE PTR DS:[EAX+0x1],0x2        ; OP_CONST, lea是OP_MEMORY
004A3364   74 54                 |||JE SHORT ollydbg.004A33BA
第二行和第三行都是不能hook的
*/


PVOID lea_check = (PVOID)0x004A335D;

// 这里也许可以进一步判是否是lea操作，但是OD使用的反汇编引擎跟公开的disasm好像不一致，
// 在disasm.h中DX_LEA是0x03000000，而调试OD中发现是0x04000000，暂时不管
void __declspec(naked) lea_patch()
{
    __asm
    {
        MOV EAX,DWORD PTR [EBP-0x38];
        TEST BYTE PTR [EAX+0x1],0x1; 
        je back;
        MOV BYTE PTR DS:[EAX+0x1],0x2 // 将OP_MEORY变成OP_CONST，最终效果一样
back:
        jmp lea_check;
    }
} 

static BOOL IsAlpha(const BYTE ch)
{
    if (ch >= 0x20 && ch <= 0x7f)
        return TRUE;
    else
        return FALSE;
}

static void GB2312ToUnicode(unsigned short* gb, unsigned short* un)
{
    MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)gb, 2, (LPWSTR)un, 1);
}
static BOOL GetStrW(
                        const STR_FINDER_STRING_TYPE StrType,
                        const BYTE *pbyBuf,
                        WCHAR *pszStr,
                        int *pnLen
                        )
{
    if (StrType == enumSFST_Unicode)
    {
        return FALSE;
    }
    BOOL bFound = TRUE;

    int i = 0;
    int nLen = 0;
    lstrcpyW(pszStr, L"ASCII ");
    pszStr += lstrlenW(L"ASCII ");

    while (i < TEXTLEN)
    {
        if (enumSFST_Ascii == StrType)
        {
            if ('\0' == pbyBuf[i])
                break;
        }
        else if (enumSFST_Unicode == StrType)
        {
            if (i + 1 >= TEXTLEN || '\0' == pbyBuf[i] && '\0' == pbyBuf[i + 1])
                break;
        }

        if ('\r' == pbyBuf[i])
        {
            *pszStr++ = TEXT('\\');
            *pszStr++ = TEXT('r');
            nLen += 2;
        }
        else if ('\n' == pbyBuf[i])
        {
            *pszStr++ = TEXT('\\');
            *pszStr++ = TEXT('n');
            nLen += 2;
        }
        else if ('\t' == pbyBuf[i])
        {
            *pszStr++ = TEXT('\\');
            *pszStr++ = TEXT('t');
            nLen += 2;
        }
        else if (IsAlpha(pbyBuf[i]))
        {
            *pszStr++ = pbyBuf[i];
            *pszStr++ = 0;
            nLen += 2;
        }
        else if (IsGraphicCH(
            *(USHORT *)&pbyBuf[i]) ||
            IsSimplifiedCH(*(USHORT *)&pbyBuf[i])
            )
        {
            GB2312ToUnicode((unsigned short*)&pbyBuf[i], (unsigned short*)pszStr);
            //*pszStr++ = pbyBuf[i];
            ++i;
            pszStr += 1;
            //*pszStr++ = pbyBuf[i];
            nLen += 2;
        }
        else if ('\0' == pbyBuf[i])
        {
            if (enumSFST_Unicode == StrType)
            {
                // Skip '\0'
            }
            else
            {
                bFound = FALSE;
                break;
            }
        }
        else
        {
            bFound = FALSE;
            break;
        }
        ++i;
    }

    if (0 == nLen)
        bFound = FALSE;

    *pszStr = TEXT('\0');

    if (bFound)
    {
        if (i >= TEXTLEN)
        {
            _tcscpy((TCHAR*)&pszStr[TEXTLEN - 4], TEXT(" ..."));
            nLen = TEXTLEN; // cut length to TEXTLEN
        }
    }

    if (pnLen)
        *pnLen = nLen;

    return bFound;
}

// 返回一共有多少个unicode
int __cdecl hook_Isstring(ulong addr
                          ,int isstatic
                          ,wchar_t *symb // 缓存地址
                          ,int nsymb)    // 缓存大小
{
    int ret;
    // 原函数对unicode的支持还是比较好的，我们只处理ascii码情况
    ret = org_Isstring(addr, isstatic, symb, nsymb);
    if(0 == ret)
    {
        t_memory *pMem;
        pMem = Findmemory(addr);
        if ((NULL == pMem) || ('\0' == pMem->sectname[0]))
            return ret;

        BYTE szBuf[TEXTLEN];
        int nStrLen;
        int nRetCode;
        nRetCode = Readmemory(
            szBuf, addr, sizeof(szBuf), MM_RESTORE | MM_SILENT
            );
        if (!nRetCode)
        {
            return ret;
        }
        int len;
        if (GetStrW(enumSFST_Ascii, szBuf, symb, &len))
        {
            return lstrlenW(symb);
        }
    }
    return ret;
}

static int* mbcscodepage = NULL;

int str_patch::ODBG2_Plugininit( void )
{
    HMODULE hMain = GetModuleHandle(NULL);

    // "Use IsTextUnicode"，未导出，让OD不使用内部的_IstextW判断unicode
    //*(DWORD*)0x0057DD38 = 0;

    // Code page for ASCII dumps 
    *const_cast<int*>(&asciicodepage) = 936;
    // Code page for multibyte dumps，at 005BE6A0，导出但是lib里没有
    mbcscodepage = (int*)GetProcAddress(hMain, "mbcscodepage");
    if (mbcscodepage)
    {
        *mbcscodepage = 936;
    }

    //DWORD dwOld;
    //VirtualProtect((PVOID)0x00487720, 0x1000, PAGE_EXECUTE_READWRITE, &dwOld);
    //*(unsigned char*)0x00487720 = 0x8B;
    //static const unsigned char p1 [] = 
    //{
    //    0x8d, 0x9d, 0xe1, 0xfe, 0xff, 0xff, 0x33, 0xf6, 0x46, 0x83, 0x7d, 0xf0, 0x02, 0x75, 0x05, 0x43,
    //    0x46, 0x90, 0x90, 0x90, 0x33, 0xc9, 0x8a, 0x8d, 0xe0, 0xfe, 0xff, 0xff, 0x3b, 0xfe, 0x89, 0x4d,
    //    0xfc, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x7e, 0x21, 0x8b, 0x03, 0x50, 0xe8, 0x5f, 0xb2, 0xf7,
    //    0xff, 0x59, 0x89, 0x45, 0xf8, 0x83, 0x7d, 0xf8, 0x00, 0x74, 0x0f, 0x83, 0x7d, 0xf8, 0x02, 0x75,
    //    0x03, 0x43, 0x46, 0x90
    //};
    //memcpy((unsigned char*)0x00487730, p1, sizeof(p1));
    //VirtualProtect((PVOID)0x00487720, 0x1000, dwOld, &dwOld);

    // 对判断ascii中文很关键
    //org_IsTextA  = (ISTEXTA)GetProcAddress(hMain, "_IstextA");
    //hook(&(PVOID&)org_IsTextA, hook_IsTextA);

    // IsTextUnicode没有我们的hook_IsTextW
    org_IsTextW  = (ISTEXTW)GetProcAddress(hMain, "_IstextW");
    hook(&(PVOID&)org_IsTextW, hook_IsTextW);

    // 这个让界面显示的时候正常
    org_Asciitounicode = (ASCIITOUNICODE)GetProcAddress(hMain, "_Asciitounicode");
    hook(&(PVOID&)org_Asciitounicode, hook_Asciitounicode);

    //org_Utftounicode = (UTFTOUNICODE)GetProcAddress(hMain, "_Utftounicode");
    //hook(&(PVOID&)org_Utftounicode, hook_Utftounicode);

    org_Isstring = (ISSTRING)GetProcAddress(hMain, "_Isstring");
    hook(&(PVOID&)org_Isstring, hook_Isstring);

    hook(&(PVOID&)lea_check, lea_patch);
    return 0;
}


unsigned char MyArray [0x0019] =
{
    
};
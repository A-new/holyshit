#include "OD2str.h"
#include "hook.h"
#include "StrFinder.h"
#include "../sdk/sdk.h"
#include "../common/define.h"

/*
shit！OD对lea指令不作处理？lea eax,dword ptr[xxx]如果xxx是字符串，是不会去找的
搜索字符串跟分析有关，每次动态改了代码后需要C-A*/

str_patch::str_patch( IConfigForStrpatch* i)
: m_IConfigForStrpatch(i)
{

}
typedef int (__cdecl* ISTEXTW)(int);
typedef int (__cdecl* ASCIITOUNICODE)(const char *s,int ns,wchar_t *w,int nw);
typedef int (__cdecl *ISSTRING)(ulong addr,int isstatic,wchar_t *symb,int nsymb);
static ISTEXTW org_IsTextW = NULL;
static ASCIITOUNICODE org_Asciitounicode = NULL;
static ISSTRING org_Isstring = NULL;


// from 52论坛的 中文搜索插件
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
static bool isSimGra(const unsigned short dch)
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

static int __cdecl hook_Asciitounicode(const char *s,int ns,wchar_t *w,int nw)
{
    // ns的长度是缓冲区长度
    int ret = MultiByteToWideChar(CP_ACP, 0, s, -1, w, nw);//org_Asciitounicode(s, ns, w, nw);
    return  ret - 1; // 多了个NULL
}


/*
004A335D   8B45 C8               ||/MOV EAX,DWORD PTR SS:[EBP-0x38]
004A3360   F640 01 02            |||TEST BYTE PTR DS:[EAX+0x1],0x2        ; OP_CONST, lea是OP_MEMORY
004A3364   74 54                 |||JE SHORT ollydbg.004A33BA
第二行和第三行都是不能hook的
*/


PVOID lea_check = (PVOID)HARDCODE(0x004A335D);

// 这里也许可以进一步判是否是lea操作，但是OD使用的反汇编引擎跟公开的disasm好像不一致，
// 在disasm.h中DX_LEA是0x03000000，而调试OD中发现是0x04000000，暂时不管
static void __declspec(naked) lea_patch()
{
    /*HARDCODE*/
    __asm
    {
        pushad;
        pushfd;

        MOV EAX,DWORD PTR [EBP-0x1D1C]; // 取指令
        AND EAX, 0xFF000000;
        CMP EAX, 0x04000000;            // lea，跟disasm.h不一样
        JNE back;

        MOV EAX,DWORD PTR [EBP-0x38];
        TEST BYTE PTR [EAX+0x1],0x1;    // 1 == OP_MEORY
        je back;

        MOV BYTE PTR DS:[EAX+0x1],0x2; // 将OP_MEORY变成OP_CONST，最终效果一样
back:
        popfd;
        popad;
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
            nLen += 1;
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
static int __cdecl hook_Isstring(ulong addr
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
        //int nStrLen;
        int nRetCode;
        nRetCode = Readmemory(
            szBuf, addr, sizeof(szBuf), MM_RESTORE | MM_SILENT
            );
        if (!nRetCode)
        {
            return ret;
        }
        int len;
        if (GetStrW(enumSFST_Ascii, szBuf, symb, &len)
            /*&& len >= 2*/)
        {
            return lstrlenW(symb);
        }
    }
    return ret;
}




static int* mbcscodepage = NULL;

static PVOID search_patchAddr = (PVOID)HARDCODE(0x004A1961);
static int __cdecl MyUnicodetoutf(const wchar_t *w,int nw,char *s,int ns)
{
    int ret = WideCharToMultiByte(CP_ACP,NULL, w, nw,s, ns, NULL,NULL);
    s[ret] = '\0';
    return ret;
}

/*
0048772D   8945 F0          MOV DWORD PTR SS:[EBP-0x10],EAX                                ; fuck!后面UNICODE还要根据这个来判断，因为英文
00487730   837D F0 02       CMP DWORD PTR SS:[EBP-0x10],0x2
*/
static PVOID patch1 = (PVOID)HARDCODE(0x0048772D);
static void __declspec(naked) patch1_do()
{
    __asm
    {
        cmp eax,0;
        jne back;
        MOV EAX,DWORD PTR [EBP-0x120];
        PUSH EAX;
        call hook_IsTextW;
        add esp, 4;
back:
        jmp patch1;
    }
}
/*
00487C0C   8D4E 02          LEA ECX,DWORD PTR DS:[ESI+0x2]
00487C0F   3BF9             CMP EDI,ECX                              ; EDI最大256即0x100
00487C11   7C 61            JL SHORT ollydbg.00487C74                ; ECX当前0x102
*/
static PVOID patch2 = (PVOID)HARDCODE(0x00487C0C);
static void __declspec(naked) patch2_do()
{
    __asm
    {
        LEA ECX,DWORD PTR [ESI+0x2];
        CMP EDI, ECX;
        JGE back;
        SUB ESI, 0x2
back:
        jmp patch2;
    }
}
static void __declspec(naked) search_patch()
{
    __asm
    {
        call MyUnicodetoutf;
        jmp search_patchAddr;
    }
}
int str_patch::ODBG2_Plugininit( void )
{
    HMODULE hMain = GetModuleHandle(NULL);

    // "Use IsTextUnicode"，未导出，让OD不使用内部的_IstextW判断unicode
    //*(DWORD*)HARDCODE(0x0057DD38) = 0;

    // Code page for ASCII dumps 
    *const_cast<int*>(&asciicodepage) = 936;
    // Code page for multibyte dumps，at 005BE6A0，导出但是lib里没有
    mbcscodepage = (int*)GetProcAddress(hMain, "mbcscodepage");
    if (mbcscodepage)
    {
        *mbcscodepage = 936;
    }

    // IsTextUnicode没有我们的hook_IsTextW
    org_IsTextW  = (ISTEXTW)GetProcAddress(hMain, "_IstextW");
    hook(&(PVOID&)org_IsTextW, hook_IsTextW);

    // 暂时用不到了，因为_Isstring被hook了
    //org_Asciitounicode = (ASCIITOUNICODE)GetProcAddress(hMain, "_Asciitounicode");
    //hook(&(PVOID&)org_Asciitounicode, hook_Asciitounicode);

    //org_Utftounicode = (UTFTOUNICODE)GetProcAddress(hMain, "_Utftounicode");
    //hook(&(PVOID&)org_Utftounicode, hook_Utftounicode);

    org_Isstring = (ISSTRING)GetProcAddress(hMain, "_Isstring");
    hook(&(PVOID&)org_Isstring, hook_Isstring);

    hook(&(PVOID&)lea_check, lea_patch);

    /*
    004A195C   E8 DF1AF6FF      CALL ollydbg._Unicodetoutf
    搜索的时候却只有ASCII码和UNICODE码
    */
    hook(&(PVOID&)search_patchAddr, search_patch);
    hook(&(PVOID&)patch1, patch1_do);
    hook(&(PVOID&)patch2, patch2_do);
    return 0;
}

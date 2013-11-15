#include "OD2str.h"
#include "hook.h"
#include "StrFinder.h"
#include "../sdk/sdk.h"
#include "../common/define.h"

/*
shit��OD��leaָ�������lea eax,dword ptr[xxx]���xxx���ַ������ǲ���ȥ�ҵ�
�����ַ����������йأ�ÿ�ζ�̬���˴������ҪC-A*/

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


// from 52��̳�� �����������
static int IsSimplifiedCH(unsigned short &dch)
{
    unsigned char highbyte;
    unsigned char lowbyte;

    highbyte = dch & 0x00ff;
    lowbyte  = dch / 0x0100;

    if (highbyte==0 || lowbyte==0)
        return 0;

    //    ����    ��λ��Χ   ��λ��  �ַ��� �ַ�����
    // ˫�ֽ�2�� B0A1��F7FE   6768    6763    ����
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

    //    ����    ��λ��Χ   ��λ��  �ַ��� �ַ�����
    // ˫�ֽ�1�� A1A1��A9FE    846     718  ͼ�η���
    if (
        (highbyte >= 0xa1 && highbyte <= 0xa9) &&
        (lowbyte  >= 0xa1 && lowbyte  <= 0xfe)
        )
        nRet = 1;
    //    ����    ��λ��Χ   ��λ��  �ַ��� �ַ�����
    // ˫�ֽ�5�� A840��A9A0    192     166  ͼ�η���
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
    // ns�ĳ����ǻ���������
    int ret = MultiByteToWideChar(CP_ACP, 0, s, -1, w, nw);//org_Asciitounicode(s, ns, w, nw);
    return  ret - 1; // ���˸�NULL
}


/*
004A335D   8B45 C8               ||/MOV EAX,DWORD PTR SS:[EBP-0x38]
004A3360   F640 01 02            |||TEST BYTE PTR DS:[EAX+0x1],0x2        ; OP_CONST, lea��OP_MEMORY
004A3364   74 54                 |||JE SHORT ollydbg.004A33BA
�ڶ��к͵����ж��ǲ���hook��
*/


PVOID lea_check = (PVOID)HARDCODE(0x004A335D);

// ����Ҳ����Խ�һ�����Ƿ���lea����������ODʹ�õķ���������������disasm����һ�£�
// ��disasm.h��DX_LEA��0x03000000��������OD�з�����0x04000000����ʱ����
static void __declspec(naked) lea_patch()
{
    /*HARDCODE*/
    __asm
    {
        pushad;
        pushfd;

        // ������lea��OP_MEMORY��movҲ�У����������ٶ���Ӱ��
        //MOV EAX,DWORD PTR [EBP-0x1D1C]; // ȡָ��
        //AND EAX, 0xFF000000;
        //CMP EAX, 0x04000000;            // lea����disasm.h��һ��
        //JNE back;

        MOV EAX,DWORD PTR [EBP-0x38];
        TEST BYTE PTR [EAX+0x1],0x1;    // 1 == OP_MEORY
        je back;

        MOV BYTE PTR DS:[EAX+0x1],0x2; // ��OP_MEORY���OP_CONST������Ч��һ��
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

// ����һ���ж��ٸ�unicode
static int __cdecl hook_Isstring(ulong addr
                          ,int isstatic
                          ,wchar_t *symb // �����ַ
                          ,int nsymb)    // �����С
{
    int ret;
    // ԭ������unicode��֧�ֻ��ǱȽϺõģ�����ֻ����ascii�����
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
        if (GetStrW(enumSFST_Ascii, szBuf, symb, &len))
        {
            return lstrlenW(symb);
        }
        else
        {
            // �������
            ulong addr2 = *((ulong*)szBuf);
            pMem = Findmemory(addr2);
            if ((NULL == pMem) || ('\0' == pMem->sectname[0]))
                return ret;
            return org_Isstring(addr2, isstatic, symb, nsymb);
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
00487BE1   837D F0 00                 CMP DWORD PTR SS:[EBP-0x10],0x0          ; �׸��ֽ��Ƿ����ַ����Ǹ���Ӣ�����жϵģ�������������
00487BE5   0F84 89000000              JE ollydbg.00487C74

�����޷�patch��ֻ������������patch
00487C74   833D 38DD5700 00           CMP DWORD PTR DS:[0x57DD38],0x0
*/
static PVOID patch_unicode = (PVOID)HARDCODE(0x00487BE1);
static void __declspec(naked) patch_unicode_do()
{
    __asm
    {
        CMP DWORD PTR [EBP-0x10],0x0;
        jne back;
        MOV EAX,DWORD PTR [EBP-0x120];
        PUSH EAX;
        call hook_IsTextW;
        add esp, 4;
        cmp eax, 0;
        je back;
        MOV DWORD PTR [EBP-0x10],EAX
back:
        jmp patch_unicode;
    }
}
/*
00487C0C   8D4E 02          LEA ECX,DWORD PTR DS:[ESI+0x2]
00487C0F   3BF9             CMP EDI,ECX                              ; EDI���256��0x100
00487C11   7C 61            JL SHORT ollydbg.00487C74                ; ECX��ǰ0x102
*/
static PVOID patch_maxlen = (PVOID)HARDCODE(0x00487C0C);
static void __declspec(naked) patch_maxlen_do()
{
    __asm
    {
        LEA ECX,DWORD PTR [ESI+0x2];
        CMP EDI, ECX;
        JGE back;
        SUB ESI, 0x2
back:
        jmp patch_maxlen;
    }
}
/*
00487C1C   66:83BC35 E0FEFF>CMP WORD PTR SS:[EBP+ESI-0x120],0x0      ; ������ĩβ�Ƿ�Ϊ0
*/
static PVOID patch2_maxlen = (PVOID)HARDCODE(0x00487C1C);
static void __declspec(naked) patch2_maxlen_do()
{
    __asm
    {
        CMP EDI, ECX;
        JG back;
        ADD ESI, 0x2 // ��ԭ
back:
        jmp patch2_maxlen;
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

    // "Use IsTextUnicode"��δ��������OD��ʹ���ڲ���_IstextW�ж�unicode
    //*(DWORD*)HARDCODE(0x0057DD38) = 0;

    // Code page for ASCII dumps 
    *const_cast<int*>(&asciicodepage) = 936;
    // Code page for multibyte dumps��at 005BE6A0����������lib��û��
    mbcscodepage = (int*)GetProcAddress(hMain, "mbcscodepage");
    if (mbcscodepage)
    {
        *mbcscodepage = 936;
    }

    // IsTextUnicodeû�����ǵ�hook_IsTextW
    org_IsTextW  = (ISTEXTW)GetProcAddress(hMain, "_IstextW");
    hook(&(PVOID&)org_IsTextW, hook_IsTextW);

    // ���ַ���������ʾ���������������һ��
    org_Asciitounicode = (ASCIITOUNICODE)GetProcAddress(hMain, "_Asciitounicode");
    hook(&(PVOID&)org_Asciitounicode, hook_Asciitounicode);

    //org_Utftounicode = (UTFTOUNICODE)GetProcAddress(hMain, "_Utftounicode");
    //hook(&(PVOID&)org_Utftounicode, hook_Utftounicode);

    org_Isstring = (ISSTRING)GetProcAddress(hMain, "_Isstring");
    hook(&(PVOID&)org_Isstring, hook_Isstring);

    hook(&(PVOID&)lea_check, lea_patch);

    /*
    004A195C   E8 DF1AF6FF      CALL ollydbg._Unicodetoutf
    ������ʱ��ȴֻ��ASCII���UNICODE��
    */
    hook(&(PVOID&)search_patchAddr, search_patch);
    hook(&(PVOID&)patch_unicode, patch_unicode_do);
    hook(&(PVOID&)patch_maxlen, patch_maxlen_do);
    hook(&(PVOID&)patch2_maxlen, patch2_maxlen_do);
    return 0;
}

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

// ��__stdcallʡ����
static int __stdcall IsTextA_check(int a)
{
    return isSimGra(a) ? 2 : 0;
}

//0x004A335D
// �����ַ��ж���ʱ��֧�ַ���(������������Դ�롱)
static void __declspec(naked) __cdecl hook_IsTextA()
{
    __asm{
        mov eax, dword ptr[esp + 4];
        push eax;
        call org_IsTextA;
        add esp, 4;
        cmp eax, 1;
        je back;
        cmp dword ptr [esp], /*HARDCODE*/0x0048772C;
        je check;
        cmp dword ptr [esp], /*HARDCODE*/0x00487761;
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
    // ns�ĳ����ǻ���������
    int ret = MultiByteToWideChar(CP_ACP, 0, s, -1, w, nw);//org_Asciitounicode(s, ns, w, nw);
    return  ret - 1; // ���˸�NULL
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
004A3360   F640 01 02            |||TEST BYTE PTR DS:[EAX+0x1],0x2        ; OP_CONST, lea��OP_MEMORY
004A3364   74 54                 |||JE SHORT ollydbg.004A33BA
�ڶ��к͵����ж��ǲ���hook��
*/


PVOID lea_check = (PVOID)HARDCODE(0x004A335D);

// ����Ҳ����Խ�һ�����Ƿ���lea����������ODʹ�õķ���������������disasm����һ�£�
// ��disasm.h��DX_LEA��0x03000000��������OD�з�����0x04000000����ʱ����
void __declspec(naked) lea_patch()
{
    __asm
    {
        MOV EAX,DWORD PTR [EBP-0x38];
        TEST BYTE PTR [EAX+0x1],0x1; 
        je back;
        MOV BYTE PTR DS:[EAX+0x1],0x2 // ��OP_MEORY���OP_CONST������Ч��һ��
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

// ����һ���ж��ٸ�unicode
int __cdecl hook_Isstring(ulong addr
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
    }
    return ret;
}

static int* mbcscodepage = NULL;

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

    // ���ж�ascii���ĺܹؼ�
    //org_IsTextA  = (ISTEXTA)GetProcAddress(hMain, "_IstextA");
    //hook(&(PVOID&)org_IsTextA, hook_IsTextA);

    // IsTextUnicodeû�����ǵ�hook_IsTextW
    org_IsTextW  = (ISTEXTW)GetProcAddress(hMain, "_IstextW");
    hook(&(PVOID&)org_IsTextW, hook_IsTextW);

    // ����ý�����ʾ��ʱ������
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
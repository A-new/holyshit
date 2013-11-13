#include "OD2str.h"
#include "hook.h"
#include "StrFinder.h"

/*OD识别不了ascii中文，就按utf来处理*/
str_patch::str_patch( IConfigForStrpatch* i)
: m_IConfigForStrpatch(i)
{

}
typedef int (__cdecl* ISTEXTA)(int);
typedef int (__cdecl* ISTEXTW)(int);
typedef int (__cdecl* ASCIITOUNICODE)(const char *s,int ns,wchar_t *w,int nw);
typedef int (__cdecl* UTFTOUNICODE)(const char *t,int nt,wchar_t *w,int nw);
static ISTEXTA org_IsTextA = NULL;
static ISTEXTW org_IsTextW = NULL;
static ASCIITOUNICODE org_Asciitounicode = NULL;
static UTFTOUNICODE org_Utftounicode = NULL;
static int __cdecl hook_IsTextA(int a)
{
    int ret = org_IsTextA(a);
    if (0 == ret)
    {
        return ret;
        //CStrFinder::IsGraphicCH()
    }
    return ret;
}

static int __cdecl hook_IsTextW(int a)
{
    int ret = org_IsTextW(a);
    if (ret == 0)
    {
        if (CStrFinder::IsGraphicCH(*(USHORT *)&a) 
            ||
            CStrFinder::IsSimplifiedCH(*(USHORT *)&a))
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

int str_patch::ODBG2_Plugininit( void )
{
    static bool pathed =  false;
    if (!pathed)
    {
        pathed = true;

        HMODULE hMain = GetModuleHandle(NULL);
        org_IsTextA  = (ISTEXTA)GetProcAddress(hMain, "_IstextA");
        hook(&(PVOID&)org_IsTextA, hook_IsTextA);

        org_IsTextW  = (ISTEXTW)GetProcAddress(hMain, "_IstextW");
        hook(&(PVOID&)org_IsTextW, hook_IsTextW);

        org_Asciitounicode = (ASCIITOUNICODE)GetProcAddress(hMain, "_Asciitounicode");
        hook(&(PVOID&)org_Asciitounicode, hook_Asciitounicode);

        org_Utftounicode = (UTFTOUNICODE)GetProcAddress(hMain, "_Utftounicode");
        hook(&(PVOID&)org_Utftounicode, hook_Utftounicode);
    }
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   StrFinder.h
//  Version     :   1.0
//  Creater     :   Luo Cong
//  Date        :   2006-7-28 10:31:26
//  Comment     :   
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __STR_FINDER_H__
#define __STR_FINDER_H__

#ifndef PROCESS_ERROR
#define PROCESS_ERROR(Condition) if (!(Condition)) goto Exit0
#endif

#include <windows.h>
//#include "define.h"

typedef enum tagSTR_FINDER_STRING_TYPE
{
    enumSFST_Ascii      = 1,
    enumSFST_Unicode    = 2,
    enumSFST_Auto       = 1 | 2,
    enumSFST_Msg        = 4 // 从g_szStrMsg取
} STR_FINDER_STRING_TYPE;

typedef void STR_FINDER_CALLBACK(
    const int nStrIndex,
    const DWORD dwBase,
    const DWORD dwOffset,
    const DWORD dwSize,
    const STR_FINDER_STRING_TYPE StrType,
    const char* // 改为字符串地址，就不需要内存占用了
);

static const char *g_szStrFinderStrType[] =
{
    "ASCII",
    "UNICODE",
    "AUTO"
};

static const TCHAR *g_szStrMsg[] =
{
    TEXT("(Initial CPU selection)"),    
};

class CStrFinder
{
public:
    CStrFinder();
    ~CStrFinder();

    BOOL Find(
        DWORD dwCurEip,
        const STR_FINDER_STRING_TYPE StrType,
        DWORD dwBase,
        DWORD dwSize,
        STR_FINDER_CALLBACK *pCallBack
    );

    // static供str_patch使用
public:
    static BOOL IsPrintAble(const BYTE ch);
    static BOOL IsAlpha(const BYTE ch);
    static BOOL IsSimplifiedCH(const USHORT dch);
    static BOOL IsGraphicCH(const USHORT dch);
    static BOOL GetStr(
        const STR_FINDER_STRING_TYPE StrType,
        const BYTE *pbyBuf,
        int *pnLen
        );

    static BOOL FollowImmediateAddr(
        const STR_FINDER_STRING_TYPE StrType,
        DWORD dwImmAddr,
        BOOL *pbFound,
        int& StrTypeGot
        );

};

#endif  // __STR_FINDER_H__
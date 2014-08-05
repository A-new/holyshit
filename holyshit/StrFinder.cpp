////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   StrFinder.cpp
//  Version     :   1.0
//  Creater     :   Luo Cong
//  Date        :   2006-7-28 10:31:48
//  Comment     :   
//
////////////////////////////////////////////////////////////////////////////////

#include "StrFinder.h"
#include <assert.h>
#include "../sdk/sdk.h"



#ifndef ASSERT
#define ASSERT assert
#endif

#define defINITCPUSEL           _T("(Initial CPU selection)")
#define defINIKEY               "Restore UStrRef Window"
#define defPLUGINNAME           "Ultra String Reference Plugin"
#define defPASSEDTHEENDOFFILE   _T("Passed the end of file")

CStrFinder::CStrFinder()
{
}

CStrFinder::~CStrFinder()
{
}

BOOL CStrFinder::IsPrintAble(const BYTE ch)
{
    BOOL bRet;

    if (ch >= 0x20 && ch < 0x7f)
        bRet = TRUE;
    else if (0xff == ch)
        bRet = FALSE;
    else if (0x80 == (ch & 0x80))
        bRet = TRUE;
    else
        bRet = FALSE;

    return bRet;
}

BOOL CStrFinder::IsAlpha(const BYTE ch)
{
    if (ch >= 0x20 && ch <= 0x7f)
        return TRUE;
    else
        return FALSE;
}

BOOL CStrFinder::IsSimplifiedCH(const USHORT dch)
{
    BYTE highbyte;
    BYTE lowbyte;

    highbyte = dch & 0x00ff;
    lowbyte  = dch / 0x0100;

    //    区名    码位范围   码位数  字符数 字符类型
    // 双字节2区 B0A1—F7FE   6768    6763    汉字
    if (
        (highbyte >= 0xb0 && highbyte <= 0xf7) &&
        (lowbyte  >= 0xa1 && lowbyte  <= 0xfe)
    )
        return TRUE;
    else
        return FALSE;
}

BOOL CStrFinder::IsGraphicCH(const USHORT dch)
{
    BOOL bRet;
    BYTE highbyte;
    BYTE lowbyte;

    highbyte = dch & 0x00ff;
    lowbyte  = dch / 0x0100;

    //    区名    码位范围   码位数  字符数 字符类型
    // 双字节1区 A1A1—A9FE    846     718  图形符号
    if (
        (highbyte >= 0xa1 && highbyte <= 0xa9) &&
        (lowbyte  >= 0xa1 && lowbyte  <= 0xfe)
    )
        bRet = TRUE;
    //    区名    码位范围   码位数  字符数 字符类型
    // 双字节5区 A840—A9A0    192     166  图形符号
    else if (
        (highbyte >= 0xa8 && highbyte <= 0xa9) &&
        (lowbyte  >= 0x40 && lowbyte  <= 0xa0)
    )
        bRet = TRUE;
    else
        bRet = FALSE;

    return bRet;
}

BOOL CStrFinder::GetStr(
    const STR_FINDER_STRING_TYPE StrType,
    const BYTE *pbyBuf,
    int *pnLen
)
{
    BOOL bFound = TRUE;

    int i = 0;
    int nLen = 0;

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
            nLen += 2;
        }
        else if ('\n' == pbyBuf[i])
        {
            nLen += 2;
        }
        else if ('\t' == pbyBuf[i])
        {
            nLen += 2;
        }
        else if (IsAlpha(pbyBuf[i]))
        {
            ++nLen;
        }
        else if (IsGraphicCH(
            *(USHORT *)&pbyBuf[i]) ||
            IsSimplifiedCH(*(USHORT *)&pbyBuf[i])
        )
        {
            ++i;
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

    if (pnLen)
        *pnLen = nLen;

    return bFound;
}

BOOL CStrFinder::FollowImmediateAddr(
    const STR_FINDER_STRING_TYPE StrType,
    DWORD dwImmAddr,
    BOOL *pbFound,
    int& StrTypeGot
)
{
    BOOL bRetResult = FALSE;
    int nRetCode;

    t_memory *pMem;
    BYTE szBuf[TEXTLEN];
    int nStrLen;

    ASSERT(pbFound);
    *pbFound = FALSE;

    pMem = Findmemory(dwImmAddr);
#ifndef OD1_EXPORTS
    PROCESS_ERROR(pMem && pMem->sectname[0]);
#else
    PROCESS_ERROR(pMem && pMem->sect[0]);
#endif

    nRetCode = Readmemory(
        &dwImmAddr, dwImmAddr, sizeof(dwImmAddr), MM_RESTORE | MM_SILENT
    );
    PROCESS_ERROR(nRetCode);

    pMem = Findmemory(dwImmAddr);
#ifndef OD1_EXPORTS
    PROCESS_ERROR(pMem && pMem->sectname[0]);
#else
    PROCESS_ERROR(pMem && pMem->sect[0]);
#endif

    nRetCode = Readmemory(
        szBuf, dwImmAddr, sizeof(szBuf), MM_RESTORE | MM_SILENT
    );
    PROCESS_ERROR(nRetCode);

    if(StrType & enumSFST_Ascii)
    {
        *pbFound = GetStr(enumSFST_Ascii, szBuf, &nStrLen);
        if(*pbFound) 
        {   
            StrTypeGot = enumSFST_Ascii;
        }
    }
    if((FALSE == *pbFound)
        && (StrType & enumSFST_Unicode))
    {
        *pbFound = GetStr(enumSFST_Unicode, szBuf, &nStrLen);
        if(*pbFound) 
        {
            StrTypeGot = enumSFST_Unicode;
        }
    }

    bRetResult = TRUE;
Exit0:
    return bRetResult;
}

BOOL CStrFinder::Find(
    DWORD dwCurEip,
    const STR_FINDER_STRING_TYPE StrType,
    DWORD dwBase,
    DWORD dwSize,
    STR_FINDER_CALLBACK *pCallBack
)
{
    BOOL bRetResult = FALSE;
    int nRetCode;

    int i;
    DWORD dwOffset;
    DWORD dwAddr;
    BYTE szCmd[MAXCMDSIZE];
    BYTE szBuf[TEXTLEN];
    DWORD dwCmdSize;
    t_disasm da;
    t_memory *pMem;
    int nStrIndex = 0;
    //BYTE pszStr[TEXTLEN * 2];
    const BYTE* pszStr;
    int nStrLen;
    DWORD dwImmAddr;
    BOOL bFound;

    ASSERT(pCallBack);

    for (dwOffset = 0; dwOffset < dwSize; dwOffset += dwCmdSize)
    {
        dwAddr = dwBase + dwOffset;

#ifndef OD1_EXPORTS
        nRetCode = Readmemory(szCmd, dwAddr, MAXCMDSIZE, MM_SILENT);
        PROCESS_ERROR(nRetCode);
        dwCmdSize = Disasm(
            szCmd, MAXCMDSIZE, dwAddr, NULL, &da, DA_TEXT|DA_OPCOMM|DA_MEMORY, 0, 0
            );
#else
        nRetCode = Readcommand(dwAddr, (char *)szCmd);
        PROCESS_ERROR(nRetCode);
        dwCmdSize = Disasm(
            szCmd, MAXCMDSIZE, dwAddr, NULL, &da, DISASM_CODE, 0
            );
#endif

        if (dwCurEip == dwAddr)
        {
            pCallBack(nStrIndex++, dwBase, dwOffset, dwSize, enumSFST_Msg, (const char*)0);
            continue;
        }

        if (
            (0 != _tcsncicmp(da.result, _T("push"), 4)) &&
            (0 != _tcsncicmp(da.result, _T("mov"), 3)) &&
            (0 != _tcsncicmp(da.result, _T("lea"), 3))
        )
            continue;

#ifndef OD1_EXPORTS
        ulong  immconst = da.memconst;
#else
        ulong  immconst = da.immconst
#endif

        pMem = Findmemory(immconst);
        if ((NULL == pMem) || 
#ifndef OD1_EXPORTS
            ('\0' == pMem->sectname[0]))
#else
            ('\0' == pMem->sect[0]))
#endif
            continue;

        int StrTypeGot;
        for (
            dwImmAddr = immconst, bFound = TRUE, i = 0;
            bFound && i < 8;    // loop 8 times at most!
            dwImmAddr += sizeof(DWORD), ++i
        )
        {
            
            nRetCode = FollowImmediateAddr(StrType, dwImmAddr, &bFound, StrTypeGot);
            if (!nRetCode)
                break;

            if (bFound)
                pCallBack(nStrIndex++, dwBase, dwOffset, dwSize, static_cast<STR_FINDER_STRING_TYPE>(StrTypeGot), (const char *)pszStr);
        }

        nRetCode = Readmemory(
            szBuf, immconst, sizeof(szBuf), MM_RESTORE | MM_SILENT
        );
        PROCESS_ERROR(nRetCode);

        if(StrType & enumSFST_Ascii)
        {
            bFound = GetStr(enumSFST_Ascii, szBuf,  &nStrLen);
            if(bFound) StrTypeGot = enumSFST_Ascii;
        }
        if((FALSE == bFound)
            && (StrType & enumSFST_Unicode))
        {
            bFound = GetStr(enumSFST_Unicode, szBuf, &nStrLen);
            if(bFound) StrTypeGot = enumSFST_Unicode;
        }
        //bFound = GetStr(StrType, szBuf, &pszStr, &nStrLen);
        if (bFound)
            pCallBack(nStrIndex++, dwBase, dwOffset, dwSize, static_cast<STR_FINDER_STRING_TYPE>(StrTypeGot), (char *)pszStr);
    }

    bRetResult = TRUE;
Exit0:
    return bRetResult;
}
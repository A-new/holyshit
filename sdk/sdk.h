#pragma once
// 项目注意，需要在工程属性里设置"结构成员对齐"为1，否则使用类静态函数可能出问题


#include <windows.h>
#include <tchar.h>

#ifdef HOLYSHIT_EXPORTS // od110
#include "plugin110.h"
#ifdef _UNICODE
#error "only support ansi"
#endif // _UNICODE
#else
#include "plugin201.h"
#ifndef _UNICODE
#error "only support unicode"
#endif // _UNICODE
#endif // HOLYSHIT_EXPORTS


t_dump *sdk_Getcpudisasmdump();
t_dump * sdk_Getmodulesdump();
#ifndef HOLYSHIT_EXPORTS // od2
void cdecl Getdisassemblerrange(ulong *pbase,ulong *psize);
#define Findname(addr, type, image1) FindnameW(addr, type, image1, TEXTLEN)
typedef t_sorthdr t_sortheader;
#define BAR_NOSORT 0
#define MM_RESTORE 0
#define Browsefilename(t, n, d, m) Browsefilename(t, n, 0, 0, d, hwollymain, m)
#else // od1
#define DRAW_VARWIDTH 0
#endif //end HOLYSHIT_EXPORTS


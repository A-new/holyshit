#pragma once

// ������Ҫ��������ͷ�ļ������ֻ�к�

#define PLUGIN_NAME TEXT("holyshit")
namespace std
{
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
}
#pragma once

// 尽量不要包含其他头文件，最好只有宏

#define PLUGIN_NAME TEXT("holyshit")
namespace std
{
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
}
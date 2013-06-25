#pragma once
#include <windows.h>
#define DEFAULT_WIDTH_LABEL 100
#define DEFAULT_WIDTH_COMMENT 200
#define WIDTH_LABEL TEXT("width_label")
#define WIDTH_COMMENT TEXT("width_comment")
#define INI_PATH TEXT("toolbar_ini")
#define PLUGIN_NAME TEXT("holyshit")

void hook_DRAWFUNC_cpudasm();
int get_width_label();
int get_width_comment();
void set_width_label(int);
void set_width_comment(int);

//void hook_CreateProcessInternalW();
void hook_DllCheck();

// ÎªODÌí¼ÓÓ²¼þ
void hook_0040869C();
void hook_00439191();

void hook_Sethardwarebreakpoint();
void hook( PVOID *ppPointer, PVOID pDetour );



#include "config.h"
#include "define.h"
#include "../sdk/sdk.h"
#include "toolbar.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

bool CConfig::label_mix_comment()
{
    return false;
}

#define ENABLE_JMP TEXT("enable_jmp")

void CConfig::loadall()
{
    // label相关初始化
    width_label = get_int((TCHAR*)m_ILabelForConfig->WIDTH_LABEL(), m_ILabelForConfig->DEFAULT_WIDTH_LABEL());
    width_comment = get_int((TCHAR*)m_ILabelForConfig->WIDTH_COMMENT(), m_ILabelForConfig->DEFAULT_WIDTH_COMMENT());
    label_enabled_ = get_int((TCHAR*)m_ILabelForConfig->LABEL_ENABLE(), 1) != 0;

#ifdef HOLYSHIT_EXPORTS
    enable_jmp = get_int(ENABLE_JMP, 0) != 0;
#else
    enable_jmp = get_int(ENABLE_JMP, 1) != 0;
#endif

    //if (!label_enabled_)
    //{
    //    check_.push_back(NM_COMMENT);
    //}
    check_.push_back(NM_LABEL);
    check_.push_back(NM_EXPORT);

    TCHAR szTB[MAX_PATH];
    get_str(INI_PATH, szTB, TEXT(""));
    ini_path = TEXT("");
    ini_path = szTB;

    // 尝试在holyshitxxx.dll同级目录找toolbar.ini
    if (!PathFileExists(ini_path.c_str()))
    {
        TCHAR szToolBarTest[MAX_PATH];
        GetModuleFileName(plugin_mod, szToolBarTest, MAX_PATH);
        PathRemoveFileSpec(szToolBarTest);
        PathAppend(szToolBarTest, TEXT("toolbar"));
        PathAppend(szToolBarTest, TEXT("toolbar.ini"));
        if(PathFileExists(szToolBarTest))
            ini_path = szToolBarTest;
    }
}

void CConfig::saveall(bool now)
{
    if (label_enabled())
    {
        if (now)
        {
            width_label = m_ILabelForConfig->get_width_label_now();
            width_comment = m_ILabelForConfig->get_width_comment_now();
        }

        if(width_label && width_comment)
        {
            set_int((TCHAR*)m_ILabelForConfig->WIDTH_LABEL(), width_label);
            set_int((TCHAR*)m_ILabelForConfig->WIDTH_COMMENT(), width_comment);
        }
    }
    set_int((TCHAR*)m_ILabelForConfig->LABEL_ENABLE(), label_enabled_);
    set_int(ENABLE_JMP, enable_jmp);
    set_str(INI_PATH, (LPTSTR)ini_path.c_str());
}


int CConfig::get_int( TCHAR* key, int def )
{
#ifdef HOLYSHIT_EXPORTS
    return Pluginreadintfromini(plugin_mod, key, def);
#else
    int ret = def;
    Getfromini(NULL, PLUGIN_NAME, key, L"%i", &ret);
    return ret;
#endif
}

int CConfig::set_int( TCHAR* key, int value )
{
#ifdef HOLYSHIT_EXPORTS
    return Pluginwriteinttoini(plugin_mod, key, value);
#else
    return Writetoini(NULL, PLUGIN_NAME, key, L"%i", value);
#endif
}

int CConfig::get_str( TCHAR* key, LPTSTR str_buf, LPTSTR def )
{
#ifdef HOLYSHIT_EXPORTS
    return Pluginreadstringfromini(plugin_mod, key, str_buf, def);
#else
    return Getfromini(NULL, PLUGIN_NAME, key, L"%s", str_buf);
#endif
}

int CConfig::set_str( TCHAR* key, LPTSTR str )
{
#ifdef HOLYSHIT_EXPORTS
    return Pluginwritestringtoini(plugin_mod, key, str);
#else
    return Writetoini(NULL, PLUGIN_NAME, key, L"%s", str);
#endif
}

CConfig& CConfig::getInstance()
{

    static CConfig b;
    return b;
}

void CConfig::set_mod( HMODULE mod )
{plugin_mod = mod;}

bool CConfig::label_enabled() const{return label_enabled_;}

const std::vector<int>& CConfig::check() const
{return check_;}

int CConfig::get_width_label() const
{return width_label;}

int CConfig::get_width_comment() const
{return width_comment;}

void CConfig::set_width_label( int i )
{width_label = i;}

void CConfig::set_width_comment( int i )
{width_comment = i;}

std::tstring CConfig::get_ini_path()
{return ini_path;}

void CConfig::set_ini_path( const std::tstring& path )
{ ini_path = path;}


//int CConfig::_ODBG_Plugininit( int ollydbgversion,HWND hw, ulong *features )
//{
//    
//}

void CConfig::_ODBG_Plugindestroy( void )
{
    saveall(true);
}

void CConfig::ODBG2_Plugindestroy( void )
{
    saveall(true);
}




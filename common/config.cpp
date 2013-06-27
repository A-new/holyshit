#include "config.h"
#include "define.h"
#include "../sdk/sdk.h"
#include "toolbar.h"
#include "label.h"

bool CConfig::label_mix_comment()
{
    return false;
}

void CConfig::loadall()
{
    // label相关初始化
    width_label = get_int(WIDTH_LABEL, DEFAULT_WIDTH_LABEL);
    width_comment = get_int(WIDTH_COMMENT, DEFAULT_WIDTH_COMMENT);
    label_enabled_ = get_int(LABEL_ENABLE, 1);
    //if (!label_enabled_)
    //{
    //    check_.push_back(NM_COMMENT);
    //}
    check_.push_back(NM_LABEL);
    check_.push_back(NM_EXPORT);

    TCHAR szTB[MAX_PATH];
    CConfig_Single.get_str(INI_PATH, szTB, TEXT(""));
    ini_path = TEXT("");
    ini_path = szTB;
}

void CConfig::saveall(bool now)
{
    if (label_enabled())
    {
        if (now)
        {
            width_label = get_width_label_now();
            width_comment = get_width_comment_now();
        }
        set_int(WIDTH_LABEL, width_label);
        set_int(WIDTH_COMMENT, width_comment);
    }
    set_int(LABEL_ENABLE, label_enabled_);
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





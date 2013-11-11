#pragma once
#include <windows.h>
#include "define.h"
#include <string>
#include <vector>
#include "label.h"

#define CConfig_Single (CConfig::getInstance())

class CConfig : public IConfigForLabel
{
public:
    static CConfig& getInstance();

    void setILabelForConfig(ILabelForConfig* i){m_ILabelForConfig = i;}

    void loadall();
    void saveall(bool now = false);

    // od1才用
    void set_mod(HMODULE mod);

    // label
    virtual bool label_enabled() const;

    bool jmp_enabled(){return enable_jmp;}

    const std::vector<int>& check() const;
    // label.cpp
    int get_width_label();
    int get_width_comment();
    void set_width_label(int i);
    void set_width_comment(int i);

    bool label_mix_comment();
    void label_mix_comment_set(bool add);

    // toolbar
    std::tstring get_ini_path();
    void set_ini_path(const std::tstring& path);

protected:
    int get_int(TCHAR* key, int def);
    int set_int(TCHAR* key, int value);

    int get_str(TCHAR* key, LPTSTR str_buf, LPTSTR def);
    int set_str(TCHAR* key, LPTSTR str);

private:
    HMODULE plugin_mod;

    bool label_enabled_;
    int width_label;
    int width_comment;
    std::vector<int> check_;

    bool enable_jmp; // OD1默认不开启
    
    std::tstring ini_path;

    ILabelForConfig* m_ILabelForConfig;
};
#pragma once
#include <windows.h>
#include "define.h"
#include <string>
#include <vector>

#define CConfig_Single (CConfig::getInstance())

class CConfig
{
public:
    static CConfig& getInstance();

    void loadall();
    void saveall(bool now = false);

    // od1≤≈”√
    void set_mod(HMODULE mod);

    // label
    bool label_enabled();

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
    
    std::tstring ini_path;

};
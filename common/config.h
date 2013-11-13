#pragma once
#include <windows.h>
#include "define.h"
#include <string>
#include <vector>
#include "label.h"
#include "jmpstack.h"
#include "toolbar.h"
#include "OD2str.h"

//#define CConfig_Single (CConfig::getInstance())

class CConfig 
    : public IPlugin110
    , public IPlugin201
    , public IConfigForLabel
    , public IConfigForJmpStack
    , public IConfigForToolbar
    , public IConfigForStrpatch
{
public:
    static CConfig& getInstance();

    // IPlugin110
    //int  _ODBG_Plugininit(int ollydbgversion,HWND hw,
    //        ulong *features); // 由于牵涉太多，必须放Plugin110.cpp里写
    void _ODBG_Plugindestroy(void);

    // IPlugin201
    void ODBG2_Plugindestroy(void);

    void setILabelForConfig(ILabelForConfig* i){m_ILabelForConfig = i;}

    void loadall();
    void saveall(bool now = false);

    // 
    void set_mod(HMODULE mod);

    // label
    virtual bool label_enabled() const;
    virtual const std::vector<int>& check() const;

    // jmpstack
    virtual bool jmp_enabled(){return enable_jmp;}

    // toolbar
    virtual std::tstring get_ini_path();

    virtual int get_width_label() const;
    virtual int get_width_comment() const;
    void set_width_label(int i);
    void set_width_comment(int i);

    bool label_mix_comment();
    void label_mix_comment_set(bool add);


    void set_ini_path(const std::tstring& path);

    virtual bool patch_str(){return true;}
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
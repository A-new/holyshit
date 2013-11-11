#pragma once

#include "IPlugin110.h"
#include "IPlugin201.h"

class IConfigForLabel
{
public:
    virtual bool label_enabled() const  = 0;
};

class ILabelForConfig
{
public:
    virtual int DEFAULT_WIDTH_LABEL() const = 0;
    virtual int DEFAULT_WIDTH_COMMENT() const = 0;
    virtual LPCTSTR WIDTH_LABEL() const = 0;
    virtual LPCTSTR WIDTH_COMMENT() const = 0;
    virtual LPCTSTR LABEL_ENABLE() const = 0;

    virtual int get_width_label_now() const = 0;
    virtual int get_width_comment_now() const = 0;
};

#include "IPlugin110.h"
#include "IPlugin201.h"

class Label 
    : public IPlugin110
    , public IPlugin201
    , public ILabelForConfig
{
public:
    Label(const IConfigForLabel* config);

    // IPlugin110
    virtual void  _ODBG_Pluginmainloop(DEBUG_EVENT *debugevent);

    // IPlugin201
    virtual int ODBG2_Plugininit(void);
    virtual void ODBG2_Pluginmainloop(DEBUG_EVENT *debugevent);

    // ILabelForConfig
    virtual int DEFAULT_WIDTH_LABEL() const;
    virtual int DEFAULT_WIDTH_COMMENT() const;
    virtual LPCTSTR WIDTH_LABEL() const;
    virtual LPCTSTR WIDTH_COMMENT() const;
    virtual LPCTSTR LABEL_ENABLE() const;
    virtual int get_width_label_now() const;
    virtual int get_width_comment_now() const;

protected:
    void hook_label_functions();
private:
    const IConfigForLabel* m_config;
};
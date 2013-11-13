#pragma once
#include "IPlugin201.h"

class IConfigForStrpatch
{
public:
    virtual bool patch_str() = 0;
};

class str_patch
    : public IPlugin201
{
public:
    str_patch(IConfigForStrpatch*);

    virtual int ODBG2_Plugininit(void);
protected:
private:
    IConfigForStrpatch* m_IConfigForStrpatch;
};
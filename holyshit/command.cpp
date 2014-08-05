#include "command.h"

void CCommand::Register(const std::string& cmd, CallBack func)
{
    func_list[cmd] = func;
}

void CCommand::UnRegister( const std::string& cmd )
{
    func_list.erase(cmd);
}

void CCommand::Invoke( const std::string& cmd , int status) const
{
    ARG_LIST_PTR p = Command::command_pareser(cmd);
    if (!p->empty())
    {
        MAP_CALLBACK::const_iterator ci = func_list.find(*(p->begin()));
        if (ci != func_list.end())
        {
            p->pop_front();
            //p->erase(p->begin());
            ci->second(p);
        }
    }
}

CCommand& CCommand::get_Instance()
{
    static CCommand a;
    return a;
}

#include<boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
ARG_LIST_PTR Command::command_pareser( const std::string& str )
{
    ARG_LIST_PTR a;
    a.reset(new ARG_LIST());

    using namespace std;
    using namespace boost;
    //string s = "Field 1,\"putting quotes around fields, allows commas\",Field 3"; // 据说不会把"中间的豆号截断
    tokenizer<escaped_list_separator<char> > tok(str);
    for(tokenizer<escaped_list_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
        a->push_back(boost::trim_copy(*beg)); // 空格也会保留下来,如call:explorer,1,,2,会得到5个值(2后面还有一个空值)
    }
    return a;
}

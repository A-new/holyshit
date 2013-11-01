#pragma once
#include <boost/function.hpp>
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <deque>
/*
调用：
call:explorer，打开
call:enable_hilight
call:plugin_cmd,中文搜索,2
*/

#define CCommand_Single (CCommand::get_Instance())

// 参数列表
typedef std::deque<std::string> ARG_LIST;
typedef boost::shared_ptr<ARG_LIST> ARG_LIST_PTR;

class CCommand
{
public:
    static CCommand& get_Instance();

    // 回调原型void xx(int istatus)
    typedef boost::function<void (ARG_LIST_PTR)> CallBack;

    void Register(const std::string&, CallBack func);
    void UnRegister(const std::string& cmd);

    // 原封不动地把命令传过来，然后将cmd传给command_pareser
    void Invoke(const std::string& cmd , int status) const;// istatus都没用了，直接解析command

protected:
    CCommand(){}
    ~CCommand(){}

private:
    typedef std::map<std::string, CallBack> MAP_CALLBACK;
    MAP_CALLBACK func_list;
};


namespace Command
{
    ARG_LIST_PTR command_pareser(const std::string& str);
}
#pragma once
#include <boost/function.hpp>
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <deque>
/*
���ã�
call:explorer����
call:enable_hilight
call:plugin_cmd,��������,2
*/

#define CCommand_Single (CCommand::get_Instance())

// �����б�
typedef std::deque<std::string> ARG_LIST;
typedef boost::shared_ptr<ARG_LIST> ARG_LIST_PTR;

class CCommand
{
public:
    static CCommand& get_Instance();

    // �ص�ԭ��void xx(int istatus)
    typedef boost::function<void (ARG_LIST_PTR)> CallBack;

    void Register(const std::string&, CallBack func);
    void UnRegister(const std::string& cmd);

    // ԭ�ⲻ���ذ����������Ȼ��cmd����command_pareser
    void Invoke(const std::string& cmd , int status) const;// istatus��û���ˣ�ֱ�ӽ���command

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
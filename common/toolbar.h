#pragma once
#include <vector>
#include <map>
#include <string>
#include <windows.h>
#include <boost/tuple/tuple.hpp>

#define INI_PATH TEXT("toolbar_ini")

#pragma pack(1) // fuck!��ΪOD SDKǿ��ʹ����pack������Ӿ䣬��������ʹ����CConfig_Singleȥ�ط�����plugin110.cpp��Ĳ�һ����
// ������㲻�ܷ��ֵ����⣬��stringĪ�����������release�������warning C4742���棬ǧ��Ҫ���ԣ�

struct TOOLBAR_ITEM
{
    // handle, cmd
    typedef boost::tuple<HBITMAP, std::string> HADLE_CMD;
    std::vector<HADLE_CMD> data;

    // ���������
    size_t iStatus;
    int x; 

    TOOLBAR_ITEM() : iStatus(0){}
};

class CToolbar
{
public:
    static CToolbar& getInstance()
    {
        static CToolbar a;
        return a;
    }
    size_t init(const std::string& ini_path);
    void attach(HWND hWnd);

    std::vector<boost::tuple<LONG, LONG>> rect_calc(const LPRECT);
protected:
    TOOLBAR_ITEM get(size_t index);
    std::vector<TOOLBAR_ITEM>::iterator at(int x, int y);
    void draw(HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        bool lbtn_up = false);
    void destory();
    CToolbar();
    ~CToolbar();
    static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    void draw_internal(HDC hDC, int x, HBITMAP handle, HPEN hpen1, HPEN hpen2);
    void OnLeftButtonUp(LPARAM lParam);
private:
    std::vector<TOOLBAR_ITEM> m_bmp;
    static WNDPROC m_prevProc;
    HWND m_hWnd;
    HPEN m_pen;
    HPEN m_penWhite;
    HPEN m_penBlack;
};


#define CToolbar_Global (CToolbar::getInstance())


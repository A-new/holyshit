#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <boost/tuple/tuple.hpp>

struct BMP_DATA
{
    // from ini
    std::string name;
    std::string command; // 内置+shell命令

    // 计算出来的
    int x; 
    HBITMAP handle;
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
    BMP_DATA get(size_t index);
    std::vector<BMP_DATA>::const_iterator at(int x, int y);
    void draw(HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        bool lbtn_up = false);
    void destory();
    ~CToolbar();
    static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    void draw_internal(HDC hDC, int x, HBITMAP handle, HPEN hpen1, HPEN hpen2);
    void OnLeftButtonUp(LPARAM lParam);
private:
    std::vector<BMP_DATA> m_bmp;
    static WNDPROC m_prevProc;
    HWND m_hWnd;
    HPEN m_pen;
    HPEN m_penWhite;
    HPEN m_penBlack;
};


#define CToolbar_Global (CToolbar::getInstance())


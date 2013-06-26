#include "label.h"
#include "../sdk/sdk.h"
#include "hook.h"
#include <boost/thread/mutex.hpp>

DRAWFUNC *g_func;
int width_label;
int width_comment;


#define COLUMN_LABELS 4
int DRAWFUNC_cpudasm( TCHAR *s, uchar *mask, int *select, t_table *pt, t_sortheader *ps,int column, void * cache )
{
    if (column == COLUMN_LABELS)
    {
        t_dump *p = sdk_Getcpudisasmdump();
        ulong addr = 0;

#ifdef HOLYSHIT_EXPORTS
        addr = p->lastaddr;
#else
        addr = p->addr;
        addr += *((ulong*)((char*)cache + 0x20)); // fuck! cache是自定义结构，但0x20应该不会变化太大
        //t_drawheader *td = (t_drawheader*)ps;
        //addr = td->nextaddr; // 下一行的地址,shit
#endif

        if (addr)
        {
            int len = Findname(addr, NM_LABEL, s);
            if (len)
            {
                memset(mask, DRAW_HILITE, len);
                *select = DRAW_MASK | DRAW_VARWIDTH;
            }
            return len;
        }
        return 0;
    }
    else
    {
#ifdef HOLYSHIT_EXPORTS
        return g_func(s, (char*)mask, select, ps, column);
#else
        return g_func(s, mask, select, pt, ps, column, cache);
#endif
    }
}

#ifdef HOLYSHIT_EXPORTS
int MyDRAWFUNC(char *s,char *mask,int *select,t_sortheader *ps,int column)//(char *,char *,int *,t_sortheader *,int)
{
    return DRAWFUNC_cpudasm(s, (uchar*)mask, select, 0, ps, column, 0);
}
#endif

//typedef void    (cdecl *DEFAULTBAR)(t_bar *pb);
//DEFAULTBAR OrgDefaultbar;
//void cdecl MyDefaultbar(t_bar *pb)
//{
//    OrgDefaultbar(pb);
//}
//void hook_Defaultbar()
//{
//    HMODULE hMod = GetModuleHandle(NULL);
//    //int temp = (int)&;
//    OrgDefaultbar = (DEFAULTBAR)GetProcAddress(hMod, "_Defaultbar");
//    hook(&(PVOID&)OrgDefaultbar, MyDefaultbar);
//}

void hook_label_functions()
{
    static boost::mutex mu;
    static bool hooked = false;
    if (!hooked)
    {
        boost::mutex::scoped_lock lLock(mu);
        if (!hooked)
        {
            t_dump *td = sdk_Getcpudisasmdump();
            t_table *p = &td->table;
            if (p && p->hw)
            {
                t_bar* tb = &p->bar;
                tb->nbar = 5;

                tb->name[COLUMN_LABELS] = _T("Label");
                tb->mode[COLUMN_LABELS] = BAR_NOSORT;

                int size_font = tb->dx[2] / tb->defdx[2];
                tb->defdx[COLUMN_LABELS] = width_label / size_font; // 字符串长度，需要重新计算，否则后面调用Defaultbar会显示不正常
                tb->dx[COLUMN_LABELS] = width_label;

                tb->defdx[3] = width_comment / size_font;
                tb->dx[3] = width_comment;

                g_func = p->drawfunc;

#ifdef HOLYSHIT_EXPORTS
                hook(&(PVOID&)g_func, MyDRAWFUNC);
#else // od2
                hook(&(PVOID&)g_func, DRAWFUNC_cpudasm);
#endif

                InvalidateRect(p->hw, NULL, TRUE);
                hooked = true;

                //hook_Defaultbar();
            }
        }
    }

}



int get_width_label()
{
    t_dump *td = sdk_Getcpudisasmdump();
    if (td)
    {
        t_table *p = &td->table;
        if (p)
        {
            t_bar* tb = &p->bar;
            if (tb && tb->nbar == 5)
            {
                return tb->dx[COLUMN_LABELS];
            }
        }
    }
    return 0;
}

int get_width_comment()
{
    t_dump *td = sdk_Getcpudisasmdump();
    if (td)
    {
        t_table *p = &td->table;
        if (p)
        {
            t_bar* tb = &p->bar;
            if (tb && tb->nbar == 5)
            {
                return tb->dx[3];
            }
        }
    }
    return 0;
}

void set_width_label(int i)
{
    width_label = i;
}

void set_width_comment(int i)
{
    width_comment = i;
}


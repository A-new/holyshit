#include "label.h"
#include "../sdk/sdk.h"
#include "hook.h"
#include <boost/thread/mutex.hpp>
#include "config.h"

DRAWFUNC *g_func;

#define COLUMN_LABELS 4
int DRAWFUNC_cpudasm( TCHAR *s, uchar *mask, int *select, t_table *pt, t_sortheader *ps,int column, void * cache )
{
    int col = 3;
    if (CConfig_Single.label_enabled())
    {
        col = COLUMN_LABELS;
    }
    if (column == col)
    {
        int itest = 0;
        // �ȼ��ԭ������û��
        if (!CConfig_Single.label_enabled())
        {
#ifdef HOLYSHIT_EXPORTS
            itest = g_func(s, (char*)mask, select, ps, column);
#else
            itest = g_func(s, mask, select, pt, ps, column, cache);
#endif
            if (itest)
            {
                //�и���������N�����ǲ�û����ʾ������Ҫ�޸���
                if (itest != 1 && *s != TEXT('N'))
                {
                    return itest;
                }
            }
        }

        t_dump *p = sdk_Getcpudisasmdump();
        ulong addr = 0;

#ifdef HOLYSHIT_EXPORTS
        addr = p->lastaddr;
#else
        addr = p->addr;
        addr += *((ulong*)((char*)cache + 0x20)); // fuck! cache���Զ���ṹ����0x20Ӧ�ò���仯̫��
        //t_drawheader *td = (t_drawheader*)ps;
        //addr = td->nextaddr; // ��һ�еĵ�ַ,shit
#endif

        if (addr)
        {
            const std::vector<int>& vCheck = CConfig_Single.check();
            
            int len = 0;
            for (size_t i=0; i<vCheck.size(); ++i)
            {
                len = Findname(addr, vCheck[i], s); // ��ȡlabel
                if (len)
                {
                    memset(mask, DRAW_HILITE, len);
                    //*select = DRAW_MASK | DRAW_VARWIDTH; // ������Ҳ���ԣ����ϵĻ�ѡ����
                    break;
                }
            }

            // patch
            if (!len && itest == 1)
            {
                *s = TEXT('N');
                return itest;
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
                if (CConfig_Single.label_enabled())
                {
                    t_bar* tb = &p->bar;
                    tb->nbar = 5;

                    tb->name[COLUMN_LABELS] = _T("Label");
                    tb->mode[COLUMN_LABELS] = BAR_NOSORT;

                    int size_font = tb->dx[2] / tb->defdx[2];
                    tb->defdx[COLUMN_LABELS] = CConfig_Single.get_width_label() / size_font; // �ַ������ȣ���Ҫ���¼��㣬����������Defaultbar����ʾ������
                    tb->dx[COLUMN_LABELS] = CConfig_Single.get_width_label();

                    tb->defdx[3] = CConfig_Single.get_width_comment() / size_font;
                    tb->dx[3] = CConfig_Single.get_width_comment();

                    InvalidateRect(p->hw, NULL, TRUE);

                }

                g_func = p->drawfunc;

#ifdef HOLYSHIT_EXPORTS
                hook(&(PVOID&)g_func, MyDRAWFUNC);
#else // od2
                hook(&(PVOID&)g_func, DRAWFUNC_cpudasm);
#endif

                hooked = true;
                //hook_Defaultbar();
            }
        }
    }

}

int get_width_label_now()
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

int get_width_comment_now()
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

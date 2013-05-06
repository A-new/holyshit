#include "hook.h"
#include <windows.h>
#include "detours.h"
#include "../sdk/sdk.h"


LONG WINAPI hook( PVOID *ppPointer, PVOID pDetour )
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(ppPointer, pDetour);
    DetourTransactionCommit();
    return 0;
}


DRAWFUNC *g_func;

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


class CHookOnce
{
public:
    CHookOnce()
    {
        t_dump *td = sdk_Getcpudisasmdump();
        t_table *p = &td->table;
        if (p)
        {
            t_bar* tb = &p->bar;
            tb->nbar = 5;

            tb->name[COLUMN_LABELS] = _T("Label");
            tb->mode[COLUMN_LABELS] = BAR_NOSORT;
            tb->defdx[COLUMN_LABELS] = 100;
            tb->dx[COLUMN_LABELS] = 100;

            tb->defdx[3] = 200;
            tb->dx[3] = 200;

            g_func = p->drawfunc;

#ifdef HOLYSHIT_EXPORTS
            hook(&(PVOID&)g_func, MyDRAWFUNC);
#else // od2
            hook(&(PVOID&)g_func, DRAWFUNC_cpudasm);
#endif

            InvalidateRect(p->hw, NULL, TRUE);

        }
    }
};

void hook_DRAWFUNC_cpudasm()
{
    static CHookOnce a;
}

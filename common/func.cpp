#include "func.h"
#include "../sdk/sdk.h"


DWORD GetCurrentEIP(void)
{
    t_thread* t2;// t_thread

    t2 = Findthread(Getcputhreadid());
    return t2->reg.ip;
}

void LoadMap()
{
    //Browsefilename(char *title,char *name,char *defext,
    //    int mode);
    //Browsefilename(wchar_t *title,wchar_t *name,wchar_t *args,
    //    wchar_t *currdir,wchar_t *defext,HWND hwnd,int mode);

    if (!HasDebuggee())
    {
        return;
    }


    TCHAR path[MAX_PATH];
    memset(path, 0, sizeof(path));
    if(0 == Browsefilename(_T("Select map file"), path, _T(".map"), 0))
        return;
    
    if (_tcsicmp(path, _T(".\\")) == 0)        //just a litle check
        return;
#ifdef _UNICODE
#define tfopen _wfopen
#define tfgets fgetws
#else
#define tfopen fopen
#define tfgets fgets
#endif
    FILE *in;
    TCHAR mapseg[8];
    TCHAR mapline[TEXTLEN];
    TCHAR mapoff[] = "0x12345678\0";
    TCHAR *endptr;
    TCHAR *str2;
    TCHAR maplabel[50];
    t_module *pmodule;

    int i,j, addmenuid, mnuitems;
    long lnumber;

    if ((in = tfopen(path, _T("rt"))) == NULL)
        Addtolist (0, 1, _T("MapConv ERROR: Cannot open %s"), path);
    else
    {
        while (!feof(in))
        {
            _tcscpy(mapseg, " 0001:");
            if ((_tcsstr(mapline, mapseg) != NULL) && (_tcsstr(mapline, "CODE") == NULL) && (_tcsstr(mapline, "Program entry") == NULL))
            {
                str2 = mapoff;
                str2++;
                str2++;
                for (i=6;i<14;i++)
                    *str2++ = mapline[i];

                *str2++ = '\0';

                str2 = maplabel;
                j = strlen(mapline)-1;
                for (i=0x15;i<j;i++)
                    *str2++ = mapline[i];

                *str2++ = '\0';
                lnumber = strtol(mapoff, &endptr, 16);

                pmodule = Findmodule((ulong)GetCurrentEIP());

                if (what == 0)
                    Insertname(pmodule->codebase + lnumber , NM_LABEL, maplabel);
                else
                    Insertname(pmodule->codebase + lnumber , NM_COMMENT, maplabel);

            };
            tfgets(mapline, TEXTLEN, in);
        };
        fclose(in);
        //if (what == 0) {
        //    Addtolist(0, 0, _T("MapConv: OK: Map file successfuly imported - labels updated"));
        //}
        //else  {
        //    Addtolist(0, 0, _T("MapConv: OK: Map file successfuly imported - comments updated"));
        //}
    };

    Setcpu(0,0,0,0,CPU_ASMFOCUS);

}

bool HasDebuggee()
{
    HANDLE procHandle = NULL;
#ifdef HOLYSHIT_EXPORTS
    procHandle = (HANDLE)Plugingetvalue(VAL_HPROCESS);
#else
    procHandle = process;
#endif
    return (procHandle != NULL);
}

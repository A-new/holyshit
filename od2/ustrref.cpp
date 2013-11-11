#include "ustrref.h"
#include "../sdk/sdk.h"

static t_table   ustrref;             // Bookmark table

typedef struct t_ustrref
{
    ulong           index;              // ustrref index
    ulong           size;               // Size of index, always 1 in our case
    ulong           type;               // Type of entry, TY_xxx

    // 自定义数据开始
    ulong           addr;               // Address of string
    int             iscureip;           // Is Current EIP? 0: No, 1: Yes.
} t_ustrref;

int ustrref_sortfunc(const t_sorthdr *sh1,const t_sorthdr *sh2,const int sort) {
    int i = 0;
    const t_ustrref* b1 = reinterpret_cast<const t_ustrref*>(sh1);
    const t_ustrref* b2 = reinterpret_cast<const t_ustrref*>(sh2);

    if (1 == sort)
    {
        // Sort by address
        if (b1->addr < b2->addr)
            i = -1;
        else if (b1->addr > b2->addr)
            i = 1;
    };
    // If elements are equal or sorting is by the first column, sort by index.
    if (0 == i)
    {
        if (b1->index < b2->index)
            i = -1;
        else if (b1->index > b2->index)
            i = 1;
    };
    return i;
};

void ustrref_destfunc(t_sorthdr *sh) {
};


int ustrref_draw(wchar_t *s,uchar *mask,int *select,
                 t_table *pt,t_drawheader *ph,int column,void *cache) {
                     int m,n;                             // Number of symbols in the string
                     ulong length;
                     uchar cmd[MAXCMDSIZE];
                     t_ustrref *pmark;
                     t_disasm *pasm;
                     n=0;
                     // For simple tables, t_drawheader is the pointer to the data element. It
                     // can't be NULL, except in DF_CACHESIZE, DF_FILLCACHE and DF_FREECACHE.
                     pmark=(t_ustrref *)ph;
                     // Our cache is just a t_disasm. It is not NULL on the same conditions.
                     pasm=(t_disasm *)cache;
                     switch (column) {
    case DF_CACHESIZE:                 // Request for draw cache size
        // Columns 3 and 4 (disassembly and comment) both require calls to
        // Disasm(). To accelerate processing, I call disassembler once per line
        // and cache data between the calls. Here I inform the drawing routine
        // how large the cache must be.
        return sizeof(t_disasm);
    case DF_FILLCACHE:                 // Request to fill draw cache
        // We don't need to initialize cache when drawing begins. Note that cache
        // is initially zeroed.
        break;
    case DF_FREECACHE:                 // Request to free cached resources
        // We don't need to free cached resources when drawing ends.
        break;
    case DF_NEWROW:                    // Request to start new row in window
        // New row starts. Let us disassemble the command at the pointed address.
        // I assume that bookmarks can't be set on data. First of all, we need to
        // read the contents of memory. Length of 80x86 commands is limited to
        // MAXCMDSIZE bytes.
        length=Readmemory(cmd,pmark->addr,sizeof(cmd),MM_SILENT|MM_PARTIAL);
        if (length==0) {
            // Memory is not readable.
            StrcopyW(pasm->result,TEXTLEN,L"???");
            StrcopyW(pasm->comment,TEXTLEN,L""); }
        else
            Disasm(cmd,length,pmark->addr,Finddecode(pmark->addr,NULL),pasm,
            DA_TEXT|DA_OPCOMM|DA_MEMORY,NULL,NULL);
        break;
    case 0:                            // 0-based index
        n=StrcopyW(s,TEXTLEN,L"Alt+");
        memset(mask,DRAW_GRAY,n);
        m=Swprintf(s+n,L"%i",pmark->index);
        memset(mask+n,DRAW_NORMAL,m);
        n+=m;
        *select|=DRAW_MASK;
        break;
    case 1:                            // Address of the bookmark
        n=Simpleaddress(s,pmark->addr,mask,select);
        break;
    case 2:                            // Disassembled command
        n=StrcopyW(s,TEXTLEN,pasm->result);
        break;
    case 3:                            // Comment
        // User-defined comment has highest priority.
        n=FindnameW(pmark->addr,NM_COMMENT,s,TEXTLEN);
        // Comment created by Disasm() is on the second place.
        if (n==0)
            n=StrcopyW(s,TEXTLEN,pasm->comment);
        // Analyser comment follows.
        if (n==0)
            n=Getanalysercomment(NULL,pmark->addr,s,TEXTLEN);
        // Procedure comments have the lowest priority.
        if (n==0)
            n=Commentaddress(pmark->addr,COMM_MARK|COMM_PROC,s,TEXTLEN);
        // User-defined comments may be written in Kanji or Chinese.
        *select|=DRAW_VARWIDTH;
        break;
    default: break;
                     };
                     return n;
};

// Custom table function of bookmarks window. Here it is used only to process
// doubleclicks (custom message WM_USER_DBLCLK). This function is also called
// on WM_DESTROY, WM_CLOSE (by returning -1, you can prevent window from
// closing), WM_SIZE (custom tables only), WM_CHAR (only if TABLE_WANTCHAR is
// set) and different custom messages WM_USER_xxx (depending on table type).
// See documentation for details.
long ustrref_func(t_table *pt,HWND hw,UINT msg,WPARAM wp,LPARAM lp) {
    t_ustrref *pmark;
    switch (msg) {
    case WM_USER_DBLCLK:               // Doubleclick
        // Get selection.
        pmark=(t_ustrref *)Getsortedbyselection(
            &(pt->sorted),pt->sorted.selected);
        // Follow address in CPU Disassembler pane. Actual address is added to
        // the history, so that user can easily return back to it.
        if (pmark!=NULL) Setcpu(0,pmark->addr,0,0,0,
            CPU_ASMHIST|CPU_ASMCENTER|CPU_ASMFOCUS);
        return 1;
    default: break;
    };
    return 0;
};
static t_menu bookmarkmenu[] = {       // Menu of the bookmark window
    { L"|>STANDARD",
    L"",                            // Forwarder to standard menus
    K_NONE, NULL, NULL, 0
    }
};

int ustrref_ODBG2_Plugininit(void)
{
    if (Createsorteddata(
        &(ustrref.sorted),                // Descriptor of sorted data
        sizeof(t_ustrref),                // Size of single data item
        10,                                // Initial number of allocated items
        (SORTFUNC *)ustrref_sortfunc,      // Sorting function
        (DESTFUNC *)ustrref_destfunc,      // Data destructor
        0)!=0)                             // Simple data, no special options
        return -1;
    
    StrcopyW(ustrref.name, SHORTNAME, L"中文搜索");
    ustrref.mode=TABLE_SAVEALL;         // Save complete appearance
    ustrref.bar.visible=1;              // By default, bar is visible
    ustrref.bar.name[0]=L"ustrref";
    ustrref.bar.expl[0]=L"ustrref index";
    ustrref.bar.mode[0]=BAR_SORT;
    ustrref.bar.defdx[0]=9;
    ustrref.bar.name[1]=L"Address";
    ustrref.bar.expl[1]=L"Bookmark address";
    ustrref.bar.mode[1]=BAR_SORT;
    ustrref.bar.defdx[1]=9;
    ustrref.bar.name[2]=L"Disassembly";
    ustrref.bar.expl[2]=L"Command at the ustrref address";
    ustrref.bar.mode[2]=BAR_FLAT;
    ustrref.bar.defdx[2]=24;
    ustrref.bar.name[3]=L"Comments";
    ustrref.bar.expl[3]=L"Comments";
    ustrref.bar.mode[3]=BAR_FLAT;
    ustrref.bar.defdx[3]=256;
    ustrref.bar.nbar=4;
    ustrref.tabfunc = ustrref_func;
    ustrref.custommode=0;
    ustrref.customdata=NULL;
    ustrref.updatefunc=NULL;
    ustrref.drawfunc=(DRAWFUNC *)ustrref_draw;
    ustrref.tableselfunc=NULL;
    ustrref.menu=bookmarkmenu;

    //// Get initialization data.
    //showindisasm=0;                      // Default value
    //Getfromini(NULL,PLUGINNAME,L"Show bookmarks in Disassembler",L"%i",
    //    &showindisasm);
    //// OllyDbg saves positions of plugin windows with attribute TABLE_SAVEPOS to
    //// the .ini file but does not automatically restore them. Let us add this
    //// functionality here. To conform to OllyDbg norms, window is restored only
    //// if corresponding option is enabled.
    //if (restorewinpos!=0) {
    //    restore=0;                         // Default
    //    Getfromini(NULL,PLUGINNAME,L"Restore window",L"%i",&restore);
    //    if (restore)
            Createtablewindow(&ustrref,0,ustrref.bar.nbar,NULL,
            L"ICO_PLUGIN", L"aa");
    //    ;
    //};

}

void __cdecl ustrref_ODBG2_Pluginreset(void) {
    Deletesorteddatarange(&(ustrref.sorted),0,0xFFFFFFFF);
};

void __cdecl ustrref_ODBG2_Plugindestroy(void) {
    Destroysorteddata(&(ustrref.sorted));
};

void __cdecl ustrref_ODBG2_Pluginnotify(int code,void *data,
                                     ulong parm1,ulong parm2) {
                                         int i;
                                         t_ustrref *pmark;
                                         t_module *pmod;
                                         switch (code) {
    case PN_ENDMOD:                    // Module is removed from the memory
        // Module is unloaded from the memory. This notification comes after
        // data was saved to the .udd file. All we need is to remove bookmarks
        // from the table.
        pmod=(t_module *)data;
        if (pmod==NULL)
            break;                         // Must not happen!
        // As many scattered bookmarks may be deleted at once, I first mark them
        // all as confirmed ...
        Confirmsorteddata(&(ustrref.sorted),1);
        // ... then remove confirmation flag from the bookmarks in the range ...
        for (i=0; i<ustrref.sorted.n; i++) {
            pmark=(t_ustrref *)Getsortedbyindex(&(ustrref.sorted),i);
            if (pmark==NULL)
                continue;                    // Must not happen!
            if (pmark->addr>=pmod->base && pmark->addr<pmod->base+pmod->size)
                pmark->type&=~TY_CONFIRMED;
            ;
        };
        // ... and finally delete all non-confirmed bookmarks. Note that
        // Deletenonconfirmedsorteddata() acts on sorted data and will not
        // redraw the table window.
        if (Deletenonconfirmedsorteddata(&(ustrref.sorted))!=0 &&
            ustrref.hw!=NULL)
            InvalidateRect(ustrref.hw,NULL,FALSE);
        break;
    default: break;                    // No action necessary
                                         };
};
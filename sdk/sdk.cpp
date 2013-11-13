#include "sdk.h"

#ifdef HOLYSHIT_EXPORTS // od110
#pragma comment(lib, "OLLYDBG110.LIB")
#else
#pragma comment(lib, "ollydbg201.lib")
#endif

t_dump * sdk_Getcpudisasmdump()
{
#ifdef HOLYSHIT_EXPORTS // od110
    return (t_dump*)Plugingetvalue(VAL_CPUDASM);
#else
    return Getcpudisasmdump();
#endif
}

// Alt + E ´°¿Ú
t_dump * sdk_Getmodulesdump()
{
#ifdef HOLYSHIT_EXPORTS // od110
    return (t_dump*)Plugingetvalue(VAL_MODULES);
#else
    return NULL;//Getcpudisasmdump();
#endif
}


void cdecl Getdisassemblerrange(ulong *pbase,ulong *psize)
{
    t_dump * td = sdk_Getcpudisasmdump();
    if (td)
    {
        *pbase = td->base;
        *psize = td->size;
    }

}
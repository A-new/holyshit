#include "sdk.h"

#ifdef HOLYSHIT_EXPORTS // od110
#pragma comment(lib, "OLLYDBG110.LIB")
#else
#pragma comment(lib, "ollydbg2.lib")
#endif

t_dump * sdk_Getcpudisasmdump()
{
#ifdef HOLYSHIT_EXPORTS // od110
    return (t_dump*)Plugingetvalue(VAL_CPUDASM);
#else
    return Getcpudisasmdump();
#endif
}

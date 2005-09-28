#ifndef _OSK
#include <stdio.h>
#include <sys/types.h>
#else
#include <stdio.h>
#endif

#ifdef _OSK
void Make_Dataport_Name (char *dataport_name,char *dataport_name_stub,int proc_id)
#else
void Make_Dataport_Name (char *dataport_name,char *dataport_name_stub,pid_t proc_id)
#endif

{
  sprintf(dataport_name,"%s.%ld.DP",dataport_name_stub,(long)proc_id);
}

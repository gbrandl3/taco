#ifndef DATAPORT_H
#define DATAPORT_H

/***************************************************************************
 *
 * $Source: /home/jkrueger1/sources/taco/backup/taco/lib/dataport/dataport.h,v $
 *
 * $Author: jkrueger1 $
 * $Date: 2003-04-25 11:21:42 $
 *
 ***************************************************************************/

#if (OSK || _OSK)
/* OS-9 version */

#include <module.h>

typedef int Semaphore;

typedef struct {
     struct modhcom   header;

     Semaphore        sem;
     char             semname[32];
     int              pid;
     char             body;
} Dataport;

#else                                             /* Unix version */
#include <sys/types.h>

typedef struct {
     int              sema;
     int              shar;
     int              key;
     pid_t            creator_pid;
     char             body;
} Dataport;

#endif

#define	HASH_BASE	131
#define	HASH_MOD	500000000

#ifdef __cplusplus
extern "C" {
extern Dataport *CreateDataport(char *,long);
extern Dataport *OpenDataport(char *,long);
extern long AccessDataport(Dataport *);
extern long ReleaseDataport(Dataport *);
extern long CloseDataport(Dataport *,char *);
}
#else
extern Dataport *CreateDataport();
extern Dataport *OpenDataport();
extern long AccessDataport();
extern long ReleaseDataport();
extern long CloseDataport();
#endif /* __cplusplus */

#endif

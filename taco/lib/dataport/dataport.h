/***************************************************************************
 *
 * Author:	$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2003-05-21 16:19:00 $
 *
 ***************************************************************************/

/**
 * @defgroup dataportAPI Data port API
 * @ingroup API
 * These functions allow the access to the data port
 *
 * The dataport library allows uses to create and share data structures
 * on Unix and OS9 via shared memory. Synchronisation is achieved
 * with semaphores.
 */

#ifndef DATAPORT_H
#define DATAPORT_H

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

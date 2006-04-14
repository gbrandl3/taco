/* ndbm.h  -  The include file for ndbm users.  */

/*  This file is part of GDBM, the GNU data base manager, by Philip A. Nelson.
    Copyright (C) 1990, 1991, 1993  Free Software Foundation, Inc.

    GDBM is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.

    GDBM is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GDBM; see the file COPYING.  If not, write to
    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

    You may contact the author by:
       e-mail:  phil@cs.wwu.edu
      us-mail:  Philip A. Nelson
                Computer Science Department
                Western Washington University
                Bellingham, WA 98226
       
*************************************************************************/

/* Parameters to dbm_store for simple insertion or replacement. */
#define  DBM_INSERT  0
#define  DBM_REPLACE 1

/* The data and key structure.  This structure is defined for compatibility. */
#ifndef  _GDBM_H_ 
typedef struct {
	char *dptr;
	int   dsize;
      } datum;
#endif


/* The file information header. This is good enough for most applications. */
typedef struct {int dummy[10];} DBM;


/* These are the routines (with some macros defining them!) */
#ifdef __cplusplus
extern "C" {
#endif

extern DBM 	*dbm_open (char *, int, int);

extern void	 dbm_close (DBM *);

extern datum	 dbm_fetch (DBM *, datum);

extern int	 dbm_store (DBM*, datum, datum, int);

extern int	 dbm_delete (DBM*, datum);

extern datum	 dbm_firstkey (DBM *);

extern datum	 dbm_nextkey (DBM *);

#define		 dbm_error(dbf)  (0)

#define		 dbm_clearerr(dbf)

extern int	 dbm_dirfno (DBM *);

extern int	 dbm_pagfno (DBM *);

extern int	 dbm_rdonly (DBM *);
#ifdef __cplusplus
}
#endif

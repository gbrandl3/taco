/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:        Admin.h
 * 
 * Project:     Device Servers with SUN-RPC
 * 
 * Description: Public include for applications wanting to
 *              to use the device server administrator.
 * 
 * Author:	$Author: jkrueger1 $
 * 
 * Version:	$Revision: 1.3 $
 *  
 * Date:	$Date: 2006-12-21 08:18:21 $
 * 
 *********************************************************************/

#ifndef _admin_h
#define _admin_h

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char Adminh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/Admin.h,v 1.3 2006-12-21 08:18:21 jkrueger1 Exp $";
#endif /* _IDENT */


/*
 *   Motif application
 */

#define UNMAPPED	0
#define MAPPED		1
#define DEBUG		2
#define DIAG		3


/****************************
 *  Administrator Commands  *
 ****************************/

/*
 *   Debug Commands
 */

#define	DEBUG_ON	0
#define	DEBUG_OFF	1
#define	DEBUG_SET	2
#define	DEBUG_RESET	3

/*
 *   Diagnostics Commands
 */

#define DIAG_ON		4
#define DIAG_OFF	5
#define DIAG_GEN	6
#define DIAG_DEV	7

/*
 *   Process Commands
 */

#define ADMIN_IMPORT	8
#define ADMIN_FREE	9
#define QUIT_SERVER	10
#define RESTART_SERVER	11


/*
 *   Debug flags
 */

#define DEBUG_ON_OFF	0x80000000	

/*
 *   General Methods
 */

#define COMMENT		0
#define DBG_TRACE	0x1
#define DBG_ERROR	0x2
#define DBG_INTERRUPT	0x4
#define DBG_TIME	0x8
#define DBG_WAIT	0x10
#define DBG_EXCEPT	0x20
#define DBG_SYNC	0x40
#define DBG_HARDWARE	0x80

#define DBG_STARTUP		0x100
#define DBG_DEV_SVR_CLASS	0x200
#define DBG_API			0x400
#define DBG_COMMANDS		0x800
#define DBG_METHODS		0x1000
#define DBG_SEC             	0x2000
#define DBG_ASYNCH          	0x4000


#define COMMAND_HANDL	0x100000
#define STATE_HANDL	0x200000
#define ADMIN		0x400000
#define MSG_IMPORT	0x800000


#define ALL		0x7FFFFFFF


/*
 *   Steppermotor Methods
 */

#define STEPPER		0x1000000


/*
 *   Debug List for Motif application
 */

#define NUM_LIST_ITEMS	25

static struct _DevDebug_Entry {
			      const char *sel_item;
			      long  deb_flag;
			      } DevDebug_List [] =
{
	{ "!" 		 	, COMMENT    	},
	{ "General Methods :" 	, COMMENT    	},
	{ "!" 		 	, COMMENT    	},
	{ "command_handler()"  	, COMMAND_HANDL	},
	{ "state_handler()"    	, STATE_HANDL 	},
	{ "administrator ()"	, ADMIN		},
	{ "!" 		 	, COMMENT    	},
	{ "msg_import()" 	, MSG_IMPORT 	},
	{ "!" 		 	, COMMENT    	},
	{ "!" 		 	, COMMENT    	},
	{ "Steppermotor Methods :", COMMENT    	},
	{ "steppermotor()"    	, STEPPER 	},
	{ "!" 		 	, COMMENT    	},
	{ "Debug all :" 	, COMMENT    	},
	{ "!" 		 	, COMMENT    	},
	{ "All Bits" 		, ALL		},
};

extern int os9forkc();
#ifndef _OSK
#ifdef __cplusplus
extern "C" {
#endif
extern char **environ;
#ifdef __cplusplus
}
#endif
#else

#ifdef OSK
extern char **environ;
#else
extern char **_environ;
#endif
#endif

#endif /* _admin_h */


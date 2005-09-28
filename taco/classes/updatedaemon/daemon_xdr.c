static char RcsId[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/classes/updatedaemon/daemon_xdr.c,v 1.1 2005-09-28 20:08:33 andy_gotz Exp $";

/*+*******************************************************************

 File:          daemon_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for update daemon types.

 Author(s):     Jens Meyer

 Original:      July 1992

$Revision: 1.1 $
$Date: 2005-09-28 20:08:33 $

$Author: andy_gotz $

$Log: not supported by cvs2svn $
Revision 4.19  2005/01/24 08:12:17  taurel
Fix some compile time error in order to compile poller on Linux with CPP compiler.

Revision 4.18  2004/01/15 11:40:55  taurel
With Linux, the select call may change the timeout struc. Re-set it before any select call.

Revision 4.17  2003/12/11 16:39:02  taurel
POrted

 * Revision 4.16  2002/07/04  08:58:05  08:58:05  taurel (Emmanuel Taurel)
 * Added TACO_PATH env. variable to find poller path
 * 
Revision 4.15  2001/05/11 17:13:47  taurel
No change, check out only for debug purpose

 * Revision 4.14  2000/10/17  17:00:59  17:00:59  taurel (E.Taurel)
 * Remove some fancy printf
 * 
 * Revision 4.13  2000/10/17  15:47:30  15:47:30  goetz (Andy Goetz)
 * daemon renamed ud_daemon for Linux port
 * 
 * Revision 4.12  2000/06/15  10:36:15  10:36:15  taurel (Emmanuel Taurel)
 * The
 * 
 * Revision 4.11  99/02/25  15:59:51  15:59:51  taurel (Emmanuel TAUREL)
 * change algo. used for the signal_valid data in order to spped up data exchange between daemon and poller.
 * 
 * Revision 4.10  99/02/04  13:23:41  13:23:41  taurel (Emmanuel Taurel)
 * Add closing open file in poller startup and remove check before sending signal to poller
 * 
 * Revision 4.9  99/01/13  14:24:20  14:24:20  taurel (E.Taurel)
 * Fix bug in datac.c when the open_dc function called by the store_data function
 * 
 * Revision 4.8  96/12/04  11:32:53  11:32:53  taurel (E.Taurel)
 * Added a printf to print the number of devices
 * retrived from db with the ud_poll_list resource.
 * 
 * Revision 4.7  96/03/11  10:43:14  10:43:14  taurel (E.Taurel)
 * No change, co just for test.
 * 
 * Revision 4.6  96/02/27  15:51:02  15:51:02  taurel (Emmanuel TAUREL)
 * Ported to Solaris.
 * 
 * Revision 4.5  95/03/20  10:43:09  10:43:09  taurel (Emmanuel TAUREL)
 * Change variable name from errno to dev_errno for
 * device server error.
 * 
 * Revision 4.4  95/01/12  16:02:09  16:02:09  taurel (Emmanuel TAUREL)
 * Nothing change. co and ci jutest purpose.
 * 
 * Revision 4.3  95/01/11  15:17:40  15:17:40  taurel (Emmanuel TAUREL)
 * Remove some printf
 * 
 * Revision 4.2  95/01/11  15:14:37  15:14:37  taurel (Emmanuel TAUREL)
 * Change dataport name
 * 
 * Revision 4.1  95/01/03  17:30:31  17:30:31  taurel (Emmanuel TAUREL)
 * Ported to OS-9 ULTRA-C. Also fix two bugs for OS-9 :
 *   - Now time spent in command execution is correct
 *   - Correctly removed the dataport when the daemon is correctly killed.
 * 
 * Revision 4.0  94/10/17  14:55:57  14:55:57  taurel (Emmanuel TAUREL)
 * First
 * 
 * Revision 1.1  94/06/28  18:37:36  18:37:36  servant (Florence Servant)
 * Initial revision
 * 

Copyright (c) 1990 by European Synchrotron Radiation Facility,
		      Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <daemon_xdr.h>

#ifdef unix

#include <stdlib.h>
#include <string.h>

#endif


bool_t
xdr_DevDaemonStatus(xdrs, objp)
	XDR *xdrs;
	DevDaemonStatus *objp;
{
	if (!xdr_char(xdrs, &objp->BeingPolled)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->PollFrequency)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->TimeLastPolled)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->PollMode)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->DeviceAccessError)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->ErrorCode)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->LastCommandStatus)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->ContinueAfterError)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevDaemonStatus(objp)
        DevDaemonStatus *objp;
        {
        long  length = 0;

        length = length + xdr_length_DevChar (&objp->BeingPolled);
        length = length + xdr_length_DevLong (&objp->PollFrequency);
        length = length + xdr_length_DevLong (&objp->TimeLastPolled);
        length = length + xdr_length_DevChar (&objp->PollMode);
        length = length + xdr_length_DevChar (&objp->DeviceAccessError);
        length = length + xdr_length_DevLong (&objp->ErrorCode);
        length = length + xdr_length_DevLong (&objp->LastCommandStatus);
        length = length + xdr_length_DevChar (&objp->ContinueAfterError);

        return (length);
}



bool_t
xdr_DevDaemonData(xdrs, objp)
	XDR *xdrs;
	DevDaemonData *objp;
{
	if (!xdr_long(xdrs, &objp->ddid)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->long_data)) {
		return (FALSE);
	}

	return (TRUE);
}

long
xdr_length_DevDaemonData(objp)
        DevDaemonData *objp;
        {
        long  length = 0;

        length = length + xdr_length_DevLong (&objp->ddid);
        length = length + xdr_length_DevLong (&objp->long_data);

        return (length);
}



/* Structure for new device process resources */

bool_t
xdr_DevDaemonStruct(xdrs, objp)
	XDR *xdrs;
	DevDaemonStruct *objp;
{
	if (!xdr_wrapstring(xdrs, &objp->dev_n)) 
	{
		return (FALSE);
	}
	if (!xdr_DevVarStringArray(xdrs,&objp->cmd_list))
        {
		return (FALSE);
	}  
	if (!xdr_long(xdrs, &objp->poller_frequency)) 
	{
		return (FALSE);
	}

	return (TRUE);
}


long
xdr_length_DevDaemonStruct(objp)
        DevDaemonStruct *objp;
        {
        long  length = 0;

        length = length + xdr_length_DevChar (&objp->dev_n[0]);
        length = length + xdr_length_DevVarStringArray (&objp->cmd_list);  
        length = length + xdr_length_DevLong (&objp->poller_frequency);

        return (length);
}



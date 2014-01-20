/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:        unreg.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Source code for unregistering a TACO RPC program (server)
 *
 * Author(s):    
 *              $Author: jkrueger1 $
 *
 * Original:    January 1991
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:                $Date: 2008-04-06 09:07:51 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#ifdef HAVE_RPC_RPC_H
#	include <rpc/rpc.h>
#elif HAVE_RPC_H
#	include <rpc.h>
#else
#	error Did not found rpc.h file
#endif
#ifdef HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif

int main (int argc, char **argv)
{
	int  i;
	unsigned long nu = 0;

	for (i=1; i<argc; i++)
	{
		sscanf (argv[i],"%d", &nu); /* Flawfinder: ignore */
		if (nu > 0)
		{
			pmap_unset(nu, 1L);
			pmap_unset(nu, 2L);
			pmap_unset(nu, 3L);
			pmap_unset(nu, 4L);
			pmap_unset(nu, 5L);
		}
	}
	return 0;
}

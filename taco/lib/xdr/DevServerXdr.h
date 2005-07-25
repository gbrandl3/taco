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
 *
 * File:        DevServerXdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Public include file for all applications
 *		using device server types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef DEVSERVERXDR_H
#define DEVSERVERXDR_H

/*
 *  standart header string to use the "what" or "ident".
 */
#ifdef _IDENT
static char DevServerXdrh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/DevServerXdr.h,v 1.3 2005-07-25 13:05:45 jkrueger1 Exp $";
#endif /* _IDENT */

#include <dev_xdr.h>
#include <bpm_xdr.h>
#include <bpss_xdr.h>
#include <ct_xdr.h>
#include <daemon_xdr.h>
#include <gpib_xdr.h>
#include <haz_xdr.h>
#include <maxe_xdr.h>
#include <pss_xdr.h>
#include <ram_xdr.h>
#include <rf_xdr.h>
#include <thc_xdr.h>
#include <union_xdr.h>
#include <vgc_xdr.h>
#include <vrif_xdr.h>
#include <ws_xdr.h>
#include <seism_xdr.h>
#include <atte_xdr.h>
#include <slit_xdr.h>
#include <icv101_xdr.h>
#include <mstat_xdr.h>
#include <m4_xdr.h>
#include <grp_xdr.h>
#include <pin_xdr.h>

#include <xdr_typelist.h>

#endif /* _DevServerXdr_h */


/*+*******************************************************************

 File:          DevServerXdr.h

 Project:       Device Servers with sun-rpc

 Description:   Public include file for all applications
		using device server types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _DevServerXdr_h
#define _DevServerXdr_h

/*
 *  standart header string to use the "what" or "ident".
 */
#ifdef _IDENT
static char DevServerXdrh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/DevServerXdr.h,v 1.2 2004-02-06 13:11:22 jkrueger1 Exp $";
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


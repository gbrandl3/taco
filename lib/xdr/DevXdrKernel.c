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
 * File:        DevXdrKernel.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Contains the the list of all loaded XDR data types
 *		and the functions to load or retrieve a data type.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	November 1993
 *
 * Version:	$Revision: 1.7 $
 *
 * Date:	$Date: 2008-04-06 09:07:20 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <DevXdrKernel.h>
#include <Admin.h>

/*
 * For the library version with all XDR data types include all *.h files.
 */

#ifdef _XDR_ALL
#include <bpm_xdr.h>
#include <bpss_xdr.h>
#include <ct_xdr.h>
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
#include <pin_xdr.h>
#include <grp_xdr.h>
#endif /* _XDR_ALL */

/*
 * Define the list which contains all loaded XDR data types.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This list is defined as global variable on special
 * request of E.Taurel.
 * To access the XDR data types with maximum speed from
 * the data collector libray.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

DevDataListEntry DevData_List [MAX_NUMBER_OF_XDR_TYPES] 
			       = { 0, NULL, 0, NULL };

/*
 * Set the number of loaded, private XDR data types to zero.
 */

static long		number_of_private_xdr_types = 0;

/**
 * Loads a new XDR data type into the global list 
 * of available data types.
 * All loaded data types can be used for data transfer 
 * between device servers and clients.
 * A general purpose data type is loaded directly into 
 * the list field specified by his XDR data type number 
 * to avoid later searches in the list.
 * Private data types are loaded into list fields
 * >= NUMBER_OF_GENERAL_XDR_TYPES with increasing numbers. 
 * To retrieve a private data type, the list of available 
 * private data types has to be searched.
 * 
 * @param type  	XDR data type number.
 * @param xdr 		Function pointer of the XDR function.
 * @param size  	The size (in bytes) of the XDR data type (calculated by sizeof()). 
 * @param xdr_length 	function pointer of the XDR length calculation function.
 * @param error         Will contain an appropriate error  code if the corresponding call
 * 			returns a non-zero value.
 * @return DS_OK or DS_NOTOK
 */ 
#ifdef __STDC__
long xdr_load_type (long type, DevDataFunction xdr, long size,
			 DevDataLengthFunction xdr_length, DevLong *error)
#else
long xdr_load_type (type, xdr, size, xdr_length, error)
	long 			type;
	DevDataFunction 	xdr;
	long 			size;
	DevDataLengthFunction 	xdr_length;
	DevLong			*error;
#endif /* __STDC__ */
{
	int	i;

	*error = 0;

#ifdef EBUG
        dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_load_type() : entering routine\n");
#endif /* EBUG */

/*
 * Check error conditions.
 */

	if ((type < 0) || (xdr == NULL) || (size < 0))
	{
		*error = DevErr_BadXdrDataTypeDefinition;
		return (DS_NOTOK);
	}

	if ((NUMBER_OF_GENERAL_XDR_TYPES + number_of_private_xdr_types) 
	      == MAX_NUMBER_OF_XDR_TYPES )
	{
		*error = DevErr_XdrTypeListOverflow;
		return (DS_NOTOK);
	}

#ifdef EBUG
        dev_printdebug (DBG_API, "\nxdr_load_type() : req_type = %d", type);
#endif /* EBUG */

/*
 * General XDR data types have a type 
 * number < NUMBER_OF_GENERAL_XDR_TYPES
 * and are stored directly in the list with this
 * type number. For fast retrieval without search.
 */
	if ( type < NUMBER_OF_GENERAL_XDR_TYPES )
	{
/*
 * If a XDR data type was already loaded, do nothing.
 */
		if ( DevData_List[type].type == type )
			return (DS_OK);

		DevData_List[type].type = type;
		DevData_List[type].xdr = xdr;
		DevData_List[type].size = size;
		DevData_List[type].xdr_length = xdr_length;

		return (DS_OK);
	} 

/*
 * If a private XDR data type will be loaded,
 * it has to be stored in the next free field of the
 * list >= NUMBER_OF_GENERAL_XDR_TYPES.
 *
 * Check wether the same XDR data type is already 
 * loaded.
 */
	for ( i=0; i<number_of_private_xdr_types; i++ )
		if ( DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + i].type == type )
			return (DS_OK);
	
/*
 * The XDR data type is not yet loaded!
 */
	DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + number_of_private_xdr_types].type = type;
	DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + number_of_private_xdr_types].xdr = xdr;
	DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + number_of_private_xdr_types].size = size;
	DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + number_of_private_xdr_types].xdr_length = xdr_length;
	number_of_private_xdr_types++;

#ifdef EBUG
        dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_load_type() : leaving routine");
#endif /* EBUG */
	return (DS_OK);
}

/**
 * Loads all XDR data types, which are defined in
 * the kernel to the list of available datatypes. 
 * 
 * By default load the small kernel of general purpose data types.
 * Only with the compile option "XDR_ALL" load all former data
 * types into the kernel.
 *
 * @param error Will contain an appropriate error code if the 
 *		corresponding call returns a non-zero value.
 * 
 * @return  DS_OK or DS_NOTOK
 */
#ifdef __STDC__
long xdr_load_kernel ( DevLong *error )
#else
long xdr_load_kernel ( error )
	DevLong			*error;
#endif /* __STDC__ */
{
	*error = 0;

	if ( LOAD_VOID_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_BOOLEAN_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_USHORT_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_SHORT_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_ULONG_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_LONG_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_FLOAT_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_DOUBLE_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_STRING_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_INT_FLOAT_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_FLOAT_READPOINT(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_STATE_FLOAT_READPOINT(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_LONG_READPOINT(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_DOUBLE_READPOINT(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_CHARARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_STRINGARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_USHORTARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_SHORTARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_ULONGARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_LONGARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_FLOATARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_DOUBLEARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_FRPARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_SFRPARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_VAR_LRPARR(error) == DS_NOTOK )
		return (DS_NOTOK);
        if ( LOAD_OPAQUE_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);

#ifdef _XDR_ALL
	if ( LOAD_BPM_POSFIELD(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_BPM_ELECFIELD(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_WS_BEAMFITPARM(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VGC_STATUS(error) == DS_NOTOK ) 
		return (DS_NOTOK);
	if ( LOAD_VGC_GAUGE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VGC_CONTROLLER(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_UNION_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_NEG_STATUS(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_RAD_DOSE_VALUE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_THARR(error) == DS_NOTOK ) 
		return (DS_NOTOK);
	if ( LOAD_HAZ_STATUS(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VRIF_WDOG(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VRIF_STATUS(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VRIF_POWERSTATUS(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_GPIB_WRITE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_GPIB_MUL_WRITE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_STATE_INDIC(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_BPSS_STATE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_BPSS_READPOINT(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_GPIB_RES(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_GPIB_LOC(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_PSS_STATUS(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_RF_SIGCONFIG(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_BPSS_LINE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_CT_LIFETIME(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_MULMOVE_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_SEISM_EVENT(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_SEISM_STAT(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_LIEN_STATE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_BLADE_STATE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_PSLIT_STATE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_ATTE_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_MOTOR_LONG(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_MOTOR_FLOAT(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_PATTERNARR(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_ICV_MODE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_MSTATARR(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_LONGFLOATSET(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_LONGREAD(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_POSREAD(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_AXEREAD(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_PARREAD(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_ERRREAD(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_GRPFP_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_VAR_GRPFPARR(error) == DS_NOTOK )
		return (DS_NOTOK);
	if ( LOAD_PINSTATE_TYPE(error) == DS_NOTOK )
		return (DS_NOTOK);
#endif /* _XDR_ALL */

	return (DS_OK);
}

/**
 * Gets a XDR data type from the global list of available data types.
 * Via the pointers in the returned structure, the XDR function and 
 * the XDR length calculation function can be accessed.
 *
 * A general purpose data type is read directly from the list field 
 * specified by his XDR data type number to avoid searching in the list.
 *
 * Private data types are read from list fields >= NUMBER_OF_GENERAL_XDR_TYPES. 
 * A private data type has to be searched in the list of available private data types.
 * 
 * @param type          XDR data type number.
 * @param xdr_type 	A pointer to a structure describing the XDR data type.
 * @param error         Will contain an appropriate error code if the corresponding 
 *			call returns a non-zero value.
 * 
 * @return  DS_OK or DS_NOTOK
 */ 
#ifdef __STDC__
long xdr_get_type (long type, DevDataListEntry *xdr_type, DevLong *error)
#else
long xdr_get_type (type, xdr_type, error)
	long 			type;
	DevDataListEntry 	*xdr_type;
	DevLong			*error;
#endif /* __STDC__ */
{
	int	i;

	*error = 0;

        dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_get_type() : entering routine\n");
        dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_get_type() : %p\n", xdr_type);

/*
 * Check error conditions.
 */
	if ( type < 0 )
	{
		*error = DevErr_BadXdrDataTypeDefinition;
		return (DS_NOTOK);
	}

/*
 * General XDR data types have a type 
 * number < NUMBER_OF_GENERAL_XDR_TYPES
 * and are stored directly in the list with this
 * type number. For fast retrieval without search.
 */
	if ( type < NUMBER_OF_GENERAL_XDR_TYPES )
	{
		dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_get_type() : req_type = %d , list_type = %d", type, DevData_List[type].type);
		if ( DevData_List[type].type != type )
		{
			*error = DevErr_XdrDataTypeNotLoaded;
			return (DS_NOTOK);
		}
		dev_printdebug(DBG_TRACE | DBG_API, "\nxdr_get_type() : assign xdr_type->type.\n");
		xdr_type->type       = DevData_List[type].type;
		dev_printdebug(DBG_TRACE | DBG_API, "\nxdr_get_type() : assign xdr_type->xdr.\n");
		xdr_type->xdr        = DevData_List[type].xdr;
		dev_printdebug(DBG_TRACE | DBG_API, "\nxdr_get_type() : assign xdr_type->size.\n");
		xdr_type->size       = DevData_List[type].size;
		dev_printdebug(DBG_TRACE | DBG_API, "\nxdr_get_type() : assign xdr_type->xdr_length.\n");
		xdr_type->xdr_length = DevData_List[type].xdr_length;
		dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_get_type() : leaving routine.\n");
		return (DS_OK);
	}
	
/*
 * Search for a private XDR data type in the list.
 */
	for ( i=0; i<number_of_private_xdr_types; i++ )
	{
		if ( DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + i].type == type )
		{
        		dev_printdebug ( DBG_API, "\nxdr_get_type() : req_type = %d , list_number = %d.\n",
					type, (NUMBER_OF_GENERAL_XDR_TYPES + i));
			xdr_type->type = DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + i].type;
	      		xdr_type->xdr = DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + i].xdr;
			xdr_type->size = DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + i].size;
	 		xdr_type->xdr_length = DevData_List[NUMBER_OF_GENERAL_XDR_TYPES + i].xdr_length;

			dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_get_type() : leaving routine.\n");
			return (DS_OK);
		}
	}
	*error = DevErr_XdrDataTypeNotLoaded;
	dev_printdebug (DBG_TRACE | DBG_API, "\nxdr_get_type() : leaving routine.\n");
	return (DS_NOTOK);
}

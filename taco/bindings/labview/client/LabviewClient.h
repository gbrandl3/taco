/*+*******************************************************************

 File:		LabviewClient.h

 Project:	TACO Client for TACO Labview Device Server

 Description:	Defines the routines for getting and setting values of VI controls and indicators. 
					For any type(double) a different method is declared. 
					A bunch of type conversion methods is needed for this purpose.
	
 Author(s);	Hartmut Gilde 

 Original:	November 2003

 Version:	$Revision: 1.1 $

 Date:		$Date: 2004-01-26 08:44:22 $

 Copyright (c) 1990-1997 by FRM2 TU München, 
                            Garching, Germany

********************************************************************-*/
#include <DevServer.h>

#ifndef __LABVIEW_CLIENT__
#define __LABVIEW_CLIENT__


/* define ERRORS */

#define WRONG_CONTROL_ERROR			-1
#define WRONG_TYPE_ERROR				-2
#define WRONG_ARGSNUM_ERROR			-3
#define CONVERSION_FAILED_ERROR		-4



/// The <b>LabviewClient</b> represents a TACO client and acts as a type-wrapper for the Labview-Taco-Interface
/*!
 * LabviewClient provides typespecific <i>get</i> and <i>set</i> methods. 
 * As the Labview-Taco-Interface is designed to be simple and generic, 
 * data is always passed as a sequence of strings between Labview and TACO.  
 * 
 * This makes it necessary to convert the types on both sides.
 * For reasons of convenience and in order to prevent the user from doing the 
 * annoying conversion himself, LabviewClient internally implements some conversion 
 * functions and provides a type-specific interface to higher level applications.
 */
class LabviewClient {

private:
bool extern_server;
long error;
long readwrite;
int cmd;
int status;
devserver* lv;

protected:
DevVarStringArray* cntl_array_in;
DevVarStringArray* cntl_array_out;
int num_get_in_args; 
int num_get_out_args;
int num_set_out_args;


public:

static char* device_name;
static const int MAX_STRING_LENGTH = 1000;

LabviewClient();
LabviewClient(char*);
LabviewClient(devserver*);

~LabviewClient();

int InitDevice();
void InitVars();
void TestPutGetError(char* calling_method);

void InitGetInputBuffer ();
void InitGetOutputBuffer(DevString name);
void InitSetOutputBuffer(DevString name, DevString type, DevString value);

int GetViInfo(DevVarStringArray*);
int GetControlList(DevVarStringArray* lv_cntl_list, bool indicator=false);

int GetControlType(DevString lv_cntl_name, bool indicator=false);
int GetControlInfo(DevString lv_cntl_name, DevVarStringArray* lv_cntl_info, bool indicator=false);

/* type specific get functions */
int GetDoubleValue	(DevString lv_cntl_name, 	DevDouble* lv_cntl_value, 	bool indicator=false);
int GetFloatValue		(DevString lv_cntl_name, 	DevFloat* lv_cntl_value, 	bool indicator=false);
int GetShortValue		(DevString lv_cntl_name, 	DevShort* lv_cntl_value, 	bool indicator=false);
int GetUShortValue	(DevString lv_cntl_name, 	DevUShort* lv_cntl_value, 	bool indicator=false);
int GetLongValue		(DevString lv_cntl_name,	DevLong* lv_cntl_value, 	bool indicator=false);
int GetULongValue		(DevString lv_cntl_name, 	DevULong* lv_cntl_value, 	bool indicator=false);
int GetStringValue	(DevString lv_cntl_name, 	DevString* lv_cntl_value, 	bool indicator=false);
int GetBooleanValue	(DevString lv_cntl_name, 	DevBoolean* lv_cntl_value, bool indicator=false);

/* type specific set functions */
int SetDoubleValue	(DevString lv_cntl_name, DevDouble* lv_cntl_value);
int SetFloatValue		(DevString lv_cntl_name, DevFloat* 	lv_cntl_value);
int SetShortValue		(DevString lv_cntl_name, DevShort* 	lv_cntl_value);
int SetUShortValue	(DevString lv_cntl_name, DevUShort* lv_cntl_value);
int SetLongValue		(DevString lv_cntl_name, DevLong* 	lv_cntl_value);
int SetULongValue		(DevString lv_cntl_name, DevULong* 	lv_cntl_value);
int SetStringValue	(DevString lv_cntl_name, DevString* lv_cntl_value);
int SetBooleanValue	(DevString lv_cntl_name, DevBoolean*lv_cntl_value);

static DevString stringType	(int type);
static DevString stringDType	(int type);
static int typeFromString(DevString type_string);


/* -------------------------------*
 * auxiliary conversion functions *
 * -------------------------------*/

/* string to type */
static DevDouble  DevStringToDouble (DevString string);
static DevFloat   DevStringToFloat  (DevString string);
static DevShort   DevStringToShort  (DevString string);
static DevUShort  DevStringToUShort (DevString string);
static DevLong    DevStringToLong   (DevString string);
static DevULong   DevStringToULong  (DevString string);
static DevBoolean DevStringToBoolean(DevString string);

/*type to string */
static DevString DevDoubleToString (DevDouble  value);
static DevString DevFloatToString  (DevFloat   value);
static DevString DevShortToString  (DevShort   value);
static DevString DevUShortToString (DevUShort  value);
static DevString DevLongToString   (DevLong    value);
static DevString DevULongToString  (DevULong   value);
static DevString DevBooleanToString(DevBoolean value);

/*generic conversion functions */
static DevVoid*  DevScanToType(DevString string, int type);
static DevVoid*  DevStringToType(DevString string, int type);
static DevString DevTypeToString(DevVoid* value, 	int type);
};
#endif /* __LABVIEW_CLIENT__ */

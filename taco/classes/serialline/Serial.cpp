
//+*********************************************************************
//
// File:	Serial.cpp
//
// Project:	Device Servers in C++
//
// Description:	Code for implementing the Serial class in C++ on Linux/x86.
//
// Author(s);	Manuel Perez
//
// Original:	May 1998 by Andy Gotz
//
// Copyright (c) 1998 by European Synchrotron Radiation Facility, 
//                       Grenoble, France
//
//-*********************************************************************

#include <stdlib.h>
#include <string.h>
#include <API.h>
#include <DevErrors.h>
#include <DevSignal.h>

#include <DevServer.h>
#include <Serial.h>

#include <errno.h>
//#include <linux/types.h>
#include <stddef.h>

#include <termios.h>
#include <fcntl.h>

#include <sys/ioctl.h>



short Serial::class_inited = 0;
short Serial::class_destroyed = 0;
short Serial::debug = 0;

extern Serial *device_ds[];
extern unsigned int n_devices;


//
// In order to save space on the stack declare the buffer used as
// temporary storage by all input/output commands as static global 
//
// - andy 10/1/97
//
static char buffer[SL_MAXSTRING];




//+======================================================================
// Function:    Serial::GetResources()
//
// Description:	Interrogate the static database for Serial resources 
//		for the specified device. This routine can also be used
//		to initialise the class default resources.
//
// Arg(s) In:	char *res_name - name in the database to interrogate
//
// Arg(s) Out:	long *error - pointer to error code if routine fails.
//-=======================================================================
long Serial::GetResources (char *dev_name, long *error)
{
 int	ires;
 char 	*parity_string;
 short 	charlength, stopbits;


 //
 // Serial resource table used to access the static database
 //
 static db_resource res_table[] =
 {
  {"serialline",	D_STRING_TYPE,	NULL},
  {"timeout",		D_SHORT_TYPE,	NULL},
  {"parity",		D_STRING_TYPE,	NULL},
  {"charlength",	D_SHORT_TYPE,	NULL},
  {"stopbits",		D_SHORT_TYPE,	NULL},
  {"baudrate",		D_SHORT_TYPE,	NULL},
  {"newline",		D_SHORT_TYPE,	NULL},
 };
 static int res_tab_size = sizeof(res_table)/sizeof(db_resource);




 *error = 0;



 //
 // Prepare to interrogate the static database 
 //
 ires = 0;
 res_table[ires].resource_adr	= &(this->serialline.serialline);
 ires++;
 res_table[ires].resource_adr	= &(this->serialline.timeout);
 ires++;
 parity_string = NULL;
 res_table[ires].resource_adr	= &parity_string;
 ires++;
 charlength = -1;
 res_table[ires].resource_adr	= &charlength;
 ires++;
 stopbits = -1;
 res_table[ires].resource_adr	= &stopbits;
 ires++;
 res_table[ires].resource_adr	= &(this->serialline.baudrate);
 ires++;
 res_table[ires].resource_adr	= &(this->serialline.newline);
 ires++;



 //
 // Interrogate the static database 
 //
 if(db_getresource(dev_name,res_table,res_tab_size,error))
 {
  printf("Serial::GetResources(): db_getresource() failed, error %d\n",
	*error);
  return(DS_NOTOK);
 }
 else
 {
  if(Serial::debug) 
   printf("Serial::GetResources(): values read for %s\n", dev_name);
 }



 //
 // If parity specified then convert it to an integer
 //
 if (parity_string != NULL)
 {
  if (strcmp(parity_string,"even") == 0)
  {
   this->serialline.parity = SL_EVEN;
  }
  else
  {
   if (strcmp(parity_string,"odd") == 0)
   {
    this->serialline.parity = SL_ODD;
   }
   else
   {
    this->serialline.parity = SL_NONE;
   }
  }
 }



 //
 // Do the same for the number of data bits
 //
 switch(charlength)
 {
  case 5:
   	this->serialline.charlength = SL_DATA5;
	break;
  case 6:
    	this->serialline.charlength = SL_DATA6;
	break;
  case 7:
     	this->serialline.charlength = SL_DATA7;
	break;
  case 8:
     	this->serialline.charlength = SL_DATA8;
	break;
  default:
	break;
 }



 //
 // Do the same for the number of stop bits, note 1.5 stopbits
 // are not supported via the database !
 //
 switch(stopbits)
 {
  case 1:
  	this->serialline.stopbits = SL_STOP1;
	break;
  case 2:
   	this->serialline.stopbits = SL_STOP2;
	break;
  default:
	break;
 }


 return(DS_OK);
}





//+======================================================================
// Function:    Serial::ClassInitialise()
//
// Description:	Initialise the Serial, is called once for
//		this class per process. ClassInitialise() will initialise
//		the class variables 
//
// Arg(s) In:	none
//
// Arg(s) Out:	long *error - pointer to error code if routine fails.
//-=======================================================================

long Serial::ClassInitialise (long *error)
{
 char res_path[]="CLASS/Serial/DEFAULT";
 int iret=0;
 short	dbg;


 //
 // Serial resource table used to access the static database
 //
 static db_resource res_table[] =
 {
  {"debug",	D_SHORT_TYPE,	NULL},
 };
 static int res_tab_size = sizeof(res_table)/sizeof(db_resource);




 
 // To force the compiler to include the RcsId string in the executable
// int l = strlen(RcsId);

 // ClassInitialiase must be called once only even if there are several objets.
 Serial::class_inited = 1;

 //
 // Prepare to interrogate the static database 
 //
 res_table[0].resource_adr	= &(dbg);

 //
 // No debug information by default
 //
 dbg = 0;

 //
 // Interrogate the static database 
 //
 if(db_getresource(res_path,res_table,res_tab_size,error))
 {
  printf("Serial::ClassInitialise(): db_getresource() failed, error %d\n",
	*error);
  iret = DS_NOTOK;
 }
 else
 {
  printf("Serial::ClassInitialise(): values read for %s\n", res_path);
  printf("Serial::ClassInitialise(): debug: %d\n", dbg);
 }


 //
 // Keep debug level information in class scope
 //
 Serial::debug = dbg;
 printf("Serial::ClassInitialise(): Serial::debug: %d\n", Serial::debug);

 printf ("Serial::ClassInitialise(): returning %d\n", iret);

 return(iret);
}





//+======================================================================
// Function:    Serial::Serial()
//
// Description:	create a Serial object. This involves allocating
//		memory for this object and initialising its name.
//
// Arg(s) In:	char *name - name of object.
//
// Arg(s) Out:	DevServer *ds_ptr - pointer to object created.
//		long *error - pointer to error code (in case of failure)
//-=====================================================================

Serial::Serial (char *name, long *error)
              :Device (name, error)
{
 short 			short_array[20];
 DevVarShortArray	dargin_varsharr;

 //
 // Commands list available
 //
 this->commands_list[DevSerWriteString] = DeviceCommandListEntry(
   DevSerWriteString, (DeviceMemberFunction)(&Serial::SerWriteString), D_STRING_TYPE, D_LONG_TYPE, 0, "DevSerWriteString");
 this->commands_list[DevSerWriteChar] = DeviceCommandListEntry(
   DevSerWriteChar, (DeviceMemberFunction)(&Serial::SerWriteChar), D_VAR_CHARARR, D_LONG_TYPE, 0, "DevSerWriteChar");
 this->commands_list[DevSerReadString] = DeviceCommandListEntry(
   DevSerReadString, (DeviceMemberFunction)(&Serial::SerReadString), D_LONG_TYPE, D_STRING_TYPE, 0, "DevSerReadString");
 this->commands_list[DevSerReadChar] = DeviceCommandListEntry(
   DevSerReadChar, (DeviceMemberFunction)(&Serial::SerReadChar), D_LONG_TYPE, D_VAR_CHARARR, 0, "DevSerReadChar");
 this->commands_list[DevSerSetParameter] = DeviceCommandListEntry(
   DevSerSetParameter, (DeviceMemberFunction)(&Serial::SerSetParameter), D_VAR_SHORTARR, D_VOID_TYPE, 0, "DevSerSetParameter");
 this->commands_list[DevReset] = DeviceCommandListEntry(
   DevReset, (DeviceMemberFunction)(&Serial::Reset), D_VOID_TYPE, D_VOID_TYPE, 0, "DevReset");
 this->commands_list[DevState] = DeviceCommandListEntry(
   DevState, (DeviceMemberFunction)(&Serial::State), D_VOID_TYPE, D_SHORT_TYPE, 0, "DevState");
 this->commands_list[DevStatus] = DeviceCommandListEntry(
   DevStatus, (DeviceMemberFunction)(&Serial::Status), D_VOID_TYPE, D_STRING_TYPE, 0, "DevStatus");





 //
 // program defaults for this class - these have been judiciously chosen by AG
 //
 this->serialline.serialline 	= NULL;
 this->serialline.serialin 	= 0;
 this->serialline.serialout 	= 0;
 this->serialline.timeout 	= 25;
 this->serialline.parity 	= SL_NONE;
 this->serialline.charlength	= SL_DATA8;
 this->serialline.stopbits 	= SL_STOP1;
 this->serialline.baudrate 	= 9600;
 this->serialline.newline 	= 13;
 this->serialline.xon 		= 0;
 this->serialline.xoff 		= 0;
 this->serialline.upper 	= 0;
 this->serialline.erase 	= 0;
 this->serialline.echo 		= 0;
 this->serialline.linefeed 	= 0;
 this->serialline.null 		= 0;
 this->serialline.backspace 	= 0;
 this->serialline.del	 	= 0;


 //
 // check to see if Serial::ClassInitialise has been called
 //
 if (Serial::class_inited != 1)
 {
  if (Serial::ClassInitialise(error) != DS_OK)
  {
   printf("Serial::Serial(): returning\n");
   return;
  }
 }



 //
 // Serial is a subclass of Device
 //
 this->class_name = (char*)malloc(strlen("SerialClass")+1);
 sprintf(this->class_name,"SerialClass");



 //
 // Initialise the commands list to point to the commands list 
 // implemented for the Serial class
 //
 this->n_commands = this->commands_list.size();



 //
 // First free any previously allocated strings to avoid memory leaks
 //
 if (this->serialline.serialline != NULL)
 {
  free(this->serialline.serialline);
  this->serialline.serialline = NULL;
 }



 //
 // Read device resources from database
 //
 if (this->GetResources(this->name,error) != DS_OK)
 {
  printf("Serial::Serial(): device GetResources() failed, error %d\n",*error);
  return;
 }




 //
 // Configure the serial line if there were defined in the database
 //
 if (this->serialline.serialline != NULL)
 {
  //
  // Open the serial line 
  //
  if (Open_desc() != DS_OK)
  {
   *error =  DevErr_SerialLineInitFailed;
   printf("Serial::Serial(): returning\n");
   return;
  }


  //
  // Setup input serial line with device's default values
  //
  short_array[0]	= SL_TIMEOUT;
  short_array[1]	= this->serialline.timeout;

  short_array[2]	= SL_PARITY;
  short_array[3]	= this->serialline.parity;
   
  short_array[4]	= SL_CHARLENGTH;
  short_array[5]	= this->serialline.charlength;
   
  short_array[6]	= SL_STOPBITS;
  short_array[7]	= this->serialline.stopbits;

  short_array[8]	= SL_BAUDRATE;
  short_array[9]	= this->serialline.baudrate;

  short_array[10]	= SL_NEWLINE;
  short_array[11]	= this->serialline.newline;

  dargin_varsharr.length = 12;
  dargin_varsharr.sequence = short_array;

  if (SerSetParameter(&dargin_varsharr,NULL,error) != DS_OK)
  {
   printf("Serial::Serial(): problems setting input parameters, error %d\n",
	*error);
  }
 }
 else
 {
  printf("Serial::Serial(): no serial line to initialise !\n");
 }



 //
 // Set initial state of Serial according to internal state of card
 //
 this->state = DEVON;


 if(Serial::debug) 
  printf("Serial::Serial(): returning\n");

 return;
}





//+=====================================================================
// Function:	Serial::~Serial()
//
// Description:	destructor to destroy an object of the Serial class
//
// input:	none
//
// Output:	none
//
//-=====================================================================
Serial::~Serial()
{


 //
 // check to see if Serial class has already been destroyed
 //
 if (Serial::class_destroyed != 1)
 {
  if(Serial::debug) 
   printf("Serial::~Serial(): Destroying Serial class\n");


  //
  // Close files opened for reading and writing
  //
  if(Serial::debug) 
   printf("Serial::~Serial(): closing serial lines in and out\n");
  close(this->serialline.serialin);
  close(this->serialline.serialout);


  Serial::class_destroyed = 1;
 }

}


//+======================================================================
// Function:    Serial::StateMachine()
//
// Description:	Check if the command to be executed does not violate
//		the present state of the device.
//
// Arg(s) In:	DevCommand cmd - command to be executed.
//
// Arg(s) Out:	long *error - pointer to error code (in case of failure).
//-=====================================================================
long Serial::StateMachine (long cmd, long *error)
{
 long iret = 0;
 long int p_state, n_state;

 p_state = this->state;

//
// before checking out the state machine assume that the state
// doesn't change i.e. new state == old state
//
 n_state = p_state;

 switch (p_state)
 {
    
   	case (DEVON) :	/* device ON and everything OK */
   	{
   		switch (cmd)
   		{
   			/* Allowed Command(s) */

   			case (DevStatus):	break;
   			case (DevState):	break;
   		}
   	}
   	break;

   	case (DEVFAULT) :	/* device FAULT detected, need to reset */
   	{
   		switch (cmd)
   		{
   			/* Allowed Command(s) */

   			case (DevReset):	n_state = DEVON;break;
   			case (DevStatus):	break;
   			case (DevState):	break;
   		}
   	}
   	break;

   	default:
   		*error = DevErr_UnrecognisedState;
   		iret = DS_NOTOK;
   		break;
 }


 this->n_state = n_state;

 return(iret);
}




//+=====================================================================
// Function:	Serial::SerWriteString()
//
// Description:	write a string of characters to a serial line  and 
//		return the number of characters written
//   	
// Arg(s) In:	DevString *argin  - string to write
//   				  
// Arg(s) Out:	DevLong *argout - no. of characters written
//		long *error - pointer to error code, in case
// 			      routine fails. possible error code(s)
//-=====================================================================
long Serial::SerWriteString ( void *vargin, void *vargout, long *error)
{
 int 			nchar;
 static DevString 	*argin;
 static DevLong 	*argout;




 argin 	= (DevString *) vargin;
 argout	= (DevLong *) vargout;


 if(Serial::debug)
  printf("Serial::SerWriteString(): %s\n",*argin);


 if((nchar = write(this->serialline.serialout,*argin,strlen(*argin))) < 0)
 {
  printf("Serial::SerWriteString(): error writing to devive (errno=%d)\n",
	errno);
  perror(this->serialline.serialline);

  //TODO: check that ETIMEDOUT handles correctly the timeout error
  if(errno == ETIMEDOUT)
  {
   *error = DevErr_DeviceTimedOut;
  }
  else
  {
   *error = DevErr_DeviceWrite;
  }

  return(DS_NOTOK);
 }


 *argout = nchar;


 return(DS_OK);
}




//+=====================================================================
// Function:	Serial::SerWriteChar()
//
// Description:	write N characters to a serial line and return the
//		number of characters written
//   	
// Arg(s) In:	DevVarCharArray *argin  - characters to write
//   				  
// Arg(s) Out:	DevLong *argout - no. of characters written
//		long *error - pointer to error code, in case
//		 	      routine fails; possible error code(s) are
//-=====================================================================
long Serial::SerWriteChar( void *vargin, void *vargout, long *error)
{
 int 			nchar;
 static DevVarCharArray	*argin;
 static DevLong 	*argout;



 argin 	= (DevVarCharArray *) vargin;
 argout	= (DevLong *) vargout;


 if(Serial::debug)
  printf("Serial::SerWriteChar(): length:%d  %s\n",
	argin->length,
	argin->sequence);

 if((nchar = write(
	this->serialline.serialout,
	argin->sequence,
	argin->length)) < 0)
 {
  printf("Serial::SerWriteChar(): error writing to devive (errno=%d)\n",
	errno);
  perror(this->serialline.serialline);

  //TODO: check that ETIMEDOUT handles correctly the timeout error
  if(errno == ETIMEDOUT)
  {
   *error = DevErr_DeviceTimedOut;
  }
  else
  {
   *error = DevErr_DeviceWrite;
  }

  return(DS_NOTOK);
 }


 *argout = nchar;


 return(DS_OK);
}



//+=====================================================================
// Function:	Serial::SerReadString()
//
// Description:	read a string of character, the type of read is specified
//		in the input parameter, it can be one of SL_RAW, SL_LINE
//		or SL_NCHAR
//   	
// Arg(s) In:	long *argin  - type of read
//   				  
// Arg(s) Out:	*argout - string read
//		long *error - pointer to error code, in case
//	 	              routine fails; possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::SerReadString ( void *vargin, void *vargout, long *error)
{
 static DevLong 	*argin;
 static DevString 	*argout;
 long 			read_type;
 long			nchar;



 argin	= (DevLong *) vargin;
 argout = (DevString *) vargout;


 read_type = *argin & 0x000f;


 switch (read_type)
 {
  case SL_RAW :
 	if(Serial::debug)
  	 printf("Serial::SerReadString(): SL_RAW\n");
	if (RawReadString(NULL,argout,error) != DS_OK)
	{
	 return(DS_NOTOK);
	}

	break;

  case SL_NCHAR : 
	nchar = *argin >> 8;
 	if(Serial::debug)
  	 printf("Serial::SerReadString(): SL_NCHAR nchar=%d\n", nchar);

	if (NCharReadString(&nchar,argout,error) != DS_OK)
	{
	 return(DS_NOTOK);
	}
               	
	break;

  case SL_LINE :  
 	if(Serial::debug)
  	 printf("Serial::SerReadString(): SL_LINE\n");
	if (LineReadString(NULL,argout,error) != DS_OK)
	{
 	 return(DS_NOTOK);
	}
               
	break;

  default :
	*error = DevErr_UnknownInputParameter;
	return(DS_NOTOK);
 }

 return(DS_OK);
}



//+=====================================================================
// Function:	Serial::SerReadChar()
//
// Description:	read an array of characters, the type of read is specified
//		in the input parameter, it can be one of SL_RAW, SL_LINE
//		or SL_NCHAR
//   	
// Arg(s) In:	long *argin  - type of read
//   				  
// Arg(s) Out:	DevVarCharArray *argout - array of characters read
//		long *error - pointer to error code, in case
//	 	              routine fails; possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::SerReadChar ( void *vargin, void *vargout, long *error)
{
 static DevLong 	*argin;
 static DevVarCharArray *argout;
 long 			read_type;
 long			nchar;



 argin	= (DevLong *) vargin;
 argout = (DevVarCharArray *) vargout;


 read_type = *argin & 0x000f;


 switch (read_type)
 {
  case SL_RAW :
 	if(Serial::debug)
  	 printf("Serial::SerReadChar(): SL_RAW\n");
	if (RawReadChar(NULL,argout,error) != DS_OK)
	{
	 return(DS_NOTOK);
	}

	break;

  case SL_NCHAR : 
	nchar = *argin >> 8;
 	if(Serial::debug)
  	 printf("Serial::SerReadChar(): SL_NCHAR nchar=%d\n", nchar);

	if (NCharReadChar(&nchar,argout,error) != DS_OK)
	{
	 return(DS_NOTOK);
	}
               	
	break;

  case SL_LINE :  
 	if(Serial::debug)
  	 printf("Serial::SerReadChar(): SL_LINE\n");
	if (LineReadChar(NULL,argout,error) != DS_OK)
	{
 	 return(DS_NOTOK);
	}
               
	break;

  default :
	*error = DevErr_UnknownInputParameter;
	return(DS_NOTOK);
 }


 return(DS_OK);
}





//+=====================================================================
// Function:	Serial::RawReadString()
//
// Description:	read a string of characters from the serial line
//		if there are no characters to be read return an
//		empty string.
//   	
//		The maximum number of characters that can be read is
//		SL_MAXSTRING, if there are more characters in the received
//		buffer, they are let there for the next read.
//
// Arg(s) In:	DevVoid *argin  - none
//   				  
// Arg(s) Out:	DevString *argout - string read
//		long *error - pointer to error code, in case
//		  	      routine fails. possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::RawReadString (DevVoid *argin, DevString *argout, long *error)
{
 int	nchar;



 //
 //first "empty" buffer by null terminating it
 //
 buffer[0] = 0;

 //
 // Read the current contains of the buffer but no more than we can
 // store in our receiving buffer.
 //
 // As we are in non canonical mode (i.e. raw mode) the read won't be blocking.
 //
 nchar = SL_MAXSTRING;
 nchar = read(this->serialline.serialin,buffer,nchar);
 if (nchar < 0)
 {
  printf("Serial::RawReadString(): error reading from devive (errno=%d)\n",
	errno);
  perror(this->serialline.serialline);

  //TODO: check that ETIMEDOUT handles correctly the timeout error
  if(errno == ETIMEDOUT)
  {
   *error = DevErr_DeviceTimedOut;
  }
  else
  {
   *error = DevErr_DeviceRead;
  }

  return(DS_NOTOK);
 }
 else
 {
  buffer[nchar] = 0;
 }


      
 if(Serial::debug)
  printf("Serial::RawReadString(): nchar=%d buffer:%s\n",nchar,buffer);
   

 *argout = buffer;

 return(DS_OK);
}





//+=====================================================================
// Function:	Serial::NCharReadString()
//
// Description:	read a string of N characters from the serial line
//		if there are no characters to be read return an
//		empty string.
//   	
// Arg(s) In:	DevLong *argin  - number of characters to read
//   				  
// Arg(s) Out:	DevString *argout - string read
//		long *error - pointer to error code, in case
//		  	      routine fails. possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::NCharReadString (DevLong *argin, DevString *argout, long *error)
{
 fd_set		watchset;	// file descriptor set
 fd_set		inset;		// file descriptor set updated by select()
 int		maxfd;		// maximum file descriptor used
 struct timeval	timeend;	// current time + timeout
 struct timeval	timeout;	// timeout value
 float		timeout_s;	// timeout value in seconds
 struct timezone tz;		// not used
 int		readyfd;	// number of file descriptors ready to be read
 int		nchar;		// number of characters requested
 int		ncharin; 	// number of characters in the receiving buffer
 int		ncharread; 	// number of characters yet read





 nchar = *argin;


 //
 //first "empty" buffer by null terminating it
 //
 buffer[0] = 0;
 ncharread = 0;


 //
 // Don't read more than we can store
 //
 if (nchar > SL_MAXSTRING)
 {
  printf("Serial::NCharReadString(): WARNING, number char requested > %d\n",
	SL_MAXSTRING);
  nchar = SL_MAXSTRING;
 }


 //
 // Initialize the set, no file descriptors contained
 //
 FD_ZERO(&watchset);


 //
 // Add to the set the file descriptor to watch at
 //
 FD_SET(this->serialline.serialin, &watchset); 


 //
 // Initialize the timeout (calculate when the timeout should expire)
 //
 timeout_s = ((float)this->serialline.timeout) / 256.0; // seconds

 gettimeofday(&timeend,&tz);
 timeend.tv_usec += (int)((timeout_s - (int)timeout_s) * 1000000.0);
 timeend.tv_sec  += (int)(timeout_s); 



 //
 // Wait until the receiving buffer contains the requested number of characters
 //
 do
 {


  //
  // Set the maximum file descriptor used, this avoid the system to look
  // through all the file descriptors that a Linux process can have (up to 1024)
  //
  maxfd = this->serialline.serialin + 1;


  //
  // As select() will update will update the set, make a copy of it
  //
  inset = watchset;

  //
  // Set the timeout value (take into account time consumed by previous
  // wait periods)
  //
  gettimeofday(&timeout,&tz);
  timeout.tv_usec = timeend.tv_usec - timeout.tv_usec;
  timeout.tv_sec  = timeend.tv_sec  - timeout.tv_sec;
  if(timeout.tv_usec < 0)
  {
   timeout.tv_usec += 1000000;
   timeout.tv_sec  -= 1;
  }




  //
  // Block until characters become available on the file descriptor
  // listed in the set.
  //
  readyfd = select(maxfd, &inset, NULL, NULL, &timeout);


  //
  // Check if an error occured
  //
  if (readyfd < 0)
  {
   printf("Serial::NCharReadString(): error on select()\n");
   *error = DevErr_DeviceRead;

   return(DS_NOTOK);
  }


  //
  // Check if the timeout occured
  //
  if (readyfd == 0)
  {
   printf("Serial::NCharReadString(): timeout waiting for char to be read\n");
   *error = DevErr_DeviceTimedOut;

   return(DS_NOTOK);
  }


  //
  // Check if it's our file descriptor which is ready to be read (Should be, as
  // it was the only watched one out).
  //
  if (!FD_ISSET(this->serialline.serialin, &inset))
  {
   printf("Serial::NCharReadString(): fd_isset() wrong, who whoke me up?\n");
   *error = DevErr_DeviceRead;

   return(DS_NOTOK);
  }


  //
  // Are there enough characters in the receiving buffer?
  //
  if (ioctl(this->serialline.serialin, FIONREAD, &ncharin) < 0)
  {
   printf("Serial::NCharReadString(): error reading no. of char. (errno=%d)\n",
	 errno);
   perror(this->serialline.serialline);
   *error = DevErr_DeviceRead;
 
   return(DS_NOTOK);
  }

  if(Serial::debug)
   printf("Serial::NCharReadString(): ncharin=%d\n", ncharin);



  //
  // Empty the receiving buffer (if not done, the next call to select() will
  // immediately return that this serialline has to be read, therefore
  // the waiting loop would be very system consuming)
  //
  ncharin = read(
	this->serialline.serialin,
	buffer + ncharread,
	(ncharin > (nchar - ncharread) ? (nchar - ncharread) : ncharin));
  if (ncharin < 0)
  {
   printf("Serial::NCharReadString(): error reading from devive (errno=%d)\n",
	errno);
   perror(this->serialline.serialline);
   *error = DevErr_DeviceRead;
 
   return(DS_NOTOK);
  }
  ncharread += ncharin;

 }
 while(ncharread < nchar);


 //
 // At this point the number of characters requested have been read.
 //
 buffer[ncharread] = 0;



      
 if(Serial::debug)
  printf("Serial::NCharReadString(): nchar=%d buffer:%s\n",nchar,buffer);
   

 *argout = buffer;


 return(DS_OK);
}




//+=====================================================================
// Function:	LineReadString()
//
// Description:	read a string of N characters from the serial line
//		up and including the next newline 
//
// Arg(s) In:	DevVoid *argin  - none
//   				  
// Arg(s) Out:	DevString *argout - string read
//		long *error - pointer to error code, in case
//		  	      routine fails. possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::LineReadString (DevVoid *argin, DevString *argout, long *error)
{
 fd_set		watchset;	// file descriptor set
 fd_set		inset;		// file descriptor set updated by select()
 int		maxfd;		// maximum file descriptor used
 struct timeval	timeend;	// current time + timeout
 struct timeval	timeout;	// timeout value
 float		timeout_s;	// timeout value in seconds
 struct timezone tz;		// not used
 int		readyfd;	// number of file descriptors ready to be read
 int		nchar;		// number of characters currently read
 int		ncharin; 	// number of characters in the receiving buffer
 int		ncharread; 	// number of characters yet read
 int		eolfound;	// flag set when end of line found







 //
 //first "empty" buffer by null terminating it
 //
 buffer[0] = 0;
 ncharread = 0;
 eolfound  = 0;


 //
 // initialise output argument even before reading from serial line
 // might prevent clients from crashing - andy 17/11/98
 //
 *argout = buffer;


 //
 // Initialize the set, no file descriptors contained
 //
 FD_ZERO(&watchset);


 //
 // Add to the set the file descriptor to watch at
 //
 FD_SET(this->serialline.serialin, &watchset); 


 //
 // Initialize the timeout (calculate when the timeout should expire)
 //
 timeout_s = ((float)this->serialline.timeout) / 256.0; // seconds

 gettimeofday(&timeend,&tz);
 timeend.tv_usec += (int)((timeout_s - (int)timeout_s) * 1000000.0);
 timeend.tv_sec  += (int)(timeout_s); 



 //
 // Wait until the receiving buffer contains the end of line character
 //
 do
 {


  //
  // Set the maximum file descriptor used, this avoid the system to look
  // through all the file descriptors that a Linux process can have (up to 1024)
  //
  maxfd = this->serialline.serialin + 1;


  //
  // As select() will update will update the set, make a copy of it
  //
  inset = watchset;


  //
  // Set the timeout value (take into account time consumed by previous
  // wait periods)
  //
  gettimeofday(&timeout,&tz);
  timeout.tv_usec = timeend.tv_usec - timeout.tv_usec;
  timeout.tv_sec  = timeend.tv_sec  - timeout.tv_sec;
  if(timeout.tv_usec < 0)
  {
   timeout.tv_usec += 1000000;
   timeout.tv_sec  -= 1;
  }




  //
  // Block until characters become available on the file descriptor
  // listed in the set.
  //
  readyfd = select(maxfd, &inset, NULL, NULL, &timeout);


  //
  // Check if an error occured
  //
  if (readyfd < 0)
  {
   printf("Serial::LineReadString(): error on select()\n");
   *error = DevErr_DeviceRead;

   return(DS_NOTOK);
  }


  //
  // Check if the timeout occured
  //
  if (readyfd == 0)
  {
   printf("Serial::LineReadString(): timeout waiting for char to be read\n");
   *error = DevErr_DeviceTimedOut;

   return(DS_NOTOK);
  }


  //
  // Check if it's our file descriptor which is ready to be read (Should be, as
  // it was the only watched one out).
  //
  if (!FD_ISSET(this->serialline.serialin, &inset))
  {
   printf("Serial::LineReadString(): fd_isset() wrong, who whoke me up?\n");
   *error = DevErr_DeviceRead;

   return(DS_NOTOK);
  }


  //
  // Are there enough characters in the receiving buffer?
  //
  if (ioctl(this->serialline.serialin, FIONREAD, &ncharin) < 0)
  {
   printf("Serial::LineReadString(): error reading no. of char. (errno=%d)\n",
	 errno);
   perror(this->serialline.serialline);
   *error = DevErr_DeviceRead;
 
   return(DS_NOTOK);
  }

  if(Serial::debug)
   printf("Serial::LineReadString(): ncharin=%d\n", ncharin);


  //
  // Read the receiving buffer, character per character, looking for
  // the end of line predefined.
  //   	
  // TODO: implement in more efficient way (using double
  // buffering or fgets() ??) to do less system calls.
  //
  do
  {


   //
   // Empty the receiving buffer (if not done, the next call to select() will
   // immediately return that this serialline has to be read, therefore
   // the waiting loop would be very system consuming)
   //
   if (read(this->serialline.serialin, buffer + ncharread, 1) < 0)
   {
    printf("Serial::LineReadString(): error reading from devive (errno=%d)\n",
	errno);
    perror(this->serialline.serialline);
    *error = DevErr_DeviceRead;
 
    return(DS_NOTOK);
   }
   ncharin--;

   if(Serial::debug)
    printf("Serial::LineReadString(): char read = 0x%02x %c\n",
	buffer[ncharread],
	buffer[ncharread]>32?buffer[ncharread]:' ');

   if(buffer[ncharread] == (char)(this->serialline.newline))
    eolfound = 1;

   ncharread++;

  }
  while((ncharin > 0) && (!eolfound));
  

 }
 while(!eolfound);


 //
 // At this point the newline has been read.
 //
 buffer[ncharread] = 0;



      
 if(Serial::debug)
  printf("Serial::LineReadString(): ncharread=%d buffer:%s\n",ncharread,buffer);
   


 return(DS_OK);
}




//+=====================================================================
// Function:	Serial::RawReadChar()
//
// Description:	read an array of characters from the serial line
//		if there are no characters to be read return an
//		empty array.
//   	
// Arg(s) In:	DevVoid *argin  - none
//   				  
// Arg(s) Out:	DevString *argout - string read
//		long *error - pointer to error code, in case
//		  	      routine fails. possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::RawReadChar (DevVoid *argin, DevVarCharArray *argout, long *error)
{
 static DevString 	*tmp_argout;
 int 	i;






 //
 // Use the char pointer to be updated:
 //	(tmp_argout) -> (argout->sequence) -> (buffer)
 //
 tmp_argout = &(argout->sequence);


 //
 // Use the same command as for string
 //
 if (RawReadString(NULL,tmp_argout,error) != DS_OK)
 {
  return(DS_NOTOK);
 }


 //
 // Update the number of characters read
 //
 argout->length   = strlen(*tmp_argout);


 if(Serial::debug)
 {
  for(i=0;i <= (argout->length); i++)
   printf("Serial::RawReadChar(): char return = 0x%02x %c\n",
	argout->sequence[i],
	argout->sequence[i]>32?argout->sequence[i]:' ');
 }

 return(DS_OK);
}




//+=====================================================================
// Function:	Serial::NCharReadChar()
//
// Description:	read an array of N characters from the serial line
//		if there are no characters to be read return an
//		empty array.
//   	
// Arg(s) In:	DevLong *argin  - number of characters to read
//   				  
// Arg(s) Out:	DevString *argout - string read
//		long *error - pointer to error code, in case
//		  	      routine fails. possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::NCharReadChar (DevLong *argin, DevVarCharArray *argout,long *error)
{
 static DevString 	*tmp_argout;
 int 	i;





 //
 // Use the char pointer to be updated:
 //	(tmp_argout) -> (argout->sequence) -> (buffer)
 //
 tmp_argout = &(argout->sequence);



 //
 // Use the same command as for string
 //
 if (NCharReadString(argin, tmp_argout, error) != DS_OK)
 {
  return(DS_NOTOK);
 }



 //
 // Update the number of characters read
 //
 argout->length   = strlen(*tmp_argout);


 if(Serial::debug)
 {
  for(i=0;i <= (argout->length); i++)
   printf("Serial::RawReadChar(): char return = 0x%02x %c\n",
	argout->sequence[i],
	argout->sequence[i]>32?argout->sequence[i]:' ');
 }

 return(DS_OK);
}




//+=====================================================================
// Function:	Serial::LineReadChar()
//
// Description:	read an array of N characters from the serial line
//		up and including the next newline 
//   	
// Arg(s) In:	DevVoid *argin  - none
//   				  
// Arg(s) Out:	DevString *argout - string read
//		long *error - pointer to error code, in case
//		  	      routine fails. possible error code(s)
//                              are DevErr_DeviceTimeout
//-=====================================================================
long Serial::LineReadChar (DevVoid *argin, DevVarCharArray *argout, long *error)
{
 static DevString 	*tmp_argout;
 int 	i;





 //
 // Use the char pointer to be updated:
 //	(tmp_argout) -> (argout->sequence) -> (buffer)
 //
 tmp_argout = &(argout->sequence);



 //
 // Use the same command as for string
 //
 if (LineReadString(NULL, tmp_argout, error) != DS_OK)
 {
  return(DS_NOTOK);
 }



 //
 // Update the number of characters read
 //
 argout->length   = strlen(*tmp_argout);


 if(Serial::debug)
 {
  for(i=0;i <= (argout->length); i++)
  printf("Serial::LineReadChar(): char return = 0x%02x %c\n",
	argout->sequence[i],
	argout->sequence[i]>32?argout->sequence[i]:' ');
 }

 return(DS_OK);
}











//+=====================================================================
// Function:	Serial::SerSetParameter()
//
// Description:	set serial line parameters
//   	
// Arg(s) In:	DevVarShortArray *argin  - device parameters, in pairs
//   				  
// Arg(s) Out:	*argout - none
//		long *error - pointer to error code, in case
//		 	      routine fails. possible error code(s):
//
//-=====================================================================
long Serial::SerSetParameter ( void *vargin, void *vargout, long *error)
{
 static DevVarShortArray *argin;
 struct termios		termin;
 struct termios		termout;
 int			i;

 short			timeout;
 float			timeoutl;
 short			parity;
 unsigned int		par;
 short			charlength;
 unsigned int		cs;
 short			stopbits;
 unsigned int		stopb;
 short			baudrate;
 speed_t		speed;





 argin = (DevVarShortArray *) vargin;




 //
 // First get the existing serial line setup from the file descriptors opened
 // for input and for output
 //
 if (tcgetattr(this->serialline.serialin, &termin) < 0)
 {
  printf("Serial::SerSetParameter(): tcgetattr() error on in line(errno=%d)\n",
	errno);
  perror(this->serialline.serialline);
  *error =  DevErr_SerialLineInitFailed;

  return(DS_NOTOK); 
 }


 if (tcgetattr(this->serialline.serialout, &termout) < 0)
 {
  printf("Serial::SerSetParameter(): tcgetattr() error on out line(errno=%d)\n",
	errno);
  perror(this->serialline.serialline);
  *error =  DevErr_SerialLineInitFailed;

  return(DS_NOTOK); 
 }
   


 //
 // Setup default arbitrary behaviour:
 //
 //	TODO: seems dangerous to start from the current settings instead
 //	of from blank settings, some flags not set here may be forced from
 //	the ouside using stty.
 //
 
	//
	// Avoid any side effect as the two descriptors have been opened
	// on the same physical device.
 	//
 termin.c_oflag     	= 0;
 termout.c_iflag    	= 0;

	//
	// Read return immediately
	//
 termin.c_cc[VMIN]   	= 0;	
 termin.c_cc[VTIME]  	= 0;	

 termout.c_cc[VMIN]  	= 0;	// should not be needed for out line
 termout.c_cc[VTIME] 	= 0;

	//
	// Non canonical mode, i.e. raw mode
	//
 termin.c_lflag     	&= ~ICANON;
 termout.c_lflag    	&= ~ICANON;	// should not be needed for out line

	//
	// Do not echo
	//
 termin.c_lflag     	&= ~(ECHO | ECHOCTL | ECHONL | ECHOE | ECHOKE | ECHOK);
 termout.c_lflag    	&= ~(ECHO | ECHOCTL | ECHONL | ECHOE | ECHOKE | ECHOK);

	//
	// Do not activate implementation-dependent processing of characters
	//
 termin.c_lflag     	&= ~(IEXTEN);
 termout.c_lflag     	&= ~(IEXTEN);

	//
	// Control characters don't cause corresponding signals 
	// (INTR -> SIGINT, QUIT -> SIGQUIT, SUSP -> SIGTSTP)
	//
 termin.c_lflag     	&= ~ISIG;
 termout.c_lflag    	&= ~ISIG;	// should not be needed for out line

	//
	// Ignore modem control lines
	//
 termin.c_cflag    	|= CLOCAL;
 termout.c_cflag    	|= CLOCAL;

	//
	// Allow receiving characters
	//
 termin.c_cflag    	|= CREAD;

	//
	// Ignore hardware flow control (RTS and CTS lines)
	//
 termin.c_cflag    	&= ~(CRTSCTS | HUPCL);
 termout.c_cflag    	&= ~(CRTSCTS | HUPCL);

	//
	// Ignore break conditions received
	//
 termin.c_iflag     	|= IGNBRK;
 termin.c_iflag     	&= ~BRKINT;

	//
	// Ignore bytes received with parity error
	//
 termin.c_iflag     	|= IGNPAR;
 termin.c_iflag     	&= ~INPCK;

	//
	// XON/XOFF switched off
	//
 termin.c_iflag     	&= ~(IXON | IXOFF);

	//
	// Do not convert upper-case characters to lower-case ones and
	// vice-versa
	//
 termin.c_iflag     	&= ~IUCLC;
 termout.c_oflag     	&= ~OLCUC;

	//
	// Do not convert \n -> \r or \r -> \n and do not ignore \r
	//
 termin.c_iflag		&= ~ICRNL;
 termin.c_iflag		&= ~INLCR;
 termin.c_iflag		&= ~IGNCR;
 termout.c_oflag	&= ~ONOCR;

	//
	// Do not convert \n -> \r+\n or \r -> \n
	//
 termout.c_oflag	&= ~ONLCR;
 termout.c_oflag	&= ~OCRNL;

	//
	// Do not force column to 0 if \r or \n are sent
	//
 termout.c_oflag	&= ~ONLRET;

	//
	// Do not expand tabs to 8 spaces
	//
 termout.c_oflag	&= ~XTABS;

	//
	// Delete character forced to zero
	//
 termin.c_cc[VERASE]	= 0;	// not used in non canonical mode
 termout.c_cc[VERASE]	= 0;	// not used in non canonical mode

	//
	// Reprint line character forced to zero
	//
 termin.c_cc[VERASE]	= 0;	// not used in non canonical mode
 termin.c_cc[VREPRINT]	= 0;	// not used in non canonical mode

 termout.c_cc[VERASE]	= 0;	// not used in non canonical mode
 termout.c_cc[VREPRINT]	= 0;	// not used in non canonical mode




 //
 // Read device parameters passed
 //
 for (i=0; i<argin->length; i=i+2)
 {
  switch (argin->sequence[i]) 
  {
   case SL_TIMEOUT :        
	//
	// Setup the serial line for timeouts if timeout > 0
	// 
	// Only modifies the timeout for the file descriptor opened
	// for input
	//
	// The units of this timeout are in 1/256th of a second to
	// be compatible with the SerialLineds running under OS9.
	// Under Linux the internal units are 1/10th of a second, so
	// a conversion has first to be done.
	//
 	timeout = argin->sequence[i+1];
	if (timeout > 0)
	{
	 timeoutl = ((float)timeout * 10.0) / 256.0;

	 //
	 // In fact, the timeout on read() calls is not handle using
 	 // this VTIME parameter but using the select() command.
	 //
 	 termin.c_cc[VTIME]  = (cc_t)timeoutl;	

		// should not be done, yet done previously
	 this->serialline.timeout = timeout;

	 if(Serial::debug)
 	  printf("Serial::SerSetParameter(): timeout %d(1/256s) %d(1/10s)\n",
		timeout, termin.c_cc[VTIME]);
	}

	break;

   case SL_PARITY :
    	//
	// setup up parity according to input parameter
    	//
	parity = argin->sequence[i+1];
 	switch(parity)
	{
	 case SL_EVEN   : par = PARENB;            break;
	 case SL_ODD    : par = (PARENB | PARODD); break;
	 case SL_NONE   : par = 0;                 break;
	 default	: par = 0;                 break;
	}

	termin.c_cflag  &= ~PARENB;	// erase previous speed settings
	termin.c_cflag  |= par;

	termout.c_cflag &= ~PARENB;	// erase previous speed settings
	termout.c_cflag |= par;

	if(Serial::debug)
  	 printf("Serial::SerSetParameter(): parity %02x (none=0 odd=1 even=3) code=0x%02x\n",
		parity, par);

	break;

   case SL_CHARLENGTH :
	//
	// setup up no. of data bits per character according to input parameter
	//
	charlength = argin->sequence[i+1];
 	switch(charlength)
	{
	 case SL_DATA5	: cs = CS5; break;
	 case SL_DATA6	: cs = CS6; break;
	 case SL_DATA7	: cs = CS7; break;
	 case SL_DATA8	: cs = CS8; break;
	 default	: cs = CS8; break;
	}

	termin.c_cflag  &= ~CSIZE;	// erase previous speed settings
	termin.c_cflag  |= cs;

	termout.c_cflag &= ~CSIZE;	// erase previous speed settings
	termout.c_cflag |= cs;


	if(Serial::debug)
  	 printf("Serial::SerSetParameter(): charlength %02x (data8=0 7=1 6=2 5=3) code=0x%02x\n",
		charlength, cs);

	break;

   case SL_STOPBITS :
	//
	// setup up no. of stop bits per character according to input parameter
	//
	stopbits = argin->sequence[i+1];
 	switch(stopbits)
	{
	 case SL_STOP1	: stopb = 0;      break;
	 case SL_STOP2	: stopb = CSTOPB; break;
	 default	: stopb = 0;      break;
	}

	termin.c_cflag  &= ~CSTOPB;	// erase previous speed settings
	termin.c_cflag  |= stopb;

	termout.c_cflag &= ~CSTOPB;	// erase previous speed settings
	termout.c_cflag |= stopb;

	if(Serial::debug)
  	 printf("Serial::SerSetParameter(): stopbits=%d (stop1=0 stop2=2) code=0x%02x\n",
		stopbits, stopb);

	break;

   case SL_BAUDRATE :
	//	
	// convert BAUDRATE to speed_t values understood by termios structure
	//	
	baudrate = argin->sequence[i+1];
	switch (baudrate) 
	{
	 case 19200 : speed = B19200; break;
	 case 9600  : speed =  B9600; break;
	 case 4800  : speed =  B4800; break;
	 case 2400  : speed =  B2400; break;
	 case 1800  : speed =  B1800; break;
	 case 1200  : speed =  B1200; break;
	 case 600   : speed =   B600; break;
	 case 300   : speed =   B300; break;
	 case 150   : speed =   B150; break;
	 case 134   : speed =   B134; break;
	 case 110   : speed =   B110; break;
	 case 75    : speed =    B75; break;
	 case 50    : speed =    B50; break;

	 default    : speed =  B9600; break;
	}

	termin.c_cflag  &= ~CBAUD;	// erase previous speed settings
	termin.c_cflag  |= speed;

	termout.c_cflag &= ~CBAUD;	// erase previous speed settings
	termout.c_cflag |= speed;

	if(Serial::debug)
  	 printf("Serial::SerSetParameter(): baud rate=%d code=0x%02x\n",
		baudrate, speed);

	break;

   case SL_NEWLINE :
	//
	// Setup new NEWLINE character
	//
	// This character settings is ignore in non canonical mode, i.e
 	// raw mode (current mode used here, see above ICANON)
	//
	termin.c_cc[VEOL]  = (char)argin->sequence[i+1];
	termout.c_cc[VEOL] = (char)argin->sequence[i+1];

	if(Serial::debug)
  	 printf("Serial::SerSetParameter(): newline 0x%02x\n",
		termin.c_cc[VEOL]);

	break;

   default :

	break;

  } // end of switch(argin->sequence[i])
 }  // end of for(i)



 //
 // Now do it ! i.e. modify the open device descriptor
 //
 if ((tcsetattr(this->serialline.serialin, TCSAFLUSH, &termin)) < 0)
 {
  printf("Serial::SerSetParameter(): tcsetattr() error on in line(errno=%d)\n",
	errno);
  perror(this->serialline.serialline);
  *error =  DevErr_SerialLineInitFailed;

  return(DS_NOTOK);
 }



 if ((tcsetattr(this->serialline.serialout, TCSAFLUSH, &termout)) < 0)
 {
  printf("Serial::SerSetParameter(): tcsetattr() error on out line(errno=%d)\n",
	errno);
  perror(this->serialline.serialline);
  *error =  DevErr_SerialLineInitFailed;

  return(DS_NOTOK);
 }



 return(DS_OK);
}





//+=====================================================================
// Function:	Serial::Reset()
//
// Description:	routine to reset the serial line by closing it and
//		then reopening and reinitialising it.
//
// Arg(s) In:	DevVoid	*argin  - none
//   				  
// Arg(s) Out:	DevVoid	*argout - none
//-=====================================================================
long Serial::Reset( void *vargin, void *vargout, long *error)
{
 short 			short_array[20];
 DevVarShortArray	dargin_varsharr;




 //
 // Close files opened for reading and writing
 //
 if(Serial::debug)
  printf("Serial::Reset(): closing serial lines in and out\n");
 close(this->serialline.serialin);
 close(this->serialline.serialout);



 //
 // Configure the serial line if there were defined in the database
 //
 if (this->serialline.serialline != NULL)
 {
  //
  // Open the serial line 
  //
  if (Open_desc() != DS_OK)
  {
   *error =  DevErr_SerialLineInitFailed;
   printf("Serial::Reset(): returning\n");
   return(DS_NOTOK);
  }


  //
  // Setup input serial line with device's default values
  //
  short_array[0]	= SL_TIMEOUT;
  short_array[1]	= this->serialline.timeout;

  short_array[2]	= SL_PARITY;
  short_array[3]	= this->serialline.parity;
   
  short_array[4]	= SL_CHARLENGTH;
  short_array[5]	= this->serialline.charlength;
   
  short_array[6]	= SL_STOPBITS;
  short_array[7]	= this->serialline.stopbits;

  short_array[8]	= SL_BAUDRATE;
  short_array[9]	= this->serialline.baudrate;

  short_array[10]	= SL_NEWLINE;
  short_array[11]	= this->serialline.newline;

  dargin_varsharr.length = 12;
  dargin_varsharr.sequence = short_array;

  if (SerSetParameter(&dargin_varsharr,NULL,error) != DS_OK)
  {
   printf("Serial::Reset(): problems setting input parameters, error %d\n",
	*error);
  }
 }
 else
 {
  printf("Serial::Reset(): no serial line to initialise !\n");
 }


 return(DS_OK);
}




//+=====================================================================
// Function:    Serial::State()
//
// Description:	Return the state as a short.
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevShort *vargout - state
//		long *error - pointer to error code (in the case of failure)
//-=====================================================================

long Serial::State ( void *vargin, void *vargout, long *error)
{


 *error = 0;

 *(short*)vargout = this->state;


 return(DS_OK);
}





//+=====================================================================
// Function:    Serial::Status()
//
// Description:	Return a string with the serial line configuration parameter
//
//		TODO: should be better to return the current serial
//		line settings.
//
//		TODO: understand why the termios structure are the same
//		for in and out lines, seems to be the one used for the
//		last tcsetattr().
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevString *vargout - contains string.
//		long *error - pointer to error code (in the case of failure)
//-=====================================================================

long Serial::Status ( void *vargin, void *vargout, long *error)
{
 struct termios		termin;
 struct termios		termout;
 static	char		append[128];
 static char		mess[4096];
 static char		**status;
 printf("!!!!!!!!!!! TEMPORARY MODIFICATION FOR DEBUG !!!!!!!!!!!\n");
 //char		eol = '\r';	// end of line used for returned message
 char		eol = '\n';	// end of line used for returned message



 *error = 0;



 //
 // Compose string to return with serial configuration parameters
 //
 sprintf(mess,"The serial line has following configuration : %c%c", eol, eol);

 if (this->serialline.serialline != NULL)
 {
  sprintf(append,"descriptor=%s%c",this->serialline.serialline, eol);
 }
 else
 {
  sprintf(append,"descriptor=none%c", eol);
 }
 strcat(mess,append);


 //
 // Current parameters defined in the internal structure
 //
 sprintf(append,"%cParameters defined internally:%c", eol, eol);
 strcat(mess,append);

 sprintf(append,"baudrate=%d%c",this->serialline.baudrate, eol);
 strcat(mess,append);
 if (this->serialline.parity == SL_EVEN) sprintf(append,"parity=even%c", eol);
 if (this->serialline.parity == SL_ODD)  sprintf(append,"parity=odd%c", eol);
 if (this->serialline.parity == SL_NONE) sprintf(append,"parity=none%c", eol);
 strcat(mess,append);

 sprintf(append,"charlength=%d%c",this->serialline.charlength, eol);
 strcat(mess,append);

 sprintf(append,"stopbits=%d%c",this->serialline.stopbits, eol);
 strcat(mess,append);

 sprintf(append,"newline=%d%c",this->serialline.newline, eol);
 strcat(mess,append);



 //
 // Current parameters of the input terminal
 // (see file /usr/include/asm/termbits.h for values decoding)

 //
 sprintf(append,"%cParameters of the input terminal:%c", eol, eol);
 strcat(mess,append);

 if (tcgetattr(this->serialline.serialin, &termin) < 0)
 {
  printf("Serial::Status(): tcgetattr() error on in line(errno=%d)\n",
	errno);
  perror(this->serialline.serialline);
  *error =  DevErr_SerialLineInitFailed;

  return(DS_NOTOK); 
 }
 
  


 //
 // Current parameters of the output terminal
 // (see file /usr/include/asm/termbits.h for values decoding)
 //
 sprintf(append,"%cParameters of the output terminal:%c", eol, eol);
 strcat(mess,append);

 if (tcgetattr(this->serialline.serialout, &termout) < 0)
 {
  printf("Serial::Status(): tcgetattr() error on out line(errno=%d)\n",
	errno);
  perror(this->serialline.serialline);
  *error =  DevErr_SerialLineInitFailed;

  return(DS_NOTOK); 
 }




 //
 // Change end of message for UNIX
 // TODO: why do such a thing ?
 //
 mess[strlen(mess)-1] = 0x0a;



 //
 // Return the string
 //
 status = (char **)vargout;
 *status = mess;



 return(DS_OK);
}


//+=====================================================================
// Function:    Serial::Decode_parameters()
//
// Description:	Return a string with the serial line configuration parameter
//		decoded from the termio structure passed.
//
// Arg(s) In:	struct termios term - structure to decode
//		char eol - end of line character to use
//
//+=====================================================================
char * Serial::Decode_parameters(struct termios	term, char eol )
{
 static char		mess[4096];
 static	char		append[128];

 mess[0] = 0;
 sprintf(append,"c_cc=0%o%c", term.c_cc,    eol);
 strcat(mess,append);
 sprintf(append,"c_lflag=0%o%c", term.c_lflag, eol);
 strcat(mess,append);
 sprintf(append,"c_cflag=0%o%c", term.c_cflag, eol);
 strcat(mess,append);
 sprintf(append,"c_iflag=0%o%c", term.c_iflag, eol);
 strcat(mess,append);
 sprintf(append,"c_oflag=0%o%c", term.c_oflag, eol);
 strcat(mess,append);


 return mess;
}



//+=====================================================================
// Function:    Serial::Open_desc()
//
// Description:	Open the device desciptors and update member variable.
//
//+=====================================================================
long Serial::Open_desc ( void )
{
 char 	dd_name[16] = "/dev/";
 int	desc;


 //
 // create the associated device descriptor filename
 //
 //strncat(dd_name, this->serialline.serialline, 16 - strlen(dd_name));
 strncpy(dd_name, this->serialline.serialline, 16);
 
 if(Serial::debug)
  printf("Serial::Open_desc(): serial line to be opened %s\n", dd_name);


 //
 // open the serial line once for reading but not as a controlling tty
 //
 desc = open(dd_name,O_RDONLY | O_NOCTTY);
 if (desc < 0)
 {
  printf("Serial::Open_desc(): error opening in O_RDONLY %s\n", dd_name);
  perror(dd_name);
  return(DS_NOTOK);
 }
 this->serialline.serialin = desc;



 //
 // open the serial line once for writing but not as a controlling tty
 //
 desc = open(dd_name,O_WRONLY | O_NOCTTY);
 if (desc < 0)
 {
  printf("Serial::Open_desc(): error opening in O_WRONLY %s\n", dd_name);
  perror(dd_name);
  return(DS_NOTOK);
 }
 this->serialline.serialout = desc;


 return(DS_OK);
}

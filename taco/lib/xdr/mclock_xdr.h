/**************************************************************************/
/*                                                                        */
/*  XDR data type definition  "mclock_xdr.h" with : C type definition ,   */
/*  declaration of xdr functions , declaration of xdr length calculation  */
/*  functions , definition for data type number and definition of the     */
/*  load macro .                                                          */
/*                                                                        */
/**************************************************************************/


#include <SysNumbers.h>


/**************************************************************************/

/*  C type definition  */
struct DevTm
       {
	 short tm_year;
	 short tm_mon;
	 short tm_mday;
	 short tm_hour;
	 short tm_min;
	 short tm_sec;
	 short tm_wday;
	 short tm_yday;
	 short tm_isdst;
       };
typedef struct	DevTm	DevTm;

/*  declaration of xdr functions  */
bool_t	xdr_DevTm();

/*  declaration of xdr length calculation function  */
long 	xdr_length_DevTm();



/**************************************************************************/

/*  C type definition  */
struct DevStructMasterClock
       {
	 char 	*convert_dh;
	 long 	nb_sec;
	 DevTm 	*timeptr;
       };
typedef	struct	DevStructMasterClock	DevStructMasterClock;

/*  declaration of xdr functions  */
bool_t	xdr_DevStructMasterClock();

/*  declaration of xdr length calculation function  */
long	xdr_length_DevStructMasterClock();

/*  definition of the data type number  */
#define	D_STRUCT_MCLOCK	DevMClockBase + 1

/*  definition of the load macro  */
#define	LOAD_STRUCT_MCLOCK(A)	xdr_load_type(D_STRUCT_MCLOCK, \
					 xdr_DevStructMasterClock, \
					 sizeof(DevStructMasterClock), \
					 xdr_length_DevStructMasterClock, \
					 A)


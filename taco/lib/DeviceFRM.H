/*
 *******************************************************************************
 * Module:
 *		$Id: DeviceFRM.H,v 1.1 2003-04-25 11:21:28 jkrueger1 Exp $
 * Description:
 *
 * Author:
 *		$Author: jkrueger1 $
 * History:
 *		$Log: not supported by cvs2svn $
 *		Revision 8.3  2002/09/13 07:40:12  jkrueger
 *		add unreg program to the distribution.
 *		prepare something for the gcc 3.2 compiler (not complete yet.)
 *		Jens.
 *		
 *		Revision 8.2  2001/04/10 15:32:24  pedersen
 *		some changes to automake-part
 *		
 *		Revision 8.1  2001/04/10 13:25:34  pedersen
 *		added missing files
 *		
 *		Revision 1.1.2.1  2001/04/10 13:18:22  pedersen
 *		addes missing files
 *		
 *		Revision 8.37  2001/01/19 16:18:26  pedersen
 *		 redo of esrf changes
 *		
 *		Revision 8.34  2000/11/27 13:47:29  jkrueger
 *		improve: taco mapping - remove virtual from taco... methods
 *		modify: it is now possible to export different devices from one server -
 *		             remove protected static members
 *		
 *		Revision 8.33  2000/09/12 11:03:15  jkrueger
 *		Changing the Device class to more C++ like programming.
 *		
 *******************************************************************************
 */

#ifndef FRMDEVICE_H__
#define FRMDEVICE_H__

#include <map>
#include <string>
#include <API.h>
#include <DevCmds.h>
#include "Device.H"

class FRMDevice:public DeviceBase
{
private:
    static const short	VER_MAJOR = 0;
    static const short  VER_MINOR = 0;
    static const short  VER_RELEASE = 1;
    static short	classInited;

public:
    typedef long	DevEvent;
    typedef long	(FRMDevice::*DeviceMemberFunction)(void *, void *); 
    typedef struct	_DeviceCommandListEntry
  			{
  			    DeviceMemberFunction	fn;
  			    DevArgType			arginType;
  			    DevArgType			argoutType;
  			    long			minAccess;
			    _DeviceCommandListEntry()
			    {
				fn = NULL;
				arginType = argoutType = minAccess = 0;
			    };
			    _DeviceCommandListEntry(DeviceMemberFunction f, 
						DevArgType in, DevArgType out, long acc)
			    {
				fn = f;
				arginType = in;
				argoutType = out;
				minAccess = acc;
			    };
  			}DeviceCommandListEntry;
    typedef std::map<DevCommand, DeviceCommandListEntry>	DeviceCommandList;

    typedef struct	_DeviceEventListEntry
			{
			    DevArgType			argType;
			    _DeviceEventListEntry()
			    {
				argType = 0;
			    };
			    _DeviceEventListEntry(DevArgType type)
			    {
				argType = type;
			    };
			}DeviceEventListEntry;
    typedef std::map<DevEvent, DeviceEventListEntry>	DeviceEventList;

protected:
    std::string			className;
    long			nEvents,
    				nCommands;
    DeviceCommandList   	commandList;
    DeviceEventList		eventList;
    std::string			devType,
    				devName;
    short			state,
    				nextState;

protected:
    virtual long	ClassInitialise(void)
				throw (long);
    virtual long	GetResources(const std::string = "")
				throw (long);
    virtual long	StateMachine(long)
				throw (long);
    virtual short	State(void) throw (long);
    virtual std::string	Status(void) throw (long);
    virtual void	On(void) throw (long);
    virtual void	Off(void) throw (long);
    virtual void	Reset(void) throw (long);
    virtual long	Version(void) throw (long);

    	    long	tacoOn(void *, void *) throw (long);
    	    long	tacoOff(void *, void *) throw (long);
    	    long	tacoState(void *, void *) throw (long);
    	    long	tacoStatus(void *, void *) throw (long);
    	    long	tacoReset(void *, void *) throw (long);
    	    long	tacoVersion(void *, void *) throw (long);
	    long	FindCommand(long) throw (long);
public:
			FRMDevice(const std::string, long &);
    virtual		~FRMDevice();
    virtual long	Command(const long, void *, long, void *, long, long*);

	    const char* GetClassName(void);				// needed in svc_api.c 
	    const char* GetDevType(void);				// needed in svc_api.c 
	    const char* GetDevName(void);				// needed in svc_api.c
	    long	GetMinAccessRight(const long) throw (long);	// needed in sec_api.c
	    unsigned	GetCommandNumber(void);				// needed in svc_api.c
	    long	CommandQuery(_dev_cmd_info *);			// needed in svc_api.c
	    unsigned 	GetEventNumber(void);				// needed in svc_api.c
	    long	EventQuery(_dev_event_info *);			// needed in svc_api.c

//    friend class 	DeviceCommandListEntry;
};

#endif //FRMDEVICE_H_

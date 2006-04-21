#ifndef TACO_STATES_H
#define TACO_STATES_H

#include <string>

#include <macros.h>
#include <DevStates.h>

#if !defined(SWIG) 
#	if STOPPED != 43
# 		error unexpected STOPPED value
#	endif

#	undef STOPPED

#	if RUNNING != 44
# 		error unexpected RUNNING value
#	endif

#	undef RUNNING
#endif

// Rename OVERFLOW if defined, in order to avoid conflicts with 'math.h'
#ifdef OVERFLOW
# if OVERFLOW != 3
#  error unexpected OVERFLOW value
# endif
# define TACO_MATH_OVERFLOW 3
# undef OVERFLOW
#endif

namespace TACO {
	//! Contains all %TACO device states
	namespace State {
		// Standard states
		const short ALARM = DEVALARM;
		const short AUTOMATIC = DEVAUTOMATIC;
		const short BAKEING = DEVBAKEING;
		const short BAKE_REQUESTED = DEVBAKEREQUESTED;
		const short BEAM_ENABLE = DEVBEAM_ENABLE;
		const short BLOCKED = DEVBLOCKED;
		const short CLOSED = DEVCLOSE;
		const short CLOSING = DEVCLOSING;
		const short COUNTING = DEVCOUNTING;
		const short DISABLED = DEVDISABLED;
		const short EXTRACTED = DEVEXTRACTED;
		const short FAULT = DEVFAULT;
		const short FORBIDDEN = DEVFORBIDDEN;
		const short FORCED_CLOSE = DEVFORCEDCLOSE;
		const short FORCED_OPEN = DEVFORCEDOPEN;
		const short HIGH = DEVHIGH;
		const short HV_ENABLE = DEVHV_ENABLE;
		const short INIT = DEVINIT;
		const short INSERTED = DEVINSERTED;
		const short LOCAL = DEVLOCAL;
		const short LOW = DEVLOW;
		const short MOVING = DEVMOVING;
		const short NEGATIVE_ENDSTOP = DEVNEGATIVEENDSTOP;
		const short DEVICE_OFF = DEVOFF;
		const short OFF_UNAUTHORISED = DEVOFFUNAUTHORISED;
		const short ON = DEVON;
		const short ON_NOT_REACHED = DEVON_NOT_REACHED;
		const short ON_NOT_REGULAR = DEVONNOTREGULAR;
		const short OPEN = DEVOPEN;
		const short OPENING = DEVOPENING;
		const short POSITIVE_ENDSTOP = DEVPOSITIVEENDSTOP;
		const short RAMP = DEVRAMP;
		const short REMOTE = DEVREMOTE;
		const short RESETTING = DEVRESETTING;
		const short RUN = DEVRUN;
		const short RUNNING = 44;
		const short SERVICE = DEVSERVICE;
		const short STANDBY = DEVSTANDBY;
		const short STANDBY_NOT_REACHED = DEVSTANDBY_NOT_REACHED;
		const short STARTING = DEVSTARTING;
		const short START_REQUESTED = DEVSTARTREQUESTED;
		const short STOPPED = 43;
		const short STOPPING = DEVSTOPPING;
		const short STOP_BAKE = DEVSTOPBAKE;
		const short STOP_REQUESTED = DEVSTOPREQUESTED;
		const short TRIPPED = DEVTRIPPED;
		const short UNDEFINED = DEVUNDEFINED;
		const short UNKNOWN = DEVUNKNOWN;
		const short WARMUP = DEVWARMUP;

		// Additional states
		// If you add new state you should provide a description via the
		// TACO::stateDescription function.  See below.
		const short ADDITIONAL_STATES_BASE = 1000;
		const short OFF = ADDITIONAL_STATES_BASE + 1;
		const short OVERFLOW = ADDITIONAL_STATES_BASE + 2;
		const short PRESELECTION_REACHED = ADDITIONAL_STATES_BASE + 3;
		const short STARTED = ADDITIONAL_STATES_BASE + 4;
		const short DEVICE_NORMAL = ADDITIONAL_STATES_BASE + 5;
		const short RESET = ADDITIONAL_STATES_BASE + 6;
		const short HOLDBACK = ADDITIONAL_STATES_BASE + 7;
		const short HOLD 		= ADDITIONAL_STATES_BASE + 8;
		const short COMPLETE = ADDITIONAL_STATES_BASE + 9;
		const short VACUUM_NOT_REACHED = ADDITIONAL_STATES_BASE + 11;
		const short VACUUM_FAILURE = ADDITIONAL_STATES_BASE + 12;
		const short WATER_NOT_ATTACHED = ADDITIONAL_STATES_BASE + 13;
		const short HEATER_OFF = ADDITIONAL_STATES_BASE + 14;

	}

	//! Returns the state description
	std::string stateDescription( short state) throw ();
}

#endif // TACO_STATES_H

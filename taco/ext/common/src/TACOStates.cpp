#include <map>

#include <TACOStates.h>

std::string TACO::stateDescription( short state) throw ()
{
	typedef std::map<short, const char*> DescriptionMap;
	static DescriptionMap descriptionMap;
	if (descriptionMap.empty()) {
		descriptionMap [State::ALARM] = "alarm";
		descriptionMap [State::AUTOMATIC] = "automatic";
		descriptionMap [State::BAKEING] = "baking";
		descriptionMap [State::BAKE_REQUESTED] = "bake requested";
		descriptionMap [State::BEAM_ENABLE] = "beam enable";
		descriptionMap [State::BLOCKED] = "blocked";
		descriptionMap [State::CLOSED] = "closed";
		descriptionMap [State::CLOSING] = "closing";
		descriptionMap [State::COUNTING] = "counting";
		descriptionMap [State::DISABLED] = "disabled";
		descriptionMap [State::EXTRACTED] = "extracted";
		descriptionMap [State::FAULT] = "fault";
		descriptionMap [State::FORBIDDEN] = "forbidden";
		descriptionMap [State::FORCED_CLOSE] = "forced close";
		descriptionMap [State::FORCED_OPEN] = "forced open";
		descriptionMap [State::HIGH] = "high";
		descriptionMap [State::HV_ENABLE] = "HV enable";
		descriptionMap [State::INIT] = "init";
		descriptionMap [State::INSERTED] = "inserted";
		descriptionMap [State::LOCAL] = "local";
		descriptionMap [State::LOW] = "low";
		descriptionMap [State::MOVING] = "moving";
		descriptionMap [State::NEGATIVE_ENDSTOP] = "negative endstop";
		descriptionMap [State::DEVICE_OFF] = "device off";
		descriptionMap [State::OFF_UNAUTHORISED] = "off unauthorised";
		descriptionMap [State::ON] = "on";
		descriptionMap [State::ON_NOT_REACHED] = "on not reached";
		descriptionMap [State::ON_NOT_REGULAR] = "on not regular";
		descriptionMap [State::OPEN] = "open";
		descriptionMap [State::OPENING] = "opening";
		descriptionMap [State::POSITIVE_ENDSTOP] = "positive endstop";
		descriptionMap [State::RAMP] = "ramp";
		descriptionMap [State::REMOTE] = "remote";
		descriptionMap [State::RESETTING] = "resetting";
		descriptionMap [State::RUN] = "run";
		descriptionMap [State::RUNNING] = "running";
		descriptionMap [State::SERVICE] = "service";
		descriptionMap [State::STANDBY] = "standby";
		descriptionMap [State::STANDBY_NOT_REACHED] = "standby not reached";
		descriptionMap [State::STARTING] = "starting";
		descriptionMap [State::START_REQUESTED] = "start requested";
		descriptionMap [State::STOPPED] = "stopped";
		descriptionMap [State::STOPPING] = "stopping";
		descriptionMap [State::STOP_BAKE] = "stop bake";
		descriptionMap [State::STOP_REQUESTED] = "stop requested";
		descriptionMap [State::TRIPPED] = "tripped";
		descriptionMap [State::UNDEFINED] = "undefined";
		descriptionMap [State::UNKNOWN] = "unknown";
		descriptionMap [State::WARMUP] = "warmup";

		// Additional states
		descriptionMap [State::OFF] = "off";
		descriptionMap [State::OVERFLOW] = "overflow";
		descriptionMap [State::PRESELECTION_REACHED] = "preselection reached";
		descriptionMap [State::STARTED] = "started";
		descriptionMap [State::DEVICE_NORMAL] = "device normal";
		descriptionMap [State::RESET] = "program reset";
		descriptionMap [State::HOLDBACK] = "program holdback";
		descriptionMap [State::HOLD] = "program hold";
		descriptionMap [State::COMPLETE] = "program complete";
		descriptionMap [State::VACUUM_NOT_REACHED] = "vacuum not reached";
		descriptionMap [State::VACUUM_FAILURE] = "vacuum failure";
		descriptionMap [State::WATER_NOT_ATTACHED] = "water not attached";
		descriptionMap [State::HEATER_OFF] = "heater off";
	}
	static std::string r;
	DescriptionMap::const_iterator i( descriptionMap.find( state));
	if (i != descriptionMap.end()) {
		r = i->second;
	} else {
		r = "error: no state description available";
	}
	return r;
}

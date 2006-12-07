#include <map>
// #include <iostream>

#include <pthread.h>

#include <Python.h>

//! Contains the %TACO Python client stuff
namespace TACOPythonClient {
	pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;

	inline void lock()
	{
#ifdef DEBUG
		std::cerr << "aquire lock ... ";
#endif // DEBUG
		pthread_mutex_lock( &globalLock);
#ifdef DEBUG
		std::cerr << "ok" << std::endl;
#endif // DEBUG
	}

	inline void unlock()
	{
#ifdef DEBUG
		std::cerr << "release lock ... ";
#endif // DEBUG
		pthread_mutex_unlock( &globalLock);
#ifdef DEBUG
		std::cerr << "ok" << std::endl;
#endif // DEBUG
	}

	/*
	inline bool devEventListen(
		devserver handle,
		long event,
		DevArgument argout,
		DevType type, 
		DevCallbackFunction* callback,
		void* data,
		long* id,
		long* error
	)
	{
		lock();
		bool r = dev_event_listen( handle, event, argout, type, callbackFunction, data, id, e) != DS_OK;
		unlock();
		return r;
	}
	*/

	class CommandInfoMap {
	public:
		static bool addCommandInfo( devserver& handle);

		static bool getCommandInfo( long commandNumber, long& inputType, long& outputType);

	private:
		// command number -> (input type, output type)
		typedef std::map<long, std::pair<long, long> > InfoMap;

		static InfoMap infoMap;
	};

	// event number -> (type)
	typedef std::map<long, long> EventInfoMap;

	bool queryEventInfo( devserver handle, EventInfoMap& eventInfoMap);

	bool queryEventType( devserver handle, long eventNumber, long& eventType);

	class EventManager {
	public:
		// Returns the event id if successful
		static bool addEvent(
			long event,
			PyObject* handleObject,
			PyObject* function,
			PyObject* self,
			PyObject* data,
			long& id
		);

		static bool removeEvent( long id);

		static void removeEvents( devserver handle);

	private:
		struct EventEntry {
			EventEntry(
				devserver iHandle,
				long iEvent,
				DevArgument iArgout,
				DevType iType,
				PyObject* iFunction,
				PyObject* iSelf,
				PyObject* iData
			)
				: handle( iHandle),
				  event( iEvent),
				  argout( iArgout),
				  type( iType),
				  function( iFunction),
				  self( iSelf),
				  data( iData)
			{
				// VOID
			}

			EventEntry()
			{
				// VOID
			}

			devserver handle;

			long event;

			DevArgument argout;

			DevType type;

			PyObject* function;

			PyObject* self;

			PyObject* data;
		};

		static void* callbackFunction( devserver handle, void* unused, DevCallbackData data);

		static void* handleCallback( void* data);

		static void* sync( void* unused);

		static bool cancelSyncThread();

		static bool startSyncThread();

		static pthread_t syncThreadID;

		static bool syncThreadStarted;

		// id -> EventEntry
		typedef std::map<long, EventEntry> EventMap;

		static EventMap eventMap;
	};

	bool getDeviceHandle( PyObject* handleObject, devserver& handle);
}

#ifndef TACO_CLIENT_H
#define TACO_CLIENT_H

#include <string>
#include <set>

#include <API.h>
#include <dc.h>

#include <TACOArgPutGet.h>
#include <TACOException.h>
#include <TACOExtensions.h>
#include <TACOResources.h>
#include <TACOStates.h>
#include <TACOTypes.h>

/**
 * Enables a client side runtime type check.
 *
 * The check is based on the information provided by the device server.  Since reasonable device server (for
 * example those derived from TACO::Server) do this check too, it is currently disabled.
 */
// #define TACO_CLIENT_RUNTIME_TYPE_CHECK

//! Contains all basic TACO classes, functions and constants
namespace TACO {
	class Client;

	//! Event handler type
	typedef DevCallbackFunction EventHandler;

	//! Event handler data type
	typedef DevCallbackData EventHandlerData;

	/**
	 * Synchronize the process with pending asynchronous events.
	 *
	 * @see Client::eventListen()
	 */
	void synchronize( double timeout) throw (Exception);
}

/**
 * Base class for all device clients.
 *
 * The TACO::Client class is the base class for all other %TACO device client classes and the counterpart to
 * the %TACO device servers.  It implements the access functionality to the device and hides all
 * implementation features of the %TACO communication between client and server.  Classes derived from
 * TACO::Client should be used in applications of the instrument user.
 * 
 * The concept of using %TACO at the FRM-II is a little bit different from that at ESRF.  %TACO is considered
 * as a transport layer for controlling the experiments.  This idea results in a more user friendly interface
 * of %TACO.  The original interface to the %TACO device is a little bit curious and tends to be unwiedly for
 * someone who did not work every day with %TACO.
 * 
 * One of the main problems of %TACO is the generic interface.  This interface is powerful but dangerous due
 * to the usage of pointers and furthermore you have to know what type of input and output parameters a %TACO
 * command of a special device requires.  It is not possible to make type checks at compile time.  Our idea is
 * to encapulate the commands of the %TACO device in a client class.  The advantage is an easy use of %TACO
 * without an exact knowledge of the command numbers and a type check at compile time.  An automatic mapping
 * form %TACO standard types to STL types and vice versa is implemented too.  Every command of the %TACO
 * device is encapsulated in one method of the client class.  The client class has all information about the
 * %TACO command numbers und command types.  The user has only to deal with the required parameters in an type
 * safe environment on account of type checks during compilation, not to mention the other advantages of
 * object orientated software design.  This helps to shorten the development time drastically and leads to a
 * more robust system.
 */ 
class TACO::Client {
public:
	//! Device type set
	typedef std::set<std::string> DeviceTypeSet;

	//! Constructs a client and connects to the device if desired
	Client( const std::string& name, DevLong access = 0, bool connect = true) throw (Exception);

	//! Constructs an unconnected client
	Client() throw ();

	//! Destoys the client and releases all external resources
	virtual ~Client() throw ();

	//! Copies a client
	Client( const Client& client) throw (Exception);

	//! Assignment operator
	Client& operator=( const Client& client) throw (Exception);

	/**
	 * @name Pure Client Methods
	 */
	//@{

	/**
	 * Connect the client.
	 *
	 * @exception Undocumented
	 *	This method uses the %TACO dev_import() and dev_cmd_query() functions.
	 *	The possible errors of these functions are not documented.
	 */
	virtual void connectClient() throw (Exception);

	/**
	 * Connect the client with new connection parameters.
	 *
	 * @exception Undocumented
	 *	This method uses the %TACO dev_import() and dev_cmd_query() functions.
	 *	The possible errors of these functions are not documented.
	 */
	virtual void connectClient( const std::string& name, DevLong access = 0) throw (Exception);

	//! Returns true if automatic client connection is enabled
	bool isAutoClientConnectionEnabled() const throw ()
	{
		return mAutoConnect;
	}

	//! Enable automatic client connection
	void enableAutoClientConnection( bool enable) throw ()
	{
		mAutoConnect = enable;
	}
	
	/**
	 * Disconnect the client.
	 *
	 * @exception Undocumented
	 *	This method uses the %TACO dev_free() function.
	 *	The possible errors of this function are not documented.
	 */
	virtual void disconnectClient() throw (Exception);

	/**
	 * Reconnect the client.
	 *
	 * @exception Undocumented
	 *	This method uses the %TACO dev_import(), dev_cmd_query() and dev_free() functions.
	 *	The possible errors of these functions are not documented.
	 */
	void reconnectClient() throw (Exception)
	{
		disconnectClient();
		connectClient();
	}

	//! Returns true if the client is connected
	bool isClientConnected() const throw ()
	{
		return mConnected;
	}

	//! Returns true if the client is disconnected
	bool isClientDisconnected() const throw ()
	{
		return !isClientConnected();
	}

	/**
	 * Returns the current network protocol.
	 *
	 * @exception Error::RUNTIME_ERROR
	 *
	 * @exception Error::INTERNAL_ERROR
	 *
	 * @see setClientNetworkProtocol()
	 */
	NetworkProtocol clientNetworkProtocol() throw (Exception);

	/**
	 * Sets the network protocol.
	 *
	 * This protocol is used for the communication between client and server.  The default
	 * is probably UDP.  This protocol is faster than TCP but the transfer volume is limited.  If you want
	 * to transfer huge data sets you should choose TCP.  But what is <em>huge</em> exactly? 
	 * In this case <em>huge</em> means more than 8kB data. This limit comes from the UDP itself and
	 * there is <b>no</b> way to change this limit!
	 *
	 * @exception Error::RUNTIME_ERROR
	 *
	 * @exception Undocumented
	 *	This method uses the %TACO dev_rpc_protocol() function.
	 *	The possible errors of this function are not documented.
	 *
	 * @see clientNetworkProtocol()
	 */
	void setClientNetworkProtocol( NetworkProtocol protocol) throw (Exception);

	/**
	 * Returns the current network timeout.
	 *
	 * @exception Error::RUNTIME_ERROR
	 *
	 * @exception Undocumented
	 *	This method uses the %TACO dev_rpc_timeout() function.
	 *	The possible errors of this function are not documented.
	 *
	 * @see setClientNetworkTimeout()
	 */
	double clientNetworkTimeout() throw (Exception);

	/**
	 * Sets the network timeout.
	 *
	 * @exception Error::RANGE_ERROR
	 *
	 * @exception Error::RUNTIME_ERROR
	 *
	 * @exception Undocumented
	 *	This method uses the %TACO dev_rpc_timeout() function.
	 *	The possible errors of this function are not documented.
	 *
	 * @see clientNetworkTimeout()
	 */
	void setClientNetworkTimeout( double timeout) throw (Exception);

	//@}

	/**
	 * @name Pure Device Methods
	 */
	//@{

	//! Returns the device name
	std::string deviceName() const throw ()
	{
		return mName;
	}

	/**
	 * Switches the device on.
	 *
	 * @exception Unknown
	 * The errors vary from server to server.
	 */
	virtual void deviceOn() throw (Exception);

	//! Returns true if automatic device on is enabled
	bool isAutoDeviceOnEnabled() const throw ()
	{
		return mAutoDeviceOn;
	}

	//! Enable automatic device on
	void enableAutoDeviceOn( bool enable) throw ()
	{
		mAutoDeviceOn = enable;
	}

	/**
	 * Switches the device off.
	 *
	 * @exception Unknown
	 * The errors vary from server to server.
	 */
	virtual void deviceOff() throw (Exception);

	/**
	 * Returns true if the device is off.
	 *
	 * @exception Unknown
	 * The errors vary from server to server.
	 */
	bool isDeviceOff() throw (Exception)
	{
		return deviceState() == State::DEVICE_OFF;
	}

	/**
	 * Resets the device.
	 *
	 * @exception Unknown
	 * The errors vary from server to server.
	 */
	virtual void deviceReset() throw (Exception);

	/**
	 * Returns the device state.
	 *
	 * @exception Unknown
	 * The errors vary from server to server,
	 * but it is unlikely that this method throws an exception.
	 */
	virtual short deviceState() throw (Exception);

	/**
	 * Returns the device status.
	 *
	 * @exception Unknown
	 * The errors vary from server to server,
	 * but it is unlikely that this method throws an exception.
	 */
	virtual std::string deviceStatus() throw (Exception);

	/**
	 * Returns the device version.
	 *
	 * @exception Unknown
	 * The errors vary from server to server.
	 */
	virtual std::string deviceVersion() throw (Exception);

	/**
	 * Returns the device types.
	 *
	 * @exception Error::RUNTIME_ERROR
	 */
	DeviceTypeSet deviceTypes() const throw (Exception)
	{
		if (isClientConnected()) {
			return mServerTypes;
		} else {
			throw Exception( Error::RUNTIME_ERROR, "client not connected");
		}
	}

	/**
	 * Returns the value of a device resource with the given <em>name</em>.
	 */
	std::string deviceQueryResource( const std::string& name) throw (Exception);

	/**
	 * Updates the <em>value</em> of a device resource with the given <em>name</em>.
	 */
	void deviceUpdateResource( const std::string& name, const std::string& value) throw (Exception);

	/**
	 * Updates all device resources.
	 */
	void deviceUpdate() throw (Exception);

	/**
	 * Queries all device resources information.
	 */
	ResourceInfoSet deviceQueryResourceInfo() throw (Exception);

	//! Queries all device commands informations.
	CommandInfoMap deviceQueryCommandInfo() throw (Exception) {return mCommandInfoMap;} 

	//@}

	/**
	 * @name Event Management Methods
	 */
	//@{

	/**
	 * Start listen to an event.
	 *
	 * @warning
	 * The pointer to the <em>event data</em> must be valid during the time listening to the event.
	 *
	 * @see eventUnlisten(), synchronize()
	 */
	template<typename T> DevLong eventListen( DevEvent event, EventHandler* handler, void* userData, T* eventData) throw (Exception)
	{
		checkConnection();
		DevLong id;
		DevLong e;
		if (dev_event_listen( mDeviceHandle, event, eventData, type<T>(), handler, userData, &id, &e) != DS_OK) {
			throw Exception( e);
		}
		return id;
	}

	/**
	 * Start listen to an event.
	 *
	 * @see eventUnlisten(), synchronize()
	 */
	DevLong eventListen( DevEvent event, EventHandler* handler, void* userData) throw (Exception);

	/**
	 * Start listen to an event.
	 *
	 * The member function eventHandler() is used as the event handler.
	 *
	 * @warning
	 * The pointer to the <em>event data</em> must be valid during the time listening to the event.
	 *
	 * @see eventHandler(), eventUnlisten(), synchronize()
	 */
	template<typename T> DevLong eventListen( DevEvent event, T* eventData) throw (Exception)
	{
		return eventListen<T>( event, eventHandlerCaller, this, eventData);
	}

	/**
	 * Start listen to an event.
	 *
	 * The member function eventHandler() is used as the event handler.
	 *
	 * @see eventHandler(), eventUnlisten(), synchronize()
	 */
	DevLong eventListen( DevEvent event) throw (Exception)
	{
		return eventListen( event, eventHandlerCaller, this);
	}

	/**
	 * Stop listen to an event.
	 *
	 * @see eventListen(), synchronize()
	 */
	void eventUnlisten( DevEvent event, DevLong id) throw (Exception);

	//@}

protected:
	/**
	 * Standard event handler.
	 *
	 * @see eventListen( DevEvent event, T& eventData), eventListen( DevEvent event), synchronize()
	 */
	virtual void* eventHandler( EventHandlerData data) throw ();

	//! Executes a command
	void execute( DevCommand cmd, void* argin, DevType inType, void* argout, DevType outType) throw (Exception);

	//! Executes a command without any error handling
	void executeCore( DevCommand cmd, void* argin, DevType inType, void* argout, DevType outType) throw (Exception);

	//! Executes a command
	template<typename OUT, typename IN> OUT execute( DevCommand cmd, const IN& i) throw (Exception)
	{
		ArgGet<OUT> output;
		ArgPut<IN> input( i);
#ifdef TACO_CLIENT_RUNTIME_TYPE_CHECK
		const CommandInfo info = commandInfo( cmd);
		if (info.inputType != input.type() || info.outputType != output.type()) {
			throw Exception( Error::TYPE_ERROR);
		}
#endif // TACO_CLIENT_RUNTIME_TYPE_CHECK
		execute( cmd, input.address(), input.type(), output.address(), output.type());
		return output.object();
	}

	//! Executes a command
	template<typename OUT> OUT execute( DevCommand cmd) throw (Exception)
	{
		ArgGet<OUT> output;
		ArgPut<void> input;
#ifdef TACO_CLIENT_RUNTIME_TYPE_CHECK
		const CommandInfo info = commandInfo( cmd);
		if (info.inputType != input.type() || info.outputType != output.type()) {
			throw Exception( Error::TYPE_ERROR);
		}
#endif // TACO_CLIENT_RUNTIME_TYPE_CHECK
		execute( cmd, input.address(), input.type(), output.address(), output.type());
		return output.object();
	}

	//! Checks if the <em>type</em> is supported by the connected device
	void checkDeviceType( const std::string& type) throw (Exception);

	//! Checks if the client device types go with the server types
	void checkDeviceTypes() throw (Exception);

	//! Addes a device type to the client
	void addDeviceType( const std::string& type) throw ()
	{
		mClientTypes.insert( type);
	}

private:
	//! Calls eventHandler()
	static void* eventHandlerCaller( devserver unused, void* thisPointer, EventHandlerData eventHandlerData) throw ();

	/**
	 * Checks the connection to the device server.
	 *
	 * If the client is disconnected and automatic client connection is enabled a connection will be established.
	 * The function returns only successfully if the client is connected after the call.
	 */
	void checkConnection() throw (Exception);

	//! Low level device handle for the %TACO API functions
	devserver mDeviceHandle;

	//! Low level device handle for the %DC API functions
	dc_dev_imp m_dc;

	//! Name of the device
	std::string mName;

	//! Access code
	DevLong mAccess;

	//! Set of the device types from the client
	DeviceTypeSet mClientTypes;

	//! Set of the device types from the connected server
	DeviceTypeSet mServerTypes;

	//! Enables automatic client connection
	bool mAutoConnect;

	//! Enables automatic device on
	bool mAutoDeviceOn;

	//! Indicator for Pseudo devices
	bool mPseudo;

	//! Indicator for connection
	bool mConnected;

	//! Map with the command infos of the device
	CommandInfoMap mCommandInfoMap;

#ifdef TACO_CLIENT_RUNTIME_TYPE_CHECK
	/**
	 * Returns the command information to the <em>command number</em>.
	 *
	 * @exception Error::RUNTIME_ERROR
	 *
	 * @exception Error::CLIENT_NOT_CONNECTED
	 */
	CommandInfo commandInfo( DevCommand commandNumber) const throw (Exception)
	{
		if (isClientConnected()) {
			CommandInfoMap::const_iterator i = mCommandInfoMap.find( commandNumber);
			if (i != mCommandInfoMap.end()) {
				return i->second;
			} else {
				throw Exception( Error::RUNTIME_ERROR, "command not supported");
			}
		} else {
			throw Exception( Error::RUNTIME_ERROR, "client not connected");
		}
	}
#endif // TACO_CLIENT_RUNTIME_TYPE_CHECK
};

#endif // TACO_CLIENT_H

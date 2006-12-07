/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2002-2004 Sebastian Huber <sebastian-huber@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <vector>
#include <iostream>

#include <TACOBasicCommands.h>
#include <TACOConverters.h>

#include <TACOClient.h>

#include <private/ApiP.h>

void TACO::synchronize( double timeout) throw (TACO::Exception)
{
	struct timeval t;
	assign( t, timeout);
	DevLong e;
	if (dev_synch( &t, &e) != DS_OK) {
		throw Exception( e);
	}
}

TACO::Client::Client( const std::string& name, DevLong access, bool connect) throw (TACO::Exception)
	: mDeviceHandle( 0), 
	  mName( name), 
	  mAccess( access), 
	  mAutoConnect( true), 
          mAutoDeviceOn( true),
	  mPseudo(false),
	  mConnected(false)
{
	if (connect) {
		connectClient();
	}
}

TACO::Client::Client() throw ()
	: mDeviceHandle( 0), 
	  mAutoConnect( true), 
	  mAutoDeviceOn( true), 
          mPseudo(false), 
          mConnected(false)
{
	// VOID
}

TACO::Client::~Client() throw ()
{
	try {
		disconnectClient();
	} catch (Exception) {
		// VOID
	}
}

TACO::Client::Client( const TACO::Client& client) throw (TACO::Exception)
	: mDeviceHandle( 0),
	  mName( client.mName),
	  mAccess( client.mAccess),
	  mAutoConnect( client.mAutoConnect),
	  mAutoDeviceOn( client.mAutoDeviceOn),
	  mPseudo(false),
	  mConnected(false)
{
	if (client.isClientConnected()) {
		connectClient();
	}
}

TACO::Client& TACO::Client::operator=( const TACO::Client& client) throw (TACO::Exception)
{
	if (this != &client) {
		mDeviceHandle = 0;
		mName = client.mName;
		mAccess = client.mAccess;
		mAutoConnect = client.mAutoConnect;
		mAutoDeviceOn = client.mAutoDeviceOn;
                mPseudo = false;
		mConnected = false;
		if (client.isClientConnected()) {
			connectClient();
		}
	}
	return *this;
}

static std::string getCmdName(const DevCommand cmd)
{
        std::string cmdName;

        char    res_path[LONG_NAME_SIZE];
        char    res_name[SHORT_NAME_SIZE];
        char    *ret_str = NULL;

        db_resource     res_tab;

        unsigned short  team,
                        server,
                        cmds_ident;
        DevLong         error;

        team = (unsigned short) (cmd  >> DS_TEAM_SHIFT) & DS_TEAM_MASK;
        server = (unsigned short) (cmd << DS_IDENT_SHIFT) & DS_IDENT_MASK;
        cmds_ident = cmd & 0x3FFFF;

        snprintf(res_path, sizeof(res_path), "CMDS/%u/%u", team, server);
        snprintf(res_name, sizeof(res_name), "%u", cmds_ident);

        res_tab.resource_name = res_name;
        res_tab.resource_type = D_STRING_TYPE;
        res_tab.resource_adr = &ret_str;

	cmdName = res_path;
	cmdName += "/";
	cmdName += res_name;
        if (db_getresource(res_path, &res_tab, 1, &error) == DS_OK)
                if (ret_str != NULL)
                {
                        cmdName = ret_str;
                        free(ret_str);
                }
        return cmdName;
}

void TACO::Client::connectClient() throw (TACO::Exception)
{
	disconnectClient();
	DevLong e;
	DevLong dc_error;
	std::string	sTemp = mName;

	db_devinfo_call info;
	std::string nethost_save = "";
	
	if (db_info.conf)
		nethost_save = db_info.conf->server_host;

	std::string nethost = "";
	char *tmp = getenv("NETHOST");
	if (tmp)
		nethost = tmp;	
	
	if (sTemp.substr(0, 2) == "//")
	{
		std::string::size_type n_pos = sTemp.find('/', 2);
		nethost = sTemp.substr(2, n_pos - 2);
		sTemp.erase(0, n_pos + 1);
	}
	else if (nethost.size())
// normalise the device name
		mName = "//" + nethost + "/" + sTemp;
	if (db_ChangeDefaultNethost(const_cast<char *>(nethost.c_str()), &e))
	{
		mDeviceHandle = 0;
		mConnected = false;
		throw Exception(e);
	}
	if (db_deviceinfo(const_cast<char *>(sTemp.c_str()), &info, &e) != DS_OK)
	{
		mDeviceHandle = 0;
		mConnected = false;
		if (nethost_save != "" && db_ChangeDefaultNethost(const_cast<char *>(nethost_save.c_str()), &e))
		{
			throw Exception(e);
		}
		throw Exception(e);
	}
	mPseudo = info.device_type != DB_Device;
	if (mPseudo)
	{
		m_dc.device_name = const_cast<char *>(mName.c_str());
		m_dc.dc_dev_error = &dc_error;
		mConnected = dc_import(&m_dc, 1, &e) == DS_OK;
	}
	else
		mConnected =  dev_import( const_cast<char*>(mName.c_str()), mAccess, &mDeviceHandle, &e) == DS_OK;
	if (!mConnected)
	{
		mDeviceHandle = 0;
		if (nethost_save != "" && db_ChangeDefaultNethost(const_cast<char *>(nethost_save.c_str()), &e))
		{
			throw Exception(e);
		}
		throw Exception(e);
	}
	setClientNetworkProtocol(TCP);


	// Query the types of the server device and check them
	try 
	{
		if (mPseudo)
		{
			dc_devinf       dc_inf;
			if (dc_dinfo(const_cast<char *>(mName.c_str()), &dc_inf, &e) == DS_OK)
			{
				for (int i = 0; i < dc_inf.devinf_nbcmd; ++i)
				{
					mCommandInfoMap[dc_inf.devcmd[i].devinf_cmd] = CommandInfo(
                                		getCmdName(dc_inf.devcmd[i].devinf_cmd),
                                		D_VOID_TYPE, "", dc_inf.devcmd[i].devinf_argout, "");
					if (dc_inf.devcmd[i].devinf_argout == D_DOUBLE_TYPE)
						mServerTypes.insert("IOAnalogInput");
					else if (dc_inf.devcmd[i].devinf_argout == D_LONG_TYPE)
						mServerTypes.insert("IODigitalInput");
				}
//				m_dc.device_name = const_cast<char *>(mName.c_str());
//				m_dc.dc_dev_error = &dc_error;
			}
		}
		else
		{
			mCommandInfoMap = queryAllCommandInfo( mDeviceHandle);
			ArgGet<std::vector<std::string> > output;
			executeCore( Command::DEVICE_TYPES, 0, D_VOID_TYPE, output.address(), output.type());
			std::vector<std::string> serverTypes( output.object());
			std::vector<std::string>::const_iterator i( serverTypes.begin());
			// mServerTypes is cleared by disconnectClient()
			while (i != serverTypes.end()) {
				mServerTypes.insert( *i++);
			}
		}
		checkDeviceTypes();
	} 
	catch (Exception) 
	{
		// Ignore this exception, because the old servers do not support device types
		if (nethost_save != "" && db_ChangeDefaultNethost(const_cast<char *>(nethost_save.c_str()), &e))
		{
			mDeviceHandle = 0;
			mConnected = false;
			throw Exception(e);
		}
	}
}

void TACO::Client::connectClient( const std::string& name, DevLong access) throw (TACO::Exception)
{
	mName = name;
	mAccess = access;
	connectClient();
}

void TACO::Client::disconnectClient() throw (TACO::Exception)
{
	if (isClientConnected()) {
		mServerTypes.clear();
		DevLong e;
		if (mPseudo)
		{
			DevLong dc_error;
			dc_dev_free d_free = {m_dc.dc_ptr, &dc_error};
			if (dc_free(&d_free, 1, &e) != DS_OK)
				throw Exception(e);
		}
		else
		{
			if (dev_free( mDeviceHandle, &e) != DS_OK) {
				throw Exception( e);
			}
			mDeviceHandle = 0;
		}
		mConnected = false;
	}
}

TACO::NetworkProtocol TACO::Client::clientNetworkProtocol() throw (TACO::Exception)
{
	checkConnection();
        if (mPseudo)
		return TCP;
	NetworkProtocol protocol = static_cast<NetworkProtocol>( mDeviceHandle->rpc_protocol);
	switch (protocol) {
	case UDP:
	case TCP:
		return protocol;
	default:
		throw Exception( Error::INTERNAL_ERROR, "unknown network protocol");
	}
}

void TACO::Client::setClientNetworkProtocol( TACO::NetworkProtocol protocol) throw (TACO::Exception)
{
	checkConnection();
	if (mPseudo)
		return;
	DevLong e;
	if (dev_rpc_protocol( mDeviceHandle, static_cast<DevLong>( protocol), &e) != DS_OK) {	
		throw Exception( e);
	}
}

double TACO::Client::clientNetworkTimeout() throw (TACO::Exception)
{
	checkConnection();
	DevLong e;
	struct timeval tv = {3, 0};
	if (!mPseudo)
	{
		if (dev_rpc_timeout( mDeviceHandle, CLGET_TIMEOUT, &tv, &e) != DS_OK) {	
			throw Exception( e);
		}
	}
	double r;
	assign( r, tv);
	return r;
}

void TACO::Client::setClientNetworkTimeout( double timeout) throw (TACO::Exception)
{
	checkConnection();
	if (mPseudo)
		return;
	DevLong e;
	struct timeval tv;
	assign( tv, timeout);
	if (dev_rpc_timeout( mDeviceHandle, CLSET_TIMEOUT, &tv, &e) != DS_OK) {	
		throw Exception( e);
	}
}

void TACO::Client::deviceOn() throw (TACO::Exception)
{
	if (!mPseudo)
		return execute<void>( Command::DEVICE_ON);
}

void TACO::Client::deviceOff() throw (TACO::Exception)
{
	if (!mPseudo)
		return execute<void>( Command::DEVICE_OFF);
}

void TACO::Client::deviceReset() throw (TACO::Exception)
{
	if (!mPseudo)
		return execute<void>( Command::DEVICE_RESET);
}

short TACO::Client::deviceState() throw (TACO::Exception)
{
	if (mPseudo)
	{
		return ::TACO::State::ON;
	}
	else
		return execute<short>( Command::DEVICE_STATE);
}

std::string TACO::Client::deviceStatus() throw (TACO::Exception)
{
	if (mPseudo)
	{
		return "on";
	}
	else
		return execute<std::string>( Command::DEVICE_STATUS);
}

std::string TACO::Client::deviceVersion() throw (TACO::Exception)
{
	if (mPseudo)
	{
		return VERSION;
	}
	else
		return execute<std::string>( Command::DEVICE_VERSION);
}

std::string TACO::Client::deviceQueryResource( const std::string& name) throw (TACO::Exception)
{
	return execute<std::string>( Command::DEVICE_QUERY_RESOURCE, name);
}

void TACO::Client::deviceUpdateResource( const std::string& name, const std::string& value) throw (TACO::Exception)
{
//	std::cout << "updating resource " << name << std::endl;
	if (!mPseudo)
		return execute<void>( Command::DEVICE_UPDATE_RESOURCE, name + " " + value);
}

void TACO::Client::deviceUpdate() throw (TACO::Exception)
{
	if (!mPseudo)
		return execute<void>( Command::DEVICE_UPDATE);
}

TACO::ResourceInfoSet TACO::Client::deviceQueryResourceInfo() throw (TACO::Exception)
{
	ResourceInfoSet r;
	if (!mPseudo)
	{
		std::string data( execute<std::string>( Command::DEVICE_QUERY_RESOURCE_INFO));
		data >> r;
	}
	return r;
}

DevLong TACO::Client::eventListen( DevEvent event, TACO::EventHandler* handler, void* userData) throw (TACO::Exception)
{
	checkConnection();
	DevLong id;
	DevLong e;
	if (dev_event_listen( mDeviceHandle, event, 0, D_VOID_TYPE, handler, userData, &id, &e) != DS_OK) {
		throw Exception( e);
	}
	return id;
}

void TACO::Client::eventUnlisten( DevEvent event, DevLong id) throw (TACO::Exception)
{
	checkConnection();
	DevLong e;
	if (dev_event_unlisten( mDeviceHandle, event, id, &e) != DS_OK) {
		throw Exception( e);
	}
}

void* TACO::Client::eventHandler( TACO::EventHandlerData data) throw ()
{
	return 0;
}

void TACO::Client::execute( DevCommand cmd, void* argin, DevType inType, void* argout, DevType outType) throw (TACO::Exception)
{
	checkConnection();

	// There is one attempt for a bad server connection with reconnection,
	// one for the device on and one for the command execution.
	int executionAttempts = 3;
	while (--executionAttempts >= 0) {
		try {
	        	executeCore( cmd, argin, inType, argout, outType);
			return;
		} catch (Exception& e) {
			if (e == DevErr_BadServerConnection) {
				try {
					reconnectClient();
				} catch (Exception& e) {
					throw "automatic client reconnection failed: " >> e;
				}
			} else if (e == Error::EXECUTION_DENIED && mAutoDeviceOn) {
				try {
					ArgGet<short> state;
					executeCore( Command::DEVICE_STATE, 0, D_VOID_TYPE, state.address(), state.type());
					if (state.object() == State::DEVICE_OFF) {
						executeCore( Command::DEVICE_ON, 0, D_VOID_TYPE, 0, D_VOID_TYPE);
					} else {
						throw;
					}
				} catch (Exception& e) {
					throw "automatic device on failed: " >> e;
				}
			} else {
				throw;
			}
		}
	}
	throw Exception( Error::RUNTIME_ERROR, "cannot execute command");
}

void TACO::Client::executeCore( DevCommand cmd, void* argin, DevType inType, void* argout, DevType outType) throw (TACO::Exception)
{
	DevLong e;
	DevLong res;

	if (mPseudo)
		res = dc_devget(m_dc.dc_ptr, cmd, argout, outType, &e);
	else
		res = dev_putget(mDeviceHandle, cmd, argin, inType, argout, outType, &e); 
	if (res != DS_OK)
	{
// 0x7FFF largest error number, hopefully never used
// If this number isn't used the generated error string will be used
		Exception ex(e, plainErrorString(0x7FFF));
		throw ex;
	}
}

void TACO::Client::checkDeviceType( const std::string& type) throw (TACO::Exception)
{
	if (mServerTypes.find( type) == mServerTypes.end()) {
		throw Exception( Error::RUNTIME_ERROR, "connected device has inappropriate device type");
	}
}

void TACO::Client::checkDeviceTypes() throw (TACO::Exception)
{
	DeviceTypeSet::const_iterator i( mClientTypes.begin());
	while (i != mClientTypes.end()) {
		checkDeviceType( *i++);
	}
}

void* TACO::Client::eventHandlerCaller( devserver unused, void* thisPointer, EventHandlerData eventHandlerData) throw ()
{
	return static_cast<Client*>( thisPointer)->eventHandler( eventHandlerData);
}

void TACO::Client::checkConnection() throw (TACO::Exception)
{
	if (!isClientConnected()) {
		if (mAutoConnect) {
			try {
				connectClient();
			} catch (Exception& e) {
				throw "automatic client connection failed: " >> e;
			}
		} else {
			throw Exception( Error::RUNTIME_ERROR, "client not connected");
		}
	}
}

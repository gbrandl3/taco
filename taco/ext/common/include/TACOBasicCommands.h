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

#ifndef TACO_BASIC_COMMANDS_H
#define TACO_BASIC_COMMANDS_H

#include <DevCmds.h>

#define TACO_COMMAND_DEVICE_VERSION 4001
#define TACO_COMMAND_DEVICE_TYPES 4002
#define TACO_COMMAND_DEVICE_UPDATE_RESOURCE 4003
#define TACO_COMMAND_DEVICE_QUERY_RESOURCE 4004
#define TACO_COMMAND_DEVICE_UPDATE 4005
#define TACO_COMMAND_DEVICE_QUERY_RESOURCE_INFO 4006

#ifdef __cplusplus

namespace TACO {
	//! Contains all %TACO commands
	namespace Command {
		const DevCommand DEVICE_STATE = DevState;
		const DevCommand DEVICE_STATUS = DevStatus;
		const DevCommand DEVICE_ON = DevOn;
		const DevCommand DEVICE_OFF = DevOff;
		const DevCommand DEVICE_RESET = DevReset;
		const DevCommand DEVICE_VERSION = TACO_COMMAND_DEVICE_VERSION;
		const DevCommand DEVICE_TYPES = TACO_COMMAND_DEVICE_TYPES;
		const DevCommand DEVICE_UPDATE_RESOURCE = TACO_COMMAND_DEVICE_UPDATE_RESOURCE;
		const DevCommand DEVICE_QUERY_RESOURCE = TACO_COMMAND_DEVICE_QUERY_RESOURCE;
		const DevCommand DEVICE_UPDATE = TACO_COMMAND_DEVICE_UPDATE;
		const DevCommand DEVICE_QUERY_RESOURCE_INFO = TACO_COMMAND_DEVICE_QUERY_RESOURCE_INFO;
	}
}

#endif /* __cplusplus */

#endif /* TACO_BASIC_COMMANDS_H */

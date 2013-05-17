/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright(c) 1994-2005 by European Synchrotron Radiation Facility,
 *                     Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:        hello.c
 *
 * Description: example device server
 *
 * Author(s):   $Author: jkrueger1 $
 *
 * Original:    December 99
 * 
 * Date:	$Date: 2013-05-17 08:05:49 $
 *
 * Version:	$Revision: 1.1 $
 */
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevServerP.h>
#include "helloP.h"

static long class_initialise(DevLong *);
static long object_create(char *, DevServer *, DevLong *);
static long object_initialise();
static long state_handler();

static long dev_on();
static long dev_off();
static long dev_state(Hello, DevVoid *, DevShort *, DevLong *);
static long dev_status();
static long dev_reset();
static long dev_hello();

static DevMethodListEntry methods_list[] = {
	{DevMethodClassInitialise, 	class_initialise},
	{DevMethodCreate,		object_create},
	{DevMethodInitialise,		object_initialise},
	{DevMethodStateHandler,		state_handler},
};

static DevCommandListEntry command_list[] = {
	{DevOn, dev_on, D_VOID_TYPE, D_VOID_TYPE, },
	{DevOff, dev_off, D_VOID_TYPE, D_VOID_TYPE, },
	{DevState, dev_state, D_VOID_TYPE, D_SHORT_TYPE, },
	{DevStatus, dev_status, D_VOID_TYPE, D_STRING_TYPE, },
	{DevReset, dev_reset, D_VOID_TYPE, D_VOID_TYPE, },
	{DevHello, dev_hello, D_STRING_TYPE, D_STRING_TYPE, },
};

HelloClassRec helloClassRec = {
	/* n_methods */ sizeof(methods_list) / sizeof(DevMethodListEntry),
	/* methods_list */ methods_list,
};

HelloClass helloClass = (HelloClass)&helloClassRec;

static long n_commands = sizeof(command_list) / sizeof(DevCommandListEntry);

static HelloRec helloRec;
Hello hello = (Hello)&helloRec;

static long class_initialise(DevLong *error)
{
	return DS_OK;
}
	
static long object_create(char *name, DevServer *ds_ptr, DevLong *error)
{
	Hello hello = (Hello)malloc(sizeof(HelloRec));
	if (!hello)
		return DS_NOTOK;
	hello->devserver.name = (char *)malloc(strlen(name) + 1);
	if (!hello)
		return DS_NOTOK;
	strcpy(hello->devserver.name, name);
	*ds_ptr = (DevServer)hello;
	return DS_OK;
}

static long object_initialise(Hello hello, DevLong *error)
{
	return DS_OK;
}

static long state_handler(Hello hello, DevLong *error)
{
	return DS_OK;
}

static long dev_state(Hello hello, DevArgument argin, DevShort *argout, DevLong *error)
{
	*argout = hello->devserver.state;
	return DS_OK;
}

static long dev_status(Hello hello, DevArgument argin, DevString *argout, DevLong *error)
{
	static char message[1024];

	switch(hello->devserver.state)
	{
		case DEVON :
			snprintf(message, sizeof(message) - 1, "%s", "On");
			break;
		case DEVOFF :
			snprintf(message, sizeof(message) - 1, "%s", "Off");
			break;
		default :
			snprintf(message, sizeof(message) - 1, "%s", "Unknown");
			break;
	}
	*argout = message;
	return DS_OK;
}

static long dev_on(Hello hello, DevArgument argin, DevArgument argout, DevLong *error)
{
	hello->devserver.state = DEVON;
	return DS_OK;
}

static long dev_off(Hello hello, DevArgument argin, DevArgument argout, DevLong *error)
{
	hello->devserver.state = DEVOFF;
	return DS_OK;
}

static long dev_reset(Hello hello, DevArgument argin, DevArgument argout, DevLong *error)
{
	hello->devserver.state = DEVON;
	return DS_OK;
}

static long dev_hello(Hello hello, DevString *argin, DevString *argout, DevLong *error)
{
        if (!strcmp(*argin, "Hello"))
	{
		DevString ret = "World";
		DevString tmp = malloc(strlen(ret) + 1);
		strcpy(tmp, ret);
		*argout = tmp;
		return DS_OK;
	}
	*error = DevErr_DeviceSyntaxError;
	return DS_NOTOK;
}

/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
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
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
#include <API.h>
#include <Admin.h>
#include <stdio.h>
#include <private/ApiP.h>

typedef struct event_relisten_info {
    long event_id; 
    long event_type;
    DevArgument argout;
    DevType argout_type;
    DevCallbackFunction *callback;
    void *user_data;
    long *event_id_ptr;
    struct event_relisten_info* next;
    struct event_relisten_info* prev;
}Event_Relisten_Info ;

Event_Relisten_Info	*event_head;

long _DLLFunc dev_event_listen_x (devserver ds, long event_type,
				  DevArgument argout, DevType argout_type, 
				  DevCallbackFunction *callback, void
				  *user_data, 
				  long *event_id_ptr, DevLong *error);

long _DLLFunc dev_event_unlisten_x (devserver ds, long event_type,
                                  long event_id, DevLong *error);

static long add_relisten_info(long, long, DevArgument, DevType, DevCallbackFunction *, void *, long *);
static long remove_relisten_info(long, long);

/**@defgroup eventAPIintern Internals of the event API
 * @ingroup eventAPI
 */

/**@ingroup dsAPIintern
 *
 * @param ds	the device server
 *
 * @retval DS_OK 
 * @retval DS_NOTOK in case of error
 */
long relisten_events(devserver ds)
{
	DevLong error;
    
	Event_Relisten_Info* start = event_head;
	dev_printdebug(DBG_TRACE | DBG_API, "relisten_events called\n");
	while(start)
	{
		dev_event_listen_x(ds, start->event_type, start->argout,
			     start->argout_type, start->callback,
			     start->user_data, start->event_id_ptr,
			     &error);
		start->event_id = *(start->event_id_ptr);
		start = start->next;
	}

	return DS_OK;
}

/**@ingroup eventAPI
 *
 * application interface to register a client as a listener
 * for events of a specified type from a device server.
 * The clients specifies a callback routine which will
 * be called every time an event of this type occurs.
 * The client has to pass pointers to output arguments of
 * a type appropriate to the event type. The client remains
 * registered until such time as a dev_event_unlisten()
 * call is issued.
 *
 * @param ds            handle to access the device.
 * @param event_type    event to listen for
 * @param callback      callback routine to be triggered on completion
 * @param user_data     pointer to user data to be passed to callback function
 * @param argout        pointer for output arguments.
 * @param argout_type   data type of output arguments.
 * @param event_id_ptr  client event identifier
 * @param error         Will contain an appropriate error code if the
 *                      corresponding call returns a non-zero value.
 * 
 * @retval DS_OK if event was successfully registered 
 * @retval DS_NOTOK otherwise 
 */
long _DLLFunc dev_event_listen (devserver ds, long event_type,
				DevArgument argout, DevType argout_type, 
				DevCallbackFunction *callback, void *user_data,
				long *event_id_ptr, DevLong *error)
{
	long ret;
	dev_printdebug(DBG_TRACE | DBG_API, "dev_event_listen called\n");
	ret = dev_event_listen_x(ds, event_type, argout,  argout_type, 
				 callback, user_data, event_id_ptr, error);
	if(ret == DS_OK)
	{
		add_relisten_info(*event_id_ptr,event_type, argout,  argout_type, 
			  callback, user_data, event_id_ptr);
	}
	return ret;
}

/**@ingroup eventAPI
 * application interface for a client to unlisten to an event. Client will be
 * unregistered in the server and will not receive anymore events of this type.
 *
 * @param ds            handle to access the device.
 * @param event_type    event to listen for
 * @param event_id      client event identifier
 * @param error         Will contain an appropriate error code if the
 *                      corresponding call returns a non-zero value.
 *
 * @retval DS_OK if event was successfully unregistered 
 * @retval DS_NOTOK otherwise
 */
long _DLLFunc dev_event_unlisten (devserver ds, long event_type,
                                  long event_id, DevLong *error)
{
	long ret;
	dev_printdebug(DBG_TRACE | DBG_API, "dev_event_unlisten called\n");
	ret = dev_event_unlisten_x(ds, event_type, event_id, error);
	if(ret == DS_OK)
		remove_relisten_info(event_id,event_type);
	return ret;
}

/**@ingroup eventAPIintern
 *
 * @param event_id
 * @param event_type
 * @param argout
 * @param argout_type
 * @param callback
 * @param user_data
 * @param event_id_ptr
 *
 * @retval DS_OK if the event added successfully 
 * @retval DS_NOTOK in case of error
 */
static long add_relisten_info(long event_id, 
		       long event_type,
		       DevArgument argout,
		       DevType argout_type,
		       DevCallbackFunction *callback,
		       void *user_data,
		       long *event_id_ptr)
{
	Event_Relisten_Info	*start = event_head,
				*newevent;
	dev_printerror(DBG_TRACE | DBG_API, "add_relisten_info called\n");
    
	newevent = (Event_Relisten_Info*)malloc(sizeof(Event_Relisten_Info));
	if(newevent)
	{
		newevent->next=NULL;
		newevent->prev=NULL;
		newevent->event_id=event_id; 
		newevent->event_type=event_type;
		newevent->argout=argout;
		newevent->argout_type=argout_type;
		newevent->callback=callback;
		newevent->user_data=user_data;
		newevent->event_id_ptr=event_id_ptr;
	}
	else
		return DS_NOTOK;
	if(start)
	{
		while(start->next)
		{
			start=start->next;
		}
		newevent->prev = start;
		start->next = newevent;
	}
	else
	{
		start = newevent;
		event_head = start;
	}
	return DS_OK;
}

/**@ingroup eventAPIintern
 *
 * @param event_id
 * @param event_type
 *
 * @retval DS_OK if event successfully unregistered 
 * @retval DS_NOTOK otherwise 
 */
static long remove_relisten_info(long event_id, long event_type)
{
	Event_Relisten_Info 	*start = event_head,
				*old;

	dev_printerror(DBG_TRACE | DBG_API, "remove_relisten_info called\n");
	while(start)
	{
		if(start->event_id == event_id && start->event_type == event_type)
		{
			old = start;
			if(old->prev != NULL)
			{
				old->prev->next = old->next;
				start = old->prev;
			}
			else
			{
				start = event_head = old->next;
			}
			if(old->next!=NULL)
			{
				old->next->prev = old->prev;
			}
			free(old);
			return DS_OK;
		}
		start = start->next;
	}
	return DS_NOTOK;
}

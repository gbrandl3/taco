#include <API.h>
#include <stdio.h>

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

long add_relisten_info(long event_id, 
		       long event_type,
		       DevArgument argout,
		       DevType argout_type,
		       DevCallbackFunction *callback,
		       void *user_data,
		       long *event_id_ptr);

long relisten_events(devserver ds);

long _DLLFunc dev_event_listen_x (devserver ds, long event_type,
				  DevArgument argout, DevType argout_type, 
				  DevCallbackFunction *callback, void
				  *user_data, 
				  long *event_id_ptr, long *error);

long _DLLFunc dev_event_unlisten_x (devserver ds, long event_type,
                                  long event_id, long *error);

long remove_relisten_info(long event_id, long event_type);



long relisten_events(devserver ds)
{
	long error;
    
	Event_Relisten_Info* start = event_head;
#ifdef EBUG
	fprintf(stderr, "relisten_events called\n");
#endif
	while(start)
	{
		dev_event_listen_x(ds, start->event_type, start->argout,
			     start->argout_type, start->callback,
			     start->user_data, start->event_id_ptr,
			     &error);
		start->event_id = *(start->event_id_ptr);
		start = start->next;
	}
}

long _DLLFunc dev_event_listen (devserver ds, long event_type,
				DevArgument argout, DevType argout_type, 
				DevCallbackFunction *callback, void *user_data,
				long *event_id_ptr, long *error)
{
	long ret;
#ifdef EBUG
	fprintf(stderr, "dev_event_listen called\n");
#endif
	ret = dev_event_listen_x(ds, event_type,
				 argout,  argout_type, 
				 callback, user_data,
				 event_id_ptr, error);
	if(ret == DS_OK)
	{
		add_relisten_info(*event_id_ptr,event_type,
			  argout,  argout_type, 
			  callback, user_data,
			  event_id_ptr);
	}
	return ret;
}

long _DLLFunc dev_event_unlisten (devserver ds, long event_type,
                                  long event_id, long *error)
{
	long ret;
#ifdef EBUG
	fprintf(stderr,"dev_event_unlisten called\n");
#endif
	ret = dev_event_unlisten_x(ds, event_type, event_id, error);
	if(ret == DS_OK)
		remove_relisten_info(event_id,event_type);
	return ret;
}


long add_relisten_info(long event_id, 
		       long event_type,
		       DevArgument argout,
		       DevType argout_type,
		       DevCallbackFunction *callback,
		       void *user_data,
		       long *event_id_ptr)
{
	Event_Relisten_Info	*start = event_head,
				*newevent;
#ifdef EBUG
	fprintf(stderr,"add_relisten_info called\n");
#endif
    
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

long remove_relisten_info(long event_id, long event_type)
{
	Event_Relisten_Info 	*start = event_head,
				*old;

#ifdef EBUG
	fprintf(stderr,"remove_relisten_info called\n");
#endif
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

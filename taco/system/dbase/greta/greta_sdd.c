#include <greta.h>

/* Some global variables */

dev_window *dev_tab[TAB_SIZE];
serv_window *serv_tab[TAB_SIZE];
res_window *gr_res_tab[TAB_SIZE];
dev_window_dc *dev_dc_tab[TAB_SIZE];


/* Some global  variables for this file */

static empty_case *empty_dev;
static int max_dev_ind;
static empty_case *empty_dev_dc;
static int max_dev_dc_ind;
static empty_case *empty_serv;
static int max_serv_ind;
static empty_case *empty_res;
static int max_res_ind;

/* Some local functions */

void add_empty_head(empty_case **,int);
void free_empty_head(empty_case **);
void free_dev_struct(int);
void free_serv_struct(int);
void free_res_struct(int);
void free_dev_dc_struct(int);

/* Functions to manipulate the Data Structures */

void add_empty_head(empty_case **head, int index)
{
 	empty_case *tmp;
 
	tmp=(empty_case *)malloc(sizeof(empty_case));
	tmp->val=index;
	tmp->next=*head;
	*head=tmp;
}
	
void free_empty_head(empty_case **head)
{
	empty_case *tmp;
 
	tmp=*head;
	*head=(*head)->next;
	free(tmp);
}
	
/*
 * DEVICES window functions
 */

/*************************************************************************
*									 *
*				init_dev_index :			 *
*				--------------				 *
*									 *
*	Function rule : to be called in DataInitialization to initialize *
*			empty_dev and max_dev_ind so as they remains     * 
*			internal to greta_sdd.c .			 *
*									 *
**************************************************************************/

void init_dev_index()
{
	empty_dev=NULL;
	max_dev_ind=0;
}

/************************************************************************
*									*
*				new_dev_index :				*
*				-------------				*
*									*
*	Function rule : returns the next available index and consequentl*
*			modifies the control structures (empty_dev list	*
*			and max_dev_ind). If no index is available,     *
*			returns -1 and doesn't touch the control 	*
*			structures .					*
*									*
*************************************************************************/

int new_dev_index()
{
	int index=-1; 
	
/* initialize index to the value returned if no index is available
*/

	if (empty_dev!=NULL)
	{/* if dev_tab isn't contigue, give to index the value of a gap */
		index=empty_dev->val;
		free_empty_head(&empty_dev);
	}
	else if (max_dev_ind<TAB_SIZE)
			index=max_dev_ind++;

	return index;

}

/****************************************************************************
*									    *
*				create_dev_struct :			    *
*				-----------------			    *
*									    *
*	Function rule : create a dev_struct corresponding to index parameter*
*			and fill its widget and name fields with others	    *
*			parameters . Index must result from a new_dev_index *
*			call, and create_dev_struct should be called from   *
*			the _xs dynamical window creation corresponding	    *
*			procedure . At this point the content of the window *
*			may be unknown, so file and last_state backup fields*
*			are initialized to "" and NULL respectively .	    *
*									    *
****************************************************************************/

void create_dev_struct (int index, char *dev_name, Widget window,
			Widget infotext, Widget resotext)
{
        int       ind_ch;

	dev_tab[index]=(dev_window *)malloc(sizeof(dev_window));
	dev_tab[index]->devname=strdup(dev_name); 
	dev_tab[index]->window=window; 
	dev_tab[index]->infotext=infotext;
	dev_tab[index]->resotext=resotext;
	dev_tab[index]->last_state=NULL;
	strcpy(dev_tab[index]->file, ""); 
        dev_tab[index]->dev_save_pathname=NULL;
	dev_tab[index]->pid = 0;
	dev_tab[index]->pn = 0;
	dev_tab[index]->host_name[0] = '\0';
	dev_tab[index]->proc_name[0] = '\0';
	dev_tab[index]->pers_name[0] = '\0';
	dev_tab[index]->exported = False;

        for (ind_ch=0; ind_ch < CH_TAB_SIZE; ind_ch++)
            (dev_tab[index]->chaine_dev_arr)[ind_ch] = NULL;
}
				
/****************************************************************************
*											*
*							freeing functions :								*
*							-----------------								*
*																			*
*	free_dev_index(index) : Mark index as available again .					*
*	free_dev_struct(index) : Destroy the associated resource file, free the	*
*							 associated dev_window and consequently put 	*
*							 dev_tab entry to NULL .						*
*	dev_remove(index) : free_dev_struct and then free_dev_index .			*
*	free_dev_sdd() : free empty_dev list and the associated structures (all	*
*					 dev_tab entries before max_dev_ind which are not NULL)	*
*																			*
****************************************************************************/

void free_dev_index(int index)
{

/* Mark index as available again */
	
	if (index==max_dev_ind-1) max_dev_ind--;
	
	else /* index is a new gap in dev_tab, to be chained at the empty_dev
			list head*/
	{
		add_empty_head(&empty_dev, index);
	}
}

void free_dev_struct(int index)
{
        int       ind_ch;

/* Destroy the associated resource file */

	if (strcmp(dev_tab[index]->file,"") != 0)
		remove(dev_tab[index]->file);

/* Free the dev_window pointed to at index */

	free(dev_tab[index]->devname);
	free(dev_tab[index]->last_state);
        if (dev_tab[index]->dev_save_pathname != NULL)
           free(dev_tab[index]->dev_save_pathname);

        for (ind_ch =0; ind_ch < CH_TAB_SIZE; ind_ch++)
             if ((dev_tab[index]->chaine_dev_arr)[ind_ch] != NULL)
             {
                free( (dev_tab[index]->chaine_dev_arr)[ind_ch] );
                (dev_tab[index]->chaine_dev_arr)[ind_ch] = NULL;
             }

	free(dev_tab[index]);
	/* Allow unused cases identification and systematic freeing of dev_tab 
	   when closing the application */
	dev_tab[index]=NULL;
	/* Index has to be chained in empty_dev */
	
}

void 
dev_remove(int index)
{

/* Free the dev_window pointed to at index */
	free_dev_struct(index);
		
/* Mark index as available again */
	free_dev_index(index);
}

void free_dev_sdd()
{
 int i;
 
/* Free the empty_dev chained list */
	while(empty_dev!=NULL) free_empty_head(&empty_dev);

/* Free each not-belonging-to-empty-dev dev_struct before max_dev_ind */
	for(i=0;i<max_dev_ind;i++) 
		if(dev_tab[i]!=NULL) free_dev_struct(i);
	/* the pointers array dev_tab itself does not need to be freed since it's
	   statically defined with TAB_SIZE */
}


/*
 * DEVICES WITH DC/HDB INFO
 */

/************************************************************************
*									*
*				init_dev_index :			*
*				--------------				*
*									*
*	Function rule : to be called in DataInitialization to initialize*
*			empty_dev and max_dev_ind so as they remains    * 
*			internal to greta_sdd.c .			*
*									*
*************************************************************************/

void init_dev_dc_index()
{
	empty_dev_dc = NULL;
	max_dev_dc_ind = 0;
}

/************************************************************************
*									*
*				new_dev_index :				*
*				-------------				*
*									*
*	Function rule : returns the next available index and consequently*
*			modifies the control structures (empty_dev list	*
*			and max_dev_ind). If no index is available, returns*
*			-1 and doesn't touch the control structures .	*
*									*
****************************************************************************/

int new_dev_dc_index()
{
	int index = -1; 
	
/* initialize index to the value returned if no index is available */

	if (empty_dev_dc != NULL)
	{
		index = empty_dev_dc->val;
		free_empty_head(&empty_dev_dc);
	}
	else if (max_dev_dc_ind < TAB_SIZE)
			index=max_dev_dc_ind++;

	return index;

}

/****************************************************************************
*									    *
*				create_dev_struct :			    *
*				-----------------			    *
*									    *
*	Function rule : create a dev_struct corresponding to index parameter*
*			and fill its widget and name fields with others	    *
*			parameters . Index must result from a new_dev_index *
*			call, and create_dev_struct should be called from   *
*			the _xs dynamical window creation corresponding	    *
*			procedure . At this point the content of the window *
*			may be unknown, so file and last_state backup fields*
*			are initialized to "" and NULL respectively .	    *
*									    *
****************************************************************************/

void create_dev_dc_struct (int index, char *dev_name, Widget window,
		      	   Widget infotext, Widget resotext, Widget dctext)
{
        int       ind_ch;

	dev_dc_tab[index]=(dev_window_dc *)malloc(sizeof(dev_window_dc));
	dev_dc_tab[index]->devname=strdup(dev_name); 
	dev_dc_tab[index]->window=window; 
	dev_dc_tab[index]->infotext=infotext;
	dev_dc_tab[index]->resotext=resotext;
	dev_dc_tab[index]->dctext=dctext;
	dev_dc_tab[index]->last_state=NULL;
	strcpy(dev_dc_tab[index]->file, ""); 
        dev_dc_tab[index]->dev_save_pathname=NULL;
	dev_dc_tab[index]->pid = 0;
	dev_dc_tab[index]->pn = 0;
	dev_dc_tab[index]->host_name[0] = '\0';
	dev_dc_tab[index]->proc_name[0] = '\0';
	dev_dc_tab[index]->pers_name[0] = '\0';
	dev_dc_tab[index]->exported = False;

        for (ind_ch=0; ind_ch < CH_TAB_SIZE; ind_ch++)
            (dev_dc_tab[index]->chaine_dev_arr)[ind_ch] = NULL;
}
				
/****************************************************************************
*																			*
*							freeing functions :								*
*							-----------------								*
*																			*
*	free_dev_index(index) : Mark index as available again .					*
*	free_dev_struct(index) : Destroy the associated resource file, free the	*
*							 associated dev_window and consequently put 	*
*							 dev_tab entry to NULL .						*
*	dev_remove(index) : free_dev_struct and then free_dev_index .			*
*	free_dev_sdd() : free empty_dev list and the associated structures (all	*
*					 dev_tab entries before max_dev_ind which are not NULL)	*
*																			*
****************************************************************************/

void free_dev_dc_index(int index)
{

/* Mark index as available again */
	
	if (index == max_dev_dc_ind - 1)
		max_dev_dc_ind--;
	
	else 
	{
		add_empty_head(&empty_dev_dc, index);
	}
}

void free_dev_dc_struct(int index)
{
        int       ind_ch;

/* Destroy the associated resource file */

	if (strcmp(dev_dc_tab[index]->file,"") != 0)
		remove(dev_dc_tab[index]->file);

/* Free the dev_window pointed to at index */

	free(dev_dc_tab[index]->devname);
	if (dev_dc_tab[index]->last_state != NULL)
		free(dev_dc_tab[index]->last_state);
        if (dev_dc_tab[index]->dev_save_pathname != NULL)
           free(dev_dc_tab[index]->dev_save_pathname);

        for (ind_ch =0; ind_ch < CH_TAB_SIZE; ind_ch++)
             if ((dev_dc_tab[index]->chaine_dev_arr)[ind_ch] != NULL)
             {
                free( (dev_dc_tab[index]->chaine_dev_arr)[ind_ch] );
                (dev_dc_tab[index]->chaine_dev_arr)[ind_ch] = NULL;
             }

	free(dev_dc_tab[index]);
	/* Allow unused cases identification and systematic freeing of dev_tab 
	   when closing the application */
	dev_dc_tab[index]=NULL;
	/* Index has to be chained in empty_dev */
	
}

void dev_dc_remove(int index)
{

/* Free the dev_window pointed to at index */

	free_dev_dc_struct(index);
		
/* Mark index as available again */

	free_dev_dc_index(index);
}

void free_dev_dc_sdd()
{
 	int i;
 
/* Free the empty_dev chained list */

	while(empty_dev_dc != NULL) 
		free_empty_head(&empty_dev_dc);

/* Free each not-belonging-to-empty-dev dev_struct before max_dev_ind */

	for(i = 0;i < max_dev_dc_ind;i++)
	{ 
		if(dev_dc_tab[i]!=NULL)
			free_dev_dc_struct(i);
	}

}



/*
 * SERVERS
 */

/****************************************************************************
*																			*
*							init_serv_index :								*
*							---------------									*
*																			*
*		Function rule : to be called in DataInitialization to initialize 	*
*						empty_serv and max_serv_ind so as they remains 		* 
*						internal to greta_sdd.c .							*
*																			*
****************************************************************************/

void 
init_serv_index()
{
	empty_serv=NULL;
	max_serv_ind=0;
}

/****************************************************************************
*																			*
*							new_serv_index :								*
*							--------------									*
*																			*
*		Function rule : returns the next available index and consequently	*
*						modifies the control structures (empty_serv list	*
*						and max_serv_ind). If no index is available, returns*
*						 -1 and doesn't touch the control structures .		*
*																			*
****************************************************************************/

int 
new_serv_index()
{
int index=-1; 
/* initialize index to the value returned if no index is available */

	if (empty_serv!=NULL)
	{/* if serv_tab isn't contigue, give to index the value of a gap */
		index=empty_serv->val;
		free_empty_head(&empty_serv);
	}
	else if (max_serv_ind<TAB_SIZE)
			index=max_serv_ind++;

	return index;

}

/************************************************************************
*									*
*			create_serv_struct :				*
*			------------------				*
*									*
*	Function rule : create a serv_struct corresponding to index	* 
*	parameter							*
*	and fill its widget and name fields with others			*
*	parameters . Index must result from a new_serv_index		*
*	call, and create_serv_struct should be called from 		*
*	the _xs dynamical window creation corresponding			*
*	procedure . At this point the content of the window 		*
*	may be unknown, so file and last_state backup fields		*
*	are not initialized here .					*
*									*
*************************************************************************/

void 
create_serv_struct (int index, char *serv_name, Widget window,
					Widget infotext, Widget devstext, Widget resotext)
{
        int       ind_ch;


	serv_tab[index]=(serv_window *)malloc(sizeof(serv_window));
	serv_tab[index]->servname=strdup(serv_name); 
	serv_tab[index]->window=window; 
	serv_tab[index]->infotext=infotext;
	serv_tab[index]->devstext=devstext; 
	serv_tab[index]->resotext=resotext;
	serv_tab[index]->last_state.devs=NULL; 
	serv_tab[index]->last_state.reso=NULL;
	strcpy(serv_tab[index]->file, ""); 
	serv_tab[index]->pid = 0;
	serv_tab[index]->pn = 0;
	serv_tab[index]->host_name[0] = '\0';
	serv_tab[index]->proc_name[0] = '\0';
	serv_tab[index]->pers_name[0] = '\0';
	serv_tab[index]->restart = True;
	
        for (ind_ch=0; ind_ch < CH_TAB_SIZE; ind_ch++)
            (serv_tab[index]->chaine_dev_arr)[ind_ch] = NULL;
}

/* Only once create_serv_struct has been called : */

/* Fill serv_tab[index]->last_state with devstext and resotext */
/* save_text first frees the text pointers before assigning them again .
so the text pointers (...devs and ...reso) has to be initialized (to NULL or 
an address resulting from memory allocation ) */

void 
save_text(int index)
{
 	serv_status *status=&(serv_tab[index]->last_state);

	if (status->devs != NULL)
		XtFree(status->devs);
	if (status->reso != NULL)
		XtFree(status->reso);
	
	status->devs=(char *)XmTextGetString(serv_tab[index]->devstext);
	status->reso=(char *)XmTextGetString(serv_tab[index]->resotext);
	status->devspos=
	(XmTextPosition)XmTextGetInsertionPosition(serv_tab[index]->devstext);
	status->resopos=	
	(XmTextPosition)XmTextGetInsertionPosition(serv_tab[index]->resotext);

}


/****************************************************************************
*																			*
*							freeing functions :								*
*							-----------------								*
*																			*
*	free_serv_index(index) : Mark index as available again .				*
*	free_serv_struct(index) : Destroy the associated resource file, free the*
*							  associated serv_window and consequently put 	*
*							  serv_tab entry to NULL .						*
*	serv_remove(index) : free_serv_struct and then free_serv_index .		*
*	free_serv_sdd() : free empty_serv list and the associated structures 	*
*					  (all serv_tab entries before 	max_serv_ind which are	*
*					   not NULL)											*
*																			*
****************************************************************************/

void 
free_serv_index(int index)
{
	
/* Mark index as available again */
	
	if (index==max_serv_ind-1) max_serv_ind--;
	
	else /* index is a new gap in serv_tab, to be chained at the empty_serv
			list head */
	{
		add_empty_head(&empty_serv, index);
	}
}

void free_serv_struct(int index)
{
        int       ind_ch;

/* Destroy the associated resource file */

	if (strcmp(serv_tab[index]->file,"") != 0)
		remove(serv_tab[index]->file);
	
/* Free the serv_window pointed to at index */

	free(serv_tab[index]->servname);
	if (serv_tab[index]->last_state.devs != NULL)	
		XtFree(serv_tab[index]->last_state.devs);
	if (serv_tab[index]->last_state.devs != NULL)	
		XtFree(serv_tab[index]->last_state.reso);	
/*	free(serv_tab[index]->last_state);*/	

        for (ind_ch =0; ind_ch < CH_TAB_SIZE; ind_ch++)
             if ((serv_tab[index]->chaine_dev_arr)[ind_ch] != NULL)
             {
                free( (serv_tab[index]->chaine_dev_arr)[ind_ch] );
                (serv_tab[index]->chaine_dev_arr)[ind_ch] = NULL;
             }

	free(serv_tab[index]);
	/* Allow unused cases identification and systematic freeing of serv_tab 
	   when closing the application */
	serv_tab[index]=NULL;
	/* Index has to be chained in empty_dev */

}

/* serv_remove free the structure associated to index and consequently
update the control pointers max_serv_ind and empty_serv */

void 
serv_remove(int index)
{	
	
/* Free the serv_window pointed to at index */
	free_serv_struct(index);
	
/* Mark index as available again */
	free_serv_index(index);

}

void free_serv_sdd()
{
 int i;
 
/* Free the empty_serv chained list */

	while(empty_serv!=NULL) free_empty_head(&empty_serv);

/* Free each serv_struct before max_serv_ind */
	for(i=0;i<max_serv_ind;i++) 
		if (serv_tab[i]!=NULL) free_serv_struct(i);
	/* the pointers array serv_tab itself does not need to be freed since it's
	   statically defined with TAB_SIZE */

}

/*
 * RESOURCES
 */

/****************************************************************************
*																			*
*							init_res_index :								*
*							--------------									*
*																			*
*		Function rule : to be called in DataInitialization to initialize 	*
*						empty_res and max_res_ind so as they remains 		* 
*						internal to greta_sdd.c .							*
*																			*
****************************************************************************/

void 
init_res_index()
{
	empty_res=NULL;
	max_res_ind=0;
}

/****************************************************************************
*																			*
*							new_res_index :									*
*							-------------									*
*																			*
*		Function rule : returns the next available index and consequently	*
*						modifies the control structures (empty_res list		*
*						and max_res_ind). If no index is available, returns	*
*						 -1 and doesn't touch the control structures .		*
*																			*
****************************************************************************/

int 
new_res_index()
{
int index=-1; 
/* initialize index to the value returned if no index is available */

	if (empty_res!=NULL)
	{/* if gr_res_tab isn't contigue, give to index the value of a gap */
		index=empty_res->val;
		free_empty_head(&empty_res);
	}
	else if (max_res_ind<TAB_SIZE)
			index=max_res_ind++;

	return index;

}

/****************************************************************************
*																			*
*							create_res_struct :								*
*							-----------------								*
*																			*
*		Function rule : create a res_struct corresponding to index parameter*
*						and fill its widget and name fields with others		*
*						parameters . Index must result from a new_res_index	*
*						call, and create_res_struct should be called from 	*
*						the _xs dynamical window creation corresponding		*
*						procedure . At this point the content of the window *
*						may be unknown, so file and last_state backup fields*
*						are not initialized here .							*
*																			*
****************************************************************************/

void 
create_res_struct (int index, char *res_filter, Widget window, Widget resotext)
{
	gr_res_tab[index]=(res_window *)malloc(sizeof(res_window));
	gr_res_tab[index]->resfilter=strdup(res_filter); 
	gr_res_tab[index]->window=window; 
	gr_res_tab[index]->resotext=resotext;
	gr_res_tab[index]->last_state=NULL;
	strcpy(gr_res_tab[index]->file, ""); 
        gr_res_tab[index]->res_save_pathname=NULL;
}
				
/****************************************************************************
*																			*
*							freeing functions :								*
*							-----------------								*
*																			*
*	free_res_index(index) : Mark index as available again .					*
*	free_res_struct(index) : Destroy the associated resource file, free the	*
*							 associated res_window and consequently put 	*
*							 gr_res_tab entry to NULL .						*
*	res_remove(index) : free_res_struct and then free_res_index .			*
*	free_res_sdd() : free empty_res list and the associated structures (all	*
*					 gr_res_tab entries before max_res_ind which are not NULL)	*
*																			*
****************************************************************************/

void free_res_index(int index)
{

/* Mark index as available again */
	
	if (index==max_res_ind -1) max_res_ind--;
	
	else /* index is a new gap in gr_res_tab, to be chained at the empty_res
			list head*/
	{
		add_empty_head(&empty_res, index);
	}
}

void free_res_struct(int index)
{
/* Destroy the associated resource file */

	remove(gr_res_tab[index]->file);

/* Free the res_window pointed to at index */

	free(gr_res_tab[index]->resfilter);
	free(gr_res_tab[index]->last_state);
        if (gr_res_tab[index]->res_save_pathname != NULL)
           free(gr_res_tab[index]->res_save_pathname);

	free(gr_res_tab[index]);
	/* Allow unused cases identification and systematic freeing of gr_res_tab 
	   when closing the application */
	gr_res_tab[index]=NULL;
	/* Index has to be chained in empty_res */
	
}

void 
res_remove(int index)
{

/* Free the res_window pointed to at index */
	free_res_struct(index);
		
/* Mark index as available again */
	free_res_index(index);
}

void free_res_sdd()
{
 int i;
 
/* Free the empty_res chained list */
	while(empty_res!=NULL) free_empty_head(&empty_res);

/* Free each not-belonging-to-empty-res res_struct before max_res_ind */
	for(i=0;i<max_res_ind;i++) 
		if(gr_res_tab[i]!=NULL) free_res_struct(i);
	/* the pointers array gr_res_tab itself does not need to be freed since it's
	   statically defined with TAB_SIZE */
}


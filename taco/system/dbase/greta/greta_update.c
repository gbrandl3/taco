/****************************************************************************

 File          :  greta_update.c

 Project       :  GRETA application

 Description   :  The module which contains all the entry points for update
 		  callbacks

 Author        :  Builder Xcessory then completed by Elsa Simard, F. Poncet
 		  and E. Taurel

 Original      :  July 1998


 $Revision: 1.1 $			$Author: jkrueger1 $
 $Date: 2003-04-25 12:54:10 $				$State: Exp $

****************************************************************************/
 
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

#include <X11/Xutil.h>
#include <X11/keysymdef.h>
 
#include <greta.h>

/* Some global variable defined in greta_cb.c or greta_sdd.c file */

extern int dev_dc_flag;
extern int device_to_update;
extern int server_to_update;
extern int respool_to_update;

extern dev_window *dev_tab[TAB_SIZE];
extern serv_window *serv_tab[TAB_SIZE];
extern res_window *gr_res_tab[TAB_SIZE];
extern dev_window_dc *dev_dc_tab[TAB_SIZE];

extern Widget widgtab[WidgetIDsNb];

/* Some local functions */

static long file_copy(char *,char *);
static long TestLine(char *,char *,int);
static long res_line(char *,ana_input *,char **);
static void check_if_sec(char *,long *);

/* Some variables global to this file */

static char entered_passwd[101];
static char *pa;


void DevManageSecRes( Widget w, XtPointer client_data,long dc_flag,long *p_pass)
{
	char mess[300];
	char *win_buf,*win_devs;
	long sec,pass = True;
	unsigned int diff;
	char domain[20];
	char *tmp,*err_mess;
	long res,error;
	Widget w_curs;
 
 	int ind = (int)client_data;
		
/* Get text */

	switch (dc_flag)
	{
	case GR_DEV_DC :
		win_buf = (char *)XmTextGetString(dev_dc_tab[ind]->resotext);
		w_curs = dev_dc_tab[ind]->window;
		break;
		
	case GR_DEV :
		win_buf = (char *)XmTextGetString(dev_tab[ind]->resotext);
		w_curs = dev_tab[ind]->window;
		break;
		
	case GR_SERV :
		win_buf = (char *)XmTextGetString(serv_tab[ind]->resotext);
		win_devs = (char *)XmTextGetString(serv_tab[ind]->devstext);
		w_curs = serv_tab[ind]->window;
		break;
		
	case GR_RES :
		win_buf = (char *)XmTextGetString(gr_res_tab[ind]->resotext);
		w_curs = gr_res_tab[ind]->window;
		break;
		
	case GR_LOAD :
		win_buf = (char *)XmTextGetString(widgtab[FileLoadTextID]);
		w_curs = XtParent(widgtab[FileLoadTextID]);
		break;
	}
	
/* Change cursor */

	xsSetCursor(XtParent(w_curs), XC_watch);
	XFlush(XtDisplay(w));

/* Check if there is a security resource */

	check_if_sec(win_buf,&sec);
	
	if ((dc_flag == GR_SERV) && (sec == False))
		check_if_sec(win_devs,&sec);
	
/* If some security resources are defined, check if a password is used */

	if (sec == True)
	{
		res = db_secpass(&pa,&error);
		
		if (res == -1)
		{
			if (error == DbErr_NoPassword)
				pass = False;
			else
			{
				err_mess = dev_error_str(error);
				strcpy(mess,"Error during dbsecpass call\n");
				strcat(mess,err_mess);
				OpenMsgBox(widgtab[ErrorBoxID],mess);
				free(err_mess);
				xsUnsetCursor(XtParent(w_curs));
    				XFlush(XtDisplay(w));
				return;
			}
		}

/* If a password is used, ask it to the user */
		
		if (pass == True)
		{
			*p_pass = True;		
			XtManageChild(XtParent(widgtab[PasswdTextID]));
		}
		else
		{
			*p_pass = False;
		}
	}
	else
	{
		*p_pass = False;
	}

	switch (dc_flag)
	{
	case GR_DEV_DC :
	case GR_DEV :
	case GR_RES :
	case GR_LOAD :
		XtFree(win_buf);
		break;
		
	case GR_SERV :
		XtFree(win_buf);
		XtFree(win_devs);
		break;
	}	
	return;
}


void DevUpdateCb( Widget w, XtPointer client_data,XtPointer call_data,long dc_flag)
{
 	char panic_file[100];
	char mess[300];
	long ind_ch,nb_chain_dev,nb_dev,nb_dev_del;
	char **dev_list;
	char **tmp_ptr;
	long i,res,error;
	long nb_res,err_line,dev_err;
	char **dev_def;
	char **res_def;
	db_error db_err;
	long left,right;
	char *win_buf,*fname;
	Time time;
	char *tmp,*err_mess;
	db_delupd_error err_stu;
	Widget w_curs;
		
    	int ind=(int)client_data;
   	XmAnyCallbackStruct *acs=(XmAnyCallbackStruct*)call_data;
	 	 
/* Get text */

	if (dc_flag == GR_DEV_DC)
	{
		win_buf = (char *)XmTextGetString(dev_dc_tab[ind]->resotext);
		w_curs = dev_dc_tab[ind]->window;
	}
	else
	{
		win_buf = (char *)XmTextGetString(dev_tab[ind]->resotext);
		w_curs = dev_tab[ind]->window;
	}
			 							
/* Analyse resource buffer */

	err_line = 0;	
	res = db_analyze_data(Db_Buffer,win_buf,&nb_dev,&dev_def,&nb_res,
			      &res_def,&err_line,&error);	

/* Display error message if the analysis fails */

	if (res == -1)
	{
		if (err_line == 0)
		{		
			sprintf(mess,"Error at line %d\n",err_line);
		
/* Call _xc.s utility LineNbToLRPos */
	
			LineNbToLRPos(win_buf,&err_line, &left, &right);
		
/* Call highlighting function */
	
			time = acs->event->xbutton.time;
			if (dc_flag == GR_DEV_DC)
				XmTextSetSelection(dev_dc_tab[ind]->resotext,(XmTextPosition)left, (XmTextPosition)right,time);
			else
				XmTextSetSelection(dev_tab[ind]->resotext,(XmTextPosition)left, (XmTextPosition)right,time);
		}
		else
		{
			err_mess = dev_error_str(error);
			strcpy(mess,err_mess);
			free(err_mess);
		}	
		XtFree(win_buf);
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
				
/* Count how many devices are involved in this update */

	if (dc_flag == GR_DEV_DC)
		tmp_ptr = dev_dc_tab[ind]->chaine_dev_arr;
	else
		tmp_ptr = dev_tab[ind]->chaine_dev_arr;	
       	ind_ch = 0;
	nb_chain_dev = 0;
       	while ((ind_ch < CH_TAB_SIZE))
       	{
          	if (tmp_ptr[ind_ch] != NULL)
             		nb_chain_dev++;
             	ind_ch++;
       	}

	nb_dev_del = nb_chain_dev + 1;
	
/* Allocate memory for the device list */

	if ((dev_list = (char **)calloc(nb_dev_del,sizeof(char *))) == NULL)
	{
		XtFree(win_buf);
		OpenMsgBox(widgtab[ErrorBoxID],"Can't allocate memory. No update has been performed");
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
	
/* Build the device list */

	if (dc_flag == GR_DEV_DC)
		dev_list[0] = dev_dc_tab[ind]->devname;
	else 
		dev_list[0] = dev_tab[ind]->devname;
       	ind_ch = 0;
	i = 1;
       	while ((ind_ch < CH_TAB_SIZE))
       	{
          	if (tmp_ptr[ind_ch] != NULL)
		{
             		dev_list[i] = tmp_ptr[ind_ch];
			i++;
		}
             	ind_ch++;
       	}	

#ifndef NDBM_USED
	res = db_delete_update(nb_dev_del,dev_list,DB_DEV_LIST,
			       nb_dev,dev_def,
			       nb_res,res_def,&err_stu);
			       
/* Free memory allocated for the update */

	FREE_STR_ARRAY(res_def,nb_res);
	FREE_STR_ARRAY(dev_def,nb_dev);
	free(dev_list);
	XtFree(win_buf);

/* Display message to the user if the call failed */

	if (res != 0)
	{
		strcpy(mess,"Can't update database\n");
		strcat(mess,"The database has not been modified at all\n");
		switch (err_stu.type)
		{
		case DB_DELETE_ERROR :
			strcat(mess,"Can't delete old resources");
			break;
			
		case DB_UPD_DEV_ERROR :
			strcat(mess,"Can't update device list");
			break;
			
		case DB_UPD_RES_ERROR :
			strcat(mess,"Can't update resource list");
			break;
		}
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
				
#else /* NDBM_USED */
	
/* Remove from database all resources belonging to involved devices */

	res = db_devicedeleteres(nb_dev_del,dev_list,&db_err);
	
/* If this call failed, try to retrieve database anterior state */

	if (res != 0)
	{
		if (dc_flag == GR_DEV_DC)
			fname = dev_dc_tab[ind]->file;
		else
			fname = dev_tab[ind]->file;
				
/* Analyse resource file */
	
		res = db_analyze_data(Db_File,fname,&nb_dev,&dev_def,
			      	      &nb_res,&res_def,&err_line,&error);	

/* Display error message if the analysis fails */

		if (res == -1)
		{
			free(dev_list);
			XtFree(win_buf);
			strcpy(panic_file,"/tmp/greta_panic");
			if (file_copy(fname,panic_file))
			{
				strcpy(panic_file,fname);
			}
			strcpy(mess,"Panic message : Can't recover database anterior state\n");
			sprintf(&(mess[strlen(mess)]),"A copy of your resources (before changes) have been stored into file %s\n",panic_file);
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}

/* Update resources in db */
	
		res = db_updres(nb_res,res_def,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			free(dev_list);
			XtFree(win_buf);
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			strcpy(panic_file,"/tmp/greta_panic");
			if (file_copy(fname,panic_file))
			{
				strcpy(panic_file,fname);
			}
			strcpy(mess,"Panic message : Can't recover database anterior state\n");
			sprintf(&(mess[strlen(mess)]),"A copy of your resources (before changes) have been stored into file %s\n",panic_file);
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}	

/* Free memory allocated for the update */

		FREE_STR_ARRAY(res_def,nb_res);
		FREE_STR_ARRAY(dev_def,nb_dev);
		free(dev_list);
		XtFree(win_buf);
		
/* Display message to the user */
			
		strcpy(mess,"Can't update database\n");
		strcat(mess,"Not able to remove old resources\n");
		strcat(mess,"The database has not been modified at all\n");
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}

/* Insert new resources from the user window */

	else
	{
	
		free(dev_list);
		XtFree(win_buf);
	
/* Update device list in db */

		if (nb_dev != 0)
		{	
			res = db_upddev(nb_dev,dev_def,&dev_err,&error);
	
/* Display message if this call fails */

			if (res == -1)
			{
				FREE_STR_ARRAY(res_def,nb_res);
				FREE_STR_ARRAY(dev_def,nb_dev);
				strcpy(mess,"Can't update database\n");
				strcat(mess,"Not able to update device list\n");
				sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
				strcat(mess,"Hint : save you resource(s) in a file (File->save button)");

				OpenMsgBox(widgtab[ErrorBoxID],mess);
				xsUnsetCursor(XtParent(w_curs));
    				XFlush(XtDisplay(w));
				return;
			}
		}

/* Update resources in db */

		if (nb_res != 0)
		{	
			res = db_updres(nb_res,res_def,&dev_err,&error);
	
/* Display message if this call fails */

			if (res == -1)
			{
				FREE_STR_ARRAY(res_def,nb_res);
				FREE_STR_ARRAY(dev_def,nb_dev);
				strcpy(mess,"Can't update database\n");
				strcat(mess,"Not able to update resource list\n");
				sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
				strcat(mess,"Hint : save you resource(s) in a file (File->save button)");
				
				OpenMsgBox(widgtab[ErrorBoxID],mess);
				xsUnsetCursor(XtParent(w_curs));
    				XFlush(XtDisplay(w));
				return;
			}
		}	

/* Free memory allocated for the update */

		FREE_STR_ARRAY(res_def,nb_res);
		FREE_STR_ARRAY(dev_def,nb_dev);
	}
#endif /* NDBM_USED */
	
/* Change cursor */

	xsUnsetCursor(XtParent(w_curs));
    	XFlush(XtDisplay(w));
	
}



void ServUpdateCb( Widget w, XtPointer client_data,XtPointer call_data)
{
 	char panic_file[100];
	char mess[300];
	long nb_dev;
	char **dev_list;
	char **tmp_ptr;
	long i,j,res,error;
	long nb_res,err_line,dev_err;
	long nb_dev1,nb_res1;
	long nb_devf,nb_resf;
	char **dev_def,**dev_def1,**dev_deff;
	char **res_def,**res_def1,**res_deff;
	db_error db_err;
	long left,right;
	char *win_buf,*devs_buf,*fname;
	Time time;
	char *tmp,*temp,*err_mess;
	Widget w_curs;
	unsigned int diff;
	char dev[DEV_NAME_LENGTH];
	long dev_nb_list,max_dev;
	char **sum_dev_def,**sum_res_def;
	long sum_dev,sum_res;
	db_delupd_error err_stu;
		
    	int ind=(int)client_data;
   	XmAnyCallbackStruct *acs=(XmAnyCallbackStruct*)call_data;

	w_curs = serv_tab[ind]->window;
		 	 
/* Get text */

	win_buf = (char *)XmTextGetString(serv_tab[ind]->resotext);
	devs_buf = (char *)XmTextGetString(serv_tab[ind]->devstext);
			 							
/* Analyse resource buffer */

	err_line = 0;	
	res = db_analyze_data(Db_Buffer,win_buf,&nb_dev,&dev_def,&nb_res,
			      &res_def,&err_line,&error);	

/* Display error message if the analysis fails */

	if (res == -1)
	{
		if (err_line != 0)
		{		
			sprintf(mess,"Error at line %d\n",err_line);
		
/* Call _xc.s utility LineNbToLRPos */
	
			LineNbToLRPos(win_buf,&err_line, &left, &right);
		
/* Call highlighting function */
	
			time = acs->event->xbutton.time;
			XmTextSetSelection(serv_tab[ind]->resotext,(XmTextPosition)left, (XmTextPosition)right,time);
		}
		else
		{
			err_mess = dev_error_str(error);
			strcpy(mess,err_mess);
			free(err_mess);
		}		
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		XtFree(win_buf);
		XtFree(devs_buf);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}

/* Analyse device buffer */

	err_line = 0;	
	res = db_analyze_data(Db_Buffer,devs_buf,&nb_dev1,&dev_def1,&nb_res1,
			      &res_def1,&err_line,&error);	

/* Display error message if the analysis fails */

	if (res == -1)
	{
		if (err_line == 0)
		{		
			sprintf(mess,"Error at line %d\n",err_line);
		
/* Call _xc.s utility LineNbToLRPos */
	
			LineNbToLRPos(devs_buf,&err_line, &left, &right);
		
/* Call highlighting function */
	
			time = acs->event->xbutton.time;
			XmTextSetSelection(serv_tab[ind]->devstext,(XmTextPosition)left, (XmTextPosition)right,time);
		}
		else
		{
			err_mess = dev_error_str(error);
			strcpy(mess,err_mess);
			free(err_mess);
		}	
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		FREE_STR_ARRAY(res_def,nb_res);
		FREE_STR_ARRAY(dev_def,nb_dev);
		XtFree(win_buf);
		XtFree(devs_buf);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
					
/* Analyse the temporary file used for this update */

	fname = serv_tab[ind]->file;	
	res = db_analyze_data(Db_File,fname,&nb_devf,&dev_deff,&nb_resf,
			      &res_deff,&err_line,&error);	

/* Display error message if the file analysis fails */

	if (res == -1)
	{
		FREE_STR_ARRAY(res_def,nb_res);
		FREE_STR_ARRAY(dev_def,nb_dev);
		FREE_STR_ARRAY(res_def1,nb_res1);
		FREE_STR_ARRAY(dev_def1,nb_dev1);
		XtFree(win_buf);
		XtFree(devs_buf);		
		sprintf(mess,"Error at line %d in temparary file %s\n",err_line,fname);
		strcat(mess,"Hint : Close the window and reopen one with for the same server");
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}

/* Allocate memory for the device list */

	if ((dev_list = (char **)calloc(DEV_BLOCK,sizeof(char *))) == NULL)
	{
		FREE_STR_ARRAY(res_def,nb_res);
		FREE_STR_ARRAY(dev_def,nb_dev);
		FREE_STR_ARRAY(res_def1,nb_res1);
		FREE_STR_ARRAY(dev_def1,nb_dev1);
		XtFree(win_buf);
		XtFree(devs_buf);
		OpenMsgBox(widgtab[ErrorBoxID],"Can't allocate memory. No update has been performed");
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
	dev_nb_list = 0;
	max_dev = DEV_BLOCK;
	
/* Extract device name */

	for (i = 0;i < nb_resf;i++)
	{
		temp = strchr(res_deff[i],'/');
		temp++;
		tmp = strchr(temp,'/');
		tmp++;
		temp = strchr(tmp,'/');
		diff = (unsigned int)(temp - res_deff[i]);
		strncpy(dev,res_deff[i],diff);
		dev[diff] = 0;

/* Is it a new device ?*/

		for (j = 0;j < dev_nb_list;j++)
		{
			if (strcmp(dev_list[j],dev) == 0)
				break;
		}

/* If it is a new device, copy it in the device list */
			
		if (j == dev_nb_list)
		{
			if (dev_nb_list == max_dev)
			{
				if ((dev_list = (char **)realloc(dev_list,sizeof(char *) * (max_dev + DEV_BLOCK))) == NULL)
				{
					FREE_STR_ARRAY(res_def,nb_res);
					FREE_STR_ARRAY(dev_def,nb_dev);
					FREE_STR_ARRAY(res_def1,nb_res1);
					FREE_STR_ARRAY(dev_def1,nb_dev1);
					XtFree(win_buf);
					XtFree(devs_buf);		
					strcat(mess,"Can't allocate memory. No update is performed\n");
					OpenMsgBox(widgtab[ErrorBoxID],mess);
					xsUnsetCursor(XtParent(w_curs));
    					XFlush(XtDisplay(w));
					return;
				}
				max_dev = max_dev + DEV_BLOCK;
			}
			if ((dev_list[dev_nb_list] = (char *)malloc(DEV_NAME_LENGTH + 1)) == NULL)
			{
				FREE_STR_ARRAY(res_def,nb_res);
				FREE_STR_ARRAY(dev_def,nb_dev);
				FREE_STR_ARRAY(res_def1,nb_res1);
				FREE_STR_ARRAY(dev_def1,nb_dev1);
				XtFree(win_buf);
				XtFree(devs_buf);		
				strcat(mess,"Can't allocate memory\n");
				OpenMsgBox(widgtab[ErrorBoxID],mess);
				xsUnsetCursor(XtParent(w_curs));
    				XFlush(XtDisplay(w));
				return;
			}
			strcpy(dev_list[dev_nb_list],dev);
			dev_nb_list++;
		}
	}
	
#ifndef NDBM_USED

/* Merge devices definition from device and resource window in one array */

	if (nb_dev == 0)
	{
		if (nb_dev1 == 0)
		{
			sum_dev = 0;
			sum_dev_def = NULL;
		}
		else
		{
			sum_dev = nb_dev1;
			sum_dev_def = dev_def1;
		}
	}
	else
	{
		if (nb_dev1 == 0)
		{
			sum_dev = nb_dev;
			sum_dev_def = dev_def;
		}
		else
		{
			sum_dev = nb_dev + nb_dev1;
			sum_dev_def = dev_def;
			
			dev_def = realloc(dev_def,(sum_dev * sizeof(char *)));
			for (i = 0;i < nb_dev1;i++)
				dev_def[i + nb_dev] = dev_def1[i];
		}
	}

	
/* Merge resource definition from device and resource window in one array */

	if (nb_res == 0)
	{
		if (nb_res1 == 0)
		{
			sum_res = 0;
			sum_res_def = NULL;
		}
		else
		{
			sum_res = nb_res1;
			sum_res_def = res_def1;
		}
	}
	else
	{
		if (nb_res1 == 0)
		{
			sum_res = nb_res;
			sum_res_def = res_def;
		}
		else
		{
			sum_res = nb_res + nb_res1;
			sum_res_def = res_def;
			
			res_def = realloc(res_def,(sum_res * sizeof(char *)));
			for (i = 0;i < nb_res1;i++)
				res_def[i + nb_res] = res_def1[i];
		}
	}
	

/* Update Database */

	res = db_delete_update(dev_nb_list,dev_list,DB_DEV_LIST,
			       sum_dev,sum_dev_def,
			       sum_res,sum_res_def,&err_stu);
			       
/* Free memory allocated for the update */

	FREE_STR_ARRAY(dev_list,dev_nb_list);
	FREE_STR_ARRAY(res_def1,nb_res1);
	FREE_STR_ARRAY(dev_def1,nb_dev1);
	FREE_STR_ARRAY(res_def,nb_res);
	FREE_STR_ARRAY(dev_def,nb_dev);
	XtFree(win_buf);
	XtFree(devs_buf);

/* Display message to the user if the call failed */

	if (res != 0)
	{
		strcpy(mess,"Can't update database\n");
		strcat(mess,"The database has not been modified at all\n");
		switch (err_stu.type)
		{
		case DB_DELETE_ERROR :
			strcat(mess,"Can't delete old resources");
			break;
			
		case DB_UPD_DEV_ERROR :
			strcat(mess,"Can't update device list");
			break;
			
		case DB_UPD_RES_ERROR :
			strcat(mess,"Can't update resource list");
			break;
		}
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
				
#else /* NDBM_USED */
	
/* Remove from database all resources belonging to devices in the list */

	if (dev_nb_list != 0)
	{
		res = db_devicedeleteres(dev_nb_list,dev_list,&db_err);
	
/* If this call failed, try to retrieve database anterior state */

		if (res != 0)
		{
			if (nb_resf != 0)
			{
		
/* Try to reinsert already deleted resources */
	
				res = db_updres(nb_resf,res_deff,&dev_err,&error);
	
/* Display message if this call fails */

				if (res == -1)
				{
					FREE_STR_ARRAY(dev_list,dev_nb_list);
					FREE_STR_ARRAY(res_def,nb_res);
					FREE_STR_ARRAY(dev_def,nb_dev);
					FREE_STR_ARRAY(res_def1,nb_res1);
					FREE_STR_ARRAY(dev_def1,nb_dev1);
					XtFree(win_buf);
					XtFree(devs_buf);
					strcpy(panic_file,"/tmp/greta_panic");
					if (file_copy(fname,panic_file))
					{
						strcpy(panic_file,fname);
					}
					strcpy(mess,"Panic message : Can't recover database anterior state\n");
					sprintf(&(mess[strlen(mess)]),"A copy of your resources (before changes) have been stored into file %s\n",panic_file);
					OpenMsgBox(widgtab[ErrorBoxID],mess);
					xsUnsetCursor(XtParent(w_curs));
    					XFlush(XtDisplay(w));
					return;
				}
			}	

/* Free memory allocated for the update */

			FREE_STR_ARRAY(dev_list,dev_nb_list);
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			FREE_STR_ARRAY(res_def1,nb_res1);
			FREE_STR_ARRAY(dev_def1,nb_dev1);
			XtFree(win_buf);
			XtFree(devs_buf);
		
/* Display message to the user */
			
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to remove old resources\n");
			strcat(mess,"The database has not been modified at all\n");
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}

/* Insert new resources from the user window */
	
	FREE_STR_ARRAY(dev_list,dev_nb_list);
	FREE_STR_ARRAY(res_deff,nb_resf);
	FREE_STR_ARRAY(dev_deff,nb_devf);
	
/* Update device list in db (from the resource window) */

	if (nb_dev1 != 0)
	{	
		res = db_upddev(nb_dev1,dev_def1,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			FREE_STR_ARRAY(res_def1,nb_res1);
			FREE_STR_ARRAY(dev_def1,nb_dev1);
			XtFree(win_buf);
			XtFree(devs_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update device list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");

			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}

/* Update device list in db (from the device window) */
		
	if (nb_dev != 0)
	{	
		res = db_upddev(nb_dev,dev_def,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			FREE_STR_ARRAY(res_def1,nb_res1);
			FREE_STR_ARRAY(dev_def1,nb_dev1);
			XtFree(win_buf);
			XtFree(devs_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update device list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");

			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}

/* Update resources in db (from resource window) */

	if (nb_res != 0)
	{	
		res = db_updres(nb_res,res_def,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			FREE_STR_ARRAY(res_def1,nb_res1);
			FREE_STR_ARRAY(dev_def1,nb_dev1);
			XtFree(win_buf);
			XtFree(devs_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update resource list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");
				
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}
		
/* Update resources in db (from resource window) */

	if (nb_res1 != 0)
	{	
		res = db_updres(nb_res1,res_def1,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			FREE_STR_ARRAY(res_def1,nb_res1);
			FREE_STR_ARRAY(dev_def1,nb_dev1);
			XtFree(win_buf);
			XtFree(devs_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update resource list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");
				
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}	

/* Free memory allocated for the update */

	FREE_STR_ARRAY(res_def,nb_res);
	FREE_STR_ARRAY(dev_def,nb_dev);
	FREE_STR_ARRAY(res_def1,nb_res1);
	FREE_STR_ARRAY(dev_def1,nb_dev1);
	XtFree(win_buf);
	XtFree(devs_buf);
#endif /* NDBM_USED */
					
/* Change cursor */

	xsUnsetCursor(XtParent(w_curs));
    	XFlush(XtDisplay(w));
	
}




void ResUpdateCb(Widget w,XtPointer client_data,XtPointer call_data)
{
 	char panic_file[100];
	char mess[300];
	long nb_dev;
	char **dev_list;
	char **tmp_ptr;
	long i,j,res,error,l;
	long nb_res,err_line,dev_err;
	long nb_devf,nb_resf;
	char **dev_def,**dev_deff;
	char **res_def,**res_deff;
	db_error db_err;
	long left,right;
	char *win_buf,*fname;
	Time time;
	char *tmp,*temp,*err_mess;
	db_delupd_error err_stu;
	Widget w_curs;
	unsigned int diff;
	char dev[DEV_NAME_LENGTH];
		
    	int ind=(int)client_data;
   	XmAnyCallbackStruct *acs=(XmAnyCallbackStruct*)call_data;

	w_curs = gr_res_tab[ind]->window;
		 	 
/* Get text */

	win_buf = (char *)XmTextGetString(gr_res_tab[ind]->resotext);
			 							
/* Analyse resource buffer */

	err_line = 0;	
	res = db_analyze_data(Db_Buffer,win_buf,&nb_dev,&dev_def,&nb_res,
			      &res_def,&err_line,&error);	

/* Display error message if the analysis fails */

	if (res == -1)
	{
		if (err_line != 0)
		{		
			sprintf(mess,"Error at line %d\n",err_line);
		
/* Call _xc.s utility LineNbToLRPos */
	
			LineNbToLRPos(win_buf,&err_line, &left, &right);
		
/* Call highlighting function */
	
			time = acs->event->xbutton.time;
			XmTextSetSelection(gr_res_tab[ind]->resotext,(XmTextPosition)left, (XmTextPosition)right,time);
		}
		else
		{
			err_mess = dev_error_str(error);
			strcpy(mess,err_mess);
			free(err_mess);
		}	
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		XtFree(win_buf);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
					
/* Analyse the temporary file used for this update */

	fname = gr_res_tab[ind]->file;	
	res = db_analyze_data(Db_File,fname,&nb_devf,&dev_deff,&nb_resf,
			      &res_deff,&err_line,&error);	

/* Display error message if the file analysis fails */

	if (res == -1)
	{
		FREE_STR_ARRAY(res_def,nb_res);
		FREE_STR_ARRAY(dev_def,nb_dev);
		XtFree(win_buf);
		sprintf(mess,"Error at line %d in temparary file %s\n",err_line,fname);
		strcat(mess,"Hint : Close the window and reopen one with for the same server");
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}

#ifndef NDBM_USED

/* Update database */

	res = db_delete_update(nb_resf,res_deff,DB_RES_LIST,
			       nb_dev,dev_def,
			       nb_res,res_def,&err_stu);
			       
/* Free memory allocated for the update */

	FREE_STR_ARRAY(res_def,nb_res);
	FREE_STR_ARRAY(dev_def,nb_dev);
	FREE_STR_ARRAY(res_deff,nb_resf);
	FREE_STR_ARRAY(dev_deff,nb_devf);
	XtFree(win_buf);
			
/* Display message to the user if the call failed */

	if (res != 0)
	{
		strcpy(mess,"Can't update database\n");
		strcat(mess,"The database has not been modified at all\n");
		switch (err_stu.type)
		{
		case DB_DELETE_ERROR :
			strcat(mess,"Can't delete old resources");
			break;
			
		case DB_UPD_DEV_ERROR :
			strcat(mess,"Can't update device list");
			break;
			
		case DB_UPD_RES_ERROR :
			strcat(mess,"Can't update resource list");
			break;
		}
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
				
#else /* NDBM_USED */

/* Remove resource value at the end of resource definition */

	for (i = 0;i < nb_resf;i++)
	{
		if ((tmp = strchr(res_deff[i],':')) == NULL)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			FREE_STR_ARRAY(res_deff,nb_resf);
			FREE_STR_ARRAY(dev_deff,nb_devf);
			XtFree(win_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"The database has not been modified at all\n");
			strcat(mess,"Wrong syntax in GRETA temporary file\n");
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
		diff = (unsigned int)(tmp - res_deff[i]);
		res_deff[i][diff] = '\0';		
	}
	
/* Remove from database all resources displayed when the window poped up */

	res = db_delreslist(res_deff,nb_resf,&error);	

/* Display message if this call fails */

	if (res == -1)
	{
		FREE_STR_ARRAY(res_def,nb_res);
		FREE_STR_ARRAY(dev_def,nb_dev);
		FREE_STR_ARRAY(res_deff,nb_resf);
		FREE_STR_ARRAY(dev_deff,nb_devf);
		XtFree(win_buf);
		strcpy(mess,"Can't update database\n");
		strcat(mess,"The database has not been modified at all\n");
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}	

/* Insert new resources from the user window */
	
	FREE_STR_ARRAY(res_deff,nb_resf);
	FREE_STR_ARRAY(dev_deff,nb_devf);

/* Update device list in db (from the device window) */
		
	if (nb_dev != 0)
	{	
		res = db_upddev(nb_dev,dev_def,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			XtFree(win_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update device list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");

			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}

/* Update resources in db (from resource window) */

	if (nb_res != 0)
	{	
		res = db_updres(nb_res,res_def,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			XtFree(win_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update resource list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");
				
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}

/* Free memory allocated for the update */

	FREE_STR_ARRAY(res_def,nb_res);
	FREE_STR_ARRAY(dev_def,nb_dev);
	XtFree(win_buf);
#endif /* NDBM_USED */
					
/* Change cursor */

	xsUnsetCursor(XtParent(w_curs));
    	XFlush(XtDisplay(w));
	
}





/****************************************************************************
                                                                           
 Function     : void LoadUpdate()

 Description  : callback attached to "Update Data Base" pushbutton in the
                loaded resource file window.
                This function tries to update the static data base with the
                content of the window as if it was a "correct" resource file.
                In case of error an error message is popped up.

 Arg(s) In    : Widget      w           : unused.
                XtPointer   client_data : unused.
                XtPointer   call_data   : unused.

 Arg(s) Out   : none

 Return(s)    : none
                                                                           
***************************************************************************/

void LoadUpdate( Widget w, XtPointer client_data, XtPointer call_data)
{
 	char panic_file[100];
	char mess[300];
	long nb_dev,nb_res;
	long i,j,res,error;
	long err_line,dev_err;
	char **dev_def;
	char **res_def;
	db_error db_err;
	long left,right;
	char *win_buf,*fname;
	Time time;
	char *err_mess;
	Widget w_curs;
		
   	XmAnyCallbackStruct *acs=(XmAnyCallbackStruct*)call_data;

	w_curs = XtParent(widgtab[FileLoadTextID]);
		 	 
/* Get text */

	win_buf = (char *)XmTextGetString(widgtab[FileLoadTextID]);
			 							
/* Analyse resource buffer */
	
	res = db_analyze_data(Db_Buffer,win_buf,&nb_dev,&dev_def,&nb_res,
			      &res_def,&err_line,&error);	

/* Display error message if the analysis fails */

	if (res == -1)
	{
		
		sprintf(mess,"Error at line %d\n",err_line);
		
/* Call _xc.s utility LineNbToLRPos */
	
		LineNbToLRPos(win_buf,&err_line, &left, &right);
		
/* Call highlighting function */
	
		time = acs->event->xbutton.time;
		XmTextSetSelection(widgtab[FileLoadTextID],(XmTextPosition)left, (XmTextPosition)right,time);
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		XtFree(win_buf);
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
					
/* Insert new resources from the user window */
/* Update device list in db (from the device window) */
		
	if (nb_dev != 0)
	{	
		res = db_upddev(nb_dev,dev_def,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			XtFree(win_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update device list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");

			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}

/* Update resources in db (from resource window) */

	if (nb_res != 0)
	{	
		res = db_updres(nb_res,res_def,&dev_err,&error);
	
/* Display message if this call fails */

		if (res == -1)
		{
			FREE_STR_ARRAY(res_def,nb_res);
			FREE_STR_ARRAY(dev_def,nb_dev);
			XtFree(win_buf);
			strcpy(mess,"Can't update database\n");
			strcat(mess,"Not able to update resource list\n");
			sprintf(&(mess[strlen(mess)]),"%s\n",dev_error_str(error));
			strcat(mess,"Hint : save you resource(s) in a file (File->save button)");
				
			OpenMsgBox(widgtab[ErrorBoxID],mess);
			xsUnsetCursor(XtParent(w_curs));
    			XFlush(XtDisplay(w));
			return;
		}
	}	

/* Free memory allocated for the update */

	FREE_STR_ARRAY(res_def,nb_res);
	FREE_STR_ARRAY(dev_def,nb_dev);
	XtFree(win_buf);
					
/* Change cursor */

	xsUnsetCursor(XtParent(w_curs));
    	XFlush(XtDisplay(w));
	
}

static long file_copy(char *source_file_name,char *dest_file_name)
{
	FILE *source;
	FILE *dest;
	char buffer[512];
	
	if ((source = fopen(source_file_name,"r")) == NULL)
		return(-1);
	if ((dest = fopen(dest_file_name,"w")) == NULL)
		return(-1);

	while (fgets(buffer,512,source) != NULL)
	{
		fputs(buffer,dest);
	}
	
	if (feof(source) == 0)
	{
		fclose(source);
		fclose(dest);
		return(-1);
	}
			
	fclose(source);
	fclose(dest);
	
	return(0);
}
	
	

/****************************************************************************
                                                                           
 Function     : void passwd_entry_event_hand()

 Description  : Event handler attached to the text entry for the "security
                password". This event handler is called each time a keyboard
                key is pressed.
                This function will add the character typed into the password
                string and displays a "*" in the corresponding text widget.

 Arg(s) In    : Widget      w           : the text entry widget for password.
                XtPointer   client_data : unused.
                XEvent      *event      : X11 event.
                Boolean     *continue_to_dispatch : unused.

 Arg(s) Out   : none

 Return(s)    : none
                                                                           
***************************************************************************/

void passwd_entry_event_hand( Widget w, XtPointer client_data,
                              XEvent *event, Boolean *continue_to_dispatch)
{
    XKeyEvent       key_evt;
    char            returned_str[5];
    KeySym          returned_keysym;
    XComposeStatus  ret_stat_inout;
    int             str_long;
    int             long_passwd, long_strtw;
    char            *textw_str, disp_str[101];
    XmTextPosition   cur_posit;




    if (event->type != KeyPress)
       return;

    key_evt = event->xkey;


    str_long = XLookupString( &key_evt, returned_str, 1, &returned_keysym,
                                                        &ret_stat_inout);

    if ( returned_keysym == XK_Return )
       return;

    if ((returned_keysym == XK_BackSpace) || (returned_keysym == XK_Delete))
    {
       long_passwd = strlen(entered_passwd);
       if ( long_passwd > 0 )
       {
          textw_str = XmTextGetString(w);
          long_strtw = strlen(textw_str);
          strncpy(disp_str, textw_str, (long_strtw - 1));
          disp_str[long_strtw - 1] = '\0';
          XmTextSetString(w, disp_str);
          XtFree(textw_str);

          entered_passwd[long_passwd - 1] = '\0'; 
       }
    }
    else
    {
       if (str_long == 0)
          return;

       cur_posit = XmTextGetLastPosition (w);
       XmTextInsert(w, cur_posit, "*");

       returned_str[1] = '\0';
       strcat(entered_passwd, returned_str);
    }
}








/****************************************************************************
                                                                           
 Function     : void PasswdCancelCb()

 Description  : callback attached to "Cancel" pushbutton in the
                Security password window.
                This function resets the entered password and cancels the
                "update" operation??

 Arg(s) In    : Widget      w           : unused.
                XtPointer   client_data : unused.
                XtPointer   call_data   : unused.

 Arg(s) Out   : none

 Return(s)    : none
                                                                           
***************************************************************************/

void PasswdCancelCb( Widget w, XtPointer client_data, XtPointer call_data)
{
	Widget w_curs;
	
   	XmTextSetString(widgtab[PasswdTextID], "");
   	entered_passwd[0] = '\0';
	
	switch(dev_dc_flag)
	{
	case GR_DEV_DC :
		w_curs = dev_dc_tab[device_to_update]->window;
		break;
		
	case GR_DEV :
		w_curs = dev_tab[device_to_update]->window;
		break;
		
	case GR_SERV :
		w_curs = serv_tab[server_to_update]->window;
		break;
		
	case GR_RES :
		w_curs = gr_res_tab[respool_to_update]->window;
		break;
		
	case GR_LOAD :
		w_curs = XtParent(widgtab[FileLoadTextID]);
		break;
	}

	free(pa);		
	xsUnsetCursor(XtParent(w_curs));
    	XFlush(XtDisplay(w));		
}







/****************************************************************************
                                                                           
 Function     : void PasswdOkCb()

 Description  : callback attached to "OK" pushbutton in the
                Security password window.
                This function compares the entered password with the real
                password if the same the update is done if not it is cancelled.

                In all cases this function resets the entered password before
                returning.


 Arg(s) In    : Widget      w           : unused.
                XtPointer   client_data : unused.
                XtPointer   call_data   : unused.

 Arg(s) Out   : none

 Return(s)    : none
                                                                           
***************************************************************************/

void PasswdOkCb( Widget w, XtPointer client_data, XtPointer call_data)
{
	char mess[300];
	Widget w_curs;
	
	if (strcmp(entered_passwd, pa) != 0)
	{
		XmTextSetString(widgtab[PasswdTextID],"");
		entered_passwd[0] = '\0';
		
		strcpy(mess,"Can't update database\n");
		strcat(mess,"Wrong security password\n");
				
		OpenMsgBox(widgtab[ErrorBoxID],mess);
		switch(dev_dc_flag)
		{
		case GR_DEV_DC : 
			w_curs = dev_dc_tab[device_to_update]->window;
			break;
			
		case GR_DEV :
			w_curs = dev_tab[device_to_update]->window;
			break;
			
		case GR_SERV :
			w_curs = serv_tab[server_to_update]->window;
			break;
			
		case GR_RES :
			w_curs = gr_res_tab[respool_to_update]->window;
			break;
			
		case GR_LOAD :
			w_curs = XtParent(widgtab[FileLoadTextID]);
			break;
		}

		free(pa);		
		xsUnsetCursor(XtParent(w_curs));
    		XFlush(XtDisplay(w));
		return;
	}
	else
	{
		XmTextSetString(widgtab[PasswdTextID], "");
    		entered_passwd[0] = '\0';
		
		xsSetCursor(XtParent(w), XC_watch);
		XFlush(XtDisplay(w));

		switch (dev_dc_flag)
		{
		case GR_DEV_DC :
		case GR_DEV :				
			DevUpdateCb(w,(XtPointer)device_to_update,call_data,dev_dc_flag);
			break;
			
		case GR_SERV :
			ServUpdateCb(w,(XtPointer)server_to_update,call_data);
			break;
			
		case GR_RES :
			ResUpdateCb(w,(XtPointer)respool_to_update,call_data);
			break;
			
		case GR_LOAD :
			LoadUpdate(w,(XtPointer)0,call_data);
			break;
		}
		
		xsUnsetCursor(XtParent(w));
		XFlush(XtDisplay(w));
	}
	
	free(pa);
}



static void check_if_sec(char *buffer,long *p_sec_flag)
{
	char line[160];
	char line1[160];
	int i,j,k;
	char *ptr,*temp;
	unsigned int diff;
	char domain[40];
	long length_to_eol;
	char *tmp_resdef;
	ana_input in;

/* Init buffer pointer and analyze input structure */

	ptr = (char *)buffer;
	*p_sec_flag = False;

	in.in_type = Db_Buffer;	
	in.buf = &ptr;
	in.f = NULL;
			
/* Buffer exploration */

	while(ptr != NULL)
	{
		length_to_eol = strcspn(ptr,"\n");
		strncpy(line,ptr,length_to_eol);
		line[length_to_eol] = '\0';
		k = strlen(line);
		
/* Skip comment line */

		if (line[0] == '#')
		{
			ptr = strstr(ptr,"\n");
			if (ptr != NULL)
				ptr++;
			continue;
		}
		
/* Skip empty line */

		if (k == 0)
		{
			ptr = strstr(ptr,"\n");
			if (ptr != NULL)
				ptr++;
			continue;
		}
		if (line[k] == 0x0a)
			line[k] = '\0';

		for (i = 0;i < k;i++)
		{
			if (line[i] != ' ')
				break;
		}
		
/* Skip line with blank characters only */

		if (i == k)
		{
			ptr = strstr(ptr,"\n");
			if (ptr != NULL)
				ptr++;
			continue;
		}

/* Test resource file line and do something according to test result */

		switch(TestLine(line,line1,k))
		{
		
/* Simple resource definition */

		case 1 :
			temp = strchr(line1,':');
			if (temp == NULL)
				break;
			temp = strchr(line1,'/');
			if (temp == NULL)
				break;
			diff = (unsigned int)(temp - line1);
			strncpy(domain,line1,diff);
			domain[diff] = 0;
			
			if (strcmp(domain,"sec") == 0)
			{
				*p_sec_flag = True;
				return;
			}
			break;

/* Array resource definition */

		case 2 :

			if (res_line(line1,&in,&tmp_resdef) == -1)
				break;

			temp = strchr(tmp_resdef,'/');
			if (temp == NULL)
			{
				free(tmp_resdef);
				break;
			}
			diff = (unsigned int)(temp - tmp_resdef);
			strncpy(domain,line1,diff);
			domain[diff] = 0;
			free(tmp_resdef);
			
			if (strcmp(domain,"sec") == 0)
			{
				*p_sec_flag = True;
				return;
			}

			break;
		}
		
/* Go to next buffer or file line */

		ptr = strstr(ptr,"\n");
		if (ptr != NULL)
			ptr++;
	}

}



/****************************************************************************
*                                                                           *
*		Code for TestLine  function                                 *
*                        --------                                           *
*                                                                           *
*    Function rule : To change the line to lower case letters if it is      *
*                    necessary and to return a value which indicate which   *
*                    type of line it is (device definition, simple resource *
*                    definition or resources array definition)              *
*                                                                           *
*    Argin : - A pointer to a buffer where is stored a line of the resource *
*              file 						            *
*	     - A pointer to a buffer where the modified line will be store  *
*            - The length of the original line                              *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    Return value : -1 : Error  					    *
*                    0 : It is a device definition line                     *
*                    1 : It is a simple resource definition line            *
*                    2 : It is definition for an array of resources         *
*                                                                           *
****************************************************************************/

static long TestLine(char *line,char *line1,int k)
{
	char *tmp;
	unsigned int diff;
	int string = 0;
	int iret = 1;
	int i,j;
	int a = 0;

#ifdef DEBUG
printf("Start of TestLine \n");
#endif /* DEBUG */

/* Return error in this line is not a definition line */

	if ((tmp = strchr(line,':')) == NULL)
		return(-1);

/* Change all the letters before the : to lower case */

	diff = (unsigned int)(tmp - line) + 1;
	i = 0;
	for (j=0;j<diff;j++)
	{
		if (line[j] != ' ' && line[j] != 0x09)
			line1[i++] = tolower(line[j]);
	}
	line1[i] = 0;

/* Is it a device definition line ? In this case, all the line must be 
   translated to lower case letter */

	if (strstr(line1,"device:") != NULL)
	{
		for (j = diff;j < k;j++)
		{
			if (line[j] != ' ' && line[j] != 0x09)
			{
				line1[i++] = tolower(line[j]);
				if ((line[j] == ',') && (line[j + 1] == ','))
					j++;
			}
		}
		iret = 0;
	}

/* Now it is a resource definition line */

	else
	{

/* If the last character is \ , this is a resource array definition */

		if (line[k - 1] == '\\')
			iret = 2;

		for (j = diff;j < k;j++)
		{

/* If the " character is detected, set a flag. If the flag is already set,
   reset it */

			if (line[j] == '"')
			{
				if (string)
					string = 0;
				else
					string = 1;
				continue;
			}

/* When the string flag is set, copy character from the original line to the
   new one without any modifications */

			if (string)
			{
				line1[i++] = line[j];
				a = 1;
				continue;
			}

/* If the , character is detected not in a string definition, this is a
   resource array definition */

			if (line[j] == ',')
			{
				line1[i++] = SEP_ELT;
				if (line[j + 1] == ',')
					j++;
				iret = 2;
				continue;
			}

/* Remove space and tab */

			if (line[j] != ' ' && line[j] != 0x09)
			{
				line1[i++] = line[j];
				a = 1;
			}
		}
	}

/* If an odd number of " character has been detected, it is an error */

	if (string)
		iret = -1;

/* Leave function */

	line1[i] = 0;
	if ((strlen(line1) == diff) && (a == 0))
		iret = -1;
#ifdef DEBUG
printf("End of TestLine \n");
#endif /* DEBUG */
	return(iret);

}



/****************************************************************************
*                                                                           *
*		Code for res_line function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To extract from a resource file all the informations   *
*                    concerning a resource array			    *
*                                                                           *
*    Argin : - A pointer to a buffer where is stored a line of the resource *
*              file (The first line of the resource array definition)       *
*	     - The pointer to the FILE structure of the resource file       *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/


static long res_line(char *line1,ana_input *in,char **tmp_resdef)
{
	unsigned int diff;
	char *tmp;
	int i,j,k,l;
	int string = 0;
	register char *ptr,*ptr2;
	char base[80];
	char t_name[80];
	char tmp_line[160];
	long length_to_eol;

/* Make a copy of the resource array name */

	tmp = strchr(line1,':');
	if (tmp == NULL)
		return(-1);
	diff = (unsigned int)(tmp - line1) + 1;
	strncpy(base,line1,diff);
	base[diff] = 0;

/* Copy the first line in the resulting buffer */

	k = strlen(line1);
	ptr = (char *)malloc(k + 1);
	strcpy(ptr,line1);
	if (line1[k - 1] == '\\')
	{
		if (line1[k - 2] != SEP_ELT)
			ptr[k - 1] = SEP_ELT;
		else
			ptr[k - 1] = '\0';
	}

/* Following line examination */

	while (line1[k - 1] == '\\')
	{
		if (in->in_type == Db_File)
		{
			fgets(tmp_line,sizeof(tmp_line),in->f);
			tmp_line[strlen(tmp_line) - 1] = '\0';
		}
		else
		{
			*(in->buf) = strstr(*(in->buf),"\n");
			(*(in->buf))++;
			length_to_eol = strcspn(*(in->buf),"\n");
			strncpy(tmp_line,*(in->buf),length_to_eol);
			tmp_line[length_to_eol] = '\0';
		}

/* Verify the new line is not a simple resource definition */

		if ((ptr2 = strchr(tmp_line,':')) != NULL)
		{
			diff = (unsigned int)(ptr2 - tmp_line);
			strncpy(t_name,tmp_line,diff);
			t_name[diff] = '\0';
			k = 0;
			NB_CHAR(k,t_name,'/');
			if (k == 3)
			{
				free(ptr);
				return(-1);
			}
		}

		k = strlen(tmp_line);
		j = 0;

/* Remove space and tab characters except if they are between two ".
   Replace the , character by 0x02 except if they are between two ". */

		for (i = 0;i < k;i++)
		{
			if (tmp_line[i] == '"')
			{
				if (string)
					string = 0;
				else
					string = 1;
				continue;
			}

			if (string)
			{
				line1[j++] = tmp_line[i];
				continue;
			}

			if (tmp_line[i] == ',')
			{
				line1[j++] = SEP_ELT;
				if (tmp_line[i + 1] == ',')
					i++;
				continue;
			}

			if (tmp_line[i] != ' ' && tmp_line[i] != 0x09)
				line1[j++] = tmp_line[i];
		}

/* Error if odd number of " characters */

		if (string)
		{
			free(ptr);
			return(-1);
		}
		line1[j] = '\0';
		k = strlen(line1);

/* Test to verify that the array (in ascii characters) is not bigger than
   the allocated memory and realloc memory if needed. */
   
		ptr = (char *)realloc(ptr,strlen(ptr) + k + 1);
		
/* Add this new line to the result buffer */

		strcat(ptr,line1);
		l = strlen(ptr);
		if (line1[k - 1] == '\\')
		{
			if (ptr[l - 2] == SEP_ELT)
				ptr[l - 1] = '\0';
			else
				ptr[l - 1] = SEP_ELT;
		}
	}
	
/* Leave function */

	*tmp_resdef = ptr;
	return(0);

}

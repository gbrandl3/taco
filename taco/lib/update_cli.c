static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/update_cli.c,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";

/*+*******************************************************************

 File       :   update_cli.c

 Project    :   Static database

 Description:   Application Programmers Interface

            :   Interface to access static database for the database
	        update facility
		
 Author(s)  :   Emmanuel Taurel
                $Author: jkrueger1 $

 Original   :   May 1998

 Version:       $Revision: 1.1 $

 Date:          $Date: 2003-03-14 12:22:07 $

 Copyright (c) 1998 by European Synchrotron Radiation Facility,
                       Grenoble, France


 *-*******************************************************************/

#define PORTMAP

#include <macros.h>
#include <db_setup.h>
#include <db_xdr.h>

#if defined(_NT)
#include <API.h>
#include <ApiP.h>
#include <DevErrors.h>
#include <rpc.h>

#else
#include <API.h>
#include <ApiP.h>
#include <DevErrors.h>

#ifdef _OSK
#include <inet/socket.h>
#include <inet/netdb.h>
#include <strings.h>
#else
#include <string.h>
#include <sys/socket.h>
#ifndef vxworks
#include <netdb.h>
#else
#include <hostLib.h>
#include <taskLib.h>
#endif
#include <unistd.h>
#endif /* _OSK */
#endif	/* _NT */

#ifndef OSK
#include <stdlib.h>
#endif

#include <math.h>

#ifdef ALONE
extern CLIENT *cl;
extern int first;
#else
extern dbserver_info db_info;
#endif /* ALONE */


/* Some local functions */

static long TestLine(char *,char *,int);
static long name_line(char *,ana_input *,long *,char **,long *);
static long check_res(char *,long,char **,long *);
static long res_line(char *,ana_input *,long *,char **,long *);
static long check_dev(char *,long *,long *);
static void get_error_line(const char *,long ,long *);

/* Static and Global variables */

static struct timeval timeout_update={60,0};



/****************************************************************************
*                                                                           *
*		db_analyze_data function code                  	    	    *
*               ---------------                                        	    *
*                                                                           *
*    Function rule : To check a resource file and to build simple device and*
*		     resource definition from it. A simple device definition*
*		     is a string like :					    *
*			ds_name/pers_name/device:dev_a,dev_b,dev_c	    *
*		     A simple resource definition follows the syntax :	    *
*			dev_a/res_name:res_value			    *
*									    *
*    Argin : - in_type : Flag set to Db_File or Db_Buffer according to what *
*			 should be analysed ( a file or a graphical appli.  *
*			 buffer)					    *
*	     - buffer : The file name (if Db_File) or the buffer (Db_Buffer)*
*									    *
*    Argout : - nb_devdef : Number of device definition			    *
*	      - devdef : Device definition				    *
*	      - nb_resdef : Number of resource definition		    *
*	      - resdef : Resource definition				    *
*	      - line_err : File line where error occurs			    *
*             - p_error : Pointer for the error code in case of problems    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the variable       *
*    pointed to by "p_error". The function also initializes the line_err    *
*    argout with the file line where the error occurs. Otherwise, the 	    *
*    function returns 0             					    *
*                                                                           *
*****************************************************************************/


long db_analyze_data(long in_type,const char *buffer,long *nb_devdef,char ***devdef, \
		     long *nb_resdef,char ***resdef,long *error_line,long *p_error)
{
	FILE *file;
	long domain_nb;
	char **domain_list;
	long line_ptr,err_dev;
	char line[160];
	char line1[160];
	int i,j,k;
	char *tmp_devdef;
	char *tmp_resdef;
	long nb_dev = 0;
	char **tmp_dev = NULL;
	long nb_res = 0;
	char **tmp_res = NULL;
	char *ptr;
	long length_to_eol;
	ana_input in;

/* Try to verify function parameters */

	if ((buffer == NULL) || (nb_devdef == NULL) || (devdef == NULL) ||
	    (nb_resdef == NULL) || (error_line == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(-1);
	}
	if ((in_type != Db_File) && (in_type != Db_Buffer))
	{
		*p_error = DbErr_BadParameters;
		return(-1);
	}

/* Miscellaneous init. */

	*p_error = 0;
	*error_line = 0;
	*nb_resdef = *nb_devdef = 0;
	line_ptr = 0;

/* Ask database server for a list of all resources domain */

	if (db_getresdomainlist(&domain_nb,&domain_list,p_error) == -1)
	{
		return(-1);
	}
	
/* open resource file */

	if (in_type == Db_File)
	{
		if ((file = fopen(buffer,"r")) == NULL)
		{
			*p_error = DbErr_CantOpenResFile;
			return(-1);
		}
	}

/* Init buffer pointer and analyze input structure */

	if (in_type == Db_File)
	{
		ptr = fgets(line,sizeof(line),file);
		
		in.in_type = in_type;
		in.f = file;
		in.buf = NULL;
	}
	else
	{
		ptr = (char *)buffer;
		
		in.in_type = in_type;
		in.buf = &ptr;
		in.f = NULL;
	}
		
/* Resource file exploration */

	while(ptr != NULL)
	{
		if (in_type == Db_Buffer)
		{
			length_to_eol = strcspn(ptr,"\n");
			strncpy(line,ptr,length_to_eol);
			line[length_to_eol] = '\0';
			k = strlen(line);
		}
		else
			k = strlen(line) - 1;
		line_ptr++;
		
/* Skip comment line */

		if (line[0] == '#')
		{
			if (in_type == Db_Buffer)
			{
				ptr = strstr(ptr,"\n");
				if (ptr != NULL)
					ptr++;
			}
			else
				ptr = fgets(line,sizeof(line),file); 
			continue;
		}
		
/* Skip empty line */

		if (k == 0)
		{
			if (in_type == Db_Buffer)
			{
				ptr = strstr(ptr,"\n");
				if (ptr != NULL)
					ptr++;
			}
			else
				ptr = fgets(line,sizeof(line),file);
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
			if (in_type == Db_Buffer)
			{
				ptr = strstr(ptr,"\n");
				if (ptr != NULL)
					ptr++;
			}
			else
				ptr = fgets(line,sizeof(line),file);
			continue;
		}

/* Test resource file line and do something according to test result */

		switch(TestLine(line,line1,k))
		{
		case -1 :
			for (i = 0;i < domain_nb;i++)
				free(domain_list[i]);
			free(domain_list);
			if (nb_dev != 0)
			{
				for (j = 0;j < nb_dev;j++)
					free(tmp_dev[j]);
				free(tmp_dev);
			}
			if (nb_res != 0)
			{
				for (j = 0;j < nb_res;j++)
					free(tmp_res[j]);
				free(tmp_res);
			}
			if (in_type == Db_File)
				fclose(file); 
			*error_line = line_ptr;
			*p_error = DbErr_BadResSyntax;
			return(-1);

		case 0 : 
#ifdef DEBUG
			printf("Device definition (name_line) \n");
#endif /* DEBUG */
			if (name_line(line1,&in,&line_ptr,&tmp_devdef,p_error) == -1)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (in_type == Db_File)
					fclose(file);
				*error_line = line_ptr;
				return(-1);
			}
			if (check_dev(tmp_devdef,&err_dev,p_error) == -1)
			{
				if (in_type == Db_Buffer)
				{
					get_error_line(buffer,err_dev,&line_ptr);
				}
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (in_type == Db_File)
					fclose(file);
				*error_line = line_ptr;
				return(-1);
			}
			nb_dev++;
			if ((tmp_dev = (char **)realloc(tmp_dev,(sizeof(char *) * nb_dev))) == NULL)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (in_type == Db_File)
					fclose(file);
				*p_error = DbErr_ClientMemoryAllocation;
				return(-1);
			}
			tmp_dev[nb_dev - 1] = tmp_devdef;
			break;

		case 1 :
#ifdef DEBUG
			printf("Simple resource definition (check_res) \n");
#endif /* DEBUG */
		 	if (check_res(line1,domain_nb,domain_list,p_error) == -1)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (in_type == Db_File)
					fclose(file);
				*error_line = line_ptr;
				return(-1);
			}
			nb_res++;
			if ((tmp_res = (char **)realloc(tmp_res,(sizeof(char *) * nb_res))) == NULL)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (in_type == Db_File)
					fclose(file);
				*p_error = DbErr_ClientMemoryAllocation;
				return(-1);
			}
			if ((tmp_res[nb_res - 1] = (char *)malloc(strlen(line1) + 1)) == NULL)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (in_type == Db_File)
					fclose(file);
				*p_error = DbErr_ClientMemoryAllocation;
				return(-1);
			}
			strcpy(tmp_res[nb_res - 1],line1);
			break;

		case 2 :
#ifdef DEBUG
			printf("Array resource definition (res_line) \n");
#endif /* DEBUG */
			if (res_line(line1,&in,&line_ptr,&tmp_resdef,p_error) == -1)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (in_type == Db_File)
					fclose(file);
				*error_line = line_ptr;
				return(-1);
			}
		 	if (check_res(tmp_resdef,domain_nb,domain_list,p_error) == -1)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_res != 0)
				{
					for (j = 0;j < nb_res;j++)
						free(tmp_res[j]);
					free(tmp_res);
				}
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (in_type == Db_File)
					fclose(file);
				*error_line = line_ptr;
				return(-1);
			}
			nb_res++;
			if ((tmp_res = (char **)realloc(tmp_res,(sizeof(char *) * nb_res))) == NULL)
			{
				for (i = 0;i < domain_nb;i++)
					free(domain_list[i]);
				free(domain_list);
				if (nb_dev != 0)
				{
					for (j = 0;j < nb_dev;j++)
						free(tmp_dev[j]);
					free(tmp_dev);
				}
				if (in_type == Db_File)
					fclose(file);
				*p_error = DbErr_ClientMemoryAllocation;
				return(-1);
			}
			tmp_res[nb_res - 1] = tmp_resdef;
			break;
		}
		
/* Go to next buffer or file line */

		if (in_type == Db_Buffer)
		{
			ptr = strstr(ptr,"\n");
			if (ptr != NULL)
				ptr++;
		}
		else
			ptr = fgets(line,sizeof(line),file);
	}
		
/* Close resource file */

	if (in_type == Db_File)
		fclose(file);

/* Leave function */

	for (i = 0;i < domain_nb;i++)
		free(domain_list[i]);
	free(domain_list);
	
	*nb_devdef = nb_dev;
	*devdef = tmp_dev;
	*nb_resdef = nb_res;
	*resdef = tmp_res;
	return(0);

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
*		Code for name_line function                                 *
*                        ---------                                          *
*                                                                           *
*    Function rule : To extract from a resource file all the informations   *
*                    concerning the device name                             *
*                                                                           *
*    Argin : - A pointer to a buffer where is stored a line of the resource *
*              file (The first line with the "device" word in it )          *
*	     - The pointer to the FILE structure of the resource file       *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/


static long name_line(char *line1,ana_input *in,long *p_line_ptr,char **tmp_devdef,long *p_error)
{
	char base[80];
	unsigned int diff;
	char *tmp;
	int i,k,j;
	char *ptr;
	char tmp_line[160];
	long length_to_eol;

/* Make a copy of the device server network name */

	tmp = strchr(line1,':');
	diff = (unsigned int)(tmp - line1) + 1;
	strncpy(base,line1,diff);
	base[diff] = '\0';

/* Copy the first line in the result buffer */

	k = strlen(line1);
	if ((ptr = (char *)malloc(k + 1)) == NULL)
	{
		*p_error = DbErr_ClientMemoryAllocation;
		return(-1);
	}
	strcpy(ptr,line1);
	if (line1[k - 1] == '\\')
	{
		if (line1[k - 2] != ',')
			ptr[k - 1] = ',';
		else
			ptr[k - 1] = '\0';
	}

/* Following lines examination (discard space or tab at beginning of line) */

	while(line1[k - 1] == '\\')
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
		(*p_line_ptr)++;

		k = strlen(tmp_line);
		j = 0;
		for (i = 0;i < k;i++)
		{
			if (tmp_line[i] != ' ' && tmp_line[i] != 0x09)
			{
				line1[j++] = tolower(tmp_line[i]);
				if ((tmp_line[i] == ',') && (tmp_line[i + 1] == ','))
					i++;
			}
		}
		line1[j] = '\0';
		
		k = strlen(line1);
		if ((ptr = (char *)realloc(ptr,strlen(ptr) + k + 1)) == NULL)
		{
			*p_error = DbErr_ClientMemoryAllocation;
			return(-1);
		}
		strcat(ptr,line1);
		if (line1[k - 1] == '\\' )
		{
			if (line1[k - 2] != ',')
				ptr[strlen(ptr) - 1] = ',';
			else
				ptr[strlen(ptr) - 1] = '\0';
		}
	}

/* Leave function */

	*tmp_devdef = ptr;
	return(0);
	 
}




/****************************************************************************
*                                                                           *
*		Code for check_res function                                 *
*                        ---------                                          *
*                                                                           *
*    Function rule : To check that a simple resource definition line is     *
*		     correct						    *
*                                                                           *
*    Argin : - lin : A pointer to the modified resource definition (without *
*		     space and tab characters)                              *
*	     - d_num : Number of resource domain known by the db server     *
*	     - d_list : Name of resource domain known by the db server      *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or -1 if the resource      *
*    definition is not valid.						    *
*                                                                           *
****************************************************************************/

static long check_res(char *lin,long d_num,char **d_list,long *p_error)
{
	char t_name[80];
	unsigned int diff;
	register char *temp,*tmp;
	int i,l;

/* Verify that the resource definition is correct */

	temp = strchr(lin,':');
	if (temp == NULL)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}
	diff = (unsigned int)(temp - lin);
	strncpy(t_name,lin,diff);
	t_name[diff] = 0;
	l = 0;
	NB_CHAR(l,t_name,'/');
	if (l != 3)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}
	
/* Get domain name */

	tmp = strchr(lin,'/');
	diff = (unsigned int)(tmp++ - lin);
	strncpy(t_name,lin,diff);
	t_name[diff] = '\0';

/* Check domain name length */

	l = strlen(t_name);
	if (l > DOMAIN_NAME_LENGTH - 1)
	{
		*p_error = DbErr_DomainDefinition;
		return(-1);
	}
	
/* Select the right resource table in database */

	for (i = 0;i < d_num;i++)
	{
		if (strcmp(t_name,d_list[i]) == 0)
		{
			break;
		}
	} 

	if (i == d_num)
	{
		*p_error = DbErr_DomainDefinition;
		return(-1);
	}

/* Check that the family, member and resource name field are not blank */

	temp = strchr(tmp,'/');
	diff = (unsigned int)(temp++ - tmp);
	if (diff == 0)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}
	strncpy(t_name,tmp,diff);
	t_name[diff] = '\0';
	l = strlen(t_name);
	if (l > FAMILY_NAME_LENGTH - 1)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}
	
	tmp = strchr(temp,'/');
	diff = (unsigned int)(tmp++ - temp);
	if (diff == 0)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}
	strncpy(t_name,temp,diff);
	t_name[diff] = '\0';
	l = strlen(t_name);
	if (l > MEMBER_NAME_LENGTH - 1)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}

	temp = strchr(tmp,':');	
	if (temp == tmp)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}
	diff = (unsigned int)(temp - tmp);
	strncpy(t_name,tmp,diff);
	t_name[diff] = '\0';
	l = strlen(t_name);
	if (l > RES_NAME_LENGTH - 1)
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}

/* Leave function */

	return(0);
			
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


static long res_line(char *line1,ana_input *in,long *p_line_ptr,char **tmp_resdef,long *p_error)
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
	{
		*p_error = DbErr_BadResSyntax;
		return(-1);
	}
	diff = (unsigned int)(tmp - line1) + 1;
	strncpy(base,line1,diff);
	base[diff] = 0;

/* Copy the first line in the resulting buffer */

	k = strlen(line1);
	if ((ptr = (char *)malloc(k + 1)) == NULL)
	{
		*p_error = DbErr_ClientMemoryAllocation;
		return(-1);
	}
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
		(*p_line_ptr)++;

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
				*p_error = DbErr_BadResSyntax;
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
			*p_error = DbErr_BadResSyntax;
			free(ptr);
			return(-1);
		}
		line1[j] = '\0';
		k = strlen(line1);

/* Test to verify that the array (in ascii characters) is not bigger than
   the allocated memory and realloc memory if needed. */
   
		if ((ptr = (char *)realloc(ptr,strlen(ptr) + k + 1)) == NULL)
		{
			*p_error = DbErr_ClientMemoryAllocation;
			return(-1);
		}
		
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



/****************************************************************************
*                                                                           *
*		Code for check_dev function                                 *
*                        ---------                                          *
*                                                                           *
*    Function rule : To check that a simple device definition line is       *
*		     correct						    *
*                                                                           *
*    Argin : - lin : A pointer to the modified resource definition (without *
*		     space and tab characters)                              *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or -1 if the resource      *
*    definition is not valid.						    *
*                                                                           *
****************************************************************************/

static long check_dev(char *lin,long *p_err_dev,long *p_error)
{
	char *ptr,*ptr1;
	char *ptr_cp;
	char *tmp_ptr;
	long l,size;
	long cptr = 0;

/* Allocate memory for strok pointers */

	if ((ptr_cp = (char *)malloc(strlen(lin) + 1)) == NULL)
	{
		*p_error = DbErr_ClientMemoryAllocation;
		return(-1);
	}
	
	if ((ptr = (char *)malloc(strlen(lin) + 1)) == NULL)
	{
		free(ptr_cp);
		*p_error = DbErr_ClientMemoryAllocation;
		return(-1);
	}
	tmp_ptr = ptr;
	
/* Verify that each device definition is correct */

	strcpy(ptr_cp,lin);
		
	ptr = strtok(ptr_cp,",");
	ptr1 = strchr(ptr,':');
	ptr1++;
	size = strlen(ptr1);
	cptr++;
	l = 0;
	NB_CHAR(l,ptr,'/');
	if ((l != 4) || (size > (DEV_NAME_LENGTH - 1)))
	{
		free(ptr_cp);
		free(tmp_ptr);
		*p_err_dev = cptr;
		*p_error = DbErr_BadDevSyntax;
		return(-1);
	}	
	
	while((ptr = strtok(NULL,",")) != NULL)
	{
		cptr++;
		l = 0;
		NB_CHAR(l,ptr,'/');
		size = strlen(ptr);
		if ((l != 2) || (size > (DEV_NAME_LENGTH - 1)))
		{
			free(ptr_cp);
			free(tmp_ptr);
			*p_err_dev = cptr;
			*p_error = DbErr_BadDevSyntax;
			return(-1);
		}
	}
	

/* Leave function */

	free(ptr_cp);
	free(tmp_ptr);
	return(0);
			
}



/****************************************************************************
*                                                                           *
*		Code for get_error_line function                            *
*                        --------------                                     *
*                                                                           *
*    Function rule : To retrieve line number in input buffer with a faulty  *
*		     device definition					    *
*                                                                           *
*    Argin : - buffer : The input buffer				    *
*	     - err_dev : The faulty device number in buffer		    *
*                                                                           *
*    Argout : - p_line : Pointer for line number                            *
*                                                                           *
*                                                                           *
****************************************************************************/

static void get_error_line(const char *buffer,long err_dev,long *p_line)
{
	long line_cptr = 0;
	char *ptr;
	long length_to_eol,l;
	char line[160];
	long sum_comma = 0;

	ptr = (char *)buffer;
	
	while(ptr != NULL)
	{
		length_to_eol = strcspn(ptr,"\n");
		if (length_to_eol == 0)
			break;
		strncpy(line,ptr,length_to_eol);
		line[length_to_eol] = '\0';
		ptr = ptr + length_to_eol + 1;
		line_cptr++;

		l = 0;
		NB_CHAR(l,line,',');
		sum_comma = sum_comma + l;
		
		if (sum_comma > err_dev)
			break;
	}
	*p_line = line_cptr;
			
}



/****************************************************************************
*                                                                           *
*		db_upddev function code                       	    	    *
*               ---------                                         	    *
*                                                                           *
*    Function rule : To update device list(s) into the database. This call  *
*		     is used by the db_update facility for the update of any*
*		     device server device list				    *
*									    *
*    Argin : - devdef_nb : Device list number				    *
*	     - devdef : Device list					    *
*									    *
*    Argout : - deferr_nb : Which device definition is the error reason	    *
*             - p_error : Pointer for the error code in case of problems    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the variable       *
*    pointed to by "p_error". The function also initializes the deferr_nb   *
*    argout with the device definition  where the error occurs. Otherwise,  *
*    the function returns 0             				    *
*                                                                           *
*****************************************************************************/


long db_upddev(long devdef_nb,char **devdef,long *deferr_nb,long *p_error)
{
	db_psdev_error *recev;
	int i,j,k;
	long error;
	long exit_code = 0;
	db_res sent;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((devdef_nb == 0) || (devdef == NULL) || (deferr_nb == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(-1);
	}

/* Miscellaneous init. */

	*p_error = 0;
	*deferr_nb = 0;
		
#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(-1);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
		first++;
	}
	old_tout = timeout_update;
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_update);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
			
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif

/* Initialize data sent to server. The db_err long is used to transfer the
   delete resource flag over the network !! */

	sent.db_err = 0;
	sent.res_val.arr1_len = devdef_nb;
	
	if ((sent.res_val.arr1_val = (char **)calloc(sizeof(char *),devdef_nb)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(-1);
	}

	for (i = 0;i < devdef_nb;i++)
	{	
		k = strlen(devdef[i]);
		if ((sent.res_val.arr1_val[i] = (char *)malloc(k + 1)) == NULL)
		{
			for (j = 0;j < i;j++)
				free(sent.res_val.arr1_val[j]);
			free(sent.res_val.arr1_val);
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = DbErr_ClientMemoryAllocation;
			return(-1);
		}
		strcpy(sent.res_val.arr1_val[i],devdef[i]);
	}

		
/* Call server */

	recev = db_upddev_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_UPDDEV,(long)0,DB_UDP,&error);
		}
		if (error != 0)
		{
			for (j = 0;j < devdef_nb;j++)
				free(sent.res_val.arr1_val[j]);
			free(sent.res_val.arr1_val);
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = error;
			return(-1);
		}
	}

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (recev->error_code == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_upddev_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				for (j = 0;j < devdef_nb;j++)
					free(sent.res_val.arr1_val[j]);
				free(sent.res_val.arr1_val);
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(-1);
			}
			if ((recev->error_code == 0) || 
			    (recev->error_code != DbErr_DatabaseNotConnected))
				break;
		}
	}

/* Any problems during database access ? */

	if(recev->error_code != 0)
	{
		for (j = 0;j < devdef_nb;j++)
			free(sent.res_val.arr1_val[j]);
		free(sent.res_val.arr1_val);
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*deferr_nb = recev->psdev_err;
		*p_error = recev->error_code;
		return(-1);
	}
	
/* Free memory used to send data to server */

	for (i = 0;i < devdef_nb;i++)
		free(sent.res_val.arr1_val[i]);
	free(sent.res_val.arr1_val);	

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}




/****************************************************************************
*                                                                           *
*		db_updres function code                       	    	    *
*               ---------                                         	    *
*                                                                           *
*    Function rule : To update resource(s) into the database. This call     *
*		     is used by the db_update facility for the update of any*
*		     resources						    *
*									    *
*    Argin : - resdef_nb : Resource number				    *
*	     - resdef : Resource list					    *
*									    *
*    Argout : - deferr_nb : Which device definition is the error reason	    *
*             - p_error : Pointer for the error code in case of problems    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the variable       *
*    pointed to by "p_error". The function also initializes the deferr_nb   *
*    argout with the device definition  where the error occurs. Otherwise,  *
*    the function returns 0             				    *
*                                                                           *
*****************************************************************************/


long db_updres(long resdef_nb,char **resdef,long *deferr_nb,long *p_error)
{
	db_psdev_error *recev;
	int i,j,k;
	long error,con_type;
	long exit_code = 0;
	db_res sent;
	struct timeval old_tout;
	long size,used_tcp;
	CLIENT *local_cl;
	CLIENT *cl_tcp;
	struct sockaddr_in serv_adr;
	int tcp_so;
#ifndef vxworks
	static struct hostent *ht;
#else
	static int host_addr;
#endif
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((resdef_nb == 0) || (resdef == NULL) || (deferr_nb == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(-1);
	}

/* Miscellaneous init. */

	*p_error = 0;
	*deferr_nb = 0;

/* Compute size of data sent to server */

	size = 0;
	for (i = 0;i < resdef_nb;i++)
		size = size + strlen(resdef[i]) + 4;
	if (size > UDP_MAX_SIZE)
		used_tcp = True;
	else
		used_tcp = False;

	if (used_tcp == False)
	{				
#ifdef ALONE
/* Create RPC connection if it's the first call */

		if (!first)
		{
			cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
			if (cl == NULL)
			{
				*p_error = DbErr_CannotCreateClientHandle;
				return(-1);
			}
			clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_update);
			clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
			first++;
		}
		old_tout = timeout_update;
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   		clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
		clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
#endif /* ALONE */
	}
	else
	{
#ifdef ALONE
/* Create RPC TCP connection */

		cl_tcp = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"tcp");
		if (cl_tcp == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(-1);
		}
		clnt_control(cl_tcp,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(cl_tcp,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
		old_tout = timeout_update;
#else /* ALONE */			
#ifndef vxworks
		ht = gethostbyname(db_info.conf->server_host);
		if (ht == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(-1);
		}		
#else /* !vxworks */
		host_addr = hostGetByName(db_info.conf->server_host);
		if (host_addr == 0)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(-1);
		}
#endif /* !vxworks */


		serv_adr.sin_family = AF_INET;
#ifndef vxworks
		memcpy((void *)(&serv_adr.sin_addr),ht->h_addr,(size_t)ht->h_length);
#else  /* !vxworks */
		memcpy((void *)(&serv_adr.sin_addr),(char*)&host_addr, 4);
#endif /* !vxworks */


		serv_adr.sin_port = 0;
		tcp_so = RPC_ANYSOCK;

		cl_tcp = clnttcp_create(&serv_adr,db_info.conf->prog_number,
					DB_VERS_3,&tcp_so,0,0);

		if (cl_tcp == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(-1);
		}
		
		clnt_control(cl_tcp,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(cl_tcp,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);

#endif /* ALONE */
	}
				
#ifdef ALONE
	if (used_tcp == True)
		local_cl = cl_tcp;
	else
		local_cl = cl;
#else
	if (used_tcp == True)
		local_cl = cl_tcp;
	else
		local_cl = db_info.conf->clnt;
#endif

/* Initialize data sent to server. The db_err long is used to transfer the
   delete resource flag over the network !! */

	sent.db_err = 0;
	sent.res_val.arr1_len = resdef_nb;
	
	if ((sent.res_val.arr1_val = (char **)calloc(sizeof(char *),resdef_nb)) == NULL)
	{
		if (used_tcp == True)
			clnt_destroy(local_cl);
		else
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		}
		*p_error = DbErr_ClientMemoryAllocation;
		return(-1);
	}

	for (i = 0;i < resdef_nb;i++)
	{	
		k = strlen(resdef[i]);
		if ((sent.res_val.arr1_val[i] = (char *)malloc(k + 1)) == NULL)
		{
			for (j = 0;j < i;j++)
				free(sent.res_val.arr1_val[j]);
			free(sent.res_val.arr1_val);
			if (used_tcp == True)
				clnt_destroy(local_cl);
			else
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			}
			*p_error = DbErr_ClientMemoryAllocation;
			return(-1);
		}
		strcpy(sent.res_val.arr1_val[i],resdef[i]);
	}

		
/* Call server */

	recev = db_updres_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if (used_tcp == False)
		{
			if(error == DevErr_RPCTimedOut)
			{
				if (used_tcp == True)
					con_type = DB_TCP;
				else
					con_type = DB_UDP;
				to_reconnection((void *)&sent,(void **)&recev,&local_cl,
						(int)DB_UPDRES,(long)0,con_type,&error);
			}
		}
		if (error != 0)
		{
			for (j = 0;j < resdef_nb;j++)
				free(sent.res_val.arr1_val[j]);
			free(sent.res_val.arr1_val);
			if (used_tcp == True)
				clnt_destroy(local_cl);
			else
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			}
			*p_error = error;
			return(-1);
		}
	}

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (recev->error_code == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_updres_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				for (j = 0;j < resdef_nb;j++)
					free(sent.res_val.arr1_val[j]);
				free(sent.res_val.arr1_val);
				if (used_tcp == True)
					clnt_destroy(local_cl);
				else
				{
					clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
					clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				}
				*p_error = error;
				return(-1);
			}
			if ((recev->error_code == 0) || 
			    (recev->error_code != DbErr_DatabaseNotConnected))
				break;
		}
	}

/* Any problems during database access ? */

	if(recev->error_code != 0)
	{
		for (j = 0;j < resdef_nb;j++)
			free(sent.res_val.arr1_val[j]);
		free(sent.res_val.arr1_val);
		if (used_tcp == True)
			clnt_destroy(local_cl);
		else
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		}
		*deferr_nb = recev->psdev_err;
		*p_error = recev->error_code;
		return(-1);
	}
	
/* Free memory used to send data to server */

	for (i = 0;i < resdef_nb;i++)
		free(sent.res_val.arr1_val[i]);
	free(sent.res_val.arr1_val);	

/* Leave function */

	if (used_tcp == True)
		clnt_destroy(local_cl);
	else
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	}
	return(exit_code);

}



/****************************************************************************
*                                                                           *
*		db_secpass function code                           	    *
*               ----------                                         	    *
*                                                                           *
*    Function rule : To retrieve the list of devices  domain  name	    *
*                                                                           *
*    Argout : - p_domain_nb : Pointer for domain number			    *
*	      - ppp_list : Pointer for the domain name list. Memory is      *
*			   allocated by this function		    	    *
*             - p_error : Pointer for the error code in case of problems    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the variable       *
*    pointed to by "p_error". Otherwise, the function returns 0             *
*                                                                           *
*****************************************************************************/


long db_secpass(char **pass,long *p_error)
{
	db_res *recev;
	int i,j;
	long error;
	long nb_domain;
	long exit_code = 0;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if (pass == NULL)
	{
		*p_error = DbErr_BadParameters;
		return(-1);
	}

/* Miscellaneous init. */

	*p_error = 0;
			
#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(-1);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
		first++;
	}
	old_tout = timeout_update;
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_update);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
			
#endif /* ALONE */


/* Call server */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

	recev = db_secpass_1(local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)NULL,(void **)&recev,&local_cl,
					(int)DB_SECPASS,(long)0,DB_UDP,&error);
		}
		if (error != 0)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = error;
			return(-1);
		}
	}

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (recev->db_err == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_secpass_1(local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(-1);
			}
			if ((recev->db_err == 0) || 
			    (recev->db_err != DbErr_DatabaseNotConnected))
				break;
		}
	}

/* Any problems during database access ? */

	if(recev->db_err != 0)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = recev->db_err;
		return(-1);
	}

/* Allocate memory for password and copy it */

	if ((*pass = (char *)malloc(strlen(recev->res_val.arr1_val[0]) + 1)) == NULL)
	{

		*p_error = DbErr_ClientMemoryAllocation;
		return(-1);
	}
	strcpy(*pass,recev->res_val.arr1_val[0]);
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}




/****************************************************************************
*                                                                           *
*		db_delete_update function code                	    	    *
*               ----------------                                      	    *
*                                                                           *
*    Function rule : To update resource(s) into the database. This call     *
*		     is used by the db_update facility for the update of any*
*		     resources						    *
*									    *
*    Argin : - resdef_nb : Resource number				    *
*	     - resdef : Resource list					    *
*									    *
*    Argout : - deferr_nb : Which device definition is the error reason	    *
*             - p_error : Pointer for the error code in case of problems    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the variable       *
*    pointed to by "p_error". The function also initializes the deferr_nb   *
*    argout with the device definition  where the error occurs. Otherwise,  *
*    the function returns 0             				    *
*                                                                           *
*****************************************************************************/


long db_delete_update(long dev_nb,char **dev_name_list,long list_type,
		      long devdef_nb,char **devdef,
		      long resdef_nb,char **resdef,
		      db_delupd_error *p_error)
{
	db_delupd_error *recev;
	int i,j,k;
	long error,con_type;
	long exit_code = 0;
	arr1 sent_data[3];
	db_arr1_array sent;
	struct timeval old_tout;
	long size,used_tcp;
	CLIENT *local_cl;
	CLIENT *cl_tcp;
	struct sockaddr_in serv_adr;
	int tcp_so;
#ifndef vxworks
	static struct hostent *ht;
#else
	static int host_addr;
#endif
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if (((dev_nb != 0) && (dev_name_list == NULL)) || 
	    ((devdef_nb != 0) && (devdef == NULL)) || 
	    ((resdef_nb != 0) && (resdef == NULL)) ||
	    ((list_type != DB_DEV_LIST) && (list_type != DB_RES_LIST)))
	{
		p_error->error = DbErr_BadParameters;
		p_error->type = DB_GLOBAL_ERROR;
		return(-1);
	}

/* Miscellaneous init. */

	p_error->error = 0;
	p_error->type = 0;
	p_error->number = 0;

/* Compute size of data sent to server */

	size = 0;
	for (i = 0;i < dev_nb;i++)
		size = size + strlen(dev_name_list[i]) + 4;
	for (i = 0;i < devdef_nb;i++)
		size = size + strlen(devdef[i]);
	for (i = 0;i < resdef_nb;i++)
		size = size + strlen(resdef[i]);
		
	if (size > UDP_MAX_SIZE)
		used_tcp = True;
	else
		used_tcp = False;

	if (used_tcp == False)
	{				
#ifdef ALONE
/* Create RPC connection if it's the first call */

		if (!first)
		{
			cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
			if (cl == NULL)
			{
				p_error->error = DbErr_CannotCreateClientHandle;
				p_error->type = DB_GLOBAL_ERROR;
				return(-1);
			}
			clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_update);
			clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
			first++;
		}
		old_tout = timeout_update;
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   		clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
		clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
#endif /* ALONE */
	}
	else
	{
#ifdef ALONE
/* Create RPC TCP connection */

		cl_tcp = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"tcp");
		if (cl_tcp == NULL)
		{
			p_error->error = DbErr_CannotCreateClientHandle;
			p_error->type = DB_GLOBAL_ERROR;
			return(-1);
		}
		clnt_control(cl_tcp,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(cl_tcp,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);
		old_tout = timeout_update;
#else /* ALONE */			
#ifndef vxworks
		ht = gethostbyname(db_info.conf->server_host);
		if (ht == NULL)
		{
			p_error->error = DbErr_CannotCreateClientHandle;
			p_error->type = DB_GLOBAL_ERROR;
			return(-1);
		}		
#else /* !vxworks */
		host_addr = hostGetByName(db_info.conf->server_host);
		if (host_addr == 0)
		{
			p_error->error = DbErr_CannotCreateClientHandle;
			p_error->type = DB_GLOBAL_ERROR;
			return(-1);
		}
#endif /* !vxworks */


		serv_adr.sin_family = AF_INET;
#ifndef vxworks
		memcpy((void *)(&serv_adr.sin_addr),ht->h_addr,(size_t)ht->h_length);
#else  /* !vxworks */
		memcpy((void *)(&serv_adr.sin_addr),(char*)&host_addr, 4);
#endif /* !vxworks */


		serv_adr.sin_port = 0;
		tcp_so = RPC_ANYSOCK;

		cl_tcp = clnttcp_create(&serv_adr,db_info.conf->prog_number,
					DB_VERS_3,&tcp_so,0,0);

		if (cl_tcp == NULL)
		{
			p_error->error = DbErr_CannotCreateClientHandle;
			p_error->type = DB_GLOBAL_ERROR;
			return(-1);
		}
		
		clnt_control(cl_tcp,CLSET_TIMEOUT,(char *)&timeout_update);
		clnt_control(cl_tcp,CLSET_RETRY_TIMEOUT,(char *)&timeout_update);

#endif /* ALONE */
	}
				
#ifdef ALONE
	if (used_tcp == True)
		local_cl = cl_tcp;
	else
		local_cl = cl;
#else
	if (used_tcp == True)
		local_cl = cl_tcp;
	else
		local_cl = db_info.conf->clnt;
#endif

/* Initialize data sent to server. First the device for which all resources
   must be deleted */

	sent.arr_length = 3;
	sent.arr = sent_data;
	sent.arr_type = list_type;
	
	sent_data[0].arr1_len = dev_nb;

	if (dev_nb != 0)
	{	
		if ((sent_data[0].arr1_val = (char **)calloc(sizeof(char *),dev_nb)) == NULL)
		{
			if (used_tcp == True)
				clnt_destroy(local_cl);
			else
			{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			}
			p_error->error = DbErr_ClientMemoryAllocation;
			p_error->type = DB_GLOBAL_ERROR;
			return(-1);
		}

		for (i = 0;i < dev_nb;i++)
		{	
			k = strlen(dev_name_list[i]);
			if ((sent_data[0].arr1_val[i] = (char *)malloc(k + 1)) == NULL)
			{
				for (j = 0;j < i;j++)
					free(sent_data[0].arr1_val[j]);
				free(sent_data[0].arr1_val);
				if (used_tcp == True)
					clnt_destroy(local_cl);
				else
				{
					clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
					clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				}
				p_error->error = DbErr_ClientMemoryAllocation;
				p_error->type = DB_GLOBAL_ERROR;
				return(-1);
			}
			strcpy(sent_data[0].arr1_val[i],dev_name_list[i]);
		}
	}
	else
		sent_data[0].arr1_val = NULL;

/* Then prepare all device name */

	sent_data[1].arr1_len = devdef_nb;

	if (devdef_nb != 0)
	{	
		if ((sent_data[1].arr1_val = (char **)calloc(sizeof(char *),devdef_nb)) == NULL)
		{
			for (j = 0;j < dev_nb;j++)
				free(sent_data[0].arr1_val[j]);
			free(sent_data[0].arr1_val);
			if (used_tcp == True)
				clnt_destroy(local_cl);
			else
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			}
			p_error->error = DbErr_ClientMemoryAllocation;
			p_error->type = DB_GLOBAL_ERROR;
			return(-1);
		}

		for (i = 0;i < devdef_nb;i++)
		{	
			k = strlen(devdef[i]);
			if ((sent_data[1].arr1_val[i] = (char *)malloc(k + 1)) == NULL)
			{
				for (j = 0;j < i;j++)
					free(sent_data[1].arr1_val[j]);
				free(sent_data[1].arr1_val);
				for (j = 0;j < dev_nb;j++)
					free(sent_data[0].arr1_val[j]);
				free(sent_data[0].arr1_val);
				if (used_tcp == True)
					clnt_destroy(local_cl);
				else
				{
					clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
					clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				}
				p_error->error = DbErr_ClientMemoryAllocation;
				p_error->type = DB_GLOBAL_ERROR;
				return(-1);
			}
			strcpy(sent_data[1].arr1_val[i],devdef[i]);
		}
	}
	else
		sent_data[1].arr1_val = NULL;

/* Finally, for resources */

	sent_data[2].arr1_len = resdef_nb;

	if (resdef_nb != 0)
	{
		if ((sent_data[2].arr1_val = (char **)calloc(sizeof(char *),resdef_nb)) == NULL)
		{
			for (j = 0;j < dev_nb;j++)
				free(sent_data[0].arr1_val[j]);
			free(sent_data[0].arr1_val);
			for (j = 0;j < devdef_nb;j++)
				free(sent_data[1].arr1_val[j]);
			free(sent_data[1].arr1_val);
			if (used_tcp == True)
				clnt_destroy(local_cl);
			else
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			}
			p_error->error = DbErr_ClientMemoryAllocation;
			p_error->type = DB_GLOBAL_ERROR;
			return(-1);
		}

		for (i = 0;i < resdef_nb;i++)
		{	
			k = strlen(resdef[i]);
			if ((sent_data[2].arr1_val[i] = (char *)malloc(k + 1)) == NULL)
			{
				for (j = 0;j < i;j++)
					free(sent_data[2].arr1_val[j]);
				free(sent_data[2].arr1_val);
				for (j = 0;j < devdef_nb;j++)
					free(sent_data[1].arr1_val[j]);
				free(sent_data[1].arr1_val);
				for (j = 0;j < dev_nb;j++)
					free(sent_data[0].arr1_val[j]);
				free(sent_data[0].arr1_val);
				if (used_tcp == True)
					clnt_destroy(local_cl);
				else
				{
					clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
					clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				}
				p_error->error = DbErr_ClientMemoryAllocation;
				p_error->type = DB_GLOBAL_ERROR;
				return(-1);
			}
			strcpy(sent_data[2].arr1_val[i],resdef[i]);
		}
	}
	else
		sent_data[2].arr1_val = NULL;
				
/* Call server */

	recev = db_delete_update_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if (used_tcp == False)
		{
			if(error == DevErr_RPCTimedOut)
			{
				if (used_tcp == True)
					con_type = DB_UDP;
				else
					con_type = DB_TCP;
				to_reconnection((void *)&sent,(void **)&recev,&local_cl,
						(int)DB_DEL_UPDATE,(long)0,con_type,&error);
			}
		}
		if (error != 0)
		{
			for (k = 0;k < 3;k++)
			{
				for (j = 0;j < sent_data[k].arr1_len;j++)
					free(sent_data[k].arr1_val[j]);
				free(sent_data[k].arr1_val);
			}
			if (used_tcp == True)
				clnt_destroy(local_cl);
			else
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			}
			p_error->type = DB_GLOBAL_ERROR;
			p_error->error = error;
			return(-1);
		}
	}

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (recev->error == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_delete_update_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				for (k = 0;k < 3;k++)
				{
					for (j = 0;j < sent_data[k].arr1_len;j++)
						free(sent_data[k].arr1_val[j]);
					free(sent_data[k].arr1_val);
				}
				if (used_tcp == True)
					clnt_destroy(local_cl);
				else
				{
					clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
					clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				}
				p_error->error = error;
				p_error->type = DB_GLOBAL_ERROR;
				return(-1);
			}
			if ((recev->error == 0) || 
			    (recev->error != DbErr_DatabaseNotConnected))
				break;
		}
	}

/* Any problems during database access ? */

	if(recev->error != 0)
	{
		for (k = 0;k < 3;k++)
		{
			for (j = 0;j < sent_data[k].arr1_len;j++)
				free(sent_data[k].arr1_val[j]);
			free(sent_data[k].arr1_val);
		}
		if (used_tcp == True)
			clnt_destroy(local_cl);
		else
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		}
		*p_error = *recev;
		return(-1);
	}
	
/* Free memory used to send data to server */

	for (k = 0;k < 3;k++)
	{
		for (j = 0;j < sent_data[k].arr1_len;j++)
			free(sent_data[k].arr1_val[j]);
		free(sent_data[k].arr1_val);
	}

/* Leave function */

	if (used_tcp == True)
		clnt_destroy(local_cl);
	else
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	}
	return(exit_code);

}

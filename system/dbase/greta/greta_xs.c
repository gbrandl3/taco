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
 *
 * File         : greta_xs.c
 *
 * Project      : GRETA application
 *
 * Description  : 
 *                
 *
 * Author       : 
 *                $Author: jkrueger1 $
 *
 * Original     : 
 *
 * Version      : $Revision: 1.3 $
 *
 * Date         : $Date: 2008-04-06 09:07:39 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <greta.h>
#if HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif
#include <Xm/Text.h>
#include <Xm/TextF.h>


/* Some local functions */

void res_multi_lines(long ,char *,char *,long);

    
/****************************************************************************
*                                                                           *
*						char *xsGetDialogTitle(Widget w)					*
*							  ----------------								*
*	Returns the w widget dialogTitle resource converted into a simple 		*
*	string . (dialogTitle belongs to XmBulletinBoard resource set)			*
*                            		                                        *
****************************************************************************/
						  
char *xsGetDialogTitle(Widget w)
{

    XmString   xmstr=NULL;
    Boolean    ret_val;
    char       *title=NULL;  
 
    XtVaGetValues(w, XmNdialogTitle, &xmstr, NULL);
    if (xmstr == NULL)
        return(NULL);

    ret_val = XmStringGetLtoR(xmstr,XmFONTLIST_DEFAULT_TAG,&title);
    XmStringFree(xmstr);

    if (ret_val == True)
        return (title);
    else
        return (NULL);

}

/****************************************************************************
*                                                                           *
*						void xsSetTitle(Widget w, char *title)		*
*							 ----------------		*
*	Sets the w widget dialogTitle resource with title converted into an		*
*	Xmstring . (dialogTitle belongs to XmBulletinBoard resource set)		*
*                            		                                        *
****************************************************************************/

void 
xsSetTitle(Widget w, char *title)
{
 XmString xmstr;
 
	xmstr=XmStringCreateLocalized(title);
	XtVaSetValues(w, XmNdialogTitle, xmstr, NULL);
	XmStringFree(xmstr);
}

/****************************************************************************
*                                                                           *
*			xsXmStringTableCreate : 			    *
*			---------------------                               *
*                                                                           *
*	Takes a string table and elt nb and returns pointer to its          *
*	equivalent XmString table.					    *
*                                                                           *
*	Synopsis : 							    *
*       XmString *xsXmStringTableCreate(char *str_tbl,			    *
*				        int nb_tbl,			    *
*					int elt_size)			    *
*                            		                                    *
****************************************************************************/

XmString *xsXmStringTableCreate(char *str_tbl, int nb_tbl,int  elt_size)
{
	XmString *xmstr_tbl;
	int i;

/* Memory allocation for xmstr_tbl */

	xmstr_tbl=(XmString *)XtCalloc(nb_tbl,sizeof(XmString));
	
/* Convert str_tbl into xmstr_tbl */

	for(i=0;i<nb_tbl;i++)
		xmstr_tbl[i]=XmStringCreateLtoR((char *)(str_tbl+i*elt_size),
						XmFONTLIST_DEFAULT_TAG);
/* Return xmstr_tbl */

	return xmstr_tbl;
	
}


/****************************************************************************
*                                                                           *
*			etxsXmStringTableCreate : 			    *
*			---------------------                               *
*                                                                           *
*	Takes a string table and elt nb and returns pointer to its          *
*	equivalent XmString table.					    *
*                                                                           *
*	Synopsis : 							    *
*       XmString *xsXmStringTableCreate(char **str_tbl,int nb_tbl)	    *
*                            		                                    *
****************************************************************************/

XmString *etxsXmStringTableCreate(char **str_tbl,long nb_tbl)
{
	XmString *xmstr_tbl;
	long i;

/* Memory allocation for xmstr_tbl */

	xmstr_tbl=(XmString *)XtCalloc(nb_tbl,sizeof(XmString));
	
/* Convert str_tbl into xmstr_tbl */

	for(i = 0;i < nb_tbl;i++)
	{
/*		xmstr_tbl[i]=XmStringCreateLtoR(str_tbl[i],
						XmFONTLIST_DEFAULT_TAG);*/
		xmstr_tbl[i]=XmStringCreateLocalized(str_tbl[i]);
	}
	
/* Return xmstr_tbl */

	return xmstr_tbl;
	
}

/****************************************************************************
*                                                                           *
*			xsXmStringTableCreateFromPtrTbl :				*
*			-------------------------------                 *
*                                                                           *
*	Takes a string pointers table and elt nb and returns pointer to its 	* 
*	equivalent XmString table.												*
*                                                                           *
*	Synopsis : XmString *xsXmStringTableCreateFromPtrTbl(char **str_tbl,	*
*												int nb_tbl)					*
*                            		                                        *
****************************************************************************/

XmString *xsXmStringTableCreateFromPtrTbl(char **str_tbl, int nb_tbl)
{
 XmString *xmstr_tbl;
 int i;
/* Memory allocation for xmstr_tbl */
	xmstr_tbl=(XmString *)XtCalloc(nb_tbl,sizeof(XmString));
	
/* Convert str_tbl into xmstr_tbl */
	for(i=0;i<nb_tbl;i++)
		xmstr_tbl[i]=XmStringCreateLtoR((str_tbl[i]),
									XmFONTLIST_DEFAULT_TAG);
/* Return xmstr_tbl */
	return xmstr_tbl;
}

/****************************************************************************
*                                                                           *
*                            		                                        *
****************************************************************************/

void str_ptr_tbl_free(char **str_tbl, int nb_tbl)
{
 int i;
	
	for(i=0;i<nb_tbl;free(str_tbl[i++]));
	free(str_tbl);
}

/****************************************************************************
*                                                                           *
*     xsListGetBrowseSelection : (browse select) returns the string of		*
*                    		  	 the selected item .						*
*                            		                                        *
****************************************************************************/

char *xsListGetBrowseSelection(Widget lw)
{
 XmStringTable selection;
 char *resu;
 
	XtVaGetValues(lw, XmNselectedItems, &selection, NULL);

/* !!! For XmNselectedItems, XtGetValues returns the list items themselves and not
   a copy, thus the RETURNED ITEMS MUST NOT BE FREED . */

	XmStringGetLtoR(selection[0], XmFONTLIST_DEFAULT_TAG, &resu);
	return resu;
	
}

/****************************************************************************
*                                                                           *
*     xsListGetItemCount : returns the number of items in the list    		*
*                            		                                        *
****************************************************************************/

int xsListGetItemCount(Widget lw)
{
int cnt;

	XtVaGetValues(lw, XmNitemCount, &cnt, NULL);
	return cnt;
}
	 
/****************************************************************************
*                                                                           *
*     xsListGetPosItem : returns the string of the item at the specified    *
*                        position : 1 points the first item, and so on .    *
*                        0 points the last item of the list	.				*
*                            		                                        *
****************************************************************************/
char *xsListGetNthItem(Widget lw, int pos)
{
 int p;
 XmStringTable xmstr_tbl;
 char *resu;
	
	p= (pos==0) ? xsListGetItemCount(lw) : pos ;
	XtVaGetValues(lw, XmNitems, &xmstr_tbl, NULL);

/* !!! For XmNitems, XtGetValues returns the list items themselves and not
   a copy, thus the RETURNED ITEMS MUST NOT BE FREED . */

	XmStringGetLtoR(xmstr_tbl[pos-1], XmFONTLIST_DEFAULT_TAG, &resu);
	return resu;
}
	 
/****************************************************************************
*                                                                           *
*        xsTextPrintString : insere une chaine dans un widget text	    *
*                    		 avant le point d'insertion .		    *
*                            -----                                          *
****************************************************************************/

void xsTextPrintString(Widget tw, char *s)
{
 XmTextPosition pos;
	pos=(XmTextPosition)XmTextGetInsertionPosition(tw);
	XmTextInsert(tw,pos,s);
	pos+=strlen(s);
	XmTextSetInsertionPosition(tw,pos);
}

/****************************************************************************
*                                                                           *
*   xsTextFieldSetString : positionne la valeur chaine d'un widget   	    *
*                    	   Text Field  et son point d'insertion .	    *
*                            		   --                               *
****************************************************************************/

void xsTextFieldSetString(Widget tfw, char *s)
{
 XmTextPosition pos;
	pos=(XmTextPosition)XmTextFieldGetInsertionPosition(tfw);
	XmTextFieldSetString(tfw,s);
	pos+=strlen(s);
	XmTextFieldSetInsertionPosition(tfw,pos);
}

/****************************************************************************
*                                                                           *
*						Code for OpenMsgBox procedure						*
*								 ----------									*
*		Profile : void OpenMsgBox(Widget MsgBox, char *content)				*
*                                                                           *
*		Procedure Rule : Fills the Message Box (ie. ErrorBox or InfoBox)	*
*						 with the string content, and afterwards manages it *
*                                                                           *
****************************************************************************/

void 
OpenMsgBox(Widget MsgBox, char *content)
{
XmString xmstr;
Arg args[1];	
Cardinal argcnt=0; 

	xmstr=XmStringCreateLtoR(content,XmFONTLIST_DEFAULT_TAG);
	XtVaSetValues(MsgBox, XmNmessageString, xmstr, NULL);
	XmStringFree(xmstr);
	XtManageChild(MsgBox);	
}


void etxsXmStringTableFree(XmString *xmstr_tbl,int str_nb)
{
	int i;
	
	for (i = 0;i < str_nb;i++)
		XmStringFree(xmstr_tbl[i]);
		
	XtFree((char *)xmstr_tbl);
}

/****************************************************************************
*                                                                           *
*                                                                           *
****************************************************************************/

void xsSetCursor(Widget w, unsigned int shape)
{
 	Window win;
 	Display *disp;
 	Cursor curs;
	
	win = XtWindow(w);
	disp = XtDisplay(w);
	curs = XCreateFontCursor(disp, shape);
	XDefineCursor(disp, win, curs);
	
}

/****************************************************************************
*                                                                           *
*                                                                           *
****************************************************************************/

void xsUnsetCursor(Widget w)
{
 Window win;
 Display *disp;
 
	win=XtWindow(w);
	disp=XtDisplay(w);
	XUndefineCursor(disp, win);
	
}

/****************************************************************************
*                                                                           *
Takes a text string and a modifiable line number (the first line of the text
is number 1), and fills the out parameters line beginning and line ending
positions (the first char of the string is at the position 0 ).
line number is decremented while scanning the string till the right position is
reached . Thus it's value is wheter 1 (successful search), or n>1, indicating
the line number from the text string end on.

Return value : 0 if out parameters have been filled up
			  -1 if line nb was too big . (line contains the line number for
			  	 the next search in the on_screen_following text string )
left and right are pointer to long because they are modifiable and will be used
as XmTextPosition .
*                                                                           *
****************************************************************************/

int LineNbToLRPos(char *text, long *line, long *left, long *right)
{
 char *ptr=text;
 char *ptr1;
 int eltsize;
 
	eltsize=sizeof(char);
	while(*line>1)
	{
		if((ptr=strchr(ptr, '\n'))==NULL)
			break;
		ptr+=eltsize;
		(*line)--;
	}
	
	if (ptr==NULL) 
	{
		*left=0;
		*right=0;
		return -1;
	}
	
	*left=(ptr-text)/eltsize;
	if((ptr1=strchr(ptr, '\n'))==NULL)
		ptr1=strchr(ptr, '\0');
	else ptr1+=eltsize;
	*right=(ptr1-text)/eltsize;
	return 0;
}

/****************************************************************************
*                                                                           *
			Code for NoBlanksFieldStr
			-------------------------

strin is composed of fields delimited by '/' . Each field has to be composed of
a unique non empty word, possibly preceded and/or follow by any number of
blanks (' ' or tab ) ; exception is made for field following last '/' : it
may be completely blanks-composed .
these conditions are checked and blanks discarded while copying strin into
created *strout . 
An error occurs if strin does not pass the checks .
In case of error, -1 is returned and an approppriate error string is create 
into strerr, subsequent *strerr freeing is let to the caller's responsibility .
In case of success, *strerr freeing is unnecessary but possible, and the number
of '/' found in strin is returned . 
*                                                                           *
****************************************************************************/

int NoBlanksFieldStr(char *strin, char **strout, char **strerr)
{
 	int ctr, blank_nb, non_blank_nb;
 	char *workstr, *word, *rem, *ptr, *s, blanks_str[3];

/*
 * 1) Initialization
 */

/* Duplicate strin into a modifiable work string ;
   approximate a size for *strerr, initialize it so as its always freeable ; 
   allocated enough memory for *strout . */
   
	workstr=strdup(strin);
	s=(char *)malloc((strlen(strin)+100)*sizeof(char));
	*strerr=NULL;
	*strout=(char *)malloc((strlen(strin)+1)*sizeof(char));

/* Initialize ptr : points the concatenation point in *strout */
	ptr=*strout;
	
/* Initialize ctr that numbered the '/' separating fields, and word that points
   the word to be delimited and work on at next step */
	ctr=0;
	word=workstr;
	
/* blanks_str is composed of a ' ' and a tab (plus final '\0' */
	strncpy(blanks_str, " 	", 3);
	
/*
 * 2) Loop : takes next '/', treats the field preceding it
 */

	while((rem=strchr(word, '/'))!=NULL)
	{
	/* Count the '/' just found */
		ctr++;

	/* Mark end of word by replacing '/' by '\0' */
		rem[0]='\0';

	/* Verify word is not empty */
		if((blank_nb=strspn(word, blanks_str))>=strlen(word))
		{
			sprintf(s, "The field number %d of %s is empty .\n", ctr, strin);
			*strerr=strdup(s);
			free(s);
			free(workstr);
			free(*strout);*strout=NULL;
			return -1;
		}

	/* Discard blanks */
		word+=blank_nb;
		
		non_blank_nb=strcspn(word, blanks_str);
		
	/* Append word/ into strout */
		strncpy(ptr, word, non_blank_nb);
		ptr[non_blank_nb]='/';
		ptr=&(ptr[non_blank_nb+1]);
		
	/* Verify there is only one word in the field */
		word+=non_blank_nb;
		if((blank_nb=strspn(word, blanks_str))!=strlen(word))
		{
			sprintf(s, "The field number %d of %s consists in more than one \
word .\n", ctr, strin);
			*strerr=strdup(s);
			free(s);
			free(workstr);
			free(*strout);*strout=NULL;
			return -1;
		}
		
	/* Prepare next loop */
		word=rem+1;
	}

/*
 * 3) Treats last field : may be blank 
 */
 
/* Finish here if only blanks remain */
	if((blank_nb=strspn(word, blanks_str))==strlen(word))
	{
		ptr[0]='\0';
		free(workstr);
		free(s);
		return ctr;
	}

/* Discard blanks */
	word+=blank_nb;
		
	non_blank_nb=strcspn(word, blanks_str);
		
/* Append word following last '/' into strout */
	strncpy(ptr, word, non_blank_nb);
	ptr[non_blank_nb]='\0';	
		
/* Verify there is only one word in the field */
	word+=non_blank_nb;
	if((blank_nb=strspn(word, blanks_str))!=strlen(word))
	{
		sprintf(s, "The field number %d of %s consists in more than one \
word .\n", ctr, strin);
		*strerr=strdup(s);
		free(s);
		free(workstr);
		free(*strout);*strout=NULL;
		return -1;
	}
 
 
/*
 * 4) Terminate
 */
	
	free(workstr);
	free(s);
	return ctr;
}




void xsTransformToQuestionBox(Widget w)
{
 	Widget tmpw;

	tmpw=XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(tmpw);
}




void xsTransformToInfoBox(Widget w)
{
 	Widget tmpw;
 
	tmpw=XmMessageBoxGetChild(w,XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(tmpw);
	tmpw=XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(tmpw);
}







/****************************************************************************
                                                                           
 Function     : int xsResfileLoad

 Description  : This function reads the contents of a file line by line and
                copies them into a text widget passed through the parameter.

 Arg(s) In    : char      *file_name : the pathname of the file to load.
                Widget    textw : the text widget to copy into.

 Arg(s) Out   : none

 Return(s)    : 0 if successful and -1 otherwise.
                                                                           
***************************************************************************/

int xsResfileLoad(char  *file_name, Widget textw)
{   
    	FILE             *stream;
	struct stat	 st;
	long 		 f_size;
	char		 *buf;
	long 		 max;

/* Get file size */

	if (stat(file_name,&st) != 0)
	{
		return(-1);
	}
	f_size = st.st_size;
	
/* Allocate memory to store file */

	if ((buf = (char *)malloc(f_size + 10)) == NULL)
	{
		return(-1);
	}
	
/* Open file */

    	stream = NULL;
    	stream = fopen(file_name, "r");

    	if (stream == NULL)
	{
		free(buf);
       		return(-1);
	}

    	if ( (feof(stream) != 0) || (ferror(stream) != 0) )
    	{
       		fclose(stream);
		free(buf);
       		return(-1);
    	}

/* Read entire file in one go */

	max = fread(buf,(size_t)1,(size_t)(f_size + 2),stream);
	buf[max] = '\0';

    	if (ferror(stream) != 0)
    	{
       		fclose(stream);
		free(buf);
       		return(-1);
    	}
	
/* Copy file into text widget */

	XmTextSetString(textw,buf);
	
/* Free memory and leave function */

	free(buf);
	return(0);

}

   
/****************************************************************************
                                                                           
 Function     : int xsCreateStringForText

 Description  : This function build a unique string from an array of string in
 		order to be displayed in amotif window

 Arg(s) In    : char      **array : The string array
                long    array_size : The array size

 Arg(s) Out   : char 	  **str : The resulting string. Memory for this string
 				  is allocated here

 Return(s)    : 0 if successful and -1 otherwise.
                                                                           
***************************************************************************/


int xsCreateStringForText(char **array,long array_size,char **str)
{
	long i,size,l,nb_line;
	char *tmp;


	size = 0;
	
/* Compute size of the resulting string */

	for (i = 0;i < array_size;i++)
	{
		l = strlen(array[i]);
		if (l < MAX_LINE_SIZE)
		{
			size = size + l;
			size = size + 1;
		}
		else
		{
			nb_line = (l / (MAX_LINE_SIZE - NB_SPACE)) + 1;
			size = size + ((MAX_LINE_SIZE + 2) * nb_line);
		}
	}
	size = size + 2;
	
/* Allocate mem for the resulting string */

	if ((tmp = (char *)malloc(size)) == NULL)
	{
		return(-1);
	}
	
/* Build the string */

	for (i = 0;i < array_size;i++)
	{
		if (i == 0)
		{
			l = strlen(array[i]);
			if (l < MAX_LINE_SIZE)
				strcpy(tmp,array[i]);
			else
				res_multi_lines(l,array[i],tmp,True);
		}
		else
		{
			l = strlen(array[i]);
			if (l < MAX_LINE_SIZE)
				strcat(tmp,array[i]);
			else
				res_multi_lines(l,array[i],tmp,False);
		}
		strcat(tmp,"\n");
	}

	*str = tmp;	
	return(0);
}



/****************************************************************************
                                                                           
 Function     : void res_multi_lines

 Description  : This function build a unique string from an array of string in
 		order to be displayed in amotif window

 Arg(s) In    : long    res_length : The resource value length
                char    *res_buf : The resource value
		long    first_flag : Flag set to True is the resource is the 
				     first line in the resulting string.

 Arg(s) Out   : char 	*str : The resulting string. Memory for this string
 			       must be allocated prior call to this function

 Return(s)    : No return value.
                                                                           
***************************************************************************/


void res_multi_lines(long res_length,char *res_buff,char *str,long first_flag)
{
	long nb_line,i;
	char *ptr;
	char tmp_buff[MAX_LINE_SIZE + 20];
	long buff_ind = 0;
	long still_char,last;
	unsigned long diff;

	nb_line = 0;
	while (buff_ind < res_length)
	{
		if (nb_line == 0)
		{
			strncpy(tmp_buff,res_buff,MAX_LINE_SIZE);
			tmp_buff[MAX_LINE_SIZE] = '\0';
			ptr = strrchr(tmp_buff,',');
			diff = (unsigned long)(ptr - tmp_buff);
			tmp_buff[diff + 1] = '\\';
			tmp_buff[diff + 2] = '\n';
			tmp_buff[diff + 3] = '\0';

			if (first_flag == True)			
				strcpy(str,tmp_buff);
			else
				strcat(str,tmp_buff);
			buff_ind = diff + 1;
			nb_line++;
		}
		else
		{
			for (i = 0;i < NB_SPACE;i++)
				tmp_buff[i] = ' ';
			tmp_buff[i] = '\0';
			still_char = res_length - buff_ind;
			if (still_char > (MAX_LINE_SIZE - NB_SPACE))
			{
				strncat(&(tmp_buff[strlen(tmp_buff)]),&(res_buff[buff_ind]),MAX_LINE_SIZE - NB_SPACE);
				tmp_buff[MAX_LINE_SIZE] = '\0';
				last = False;
				
				ptr = strrchr(tmp_buff,',');
				diff = (unsigned long)(ptr - tmp_buff);
				tmp_buff[diff + 1] = '\\';
				tmp_buff[diff + 2] = '\n';
				tmp_buff[diff + 3] = '\0';
			}
			else
			{
				strcpy(&(tmp_buff[strlen(tmp_buff)]),&(res_buff[buff_ind]));
				last = True;
			}

			strcat(str,tmp_buff);
			if (last == False)
				buff_ind = buff_ind + (diff - (NB_SPACE - 1));
			else
				buff_ind = 1000000;
			nb_line++;			
		}
	}
}



/****************************************************************************
                                                                           
 Function     : int xsCreateStringForTextServ

 Description  : This function reads the contents of a file line by line and
                copies them into a text widget passed through the parameter.

 Arg(s) In    : char      **array : The string array
                long    array_size : The array size

 Arg(s) Out   : char 	  **str : The resulting string. Memory for this string
 				  is allocated here

 Return(s)    : 0 if successful and -1 otherwise.
                                                                           
***************************************************************************/


int xsCreateStringForTextServ(char **array,long array_size,char **str)
{
	long i,j,size,l,nb_line;
	char *tmp;
	char old_dev[DEV_NAME_LENGTH + 2];
	char new_dev[DEV_NAME_LENGTH + 2];
	unsigned long diff;
	char *str1;
	char *tmp_char;

	size = 0;
	
/* Compute size of the resulting string */

	for (i = 0;i < array_size;i++)
	{
		l = strlen(array[i]);
		if (l < MAX_LINE_SIZE)
		{
			size = size + l;
			size = size + 2;
		}
		else
		{
			nb_line = (l / (MAX_LINE_SIZE - NB_SPACE)) + 1;
			size = size + ((MAX_LINE_SIZE + 2) * nb_line);
		}
	}
	size = size + 2;
	
/* Allocate mem for the resulting string */

	if ((tmp = (char *)malloc(size)) == NULL)
	{
		return(-1);
	}

/* Make a copy of the first dev_name */

	str1 = array[0];
	for (i = 0;i < 3;i++)
	{
		tmp_char = strchr(str1,'/');
		if (i != 2)
		{
			tmp_char++;
			str1 = tmp_char;
		}
	}
	diff = (unsigned long)(tmp_char - array[0]);
	strncpy(old_dev,array[0],diff);
	old_dev[diff] = '\0';
				
/* Build the string. Add a \n before each new device */

	for (i = 0;i < array_size;i++)
	{
		if (i == 0)
		{
			l = strlen(array[i]);
			if (l < MAX_LINE_SIZE)
				strcpy(tmp,array[i]);
			else
				res_multi_lines(l,array[i],tmp,True);
		}
		else
		{
			str1 = array[i];
			for (j = 0;j < 3;j++)
			{
				tmp_char = strchr(str1,'/');
				if (j != 2)
				{
					tmp_char++;
					str1 = tmp_char;
				}
			}
			diff = (unsigned long)(tmp_char - array[i]);
			strncpy(new_dev,array[i],diff);
			new_dev[diff] = '\0';
			
			if (strcmp(old_dev,new_dev) != 0)
			{
				strcat(tmp,"\n");
				strcpy(old_dev,new_dev);
			}
			l = strlen(array[i]);
			if (l < MAX_LINE_SIZE)		
				strcat(tmp,array[i]);
			else
				res_multi_lines(l,array[i],tmp,False);
		}
		strcat(tmp,"\n");
	}

	*str = tmp;	
	return(0);
}

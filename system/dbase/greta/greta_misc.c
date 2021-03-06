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
 * File           : greta_misc.c
 *
 * Project        : GRETA application
 *
 * Description    : 
 *
 * Author         : 
 *                $Author: jkrueger1 $
 *
 * Original       : June 1998
 *
 * Version      : $Revision: 1.3 $
 *
 * Date         : $Date: 2006-09-18 21:59:06 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <greta.h>

/* Some local function */

static int strchr_occurrence_nb(char *, char );


/****************************************************************************
*                                                                           *
*		char *frame(char *title)				    *
*		      -----						    *
*	Takes title and put it inside a string that forms a framed title    *
*	ready to be printed as returned, or returned a null string if title *
*	length exceeds 60 characters. 					    *
*                            		                                    *
****************************************************************************/

char *frame(char *title)
{
 	char *frame,*border,*blank_line,*main_line;
 	int l,i;
 	
 	l=strlen(title);
 	if (l>60){
 		printf("frame procedure: the title to frame is to long\n");
 		return NULL;
 	}
 	else {
 		l+=22; /* size of a whole line :
 		12 ' ', then the frame (4 spaces before and after the title
 		filled as needed by '*' or ' ')then an end-of-line '\n'
 		and the end-of=string '\0' character */
 	/* Memory allocation */
 		border=malloc(l*sizeof(char));
 		blank_line=malloc(l*sizeof(char));
 		main_line=malloc(l*sizeof(char));
 		frame=malloc((l-1)*5*sizeof(char)+1);
 	
 	/* Fill the different lines */
 		strcpy(border,"            ");
 		strncat(border,"*************************************\
******************************",l-14);
		strcat(border,"\n");
 		strcpy(blank_line,"            *");
 		strncat(blank_line,"                                                  \
                              ",l-16);
 		strcat(blank_line,"*\n");
 		strcpy(main_line,"            *   ");
 		strcat(main_line,title);
 		strcat(main_line,"   *\n");
 		
 	/* Join them into frame */	
 		strcpy(frame,border); /* to erase a possible previous content */
 		strcat(frame,blank_line);
 		strcat(frame,main_line);
 		strcat(frame,blank_line);
 		strcat(frame,border);
 		
 	/* Terminate : free the various strings and return frame*/
 		free(border);
 		free(blank_line);
 		free(main_line);
 		return frame;
 	}
}

/****************************************************************************
*                                                                           *
*		Code for arg_version procedure                              *
*                        -----------                                        *
*                                                                           *
*    returns the greta version number, found in RCS header of resources     *
*    file .								    *
*                                                                           *
****************************************************************************/

char *arg_version()
{
 	char no_version[5], *version_lb;

	version_lb=(char *)malloc(20*sizeof(char));
//	sscanf(RcsId,"%*[^,],v %s %*[^$]$",no_version);
//	sprintf(version_lb,"   Version %s",no_version);
	sprintf(version_lb,"   Version %s", PACKAGE_VERSION);
	return version_lb;
}


/****************************************************************************
*									    *
*		code for the devresfile_create() procedure		    *
*			     -------------------			    *
*									    *
*	Profile : void devresfile_create(char *file,char *dev_name,         *
*                                                   char **res_text)	    *
*								            *
*       Rule : create a resource file that can be used as argument for	    *
*	       dbset_update() . It is called by the DevUpdateCb() which     *
*	       passes to it the list of resources of dev_name filling the   *
*	       DevResText of the DeviceForm, and the name of their device.  *
*									    *
****************************************************************************/

long devresfile_create(char *file, char *res_text)
{
 
  FILE     *stream;
  char     *line, s[1001];
  int      length_to_eoln;
  char     revision_text[120];
  char     *rev_nb_pointer;
  size_t   nb_chars;
  char     message_txt[301];


  strcpy(revision_text , "$Revision: 1.3 $");
  rev_nb_pointer = strstr(revision_text, ":");
  rev_nb_pointer++;
  nb_chars = strcspn(rev_nb_pointer, "$");

  strcpy(message_txt, "### File generated by GRETA  Version : ");
  strncat(message_txt, rev_nb_pointer, nb_chars);
  strcat(message_txt, " \n");



  stream=fopen(file, "w");
  if (stream == NULL)
  {   
	return(-1);
  }
  
  fputs("###\n", stream);
  fputs(message_txt, stream);
  fputs("###\n", stream);
  line=res_text;
  while (line!=NULL)
  {
      length_to_eoln = strcspn(line, "\n");
      strncpy(s, line, length_to_eoln);
      s[length_to_eoln] = '\0';
      strcat(s, "\n");
      fputs(s, stream);

      /** take the next line **/
      line=strstr(line, "\n");
      if (line != NULL)
         line = line + 1;
  }

  fclose (stream);
  return(0);

}





/****************************************************************************
*									    *
*		Code for the file_create() procedure			    *
*			     -------------				    *
*									    *
*	Profile : void file_create(char *file,char *res_text)		    *
*									    *
*       Rule : create a resource file that can be used as argument for	    *
*	       dbset_update() . It is called by the ServUpdateCb() which    *
*	       passes to it the lists of devices and resources filling the  *
*	       devstext	and resotext of the ServForm .			    *
*									    *
****************************************************************************/

long 
file_create(char *file, char *res_text)
{
  FILE     *stream;

  char     revision_text[120];
  char     *rev_nb_pointer;
  size_t   nb_chars;
  char     message_txt[301];


  strcpy(revision_text , "$Revision: 1.3 $");
  rev_nb_pointer = strstr(revision_text, ":");
  rev_nb_pointer++;
  nb_chars = strcspn(rev_nb_pointer, "$");

  strcpy(message_txt, "### File generated by GRETA  Version : ");
  strncat(message_txt, rev_nb_pointer, nb_chars);
  strcat(message_txt, " \n");



  stream=fopen(file, "w");
  if (stream == NULL)
  	return(-1);
	
  fputs("###\n", stream);
  fputs(message_txt, stream);
  fputs("###\n", stream);
  fputs(res_text, stream);
  fclose (stream);
  return(0);

}



/****************************************************************************
*									    *
*		code for the loadfile_create() procedure		    *
*			     -------------------			    *
*									    *
*	Profile : void loadfile_create(char *file, char *res_text)	    *
*								            *
*       Rule : create a resource file that contains exactly the same as the *
*              file loaded in the "loaded file window". In particular this  *
*              file has no starting comment "### Generated by GRETA ..."    *
*              and is a temporary file just used for "dbset_update" unix    *
*              command to update the data base by the content of a text     *
*              resource file.                                               *
*              This function is called by LoadFileUpdateCb().               *
*									    *
****************************************************************************/

void loadfile_create(char *file, char *res_text)
{
 
  FILE     *stream;
  char     *line, s[1001];
  int      length_to_eoln;

  stream=fopen(file, "w");
  line=res_text;
  while (line!=NULL)
  {
      length_to_eoln = strcspn(line, "\n");
      strncpy(s, line, length_to_eoln);
      s[length_to_eoln] = '\0';
      strcat(s, "\n");
      fputs(s, stream);

      /** take the next line **/
      line=strstr(line, "\n");
      if (line != NULL)
         line = line + 1;
  }

  fclose (stream);

}


/***************************************************************************
*									   *
* returns NULL if the file is empty, or else returns its content formatted *
* into a non-empty string .						   *
*									   *
****************************************************************************/

char *filetostr(char *file)
{
 	FILE *stream;
 	char *s, line[256];
 
  	stream=fopen(file, "r");
  	/* if file is empty, the first fgets begins on an end of file, and returns
  	   NULL, without touching line . */
  	if(fgets(line, sizeof(line), stream)==NULL)
  		return NULL;
  	else s=(char *)strdup(line);
  	while((fgets(line, sizeof(line), stream)) != NULL)
  	{
  		s=(char *)realloc(s, strlen(s)+strlen(line)+1);
  		strcat(s, line);
  	}
	fclose (stream);
	return s;
}


/****************************************************************************
*                                                                           *
*	Return the number of occurences of c in str			    *
*                                                                           *
****************************************************************************/

static int strchr_occurrence_nb(char *str, char c)
{
 	int i, resu, len;
 
	resu=0;
	len=strlen(str);
	for(i=0;i<len;i++)
		if(str[i]==c) resu++;
	return resu;
}


long file_concat(char *source_file_name,char *dest_file_name)
{
	FILE *source;
	FILE *dest;
	char buffer[512];
	
	if ((source = fopen(source_file_name,"r")) == NULL)
		return(-1);
	if ((dest = fopen(dest_file_name,"a")) == NULL)
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

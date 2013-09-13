/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File           : xdev_dev.c
 *
 * Project        : Xdevmenu application (new generation made by BX)
 *
 * Description    : The interface module between TACO and the rest of the appli
 *
 * Author         : Faranguiss Poncet
 *                  $Author: jkrueger1 $
 *
 * Original       : February 1997
 *
 * Version      : $Revision: 1.4 $
 *
 * Date         : $Date: 2008-04-06 09:08:04 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#if HAVE_TIME_H
#	include <time.h>
#endif

#include <API.h>       /* devserver interface include files */
#include <asc_api.h>   /* Ascii Dev interface to device server API */


#include "xdev_const.h"





typedef struct
         {
           char               *dev_name;
           devserver          dev_id;
           DevVarCmdArray     dev_cmd_table;
	   DevVarStringArray  dev_sig_config;
	   long               cmd_get_sig_config;
	   long               cmd_read_sig_values;
	   long               out_type_read_sig_values;
         } device_entry_structure;

/****  local variables for this module  ****/
static device_entry_structure     *dev_array[XDEV_MAX_DEV];

/** local_out_str statically is 400k bytes : this is a limitation. **/
/** The xdevmenu would do core dump if an execution of a command on a **/
/** device returns a very long array (doubles, strings ...etc.).  **/
/** To be entirely dynamic, this should be modified not only in xdevmenu's **/
/** code but also in the ascii_dev library. In fact the string memory **/
/** should be allocated and reallocated as needed just after the call to **/
/** the dev_putget and this call is in ascii_dev_putget's code           **/
static char                       local_out_str[400001]; /*** 400 k bytes ***/


static char                       local_err_str[5001]; /*** 5k bytes ***/
static char                       local_lapsed_str[501]; /*** 501 chars ***/

int dev_find_cmd(DevVarCmdArray  cmd_table,  char  *cmd_name);



/****  RCS identification keyword understandable by "what" command ***/
static char rcsid[] = "@(#)xdevmenu application $Header: /home/jkrueger1/sources/taco/backup/taco/system/xdevmenu/xdev_dev.c,v 1.4 2008-04-06 09:08:04 jkrueger1 Exp $";


/****************************************************************************
                                                                           
 Function     : long dev_init()

 Description  : This function imports the static data base and initializes
                the data structures.

 Arg(s) In    : none

 Arg(s) Out   : none

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long dev_init()
{
   int            ind_dev;
   unsigned int   stat_imp;
   DevLong        err;
   char           *err_str;


   for ( ind_dev = 0;  ind_dev < XDEV_MAX_DEV; ind_dev++ )
       dev_array[ind_dev] = NULL;

   stat_imp = db_import (&err);

   if (stat_imp != 0)
   {
      err_str = dev_error_str(err);
      printf("Cannot import Static Data Base.\n");
      printf(err_str);
      printf("xdevmenu aborted.\n");
      if (err_str != NULL)
         free(err_str);
      return(XDEV_NOTOK);
   }
   else
      return(XDEV_OK);
}





/****************************************************************************
                                                                           
 Function     : long dev_get_exported_devices()

 Description  : This function returns the list of names of the devices which
                are already exported taking into account the masks for DOMAIN,
                FAMILLY and MEMBER.

 Arg(s) In    : char         *Dmask : the string to filter devices by DOMAIN.
                char         *Fmask : the string to filter devices by FAMILLY.
                char         *Mmask : the string to filter devices by MEMBER.

 Arg(s) Out   : char         ***dev_names : the array of device names.
                unsigned int *nb_dev      : number of elements in dev_names.
                char         **err_str    : error string returned.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long dev_get_exported_devices( char *Dmask, char *Fmask, char *Mmask,
                                            char ***dev_names,
                                           unsigned int *nb_dev, char **err_str)
{
     char    dev_filter[301];
     int     db_get_status, ln_filter;
     DevLong error;


     if ( Dmask==NULL && Fmask==NULL && Mmask==NULL)
     { /*** return all exported devices ***/
#ifdef TANGO
        db_get_status = db_getdevexp_tango( NULL, dev_names, nb_dev, &error);
#else
        db_get_status = db_getdevexp( NULL, dev_names, nb_dev, &error);
#endif 
        if (db_get_status == 0)
           return(XDEV_OK);
        else
        {
           *err_str = dev_error_str(error);
           return(XDEV_NOTOK);
        }
     }

     if ( Dmask == NULL)
     {
        dev_filter[0] = '*';
        dev_filter[1] = '\0';
     }
     else
        strcpy(dev_filter, Dmask);


     ln_filter = strlen(dev_filter);
     if (Fmask == NULL)
     {
        if (dev_filter[ln_filter - 1] != '/')
           strcat(dev_filter, "/");

        strcat(dev_filter, "*");
     }
     else
     {
        if ( (dev_filter[ln_filter - 1] != '/') && (Fmask[0] != '/') )
           strcat(dev_filter, "/");

        strcat(dev_filter, Fmask);
     }


     ln_filter = strlen(dev_filter);
     if (Mmask == NULL)
     {
        if (dev_filter[ln_filter - 1] != '/')
           strcat(dev_filter, "/");

        strcat(dev_filter, "*");
     }
     else
     {
        if ( (dev_filter[ln_filter - 1] != '/') && (Mmask[0] != '/') )
           strcat(dev_filter, "/");

        strcat(dev_filter, Mmask);
     }

#ifdef TANGO
     db_get_status = db_getdevexp_tango( dev_filter, dev_names, nb_dev, &error);
#else
     db_get_status = db_getdevexp( dev_filter, dev_names, nb_dev, &error);
#endif 
     if (db_get_status == 0)
        return(XDEV_OK);
     else
        return(XDEV_NOTOK);
}





/****************************************************************************
                                                                           
 Function     : long dev_free_dev_names()

 Description  : This function frees the memory allocated by db_getdevexp
                function.

 Arg(s) In    : char         ***dev_names : the array of device names to free.

 Arg(s) Out   : char         ***dev_names : the array freed.
                char         **err_str    : error string returned.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long dev_free_dev_names(char ***dev_names, char **err_str)
{

     int    db_free_status;

     
     if (dev_names == NULL)
        return(XDEV_NOTOK);

#ifdef TANGO
     db_free_status = db_freedevexp_tango ( *dev_names );
#else
     db_free_status = db_freedevexp ( *dev_names );
#endif 

     if (db_free_status == 0)
     {
        *dev_names = NULL;
        return(XDEV_OK);
     }
     else
        return(XDEV_NOTOK);

}






/****************************************************************************
                                                                           
 Function     : long dev_find_device()

 Description  : This function looks up the device array to find if the device
                name passed through the input argument has already been
                imported. If this is the case the function returns in the
                output argument the index of the device in the device array.

 Arg(s) In    : char     *dev_name : the name of the device to look for.

 Arg(s) Out   : int      *dev_index : the index of the device if found.

 Return(s)    : XDEV_OK if found the device in the dev_array and
                XDEV_NOTOK if not found.
                                                                           
/***************************************************************************/

long dev_find_device (char *dev_name, int *dev_index)
{
  
   int            local_dev_index;
   unsigned int   found;

   local_dev_index = 0;
   found = FALSE;

   while ( (local_dev_index < XDEV_MAX_DEV) && (found == FALSE) )
   { 
      if (dev_array[local_dev_index] != NULL)
      {
         if (strcmp(dev_array[local_dev_index]->dev_name, dev_name) == 0)
            found = TRUE;
         else
            local_dev_index++;
      }
      else
         local_dev_index++;
   };

   if (found == FALSE)
   {
       *dev_index = -1;
       return(XDEV_NOTOK);
   }
   else
   {
       *dev_index = local_dev_index;
       return(XDEV_OK);
   }

} /*** find_dev_in_table end ***/






/****************************************************************************
                                                                           
 Function     : long dev_insert_device()

 Description  : This function inserts a device which has already been imported
                into the dev_array. If succeeded the function will execute
                the "dev_command_query" for that device and stores the info
                in the array element and returns the index where the device
                info is inserted in the dev_array.

 Arg(s) In    : char       *dev_name : the name of the device to insert.
                devserver  dev_ptr  : the device server handle.

 Arg(s) Out   : int        *dev_index : the index of the device if inserted.

 Return(s)    : XDEV_OK if succeeded to insert the device
                XDEV_NOTOK if not succeeded
                                                                           
/***************************************************************************/
long dev_insert_device (char *dev_name, devserver dev_ptr, int *dev_index)
{
  
    int             my_index;


    my_index = 0;
    while ( (my_index < XDEV_MAX_DEV) && (dev_array[my_index] != NULL) )
          my_index++;

    if ( my_index >= XDEV_MAX_DEV)
    {
       *dev_index = -1;
       return(XDEV_NOTOK);
    }

    dev_array[my_index] = (device_entry_structure *)
                          malloc (sizeof(device_entry_structure));
    dev_array[my_index]->dev_name = (char *)
                                    malloc (strlen(dev_name)+1);
    strcpy(dev_array[my_index]->dev_name, dev_name);
    dev_array[my_index]->dev_id = dev_ptr;
    dev_array[my_index]->dev_cmd_table.length = 0;
    dev_array[my_index]->dev_cmd_table.sequence = NULL;
    dev_array[my_index]->dev_sig_config.length = 0;
    dev_array[my_index]->dev_sig_config.sequence = NULL;
    
    dev_array[my_index]->cmd_get_sig_config = XDEV_SIG_CONF_UNDEF;
    dev_array[my_index]->cmd_read_sig_values = XDEV_SIG_CONF_UNDEF;
    dev_array[my_index]->out_type_read_sig_values = XDEV_SIG_CONF_UNDEF;

    *dev_index = my_index;
    return(XDEV_OK);

}





/****************************************************************************
                                                                           
 Function     : long dev_find_sig_cmds()

 Description  : This function finds in the list of the commands associated
 		with the device the two commands : DevReadSigConfig and
		DevReadSigValues and saves the command codes in the
		structure associated with the device.

 Arg(s) In    : int    dev_ind : the index of the device

 Arg(s) Out   : none

 Return(s)    : none
                                                                           
/***************************************************************************/

void dev_find_sig_cmds(int  dev_ind)
{
   int              find_sig_cmd;


   find_sig_cmd = dev_find_cmd(dev_array[dev_ind]->dev_cmd_table, "DevGetSigConfig");
   
   if (find_sig_cmd == -1)
      return;
   
   dev_array[dev_ind]->cmd_get_sig_config = (dev_array[dev_ind]->dev_cmd_table.sequence)[find_sig_cmd].cmd; 
   
   
   find_sig_cmd = dev_find_cmd(dev_array[dev_ind]->dev_cmd_table, "DevReadSigValues"); 
   
   if (find_sig_cmd == -1)
      return;
   
   dev_array[dev_ind]->cmd_read_sig_values = (dev_array[dev_ind]->dev_cmd_table.sequence)[find_sig_cmd].cmd;
   dev_array[dev_ind]->out_type_read_sig_values = (dev_array[dev_ind]->dev_cmd_table.sequence)[find_sig_cmd].out_type;


   if (   (dev_array[dev_ind]->out_type_read_sig_values != D_VAR_FLOATARR)
       && (dev_array[dev_ind]->out_type_read_sig_values != D_VAR_DOUBLEARR) )
   {
      dev_array[dev_ind]->cmd_get_sig_config = XDEV_SIG_CONF_UNDEF;
      dev_array[dev_ind]->cmd_read_sig_values = XDEV_SIG_CONF_UNDEF;
      dev_array[dev_ind]->out_type_read_sig_values = XDEV_SIG_CONF_UNDEF;
   }
}







/****************************************************************************
                                                                           
 Function     : long dev_import_device()

 Description  : This function imports the device whose name is passed through
                the input argument if the device is not already imported.
                If the import is successful the device structure is created
                and inserted in the dev_array and the index is returned.

 Arg(s) In    : char     *dev_name : the name of the device to import.

 Arg(s) Out   : int      *dev_index : the index of the device if imported.
                char     **err_str : the error string if not successful.

 Return(s)    : XDEV_OK if the device has been imported and inserted
                XDEV_NOTOK if not imported
                                                                           
/***************************************************************************/

long dev_import_device(char *dev_name, int *dev_index, char **err_str)
{

   long               status; 
   DevLong	      error;
   devserver          dev_ptr;     
   DevVarCmdArray     cmd_table;
   DevVarStringArray  sig_conf;
   char               *str_err;

   status = dev_find_device (dev_name, dev_index);

   if (status == XDEV_OK)
   {
      *err_str = (char *) malloc(501);
      strcpy(*err_str, "This device has already been imported.\n\n");
      strcat(*err_str, "Please select the corresponding icon in the main");
      strcat(*err_str, " window to use it.\n\n");
      return(XDEV_NOTOK);
   }

   status = dev_import(dev_name, 0, &dev_ptr, &error);

   if (status != DS_OK)
   {
      *err_str = dev_error_str(error);
      *dev_index = -1;
      return(XDEV_NOTOK);
   }

   status = dev_insert_device (dev_name, dev_ptr, dev_index);

   if (status == XDEV_NOTOK)
   {
      *err_str = (char *) malloc(501);
      strcpy(*err_str, "Too many devices are already imported.\n\n");
      strcat(*err_str, "Please close some of them and retry.\n\n");
      strcat(*err_str, "Device not imported.\n\n");

      status = dev_free(dev_ptr, &error);
      if (status != DS_OK)
      {
         strcat(*err_str, "Error occured when tried to free the device server");
         strcat(*err_str, " handle.\n");
      };
      return(XDEV_NOTOK);
   }

   cmd_table.length = 0;
   cmd_table.sequence = NULL;
   status = dev_cmd_query (dev_ptr, &cmd_table, &error);

   if (status != DS_OK)
   {
      str_err = dev_error_str(error);
      *err_str = (char *) malloc(501 + strlen(str_err));
      strcpy(*err_str, "Cannot get the list of the available commands.\n");
      strcat(*err_str, "The call to dev_cmd_query failed.\n\n");
      strcat(*err_str, str_err);
      if (str_err != NULL) free(str_err);
      strcat(*err_str, "\n\nDevice not imported.\n");

      status = dev_free(dev_ptr, &error);
      if (status != DS_OK)
      {
         strcat(*err_str, "Error occured when tried to free the device server");
         strcat(*err_str, " handle.\n");
      };

      free(dev_array[*dev_index]->dev_name);
      dev_array[*dev_index]->dev_name = NULL;
      free (dev_array[*dev_index]);
      dev_array[*dev_index] = NULL;
      *dev_index = -1;
      return(XDEV_NOTOK);
   }

   dev_array[*dev_index]->dev_cmd_table.length = cmd_table.length;
   dev_array[*dev_index]->dev_cmd_table.sequence = cmd_table.sequence;
   
   dev_find_sig_cmds(*dev_index);
   
   if (    (dev_array[*dev_index]->cmd_get_sig_config == XDEV_SIG_CONF_UNDEF)
        || (dev_array[*dev_index]->cmd_read_sig_values == XDEV_SIG_CONF_UNDEF)
	|| (dev_array[*dev_index]->out_type_read_sig_values == XDEV_SIG_CONF_UNDEF) )
   {
       dev_array[*dev_index]->cmd_get_sig_config = XDEV_SIG_CONF_UNDEF;
       dev_array[*dev_index]->cmd_read_sig_values = XDEV_SIG_CONF_UNDEF;
       dev_array[*dev_index]->out_type_read_sig_values = XDEV_SIG_CONF_UNDEF;
       return(XDEV_OK);
   }
   
   sig_conf.length = 0;
   sig_conf.sequence = NULL;
   status = dev_putget (dev_ptr, dev_array[*dev_index]->cmd_get_sig_config,
                                 NULL, D_VOID_TYPE,
				 &sig_conf, D_VAR_STRINGARR, &error);

   if (status != DS_OK)
   {
       dev_array[*dev_index]->cmd_get_sig_config = XDEV_SIG_CONF_UNDEF;
       dev_array[*dev_index]->cmd_read_sig_values = XDEV_SIG_CONF_UNDEF;
       dev_array[*dev_index]->out_type_read_sig_values = XDEV_SIG_CONF_UNDEF;
       return(XDEV_OK);
   }
   
   dev_array[*dev_index]->dev_sig_config.length = sig_conf.length;
   dev_array[*dev_index]->dev_sig_config.sequence = sig_conf.sequence;

   return (XDEV_OK);

}





/****************************************************************************
                                                                           
 Function     : long dev_get_cmd_names()

 Description  : This function returns the list of the command names attached
                to a device whose index in the array is passed through the
                input argument.

 Arg(s) In    : int    ind_dev : the index of the device in the array.

 Arg(s) Out   : char   ***cmd_names : the array of command names.
                int    *nb_cmds     : the number of command names.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long dev_get_cmd_names( int ind_dev, char ***cmd_names, int *nb_cmds)
{
    int          cmd_num, ind_cmd;
    DevCmdInfo   *cmd_tab;
    char         **cmd_name_arr;
   

    if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV) )
       return(XDEV_NOTOK);

    if (dev_array[ind_dev] == NULL)
       return(XDEV_NOTOK);

    cmd_num = dev_array[ind_dev]->dev_cmd_table.length;
    cmd_tab = dev_array[ind_dev]->dev_cmd_table.sequence;
    cmd_name_arr = (char **) malloc(cmd_num * sizeof(char *));

    for (ind_cmd = 0; ind_cmd < cmd_num; ind_cmd++)
    {
       cmd_name_arr[ind_cmd] = (char *)
                               malloc( strlen(cmd_tab[ind_cmd].cmd_name) + 1);
       strcpy(cmd_name_arr[ind_cmd], cmd_tab[ind_cmd].cmd_name);
    };

    *nb_cmds = cmd_num;
    *cmd_names = cmd_name_arr;
}







/****************************************************************************
                                                                           
 Function     : long dev_free_cmd_names()

 Description  : This function frees the memory allocated by dev_get_cmd_names
                function.

 Arg(s) In    : char         ***cmd_names : the array of command names to free.
                int          cmd_num   : number of elements in this array.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/


long dev_free_cmd_names(char ***cmd_names, int cmd_num)
{
   int    ind_cmd;
   char   **cmd_arr;

   cmd_arr = *cmd_names;
   for (ind_cmd=0; ind_cmd < cmd_num; ind_cmd++)
       free( cmd_arr[ind_cmd] );

   free(cmd_arr);
   *cmd_names = NULL;

   return(XDEV_OK);

}







/****************************************************************************
                                                                           
 Function     : long dev_get_cmd_profile()

 Description  : This function returns the profile of a device command : its
                argument in and out types and its argument in and out names.

 Arg(s) In    : int    ind_dev : the index of the device in the array.
                int    ind_cmd : the index of the command in the cmd_array.

 Arg(s) Out   : char   **cmd_name   : command name.
                char   **in_name    : input parameter name.
                char   **out_name   : output parameter name.
                long   *in_type     : the type of the input parameter.
                long   *out_type    : the type of the output parameter.
                char   **error_str  : the error string if function fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long dev_get_cmd_profile(int ind_dev,int ind_cmd, char   **cmd_name,
                                                  char   **in_name,
                                                  char   **out_name,
                                                  long   *in_type,
                                                  long   *out_type,
                                                  char   **error_str )
{

   DevCmdInfo    cmd_info;


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }

   if ((ind_cmd < 0) || (ind_cmd >= (dev_array[ind_dev]->dev_cmd_table.length)))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Command index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   cmd_info = (dev_array[ind_dev]->dev_cmd_table.sequence)[ind_cmd];

   if (cmd_info.cmd_name == NULL)
      (*cmd_name)[0] = '\0';
   else
      strcpy(*cmd_name, cmd_info.cmd_name);


   if (cmd_info.in_name == NULL)
      (*in_name)[0] = '\0';
   else
      strcpy(*in_name, cmd_info.in_name);

   if (cmd_info.out_name == NULL)
      (*out_name)[0] = '\0';
   else
      strcpy(*out_name, cmd_info.out_name);

   *in_type = cmd_info.in_type;
   *out_type = cmd_info.out_type;
   
}







/****************************************************************************
                                                                           
 Function     : long dev_read_sig_values()

 Description  : This function executes "DevReadSigValues" on the device.

 Arg(s) In    : int    ind_dev : the index of the device in the array.


 Arg(s) Out   : DevVarStringArray   *sig_conf : signal config for this device
                DevVarFloatArray    *sig_vals_f : the result of DevReadSigValues
		                                command if floatarray
                DevVarDoubleArray   *sig_vals_d : the result of DevReadSigValues
		                                command if doublearray
                char   **lapsed_time  : time to execute ds command in milisec.
                char   **error_str    : the error string if command fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.

 NOTICE       : All the Arg(s) out excepted from "sigs_vals" are local static
                variables. The caller should not free them after the call.
		Of course, the 4th parameter sig_vals should be freed
		by the caller through the call to the function :
		"dev_free_sig_struct'.

/***************************************************************************/

long dev_read_sig_values(int  ind_dev, DevVarStringArray   *sig_conf,
				       DevVarFloatArray    *sig_vals_f,
				       DevVarDoubleArray   *sig_vals_d,
                                       char  **lapsed_time,
                                       char  **error_str)
{

   long                 status; 
   DevLong		error;
   int                  ind_end;
   struct timeval       first, second, lapsed;
   struct timezone      tzp;
   double               nb_milli_sec;
   char                 *strerr;


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }

   local_lapsed_str[0] = '\0';
   local_err_str[0] = '\0';


   gettimeofday (&first, &tzp);

   if (dev_array[ind_dev]->out_type_read_sig_values == D_VAR_FLOATARR)
      status = dev_putget( dev_array[ind_dev]->dev_id,
                           dev_array[ind_dev]->cmd_read_sig_values,
		           NULL, D_VOID_TYPE,
		           sig_vals_f, D_VAR_FLOATARR, &error);

   if (dev_array[ind_dev]->out_type_read_sig_values == D_VAR_DOUBLEARR)
      status = dev_putget( dev_array[ind_dev]->dev_id,
                           dev_array[ind_dev]->cmd_read_sig_values,
		           NULL, D_VOID_TYPE,
		           sig_vals_d, D_VAR_DOUBLEARR, &error);
   
   gettimeofday (&second, &tzp);
   
   if (first.tv_usec > second.tv_usec)
   {
      second.tv_usec += 1000000;
      second.tv_sec--;
   }
   lapsed.tv_usec = second.tv_usec - first.tv_usec;
   lapsed.tv_sec = second.tv_sec - first.tv_sec;

   nb_milli_sec = lapsed.tv_usec / 1000;
   nb_milli_sec = nb_milli_sec + (lapsed.tv_sec * 1000);

   sprintf(local_lapsed_str, "( %.3g ms )", nb_milli_sec);
   *lapsed_time = local_lapsed_str;

   if (status != 0)
   {
      strerr = dev_error_str(error);
      strcpy(local_err_str, strerr);
      if (strerr != NULL) free(strerr);
      
      ind_end = strlen(local_err_str);
      local_err_str[ind_end] = '\n';
      local_err_str[ind_end + 1] = '\0';
      *error_str = local_err_str;
      return(XDEV_NOTOK);
   }

   sig_conf->length = dev_array[ind_dev]->dev_sig_config.length;
   sig_conf->sequence = dev_array[ind_dev]->dev_sig_config.sequence;
   *error_str = NULL;
   return(XDEV_OK);
}







/****************************************************************************
                                                                           
 Function     : long dev_read_sig_values()

 Description  : This function frees a structure allocated by the call to 
                dev_read_sig_values.

 Arg(s) In    : long  data_type : the type of the data to be freed
                                  should be either D_VAR_FLOATARR or D_VAR_DOUBLEARR
                DevVarFloatArray  *sig_vals_f : the stucture to free if data_type=floatarray
                DevVarDoubleArray *sig_vals_d : the stucture to free if data_type=doublearray



 Arg(s) Out   : None

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.

/***************************************************************************/

long dev_free_sig_struct(long data_type, 
                              DevVarFloatArray  *sig_vals_f,
			      DevVarDoubleArray *sig_vals_d )
{

    long    status; 
    DevLong error;
    
    
    if (data_type == D_VAR_FLOATARR)
    {
       if ( (sig_vals_f->length <= 0) || (sig_vals_f->sequence == NULL) )
          return (XDEV_NOTOK);
       status = dev_xdrfree(D_VAR_FLOATARR, sig_vals_f, &error);
    }

    
    if (data_type == D_VAR_DOUBLEARR)
    {
       if ( (sig_vals_d->length <= 0) || (sig_vals_d->sequence == NULL) )
          return (XDEV_NOTOK);
       status = dev_xdrfree(D_VAR_DOUBLEARR, sig_vals_d, &error);
    }


    if (status == DS_OK)
       return(XDEV_OK);
    else
       return(XDEV_NOTOK);
}






/****************************************************************************
                                                                           
 Function     : long dev_exec_cmd()

 Description  : This function executes a command on the device.

 Arg(s) In    : int    ind_dev : the index of the device in the array.
                int    ind_cmd : the index of the command in the cmd_array.
                char   *in_arg : the input argument of the device command.

 Arg(s) Out   : char   **out_arg      : output of the command.
                char   **lapsed_time  : time to execute ds command in milisec.
                char   **error_str    : the error string if command fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.

 NOTICE       : The Arg(s) Out are local static variables. The caller should
                not free them after the call.

/***************************************************************************/

long dev_exec_cmd(int  ind_dev, int  ind_cmd, char  *in_arg,
                                              char  **out_arg,
                                              char  **lapsed_time,
                                              char  **error_str)
{

   DevCmdInfo           cmd_info;
   long                 status;
   int                  ind_end;
   struct timeval       first, second, lapsed;
   struct timezone      tzp;
   double               nb_milli_sec;
   char                 *temp_out_str, *temp_err_str;


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }

   if ((ind_cmd < 0) || (ind_cmd >= (dev_array[ind_dev]->dev_cmd_table.length)))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Command index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   cmd_info = (dev_array[ind_dev]->dev_cmd_table.sequence)[ind_cmd];


   local_out_str[0] = '\0';
   local_lapsed_str[0] = '\0';
   local_err_str[0] = '\0';

   temp_out_str = (char *) local_out_str;
   temp_err_str = (char *) local_err_str;


   gettimeofday (&first, &tzp);

   if (cmd_info.in_type == D_VOID_TYPE)
      status = dev_get_ascii( dev_array[ind_dev]->dev_name, cmd_info.cmd_name,
                                       &temp_out_str, &temp_err_str);
   else
      status = dev_putget_ascii( dev_array[ind_dev]->dev_name,
                                 cmd_info.cmd_name, in_arg,
                                 &temp_out_str, &temp_err_str);

   gettimeofday (&second, &tzp);
   if (first.tv_usec > second.tv_usec)
   {
      second.tv_usec += 1000000;
      second.tv_sec--;
   }
   lapsed.tv_usec = second.tv_usec - first.tv_usec;
   lapsed.tv_sec = second.tv_sec - first.tv_sec;

   nb_milli_sec = lapsed.tv_usec / 1000;
   nb_milli_sec = nb_milli_sec + (lapsed.tv_sec * 1000);

   sprintf(local_lapsed_str, "( %.3g ms )", nb_milli_sec);
   *lapsed_time = local_lapsed_str;

   if (status != 0)
   {
      *out_arg = NULL;
      ind_end = strlen(local_err_str);
      local_err_str[ind_end] = '\n';
      local_err_str[ind_end + 1] = '\0';
      *error_str = local_err_str;
      return(XDEV_NOTOK);
   }
   else
   {
      *error_str = NULL;
      *out_arg = local_out_str;
      return(XDEV_OK);
   }
}




/****************************************************************************
                                                                           
 Function     : int dev_sig_interface_exist()

 Description  : This function returns 0 if a signal interface exists for 
                the device. It returns -1 othewise.

 Arg(s) In    : int   dev_ind : the device index

 Arg(s) Out   : none.

 Return(s)    : 0 if yes
               -1 if no
                                                                           
/***************************************************************************/

int dev_sig_interface_exist ( int dev_ind)
{  
    if (    (dev_array[dev_ind]->cmd_get_sig_config != XDEV_SIG_CONF_UNDEF)
         && (dev_array[dev_ind]->cmd_read_sig_values != XDEV_SIG_CONF_UNDEF)
	 && (dev_array[dev_ind]->out_type_read_sig_values != XDEV_SIG_CONF_UNDEF)
	 && (dev_array[dev_ind]->dev_sig_config.length > 0 )
	 && (dev_array[dev_ind]->dev_sig_config.sequence != NULL) )
	 return(0);
    else
         return(-1);
}





/****************************************************************************
                                                                           
 Function     : int dev_find_cmd()

 Description  : This function returns the index of a device command whose
                name is passed into this function.

 Arg(s) In    : DevVarCmdArray   cmd_table : the cmd array to look
                char             *cmd_name : the name of the command.

 Arg(s) Out   : none.

 Return(s)    : the index of the command in the comd array if found
                -1  if not found
                                                                           
/***************************************************************************/

int dev_find_cmd ( DevVarCmdArray  cmd_table,  char  *cmd_name)
{

    int   cmd_found;
    int   cmd_ind;

    
    cmd_found = FALSE;
    cmd_ind = 0;

    while ( (cmd_ind < cmd_table.length) && (cmd_found == FALSE) )
    {
       if ( strcmp(cmd_name, (cmd_table.sequence[cmd_ind]).cmd_name) == 0)
          cmd_found = TRUE;
       else
          cmd_ind++;
    };

    if (cmd_found == TRUE)
       return(cmd_ind);
    else
       return(-1);
}








/****************************************************************************
                                                                           
 Function     : long dev_get_cmd_name_profile()

 Description  : This function returns the profile of a device command whose
                name is passed into this function : its argument in and out
                types and its argument in and out names.

 Arg(s) In    : int    ind_dev : the index of the device in the array.
                char   *cmd_name : the name of the command.n the cmd_array.

 Arg(s) Out   : int    *ind_cmd     : the index of the command in the array.
                char   **in_name    : input parameter name.
                char   **out_name   : output parameter name.
                long   *in_type     : the type of the input parameter.
                long   *out_type    : the type of the output parameter.
                char   **error_str  : the error string if function fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long dev_get_cmd_name_profile( int ind_dev,  char  *cmd_name,
                                             int   *ind_cmd,
                                             char  **in_name,
                                             char  **out_name,
                                             long  *in_type,
                                             long  *out_type,
                                             char  **error_str )
{

   int           cmd_index;
   DevCmdInfo    cmd_info;


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }

   cmd_index = dev_find_cmd( dev_array[ind_dev]->dev_cmd_table, cmd_name);

   if ((cmd_index < 0) || (cmd_index >= (dev_array[ind_dev]->dev_cmd_table.length)))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find this command in the command list.\n");
      return(XDEV_NOTOK);
   }

   cmd_info = (dev_array[ind_dev]->dev_cmd_table.sequence)[cmd_index];
   *ind_cmd = cmd_index;


   if (cmd_info.in_name == NULL)
      (*in_name)[0] = '\0';
   else
      strcpy(*in_name, cmd_info.in_name);

   if (cmd_info.out_name == NULL)
      (*out_name)[0] = '\0';
   else
      strcpy(*out_name, cmd_info.out_name);

   *in_type = cmd_info.in_type;
   *out_type = cmd_info.out_type;
   
}








/****************************************************************************
                                                                           
 Function     : long dev_info_device()

 Description  : This function returns the information on a device whose
                index in the device array is passed into this function.

 Arg(s) In    : int    ind_dev : the index of the device in the array.

 Arg(s) Out   :
                char   **dev_name    : the name of the device.
                char   **dev_class   : the class of the device.
                char   **dev_type    : the type of the device.
                char   **serv_name   : the server name for this device.
                char   **serv_host   : the host where the server is running.
                char   **error_str   : the error string if function fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long dev_info_device( int  ind_dev, char **dev_name, char **dev_class,
                                    char **dev_type,
                                    char **serv_name, char **serv_host,
                                    char **error_str)
{

   devserver           ds_id;
   long                status;
   DevInfo             *dev_info_ptr, dev_info_array[1];
   DevLong             err_code;



   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }


   ds_id = dev_array[ind_dev]->dev_id;
   dev_info_ptr = (DevInfo *) dev_info_array;
   status = dev_inform ( &ds_id, 1, &dev_info_ptr, &err_code);

   if (status != DS_OK)
   {
      *error_str = dev_error_str(err_code);
      return(XDEV_NOTOK);
   }




   if (dev_info_array[0].device_name == NULL)
      *dev_name[0] = '\0';
   else
      strcpy(*dev_name, dev_info_array[0].device_name);


   if (dev_info_array[0].device_class == NULL)
      *dev_class[0] = '\0';
   else
      strcpy(*dev_class, dev_info_array[0].device_class);


   if (dev_info_array[0].device_type == NULL)
      *dev_type[0] = '\0';
   else
      strcpy(*dev_type, dev_info_array[0].device_type);


   if (dev_info_array[0].server_name == NULL)
      *serv_name[0] = '\0';
   else
      strcpy(*serv_name, dev_info_array[0].server_name);


   if (dev_info_array[0].server_host == NULL)
      *serv_host[0] = '\0';
   else
      strcpy(*serv_host, dev_info_array[0].server_host);

   return(XDEV_OK);
}







/****************************************************************************
                                                                           
 Function     : long dev_close_device()

 Description  : This function closes the connection to the device.

 Arg(s) In    : int    ind_dev : the index of the device in the array.

 Arg(s) Out   : char   **error_str    : the error string if command fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.

 NOTICE       : The Arg(s) Out are allocated in this function and it is the
                responsibility of the caller to free them.

/***************************************************************************/


long dev_close_device( int  ind_dev, char   **error_str)
{

   devserver            ds;
   long                 status, ret_val;
   DevLong		err;
   char                 *temp_err_str;
   


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }

   ds = dev_array[ind_dev]->dev_id;
   status = dev_free(ds, &err);

   ret_val = XDEV_OK;
   if (status != DS_OK)
   {
      *error_str = dev_error_str(err);
      ret_val = XDEV_NOTOK;
   }

   local_err_str[0] = '\0';
   temp_err_str = (char *) local_err_str;
   status = dev_free_ascii( dev_array[ind_dev]->dev_name, &temp_err_str);
   if (status != 0)
   {
      if (ret_val == XDEV_OK)
      {
         *error_str = strdup(temp_err_str);
         ret_val = XDEV_NOTOK;
      }
   }

   free(dev_array[ind_dev]->dev_cmd_table.sequence);
   dev_array[ind_dev]->dev_cmd_table.length = 0;
   dev_array[ind_dev]->dev_cmd_table.sequence = NULL;

   free(dev_array[ind_dev]->dev_name);
   dev_array[ind_dev]->dev_name = NULL;


   free(dev_array[ind_dev]);
   dev_array[ind_dev]=NULL;

   return(ret_val);
}







/****************************************************************************
                                                                           
 Function     : long dev_get_timeout()

 Description  : This function returns the rpc timeout for the selected device

 Arg(s) In    : int    ind_dev  : the index of the device in the array.

 Arg(s) Out   : long   *time_out : timeout in milliseconds.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.

/***************************************************************************/

long dev_get_timeout( int  ind_dev, long  *time_out)
{

   devserver            ds;
   DevLong              err;
   long			status;
   struct timeval       tval;
   long                 time_ms;
   


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
      return(XDEV_NOTOK);

   if ( dev_array[ind_dev] == NULL )
      return(XDEV_NOTOK);

   ds = dev_array[ind_dev]->dev_id;

   tval.tv_sec = (unsigned long) 0;
   tval.tv_usec = (long) 0;

   status = dev_rpc_timeout(ds, CLGET_TIMEOUT, &tval, &err);

   if (status != DS_OK)
      return(XDEV_NOTOK);
 
   time_ms = (long) (tval.tv_usec / 1000);
   time_ms = time_ms + (tval.tv_sec * 1000);

   *time_out = time_ms;
   return(XDEV_OK);

}








/****************************************************************************
                                                                           
 Function     : long dev_change_timeout()

 Description  : This function changes the rpc timeout for the selected device

 Arg(s) In    : int    ind_dev  : the index of the device in the array.
                long   time_val : the value of the rpc timeout in millisec.

 Arg(s) Out   : char   **error_str    : the error string if command fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.

 NOTICE       : The error_str is allocated in this function and it is the
                responsibility of the caller to free it.

/***************************************************************************/

long dev_change_timeout( int  ind_dev, long time_val, char   **error_str)
{

   devserver            ds;
   DevLong              err; 
   long			status;
   struct timeval       tval;
   


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }

   ds = dev_array[ind_dev]->dev_id;

   tval.tv_sec = (unsigned long) (time_val / 1000);
   tval.tv_usec = (long) ( (time_val - (tval.tv_sec * 1000)) * 1000 );

   status = dev_rpc_timeout(ds, CLSET_TIMEOUT, &tval, &err);

   if (status != DS_OK)
   {
      *error_str = dev_error_str(err);
      return(XDEV_NOTOK);
   }

   return(XDEV_OK);
}








/****************************************************************************
                                                                           
 Function     : long dev_change_rpc_protocol()

 Description  : This function changes the rpc protocol for the selected device

 Arg(s) In    : int    ind_dev  : the index of the device in the array.
                char   *rpc_pro : the rpc protocol to set ("TCP" or "UDP")

 Arg(s) Out   : char   **error_str    : the error string if command fails.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.

 NOTICE       : The error_str is allocated in this function and it is the
                responsibility of the caller to free it.

/***************************************************************************/

long dev_change_rpc_protocol( int  ind_dev, char *rpc_pro, char   **error_str)
{

   devserver            ds;
   long                 val_protocol, status;
   DevLong		err;
   struct timeval       tval;
   


   if ( (ind_dev < 0) || (ind_dev >= XDEV_MAX_DEV))
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Device index out of bounds.\n");
      return(XDEV_NOTOK);
   }

   if ( dev_array[ind_dev] == NULL )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "Cannot find any device at this index.\n");
      return(XDEV_NOTOK);
   }

   ds = dev_array[ind_dev]->dev_id;

   if ( (strcmp(rpc_pro, "TCP") != 0) && (strcmp(rpc_pro, "UDP") != 0) )
   {
      *error_str = (char *) malloc(101);
      strcpy(*error_str, "The RPC Protocol selected is not valid.\n");
      return(XDEV_NOTOK);
   }


   if ( strcmp(rpc_pro, "TCP") == 0 )
      val_protocol = D_TCP;
 
   if ( strcmp(rpc_pro, "UDP") == 0 )
      val_protocol = D_UDP;

   status = dev_rpc_protocol (ds, val_protocol, &err);

   if (status != DS_OK)
   {
      *error_str = dev_error_str(err);
      return(XDEV_NOTOK);
   }

   return(XDEV_OK);
}



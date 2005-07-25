/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * File:        MDSSignal_config.c
 *
 * Project:     Multi Signal Class
 *
 * Description: Device Server Class which is a top Layer of DSSignal
 *              Class to handle multiple signals. Module to extract the
 *		signal configuration of a device directly from the
 *		resource database.
 *
 * Author(s):   Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    4.12.1997
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 12:59:10 $
 *
 *********************************************************************/

#include <API.h>
#include <DevErrors.h>
#include <Admin.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <DSSignalP.h>
#include <MDSSignal.h>

static DevVarStringArray signal_names={0,NULL};
static DevVarStringArray prop_suff={0,NULL};

static char **class_res=NULL;
static long   class_res_nb=0;
static long  *class_res_id=NULL;

static char **device_res=NULL;
static long   device_res_nb=0;
static long  *device_res_id=NULL;

static char **arg_out=NULL;
static long   arg_out_nb=0;

static  short 	db_flag=False;
static	char    class_res_name[128];

#ifdef _OS9
/**********************************/
/* Definition of strdup() for OS9 */
/**********************************/
char *strdup(char *s) {
  char *f;
  f=(char *)malloc(strlen(s)+1);
  strcpy(f,s);
  return f;
}
#endif

/*===========================================================================
 Function : void FreeData()

 Descritpion: Free data allocated by the module
 ============================================================================*/
void FreeData()
{
  int i;

  /* Free resources */

  if( class_res_nb > 0 )
  {
    for( i=0 ; i<class_res_nb ; i++ )
      if( class_res[i]!=NULL )
      {
	free( class_res[i] );
	class_res[i]=NULL;
      }
    free(class_res);
    class_res=NULL;
    class_res_nb=0;
  }

  if( device_res_nb > 0 )
  {
    for( i=0 ; i<device_res_nb ; i++ )
      if( device_res[i]!=NULL )
      {
	free( device_res[i] );
	device_res[i]=NULL;
      }
    free(device_res);
    device_res=NULL;
    device_res_nb=0;
  }


  /* Free other data */

  if( signal_names.length != 0 ) 
  {
     free_var_str_array (&signal_names);
     signal_names.length=0;
  }

  if( class_res_id != NULL )
  {
    free( class_res_id );
    class_res_id = NULL;
  }

  if( device_res_id != NULL )
  {
    free( class_res_id );
    class_res_id = NULL;
  }
}

/*===========================================================================
 Function : void  Extract()

 Descritpion: Extract the 3 fields of a device name
 ============================================================================*/
void  Extract(char *dev_name , 
              char *nethost,char *domain,char *family,char *member )
{
  char *start,*end;
  char tmp[128];

  strcpy(tmp,dev_name);
  start=tmp;

  if (strncasecmp(dev_name,"//",2)==0)
  {
    start+=2;
    end=start;
    while(*end!='/') end++;
    *end='\0';
    strcpy(nethost,start);
    end++;
    start=end;
  }
  end=start;
  while( *end!='/' ) end++;
  *end='\0';
  end++;
  strcpy(domain,start);
  start=end;
  while( *end!='/' ) end++;
  *end='\0';
  end++;
  strcpy(family,start);
  strcpy(member,end);
}


/*===========================================================================
 Function : char *Parse( res , res_nb , res_name );

 Descritpion: Search the resource res into the input resource array
 ============================================================================*/
char *Parse(char **res,long res_nb,char *res_name ) {

static char ret[512];

  int i,len,r,j,k;
  int found=1;
  char c;

  len=strlen(res_name);

  i=0;
  ret[0]='\0';

  while( (i<res_nb) && (found>0) )
  {
    found=strncasecmp( res_name , res[i] , len );
    if( found>0 ) i++;
  }

  if( found==0 ) {
   
    j=len+2;
    k=0;

    while( (c=res[i][j])!='\0' ) {
      if( c!='\"' ) {
	if( c==',' )
	  ret[k++]=' ';
	else
	  ret[k++]=c;
      }
      j++;
    }

    ret[k]='\0';
    return ret;

  } else {
    return NULL;
  }
    
}


/*===========================================================================
 Function : void  ReFormat()

 Descritpion: Reformat data returned by db_getresoval()
 ============================================================================*/
void ReFormat(char *dev_name,char **arr,long res_nb )
{
  int i,len;
  char *f;
  char *p;

  len=strlen(dev_name)+1;

  for(i=0;i<res_nb;i++)
  {
    if( arr[i]!=NULL ) {
      f=arr[i];
      p=strdup( &(f[len]) );
      arr[i]=p;
      while(*p!=':' && *p!='\0') p++;
      if(*p==':') *p=' ';
      free(f);
    }
  }
}

#ifdef EBUG

static double init_time;

void InitT() {
  double t;
  struct timeval tp;
  struct timezone tz;

  gettimeofday(&tp,&tz);

  init_time=tp.tv_sec+tp.tv_usec/1e6;
}

void NowT() {
  double t;
  struct timeval tp;
  struct timezone tz;

  gettimeofday(&tp,&tz);

  t=tp.tv_sec+tp.tv_usec/1e6;

  fprintf(stderr,"[%6.3f sec]\n",t-init_time);
  init_time=t;
}

#endif


/*============================================================================
 Function:     	long dev_get_sig_list()

 Description:   Return the signal list for a device

 Arg(s) In:     char *device_name	  - Name of the device

 Arg(s) Out:    DevVarStringArray *argout - Array containing the 
					    signal list for the
					    device.
                 long           *error    - pointer to error code, in case
                                            routine fails.
 ============================================================================*/
long dev_get_sig_list (char *device_name, DevVarStringArray *argout, 
		       long *error)
{
        db_resource res_siglist[] = {
   	  {"signal_names",	D_VAR_STRINGARR, NULL},
   	};
        
	char             tmp[128];
	char             nethost[128];
	char             family[128];
	char             member[128];
	char             domain[128];
	db_devinf_imp    *tab=NULL;
	db_resource      dummy_res[1];
	DevString        dummy_str;
	char             dummy_name[128];

	*error = 0;

	/*******************************************************/
	/* Make connection to data base serveur                */
	/*******************************************************/

	if ( db_flag == False )
	{
	   /* Get a dummy resource to make the connection the
	      the correct data base serveur */

	   dummy_str=NULL;
	   strcpy(nethost,"");
	   dummy_res[0].resource_name="dummy";
	   dummy_res[0].resource_type=D_STRING_TYPE;
	   dummy_res[0].resource_adr=&(dummy_str);
	   Extract(device_name,nethost,domain,family,member);
           if(strlen(nethost)>0) sprintf(dummy_name,"//%s/sys/dummy/dummy",nethost);
	   else                  sprintf(dummy_name,"sys/dummy/dummy");

	   if ( db_getresource(dummy_name,dummy_res,1,error) == DS_NOTOK )
	   {
              return (DS_NOTOK);
           }

	   db_flag = True;
	}

	/*******************************************************/
	/* read class name with dv_dev_import()                */
	/*******************************************************/

	if ( db_dev_import (&device_name, &tab, 1, error) == DS_NOTOK )
           {
           return (DS_NOTOK);
           }

	/*******************************************************/
	/* Get the signal list                                 */
	/*******************************************************/

	argout->length=0;
	argout->sequence=NULL;

        res_siglist[0].resource_adr = argout;

	if( strncmp( device_name , "//" , 2 )==0 ) {
	  strcpy(nethost,&device_name[2]);
	  *(strchr(nethost,'/'))='\0';
	  sprintf (class_res_name, "//%s/CLASS/%s/DEFAULT",
	  nethost,tab->device_class);
	} else {
          sprintf (class_res_name, "CLASS/%s/DEFAULT", tab->device_class);
        }

        if ( db_getresource (class_res_name, res_siglist,1,error) < 0 ) {
	   printf("\nFailed to read the signal list\n");
	   free(tab);
   	   return(DS_NOTOK);
	}
	
	/* if no list is specified for the class ,Try to get it from 
	   device resources */
        
        if( argout->length==0 ) {

	  if ( db_getresource (device_name, res_siglist,1,error) < 0 )
	  {
	     printf("\nFailed to read the signal list\n");
	     free(tab);
   	     return(DS_NOTOK);
	  }

	}

   	if ( argout->length==0 )
  	{
	  #ifdef EBUG
	    fprintf(stderr,
		    "No list of signal names is specified for the class: %s\n",
		    tab->device_class);
	  #endif
          *error = DevErr_SignalListNotFound;
	  free(tab);
          return (DS_NOTOK);
        }

  	free(tab);
  	return (DS_OK);
}

/*============================================================================
 Function:     	long dev_get_sig_config()

 Description:   Extract the signal configuration for a device from
		the resource database. The result is the same as 
		calling the command DevGetSigConfig on the device.

 Arg(s) In:      char *device_name	  - Name of the device

 Arg(s) Out:    DevVarStringArray *argout - Array containing the configuration
					    of all signals known for this 
					    device.
                 long           *error    - pointer to error code, in case
                                            routine fails.
 ============================================================================*/
long dev_get_sig_config (char *device_name, DevVarStringArray *argout, 
			 long *error)
{
	char             tmp[128];
	char 		 *res;

	char		 nethost[40];
	char		 domain[40];
	char		 family[40];
	char		 member[40];
	char             *loc_nethost;

	int              i,j,k,l,arg_ln;
	int		 mem_err;
	db_resource      *sig_res;
	int              sig_res_nb;
	char             **res_dev;
	char             **res_class;

	*error = 0;

	#ifdef EBUG
	InitT();
	fprintf(stderr,"Getting signal list");
	fflush(stderr);
	#endif

	/*******************************************************/
        /*  Free old argout                                    */
	/*******************************************************/

  	if( arg_out_nb > 0 )
  	{
    	  for(i=0;i<arg_out_nb;i++)
      	    if( arg_out[i]!=NULL )
	    {
              free( arg_out[i] );
	      arg_out[i]=NULL;
	    }
    	  free(arg_out);
    	  arg_out_nb=0;
  	}

	/********************************************************/
	/* Get the signal list and properties suffixes          */
	/********************************************************/
	if( dev_get_sig_list( device_name , &signal_names , error) < 0 )
	{
	  FreeData();
          return (DS_NOTOK);
	}
        get_sig_prop_suffix(&prop_suff);

	/**********************************************************/
	/* Check the nethost to determine the strategy to recover */
	/* the signal resources.                                  */
	/**********************************************************/

        strcpy(nethost,"");
	Extract( device_name , nethost , domain , family , member );
	loc_nethost=getenv("NETHOST");

	if( (strlen(nethost)==0) ||
	    (strcasecmp(nethost,loc_nethost)==0) )
        {
	
	  /********************************************************/
	  /* Implementation via db_getresval()                    */
	  /********************************************************/
	  
	  /********************************************************/
	  /* Get the class defined resource values                */
	  /********************************************************/

	  #ifdef EBUG
	  NowT();
	  fprintf(stderr,"Getting class res");
	  fflush(stderr);
	  #endif
	  Extract( class_res_name , nethost , domain , family , member );
	  if( db_getresresoval( domain , family , member ,
			      "*",&class_res_nb,&class_res,error)<0 )
          {
	    FreeData();
            return (DS_NOTOK);
          }
	  sprintf(tmp,"%s/%s/%s",domain , family , member );
	  ReFormat( tmp , class_res , class_res_nb );

	  /********************************************************/
	  /* Get the device defined resource values               */
	  /********************************************************/
	
	  #ifdef EBUG
	  NowT();
	  fprintf(stderr,"Getting device res");
	  fflush(stderr);
	  #endif

	  Extract( device_name , nethost , domain , family , member );
	  if( db_getresresoval( domain , family , member ,
			      "*",&device_res_nb,&device_res,error)<0 )
          {
	    FreeData();
            return (DS_NOTOK);
          }
	  sprintf(tmp,"%s/%s/%s",domain,family,member);
	  ReFormat( tmp , device_res , device_res_nb );

	  /********************************************************/
	  /* Fill argout                                          */
	  /********************************************************/

	  #ifdef EBUG
	  NowT();
	  fprintf(stderr,"Create arg_out");
	  fflush(stderr);
	  #endif

          arg_ln = signal_names.length*(prop_suff.length+1)+1;

	  arg_out = (char **)malloc( arg_ln*sizeof(char *) );
        
          if( arg_out == NULL ) {
	     *error = DevErr_InsufficientMemory;
	     FreeData();
   	     return(DS_NOTOK);
	  }

	  arg_out_nb = arg_ln;

	  /* Initialise arg_out[...] to NULL */

	  for(i=0;i<arg_out_nb;i++)
	    arg_out[i]=NULL;

	  k=0;mem_err=0;
	
	  /* Write number of field per signal */

	  sprintf(tmp,"%d",prop_suff.length+1);
	  arg_out[k]=strdup(tmp);
	  mem_err=(arg_out[k]==NULL);
          k++;

	  for( j=0 ; (j<signal_names.length) && !mem_err ; j++ )
	  {

	    /* Write the signal name */

	    sprintf(tmp,"%s/%s",device_name,
			      signal_names.sequence[j] );
	    arg_out[k]=strdup(tmp);
	    mem_err=(arg_out[k] == NULL);
            k++;

	    for( i=0 ; (i<prop_suff.length) && !mem_err ; i++)
	    {
	      sprintf(tmp,"%s.%s",signal_names.sequence[j],
				  prop_suff.sequence[i] );

	      /* Try to fing the resource into device resource set */
              res=Parse( device_res , device_res_nb , tmp );
	    
	      /* Try to fing the resource into class resource set */
	      if( res==NULL )
                res=Parse( class_res , class_res_nb , tmp );
	   
	      /* Return default value */
	      if( res==NULL )
                res="Not specified";

	      arg_out[k]=strdup(res);
	      mem_err=(arg_out[k] == NULL);
	      k++;
	    }
	  }

	  if(mem_err) {
	    FreeData();
	    *error = DevErr_InsufficientMemory;
	    return(DS_NOTOK);
	  }

	  #ifdef EBUG
	  NowT();
	  #endif

	  argout->length = arg_out_nb;
	  argout->sequence = arg_out;

	  FreeData();
	  return (DS_OK);

	} else {

          /**********************************************/
          /* Implementation via db_getresource          */
          /**********************************************/
	
	  /**********************************************/
	  /* Build the db_getresurce array              */
	  /**********************************************/

	  sig_res_nb = signal_names.length*prop_suff.length;
	  sig_res=(db_resource *)malloc( sig_res_nb * sizeof( db_resource ) );
	  res_dev=(char **)malloc( sig_res_nb * sizeof( char * ) );
	  res_class=(char **)malloc( sig_res_nb * sizeof( char * ) );
          
	  if( sig_res==NULL || res_dev==NULL || res_class==NULL ) {
	     *error = DevErr_InsufficientMemory;
	     FreeData();
   	     return(DS_NOTOK);
	  }

	  for(k=0,i=0;i<signal_names.length;i++)
  	  {
	    for(j=0;j<prop_suff.length;j++)
	    {
	      sprintf(tmp,"%s.%s",signal_names.sequence[i],prop_suff.sequence[j]);
	      sig_res[k].resource_name=strdup(tmp);
	      sig_res[k].resource_type=D_STRING_TYPE;
	      res_dev[k]=NULL;
	      res_class[k]=NULL;
	      k++;
	    }
	  }

	  /***********************************************************/
	  /* Get Class resource                                      */
	  /***********************************************************/
          for(i=0;i<sig_res_nb;i++)
	    sig_res[i].resource_adr = &( res_class[i] );

	  if( db_getresource (class_res_name,sig_res,sig_res_nb,error) < 0 )
	  {
	   for(i=0;i<sig_res_nb;i++) free((char *)sig_res[i].resource_name);
	   free( res_dev );free( res_class );free( sig_res );
	   FreeData();
	   printf("\nFailed to read the class resource \n");
   	   return(DS_NOTOK);
	  }

	  /***********************************************************/
	  /* Get device resource                                     */
	  /***********************************************************/
          for(i=0;i<sig_res_nb;i++)
	    sig_res[i].resource_adr = &( res_dev[i] );

	  if( db_getresource (device_name,sig_res,sig_res_nb,error) < 0 )
	  {
	   for(i=0;i<sig_res_nb;i++) {
	     free((char *)sig_res[i].resource_name);
	     if(res_class[i]!=NULL) free(res_class[i]);
	   }
	   free( res_dev );free( res_class );free( sig_res );
	   FreeData();
	   printf("\nFailed to read the class resource \n");
   	   return(DS_NOTOK);
	  }


	  /***********************************************************/
	  /* Build argout                                            */
	  /***********************************************************/
          arg_out_nb = signal_names.length*(prop_suff.length+1)+1;
          arg_out = (char **)malloc( arg_out_nb*sizeof(char *) );

	  if( arg_out==NULL ) {
	     *error = DevErr_InsufficientMemory;
	     for(i=0;i<sig_res_nb;i++) {
	       free((char *)sig_res[i].resource_name);
	       if(res_class[i]!=NULL) free(res_class[i]);
	       if(res_dev[i]!=NULL) free(res_dev[i]);
             }
	     free( res_dev );free( res_class );free( sig_res );
	     FreeData();
   	     return(DS_NOTOK);
	  }

          for(i=0;i<arg_out_nb;i++) arg_out[i]=NULL;

	  k=0;l=0;
          sprintf(tmp,"%d",prop_suff.length+1);
          arg_out[k]=strdup(tmp);
          k++;

	  for( j=0 ; (j<signal_names.length) ; j++ )
	  {
	    /* Write the signal name */

	    sprintf(tmp,"%s/%s",device_name,
			      signal_names.sequence[j] );
	    arg_out[k]=strdup(tmp);
            k++;

	    for( i=0 ; (i<prop_suff.length) ; i++)
	    {
	      /* Try to fing the resource into device resource set */
              res=res_class[l];
	    
	      /* Try to fing the resource into class resource set */
	      if( res==NULL ) res=res_dev[l];
	   
	      /* Return default value */
	      if( res==NULL ) res="Not specified";

	      arg_out[k]=strdup(res);
	      k++;l++;
	    }
	  }

	  /* Free temporary data */
        
	  for(i=0;i<sig_res_nb;i++) {
	    free((char *)sig_res[i].resource_name);
	    if(res_class[i]!=NULL) free(res_class[i]);
	    if(res_dev[i]!=NULL) free(res_dev[i]);
	  }
	  free( res_dev );free( res_class );free( sig_res );
	  FreeData();
	  
	  argout->length = arg_out_nb;
	  argout->sequence = arg_out;

          return (DS_OK);
	}
}

/*============================================================================
 Function:     	long dev_get_sig_config_from_name()

 Description:   Return the propertie array for one signal

 Arg(s) In:     char *dev_name	  	  - dev_name
 		char *signal_name	  - signal name

 Arg(s) Out:    DevVarStringArray *argout - Array containing the configuration
					    of the given signal
                long           *error    - pointer to error code, in case
                                            routine fails.
 ============================================================================*/
long dev_get_sig_config_from_name (char *device_name, char *signal_name , 
		                   DevVarStringArray *argout, long *error)
{
  int found=0;
  int i;
  static Dssignal signal_obj=NULL;
  char tmp[128];
  char domain[128];
  char family[128];
  char member[128];
  char nethost[128];

  /**************************/
  /* Free old signal object */
  /**************************/


  if( signal_obj != NULL ) {
  
    if (ds__method_finder(signal_obj,
			  DevMethodDestroy)
			  (signal_obj,
			  error) == DS_NOTOK)
    {
      return DS_NOTOK;
    }

    signal_obj=NULL;
  }

  /***********************************************/
  /* Get the signal list and search if it exists */
  /***********************************************/

  if( dev_get_sig_list( device_name , &signal_names , error) < 0 )
  {
    return (DS_NOTOK);
  }

  i=0;
  while( i<signal_names.length && !found )
  {
    found = ( strcasecmp( signal_name , signal_names.sequence[i] ) == 0);
    if(!found) i++;
  }

  free_var_str_array (&signal_names);
  signal_names.length=0;
  
  if ( !found ) {
    *error= DevErr_SignalNameNotFound ;
    return DS_NOTOK;
  }

  /****************************************************/
  /* Create a single signal obj to extract properties */
  /****************************************************/

  Extract( class_res_name , nethost , domain , family , member );
  sprintf(tmp,"%s/%s",device_name,signal_name);

  if (ds__create(tmp, dssignalClass,
                 &signal_obj, error) == DS_NOTOK)
  {
    return(DS_NOTOK);
  }

  if (ds__method_finder(signal_obj, DevMethodInitialise)
                       (signal_obj,
                       family,
                       error) == DS_NOTOK)
  {
    return(DS_NOTOK);
  }

  if (ds__method_finder(signal_obj, DevMethodReadProperties)
		        (signal_obj,
		        argout, 
		        error) == DS_NOTOK)
  {
    return(DS_NOTOK);
  }

  return DS_OK;
}

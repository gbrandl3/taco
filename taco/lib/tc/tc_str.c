
/****************************************************************************

 File          :   tc_str.c

 Project       :   Type conversion library

 Description   :   The C code file containing all the conversion functions which
                      convert a dev_xdr kernel type ---> to string 
                   +  convert a string ---> to a dev_xdr kernel type
                   Each of these conversion functions MUST follow a strict
                   naming conversion (see README file).

 Author        :   Faranguiss Poncet

 Original      :   November 1996

 $Revision: 1.1 $                               $Author: jkrueger1 $
 $Date: 2003-04-25 11:21:45 $			$State: Exp $

 Copyright (c) 1996 by European Synchrotron Radiation Facility,
                       Grenoble, France

                       All Rights Reserved

****************************************************************************/


#include <tc_str.h>







/*--------------   Conversion from types TO STRING   ---------------------*/

long tc_DevVoid_to_str(DevVoid *d_void, char *out_str)
{
   strcpy(out_str, "No output.");
   return(0);
}


long tc_DevBoolean_to_str(DevBoolean *d_boolean, char *out_str)
{
   int    bool_val;

   bool_val = (int) *d_boolean;

   if (bool_val == 0)
      strcpy(out_str, "False");
   else
      strcpy(out_str, "True");

   return(0);
}


long tc_DevShort_to_str(DevShort *d_short, char *out_str)
{
   sprintf (out_str, "%hd", *d_short);
   return(0);
}


long tc_DevUShort_to_str(DevUShort *d_ushort, char *out_str)
{
   sprintf (out_str, "%hu", *d_ushort);
   return(0);
}


long tc_DevLong_to_str(DevLong *d_long, char *out_str)
{
   sprintf (out_str, "%ld", *d_long);
   return(0);
}


long tc_DevULong_to_str(DevULong *d_ulong, char *out_str)
{
   sprintf (out_str, "%lu", *d_ulong);
   return(0);
}


long tc_DevFloat_to_str(DevFloat *d_float, char *out_str)
{
   sprintf (out_str, "%.5g", *d_float);
   return(0);
}


long tc_DevDouble_to_str(DevDouble *d_double, char *out_str)
{
   sprintf (out_str, "%.5lg", *d_double);
   return(0);
}


long tc_DevString_to_str(DevString *d_string, char *out_str)
{
   sprintf (out_str, "%s", *d_string);
   return(0);
}


long tc_DevIntFloat_to_str(DevIntFloat *d_intfloat, char *out_str)
{
   sprintf (out_str, "%ld\n%.5g", d_intfloat->state, d_intfloat->value);
   return(0);
}


long tc_DevFloatReadPoint_to_str(DevFloatReadPoint *d_flrp, char *out_str)
{
   sprintf (out_str, "%.5g\n%.5g", d_flrp->set, d_flrp->read);
   return(0);
}


long tc_DevStateFloatReadPoint_to_str(DevStateFloatReadPoint *d_stflrp,
                                      char *out_str)
{
   sprintf (out_str, "%d\n%.5g\n%.5g",
                      d_stflrp->state, d_stflrp->set, d_stflrp->read);
   return(0);
}


long tc_DevLongReadPoint_to_str(DevLongReadPoint *d_lnrp, char *out_str)
{
   sprintf (out_str, "%ld\n%ld", d_lnrp->set, d_lnrp->read);
   return(0);
}


long tc_DevDoubleReadPoint_to_str(DevDoubleReadPoint *d_dblrp, char *out_str)
{
   sprintf (out_str, "%.5lg\n%.5lg", d_dblrp->set, d_dblrp->read);
   return(0);
}


long tc_DevVarCharArray_to_str(DevVarCharArray *d_char_arr, char *out_str)
{
   unsigned int   index;

   index = d_char_arr->length;
   (out_str)[0] = '\0';
   strncat(out_str, (d_char_arr->sequence), index);

   (out_str)[index] = '\0';

   return(0);
}


long tc_DevVarStringArray_to_str(DevVarStringArray *d_string_arr, char *out_str)
{
   unsigned int  index;
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_string_arr->length); index++)
   {
      strcat(out_str, (d_string_arr->sequence[index]));
      if (index != (d_string_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarUShortArray_to_str(DevVarUShortArray *d_ushort_arr, char *out_str)
{
   unsigned int  index;
   char          str_short[101];
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_ushort_arr->length); index++)
   {
      sprintf(str_short, "%hu", (d_ushort_arr->sequence[index]));
      strcat(out_str, str_short);
      if (index != (d_ushort_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarShortArray_to_str(DevVarShortArray *d_short_arr, char *out_str)
{
   unsigned int  index;
   char          str_short[101];
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_short_arr->length); index++)
   {
      sprintf(str_short, "%d", (d_short_arr->sequence[index]));
      strcat(out_str, str_short);
      if (index != (d_short_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarULongArray_to_str(DevVarULongArray *d_ulong_arr, char *out_str)
{
   unsigned int  index;
   char          str_long[101];
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_ulong_arr->length); index++)
   {
      sprintf(str_long, "%lu", (d_ulong_arr->sequence[index]));
      strcat(out_str, str_long);
      if (index != (d_ulong_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarLongArray_to_str(DevVarLongArray *d_long_arr, char *out_str)
{
   unsigned int  index;
   char          str_long[101];
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_long_arr->length); index++)
   {
      sprintf(str_long, "%ld", (d_long_arr->sequence[index]));
      strcat(out_str, str_long);
      if (index != (d_long_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarFloatArray_to_str(DevVarFloatArray *d_float_arr, char *out_str)
{
   unsigned int  index;
   char          str_float[101];
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_float_arr->length); index++)
   {
      sprintf(str_float, "%.5g", (d_float_arr->sequence[index]));
      strcat(out_str, str_float);
      if (index != (d_float_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarDoubleArray_to_str(DevVarDoubleArray *d_dbl_arr, char *out_str)
{
   unsigned int  index;
   char          str_dbl[101];
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_dbl_arr->length); index++)
   {
      sprintf(str_dbl, "%.5lg", (d_dbl_arr->sequence[index]));
      strcat(out_str, str_dbl);
      if (index != (d_dbl_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarFloatReadPointArray_to_str(DevVarFloatReadPointArray *d_flrp_arr, char *out_str)
{
   unsigned int       index;
   char               str_flrp[101];
   DevFloatReadPoint  d_flrp;
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_flrp_arr->length); index++)
   {
      d_flrp = d_flrp_arr->sequence[index];
      sprintf (str_flrp, "%.5g   %.5g", d_flrp.set, d_flrp.read);
      strcat(out_str, str_flrp);
      if (index != (d_flrp_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarStateFloatReadPointArray_to_str(
                 DevVarStateFloatReadPointArray *d_stflrp_arr,
                 char *out_str)
{
   unsigned int            index;
   char                    str_stflrp[101];
   DevStateFloatReadPoint  d_stflrp;
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_stflrp_arr->length); index++)
   {
      d_stflrp = d_stflrp_arr->sequence[index];
      sprintf (str_stflrp, "%d   %.5g   %.5g",
                           d_stflrp.state, d_stflrp.set, d_stflrp.read);
      strcat(out_str, str_stflrp);
      if (index != (d_stflrp_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}


long tc_DevVarLongReadPointArray_to_str( DevVarLongReadPointArray *d_lnrp_arr,
                                         char *out_str)
{
   unsigned int       index;
   char               str_lnrp[101];
   DevLongReadPoint   d_lnrp;
 

   (out_str)[0] = '\0';
   for (index=0; index < (d_lnrp_arr->length); index++)
   {
      d_lnrp = d_lnrp_arr->sequence[index];
      sprintf (str_lnrp, "%ld   %ld", d_lnrp.set, d_lnrp.read);
      strcat(out_str, str_lnrp);
      if (index != (d_lnrp_arr->length-1))
         strcat(out_str, "\n");
   };

   return(0);
}






/*--------------   Conversion FROM STRING to types   ---------------------*/






/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVoid type!              */
/* Input format : The input string can by any string                         */
/* Output arg   : - The pointer passed in is returned unchanged.             */
/*                                                                           */
/* Call example : status = tc_str_to_DevVoid("  anything  ", &data);         */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVoid(char *in_str, DevVoid *d_void)
{
      return(0);
}




/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevBoolean data            */
/* Input format : The input string should contain one of the two following : */
/*                    "True"  "False" without any extra character. These two */
/*                values are not case sensitive.                             */
/* Output arg   : - The pointer should point to a DevBoolean data            */
/*                                                                           */
/* Call example : status = tc_str_to_DevBoolean("true", &data);              */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevBoolean(char *in_str, DevBoolean *d_boolean)
{

   if (strcasecmp(in_str, "false") == 0)
   {
      *d_boolean = (DevBoolean) 0;
      return(0);
   }

   if (strcasecmp(in_str, "true") == 0)
   {
      *d_boolean = (DevBoolean) 1;
      return(0);
   }

   return(-1);
}



/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevShort number            */
/* Input format : The input string should contain numeric characters and     */
/*                white characters (spaces, tabs, returns)                   */
/* Output arg   : - The pointer should point to a DevShort data              */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevShort("  45  ", &data);              */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevShort(char *in_str, DevShort *d_short)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%hd", d_short);

   if (ret_scanf == 1)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevUShort number           */
/* Input format : The input string should contain numeric characters and     */
/*                white characters (spaces, tabs, returns)                   */
/* Output arg   : - The pointer should point to a DevUShort data             */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevUShort("  45  ", &data);             */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevUShort(char *in_str, DevUShort *d_ushort)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%hu", d_ushort);

   if (ret_scanf == 1)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevLong number             */
/* Input format : The input string should contain numeric characters and     */
/*                white characters (spaces, tabs, returns)                   */
/* Output arg   : - The pointer should point to a DevLong data               */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevLong("  458423  ", &data);           */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevLong(char *in_str, DevLong *d_long)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%ld", d_long);

   if (ret_scanf == 1)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevULong number            */
/* Input format : The input string should contain numeric characters and     */
/*                white characters (spaces, tabs, returns)                   */
/* Output arg   : - The pointer should point to a DevULong data              */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevULong("  458423  ", &data);          */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevULong(char *in_str, DevULong *d_ulong)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%lu", d_ulong);

   if (ret_scanf == 1)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevFloat number            */
/* Input format : The input string should contain numeric characters and     */
/*                white characters (spaces, tabs, returns) and floating dot  */
/* Output arg   : - The pointer should point to a DevFloat data              */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevFloat("  458.423  ", &data);         */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevFloat(char *in_str, DevFloat *d_float)
{
  int    ret_scanf;

   ret_scanf = sscanf(in_str, "%g", d_float);

   if (ret_scanf == 1)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevDouble number           */
/* Input format : The input string should contain numeric characters and     */
/*                white characters (spaces, tabs, returns) and floating dot  */
/* Output arg   : - The pointer should point to a DevDouble data             */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevDouble("  458.4238635  ", &data);    */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevDouble(char *in_str, DevDouble *d_double)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%lg", d_double);

   if (ret_scanf == 1)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevString data             */
/* Input format : The input string should contain the string data delimited  */
/*                by double quotes ("). This supposes that the DevString data*/
/*                itself can contain ANY character excepted double quotes (")*/
/*                If this format is not respected the function fails and     */
/*                returns -1.                                                */
/* Output arg   : - The d_string should point to a memory long enough to     */
/*                  contain the input string,  otherwise the result is       */
/*                  unpredictable.                                           */
/*                                                                           */
/* Call example : status = tc_str_to_DevString("\"My DevString!\"", &data);  */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevString(char *in_str, DevString *d_string)
{

   int    ret_scanf;

   ret_scanf = sscanf(in_str, "\"%[^\"]\"", *d_string);

   if (ret_scanf == 1)
      return(0);
   else
      return(-1);

}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevIntFloat data           */
/* Input format : The input string should contain a long numeric decimal data*/
/*                followed by a floating number. The separator characters    */
/*                are the usual white characters (spaces, tabs, returns).    */
/* Output arg   : - The pointer should point to a DevIntFloat structure      */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevIntFloat("  458423  89.005443 ",     */
/*                                                 &data);                   */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevIntFloat(char *in_str, DevIntFloat *d_intfl)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%ld%g", &(d_intfl->state), &(d_intfl->value) );

   if (ret_scanf == 2)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevFloatReadPoint data     */
/* Input format : The input string should contain a floating number followed */
/*                by another floating number. The separator characters       */
/*                are the usual white characters (spaces, tabs, returns).    */
/* Output arg   : - The pointer should point to a DevFloatReadPoint structure*/
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevFloatReadPoint(" 45.423  89.005443 ",*/
/*                                                       &data);             */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevFloatReadPoint(char *in_str, DevFloatReadPoint *d_flrp)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%g%g", &(d_flrp->set), &(d_flrp->read) );

   if (ret_scanf == 2)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevStateFloatReadPoint data*/
/* Input format : The input string should contain a short numeric data       */
/*                followed by a floating number itself followed by another   */
/*                floating number. The separator characters are the usual    */
/*                white characters (spaces, tabs, returns).                  */
/* Output arg   : - The pointer should point to a DevStateFloatReadPoint     */
/*                  structure, otherwise the result is unpredictable.        */
/*                                                                           */
/* Call example : status = tc_str_to_DevStateFloatReadPoint                  */
/*                             (" 458  89.005 67.443 ", &data);              */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevStateFloatReadPoint(char *in_str, DevStateFloatReadPoint *d_stflrp)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%hd%g%g", &(d_stflrp->state),
                                         &(d_stflrp->set), &(d_stflrp->read) );

   if (ret_scanf == 3)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevLongReadPoint data      */
/* Input format : The input string should contain a long numeric data        */
/*                followed by another long numeric data. The separator       */
/*                characters are the usual white characters (spaces, tabs,   */
/*                returns).                                                  */
/* Output arg   : - The pointer should point to a DevLongReadPoint structure */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevLongReadPoint                        */
/*                             (" 8999005   67435443 ", &data);              */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevLongReadPoint(char *in_str, DevLongReadPoint *d_lngrp)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%ld%ld", &(d_lngrp->set), &(d_lngrp->read) );

   if (ret_scanf == 2)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevDoubleReadPoint data    */
/* Input format : The input string should contain a floating number followed */
/*                by another floating number. The separator characters are   */
/*                usual white characters (spaces, tabs, returns).            */
/* Output arg   : - The pointer should point to a DevDoubleReadPoint         */
/*                  structure, otherwise the result is unpredictable.        */
/*                                                                           */
/* Call example : status = tc_str_to_DevDoubleReadPoint                      */
/*                             (" 89.764005 67.5367443 ", &data);            */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevDoubleReadPoint(char *in_str, DevDoubleReadPoint *d_dblrp)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%lg%lg", &(d_dblrp->set), &(d_dblrp->read) );

   if (ret_scanf == 2)
      return(0);
   else
      return(-1);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarCharArray data       */
/* Input format : The input string can be any string.                        */
/* Output arg   : - The pointer should point to a DevVarCharArray structure  */
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is done correctly by the caller.              */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarCharArray                         */
/*                             (" This is my string! ", &data);              */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarCharArray(char *in_str, DevVarCharArray *d_char_arr)
{
   int    long_str;
   int    ret_scanf;


   if (d_char_arr == NULL)
      return(-1);

   if (d_char_arr->length == 0)
      if (d_char_arr->sequence != NULL)
         return(-1);

   if (d_char_arr->length != 0)
      if (d_char_arr->sequence == NULL)
         return(-1);

   if (d_char_arr->length == 0)
      long_str = strlen(in_str);
   else
      long_str = d_char_arr->length;

   d_char_arr->length = (u_int) long_str;

   if (d_char_arr->sequence == NULL)
      d_char_arr->sequence = (char *) malloc(long_str+1);

   strncpy(d_char_arr->sequence, in_str, long_str);

   return(0);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarStringArray data     */
/* Input format : The input string contains the DevString tokens delimited by*/
/*                2 double quotes (one to start the token and the other to   */
/*                end it. Between two consecutive tokens you can put any     */
/*                number of characters in the character set : space, tab and */
/*                comma (,). If this format is not strictly respected the    */
/*                function fails. Note that each token corresponding to one  */
/*                DevString should not exceed 1000 characters.               */
/* Output arg   : - The pointer should point to a DevVarStringArray structure*/
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is correctly done for sequence field by the   */
/*                  caller. But even in this case each element of sequence   */
/*                  array is dynamically allocated here to contain the       */
/*                  corresponding string. The caller should free the memory  */
/*                  allocated for each index of the sequence array.          */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarStringArray                       */
/*    ("\"This is the 1st string!\" \"This is the 2nd one!\" , \"The last.\""*/
/*       , &data)                                                            */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarStringArray(char *in_str, DevVarStringArray *d_str_arr)
{

   int    nb_tokens, char_tok, nb_guill;
   char   *str_pointer;
   char   tok[1001];
   int    ind_token, ret_scanf;



   char_tok = (int) '"';
   str_pointer = strchr(in_str, char_tok);
   nb_guill = 0;

   while (str_pointer != NULL)
   {
     nb_guill++;
     str_pointer++;
     str_pointer = strchr(str_pointer, char_tok);
   };

   nb_tokens = nb_guill / 2;

   if (nb_tokens == 0)
      return(-1);

   if (d_str_arr == NULL)
      return(-1);

   if (d_str_arr->length == 0)
      if (d_str_arr->sequence != NULL)
         return(-1);

   if (d_str_arr->length != 0)
      if (d_str_arr->sequence == NULL)
         return(-1);

   if (d_str_arr->length == 0)
   {
      d_str_arr->length = nb_tokens;
      d_str_arr->sequence = (DevString *) malloc (sizeof(DevString)*nb_tokens);
   }
   else
      nb_tokens = d_str_arr->length;


   for (ind_token = 0; ind_token < nb_tokens; ind_token++)
      d_str_arr->sequence[ind_token] = NULL;


   str_pointer = in_str;
   ind_token = 0;
   while ( (str_pointer != NULL) && (ind_token < nb_tokens) )
   {
      str_pointer = strchr(str_pointer, char_tok); /** go to the starting " **/
      if (str_pointer != NULL)
      {
         ret_scanf = sscanf(str_pointer, "\"%[^\"]\"", tok);
         if (ret_scanf == 1)
         {
            d_str_arr->sequence[ind_token] = (DevString)
                                             malloc(strlen(tok)+1);
            strcpy(d_str_arr->sequence[ind_token] , tok);
            ind_token++;
            str_pointer++;
            str_pointer = strchr(str_pointer, char_tok); /** go to ending " **/
            if (str_pointer != NULL) str_pointer++;
         }
         else
            str_pointer = NULL;
      }
   };

   return(0);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarUShortArray data     */
/* Input format : The input string contains the numeric values immediately   */
/*                followed by a comma (,). Even the last short (numeric) data*/
/*                should immediately be followed by a comma (,). These       */
/*                numeric tokens are eventually separated by the usual white */
/*                characters (space, tabs, return).                          */
/* Output arg   : - The pointer should point to a DevVarUShortArray structure*/
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is correctly done for sequence field by the   */
/*                  caller.                                                  */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarUShortArray                       */
/*                                     (" 56, 9876, 34, 142, 65, ", &data)   */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarUShortArray(char *in_str, DevVarUShortArray *d_ushort_arr)
{

   int    ind_short, nb_shorts;
   int    char_comma;
   char   *str_pointer;
   char   tok[1001];
   int    ind_token, ret_scanf;
   DevUShort  ushort_data;



   nb_shorts = 0;
   char_comma = (int) ',';
   str_pointer = strchr(in_str, char_comma);

   while (str_pointer != NULL)
   {
     nb_shorts++;
     str_pointer++;
     str_pointer = strchr(str_pointer, char_comma);
   };

   if (nb_shorts == 0)
      return(-1);


   if (d_ushort_arr == NULL)
      return(-1);

   if (d_ushort_arr->length == 0)
      if (d_ushort_arr->sequence != NULL)
         return(-1);

   if (d_ushort_arr->length != 0)
      if (d_ushort_arr->sequence == NULL)
         return(-1);


   if (d_ushort_arr->length == 0)
   {
      d_ushort_arr->length = nb_shorts;
      d_ushort_arr->sequence = (DevUShort *)
                               malloc (sizeof(DevUShort) * nb_shorts);
   }
   else
      nb_shorts = d_ushort_arr->length;


   for (ind_short = 0; ind_short < nb_shorts; ind_short++)
      d_ushort_arr->sequence[ind_short] = 0;


   str_pointer = in_str;
   ind_short = 0;
   while ( (str_pointer != NULL) && (ind_short < nb_shorts) )
   {
      ret_scanf = sscanf(str_pointer, "%hu,", &ushort_data);
      if (ret_scanf == 1)
      {
         d_ushort_arr->sequence[ind_short] = ushort_data;
         ind_short++;
         str_pointer = strchr(str_pointer, char_comma); /** go to ending (,) **/
         if (str_pointer != NULL) str_pointer++;
      }
      else
         str_pointer = NULL;
   };

   return(0);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarShortArray data      */
/* Input format : The input string contains the numeric values immediately   */
/*                followed by a comma (,). Even the last short (numeric) data*/
/*                should immediately be followed by a comma (,). These       */
/*                numeric tokens are eventually separated by the usual white */
/*                characters (space, tabs, return).                          */
/* Output arg   : - The pointer should point to a DevVarShortArray structure */
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is correctly done for sequence field by the   */
/*                  caller.                                                  */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarShortArray                        */
/*                                     (" 56, 9876, 34, 142, 65, ", &data)   */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarShortArray(char *in_str, DevVarShortArray *d_short_arr)
{

   int    ind_short, nb_shorts;
   int    char_comma;
   char   *str_pointer;
   char   tok[1001];
   int    ind_token, ret_scanf;
   DevShort  short_data;



   nb_shorts = 0;
   char_comma = (int) ',';
   str_pointer = strchr(in_str, char_comma);

   while (str_pointer != NULL)
   {
     nb_shorts++;
     str_pointer++;
     str_pointer = strchr(str_pointer, char_comma);
   };

   if (nb_shorts == 0)
      return(-1);


   if (d_short_arr == NULL)
      return(-1);

   if (d_short_arr->length == 0)
      if (d_short_arr->sequence != NULL)
         return(-1);

   if (d_short_arr->length != 0)
      if (d_short_arr->sequence == NULL)
         return(-1);


   if (d_short_arr->length == 0)
   {
      d_short_arr->length = nb_shorts;
      d_short_arr->sequence = (DevShort *)
                               malloc (sizeof(DevShort) * nb_shorts);
   }
   else
      nb_shorts = d_short_arr->length;


   for (ind_short = 0; ind_short < nb_shorts; ind_short++)
      d_short_arr->sequence[ind_short] = 0;


   str_pointer = in_str;
   ind_short = 0;
   while ( (str_pointer != NULL) && (ind_short < nb_shorts) )
   {
      ret_scanf = sscanf(str_pointer, "%hd,", &short_data);
      if (ret_scanf == 1)
      {
         d_short_arr->sequence[ind_short] = short_data;
         ind_short++;
         str_pointer = strchr(str_pointer, char_comma); /** go to ending (,) **/
         if (str_pointer != NULL) str_pointer++;
      }
      else
         str_pointer = NULL;
   };

   return(0);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarULongArray data      */
/* Input format : The input string contains the numeric values immediately   */
/*                followed by a comma (,). Even the last long (numeric) data */
/*                should immediately be followed by a comma (,). These       */
/*                numeric tokens are eventually separated by the usual white */
/*                characters (space, tabs, return).                          */
/* Output arg   : - The pointer should point to a DevVarULongArray structure */
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is correctly done for sequence field by the   */
/*                  caller.                                                  */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarULongArray                        */
/*                                (" 569876,   34142  ,   65896, ", &data)   */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarULongArray(char *in_str, DevVarULongArray *d_ulong_arr)
{

   int    ind_long, nb_longs;
   int    char_comma;
   char   *str_pointer;
   char   tok[1001];
   int    ind_token, ret_scanf;
   DevULong  ulong_data;



   nb_longs = 0;
   char_comma = (int) ',';
   str_pointer = strchr(in_str, char_comma);

   while (str_pointer != NULL)
   {
     nb_longs++;
     str_pointer++;
     str_pointer = strchr(str_pointer, char_comma);
   };

   if (nb_longs == 0)
      return(-1);


   if (d_ulong_arr == NULL)
      return(-1);

   if (d_ulong_arr->length == 0)
      if (d_ulong_arr->sequence != NULL)
         return(-1);

   if (d_ulong_arr->length != 0)
      if (d_ulong_arr->sequence == NULL)
         return(-1);


   if (d_ulong_arr->length == 0)
   {
      d_ulong_arr->length = nb_longs;
      d_ulong_arr->sequence = (DevULong *)
                               malloc (sizeof(DevULong) * nb_longs);
   }
   else
      nb_longs = d_ulong_arr->length;


   for (ind_long = 0; ind_long < nb_longs; ind_long++)
      d_ulong_arr->sequence[ind_long] = 0;


   str_pointer = in_str;
   ind_long = 0;
   while ( (str_pointer != NULL) && (ind_long < nb_longs) )
   {
      ret_scanf = sscanf(str_pointer, "%lu,", &ulong_data);
      if (ret_scanf == 1)
      {
         d_ulong_arr->sequence[ind_long] = ulong_data;
         ind_long++;
         str_pointer = strchr(str_pointer, char_comma); /** go to ending (,) **/
         if (str_pointer != NULL) str_pointer++;
      }
      else
         str_pointer = NULL;
   };

   return(0);
}


/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarLongArray data       */
/* Input format : The input string contains the numeric values immediately   */
/*                followed by a comma (,). Even the last long (numeric) data */
/*                should immediately be followed by a comma (,). These       */
/*                numeric tokens are eventually separated by the usual white */
/*                characters (space, tabs, return).                          */
/* Output arg   : - The pointer should point to a DevVarLongArray structure  */
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is correctly done for sequence field by the   */
/*                  caller.                                                  */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarLongArray                         */
/*                                   (" 569876  , 34142, 654899, ", &data)   */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarLongArray(char *in_str, DevVarLongArray *d_long_arr)
{

   int    ind_long, nb_longs;
   int    char_comma;
   char   *str_pointer;
   char   tok[1001];
   int    ind_token, ret_scanf;
   DevLong  long_data;



   nb_longs = 0;
   char_comma = (int) ',';
   str_pointer = strchr(in_str, char_comma);

   while (str_pointer != NULL)
   {
     nb_longs++;
     str_pointer++;
     str_pointer = strchr(str_pointer, char_comma);
   };

   if (nb_longs == 0)
      return(-1);


   if (d_long_arr == NULL)
      return(-1);

   if (d_long_arr->length == 0)
      if (d_long_arr->sequence != NULL)
         return(-1);

   if (d_long_arr->length != 0)
      if (d_long_arr->sequence == NULL)
         return(-1);


   if (d_long_arr->length == 0)
   {
      d_long_arr->length = nb_longs;
      d_long_arr->sequence = (DevLong *)
                               malloc (sizeof(DevLong) * nb_longs);
   }
   else
      nb_longs = d_long_arr->length;


   for (ind_long = 0; ind_long < nb_longs; ind_long++)
      d_long_arr->sequence[ind_long] = 0;


   str_pointer = in_str;
   ind_long = 0;
   while ( (str_pointer != NULL) && (ind_long < nb_longs) )
   {
      ret_scanf = sscanf(str_pointer, "%ld,", &long_data);
      if (ret_scanf == 1)
      {
         d_long_arr->sequence[ind_long] = long_data;
         ind_long++;
         str_pointer = strchr(str_pointer, char_comma); /** go to ending (,) **/
         if (str_pointer != NULL) str_pointer++;
      }
      else
         str_pointer = NULL;
   };

   return(0);
}




/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarFloatArray data      */
/* Input format : The input string contains the float numbers immediately    */
/*                followed by a comma (,). Even the last float number        */
/*                should immediately be followed by a comma (,). These       */
/*                float tokens are eventually separated by the usual white   */
/*                characters (space, tabs, return).                          */
/* Output arg   : - The pointer should point to a DevVarFloatArray structure */
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is correctly done for sequence field by the   */
/*                  caller.                                                  */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarFloatArray                        */
/*                              (" 56.9876  , 34.142, 6548.99, ", &data)     */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarFloatArray(char *in_str, DevVarFloatArray *d_float_arr)
{

   int    ind_float, nb_floats;
   int    char_comma;
   char   *str_pointer;
   char   tok[1001];
   int    ind_token, ret_scanf;
   DevFloat  float_data;



   nb_floats = 0;
   char_comma = (int) ',';
   str_pointer = strchr(in_str, char_comma);

   while (str_pointer != NULL)
   {
     nb_floats++;
     str_pointer++;
     str_pointer = strchr(str_pointer, char_comma);
   };

   if (nb_floats == 0)
      return(-1);


   if (d_float_arr == NULL)
      return(-1);

   if (d_float_arr->length == 0)
      if (d_float_arr->sequence != NULL)
         return(-1);

   if (d_float_arr->length != 0)
      if (d_float_arr->sequence == NULL)
         return(-1);


   if (d_float_arr->length == 0)
   {
      d_float_arr->length = nb_floats;
      d_float_arr->sequence = (DevFloat *)
                               malloc (sizeof(DevFloat) * nb_floats);
   }
   else
      nb_floats = d_float_arr->length;


   for (ind_float = 0; ind_float < nb_floats; ind_float++)
      d_float_arr->sequence[ind_float] = 0.0;


   str_pointer = in_str;
   ind_float = 0;
   while ( (str_pointer != NULL) && (ind_float < nb_floats) )
   {
      ret_scanf = sscanf(str_pointer, "%g,", &float_data);
      if (ret_scanf == 1)
      {
         d_float_arr->sequence[ind_float] = float_data;
         ind_float++;
         str_pointer = strchr(str_pointer, char_comma); /** go to ending (,) **/
         if (str_pointer != NULL) str_pointer++;
      }
      else
         str_pointer = NULL;
   };

   return(0);
}




/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevVarDoubleArray data     */
/* Input format : The input string contains the float numbers immediately    */
/*                followed by a comma (,). Even the last float number        */
/*                should immediately be followed by a comma (,). These       */
/*                float tokens are eventually separated by the usual white   */
/*                characters (space, tabs, return).                          */
/* Output arg   : - The pointer should point to a DevVarDoubleArray structure*/
/*                  otherwise the result is unpredictable.                   */
/*                - If the field length is 0 the field sequence must be NULL */
/*                  otherwise the function fails.                            */
/*                - If the field length is != 0 the field sequence should be */
/*                  allocated by the caller consequently otherwise the result*/
/*                  is unpredictable.                                        */
/*                - If the fields length and sequence are NULL this function */
/*                  will allocate the memory necessary to convert the input  */
/*                  string to the requested data type. In this case the      */
/*                  caller is responsible for freeing the memory when it is  */
/*                  not used any more.                                       */
/*                - If the fields length and sequence are not NULL this      */
/*                  will not allocate the memory and it supposes that the    */
/*                  allocation is correctly done for sequence field by the   */
/*                  caller.                                                  */
/*                                                                           */
/* Call example : status = tc_str_to_DevVarDoubleArray                       */
/*                              (" 56.9876  , 34.142, 6548.99, ", &data)     */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevVarDoubleArray(char *in_str, DevVarDoubleArray *d_double_arr)
{

   int    ind_double, nb_doubles;
   int    char_comma;
   char   *str_pointer;
   char   tok[1001];
   int    ind_token, ret_scanf;
   DevDouble  double_data;



   nb_doubles = 0;
   char_comma = (int) ',';
   str_pointer = strchr(in_str, char_comma);

   while (str_pointer != NULL)
   {
     nb_doubles++;
     str_pointer++;
     str_pointer = strchr(str_pointer, char_comma);
   };

   if (nb_doubles == 0)
      return(-1);


   if (d_double_arr == NULL)
      return(-1);

   if (d_double_arr->length == 0)
      if (d_double_arr->sequence != NULL)
         return(-1);

   if (d_double_arr->length != 0)
      if (d_double_arr->sequence == NULL)
         return(-1);


   if (d_double_arr->length == 0)
   {
      d_double_arr->length = nb_doubles;
      d_double_arr->sequence = (DevDouble *)
                               malloc (sizeof(DevDouble) * nb_doubles);
   }
   else
      nb_doubles = d_double_arr->length;


   for (ind_double = 0; ind_double < nb_doubles; ind_double++)
      d_double_arr->sequence[ind_double] = 0.0;


   str_pointer = in_str;
   ind_double = 0;
   while ( (str_pointer != NULL) && (ind_double < nb_doubles) )
   {
      ret_scanf = sscanf(str_pointer, "%lg,", &double_data);
      if (ret_scanf == 1)
      {
         d_double_arr->sequence[ind_double] = double_data;
         ind_double++;
         str_pointer = strchr(str_pointer, char_comma); /** go to ending (,) **/
         if (str_pointer != NULL) str_pointer++;
      }
      else
         str_pointer = NULL;
   };

   return(0);
}



/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevMulMove data            */
/* Input format : The input string should contain 16 long numeric data       */
/*                followed by 8 float numeric data. The separator            */
/*                characters are the usual white characters (spaces, tabs,   */
/*                returns).                                                  */
/* Output arg   : - The pointer should point to a DevMulMove structure       */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevMulMove                              */
/*                ("32 87 346 12 987654 12 65 89 \n                          */
/*                     3747 47457 363 253 3664 453 12 34 \n                  */
/*                      1.2 3.45 687.373 585.55 98.7 374.1 23.008 354.01 "); */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevMulMove(char *in_str, DevMulMove *d_mulmove)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, 
   "%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%ld%g%g%g%g%g%g%g%g",
    &(d_mulmove->action[0]), &(d_mulmove->action[1]),&(d_mulmove->action[2]),
    &(d_mulmove->action[3]), &(d_mulmove->action[4]), &(d_mulmove->action[5]),
    &(d_mulmove->action[6]), &(d_mulmove->action[7]),
    &(d_mulmove->delay[0]), &(d_mulmove->delay[1]), &(d_mulmove->delay[2]),
    &(d_mulmove->delay[3]), &(d_mulmove->delay[4]), &(d_mulmove->delay[5]),
    &(d_mulmove->delay[6]), &(d_mulmove->delay[7]),
 &(d_mulmove->position[0]), &(d_mulmove->position[1]), &(d_mulmove->position[2]),
 &(d_mulmove->position[3]), &(d_mulmove->position[4]), &(d_mulmove->position[5]),
 &(d_mulmove->position[6]), &(d_mulmove->position[7])
                      );

   if (ret_scanf == 24)
      return(0);
   else
      return(-1);
}





/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevMotorLong data          */
/* Input format : The input string should contain a long numeric data        */
/*                followed by another long numeric data. The separator       */
/*                characters are the usual white characters (spaces, tabs,   */
/*                returns).                                                  */
/* Output arg   : - The pointer should point to a DevMotorLong structure     */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevMotorLong                            */
/*                             (" 8999005   67435443 ", &data);              */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevMotorLong(char *in_str, DevMotorLong *d_motorl)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%ld%ld", &(d_motorl->axisnum), &(d_motorl->value) );

   if (ret_scanf == 2)
      return(0);
   else
      return(-1);
}



/*****************************************************************************/
/*                                                                           */
/* Purpose      : converts a string (char *) to a DevMotorFloat data         */
/* Input format : The input string should contain a long numeric data        */
/*                followed by a float numeric data. The separator            */
/*                characters are the usual white characters (spaces, tabs,   */
/*                returns).                                                  */
/* Output arg   : - The pointer should point to a DevMotorFloat structure    */
/*                  otherwise the result is unpredictable.                   */
/*                                                                           */
/* Call example : status = tc_str_to_DevMotorFloat                           */
/*                             (" 8999005   345.98 ", &data);                */
/*                                                                           */
/*****************************************************************************/

long tc_str_to_DevMotorFloat(char *in_str, DevMotorFloat *d_motorf)
{
   int    ret_scanf;

   ret_scanf = sscanf(in_str, "%ld%g", &(d_motorf->axisnum), &(d_motorf->value) );

   if (ret_scanf == 2)
      return(0);
   else
      return(-1);
}









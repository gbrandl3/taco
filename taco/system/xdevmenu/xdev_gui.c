/****************************************************************************

 File          :  xdev_gui.c

 Project       :  Xdevmenu application (new generation made by BX)

 Description   :  The interface module between Motif+X11 and the reste of appli

 Author        :  Faranguiss Poncet

 Original      :  February 1997

 $Revision: 1.1 $				$Author: jkrueger1 $
 $Date: 2003-04-25 12:54:28 $					$State: Exp $

 Copyright (c) 1997 by European Synchrotron Radiation Facility,
		       Grenoble, France

                       All Rights Reserved

****************************************************************************/


#include <time.h>

#include <API.h>
#include <maxe_xdr.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>

#include "xdev_gui.h"

extern Widget  gui_widget_array[]; 

/*
 * Convenience functions from xdev_bxutil.c utilities file.
 */
extern XtPointer CONVERT(Widget, char *, char *, int, Boolean *);



/*
 * Callback functions from xdev_cb.c callback module.
 */
extern void cb_select_device( Widget , XtPointer , XtPointer );
extern void cb_default_action( Widget , XtPointer , XtPointer );



typedef struct {  char         *dev_name;
                  Widget       rc;
                  Widget       icon_button;
                  Widget       label_button;
               } gui_dev_icon_struct;

#define MAX_TACO_TYPES         100

/****  local variables for this module  ****/
static gui_dev_icon_struct    *gui_dev_array[XDEV_MAX_DEV];
static int                    gui_current_device;
static int                    gui_selected_cmd;
static int                    gui_pixmap_loaded = 0;
static Pixmap                 gui_icon_pxm = 0;
static char                   *gui_help_format[MAX_TACO_TYPES];


#define gui_gearbitmap_width 50
#define gui_gearbitmap_height 50
static char gui_gearbitmap_bits[] = {
   0x00, 0x00, 0x07, 0x80, 0x01, 0x00, 0xfc, 0x00, 0xc0, 0x0f, 0xc0, 0x07,
   0x00, 0xfc, 0x00, 0xe0, 0x0c, 0xc0, 0x1e, 0x00, 0xfc, 0x00, 0x60, 0x18,
   0xe0, 0x18, 0x00, 0xfc, 0x00, 0x60, 0x30, 0x70, 0x18, 0x00, 0xfc, 0x00,
   0x60, 0x30, 0x38, 0x18, 0x00, 0xfc, 0x00, 0x60, 0x60, 0x18, 0x0c, 0x00,
   0xfc, 0x00, 0xc0, 0xc0, 0x1f, 0x0c, 0x00, 0xfc, 0x00, 0xc0, 0xc0, 0x0f,
   0x0c, 0x00, 0xfc, 0x00, 0xc0, 0x00, 0x00, 0x06, 0x00, 0xfc, 0x00, 0xe0,
   0x00, 0x00, 0x06, 0x00, 0xfc, 0x38, 0xf0, 0x00, 0x00, 0x06, 0x00, 0xfc,
   0xfc, 0x33, 0x00, 0x00, 0x1c, 0x00, 0xfc, 0xec, 0x1f, 0x00, 0x00, 0x38,
   0x3c, 0xfc, 0x06, 0x1e, 0x00, 0x00, 0xf0, 0x3f, 0xfc, 0x06, 0x00, 0x00,
   0x00, 0xf0, 0x63, 0xfc, 0x0f, 0x00, 0xe0, 0x03, 0x00, 0x60, 0xfc, 0x3e,
   0x00, 0xf8, 0x0f, 0x00, 0xc0, 0xfc, 0x78, 0x00, 0x3c, 0x1c, 0x00, 0xc0,
   0xfc, 0xf0, 0x00, 0x0e, 0x38, 0x00, 0x70, 0xfc, 0xc0, 0x01, 0x06, 0x30,
   0x00, 0x1c, 0xfc, 0x80, 0x01, 0x06, 0x60, 0x00, 0x0e, 0xfc, 0x80, 0x01,
   0x03, 0x60, 0x80, 0x03, 0xfc, 0x80, 0x01, 0x03, 0x60, 0x80, 0x01, 0xfc,
   0x80, 0x01, 0x07, 0x60, 0x80, 0x01, 0xfc, 0xc0, 0x01, 0x06, 0x70, 0x80,
   0x01, 0xfc, 0xf0, 0x01, 0x0e, 0x30, 0x80, 0x01, 0xfc, 0x38, 0x00, 0x0e,
   0x38, 0x80, 0x03, 0xfc, 0x1e, 0x00, 0x7c, 0x1f, 0x80, 0x07, 0xfc, 0x0f,
   0x00, 0xf0, 0x0f, 0x00, 0x1e, 0xfc, 0x03, 0x00, 0xc0, 0x01, 0x00, 0x78,
   0xfc, 0x06, 0x06, 0x00, 0x00, 0x00, 0xe0, 0xfc, 0x86, 0x0f, 0x00, 0x00,
   0x00, 0x60, 0xfc, 0xfc, 0x1f, 0x00, 0x00, 0x30, 0x60, 0xfc, 0x7c, 0x38,
   0x00, 0x00, 0xf8, 0x33, 0xfc, 0x00, 0x70, 0x00, 0x00, 0xfc, 0x3f, 0xfc,
   0x00, 0x60, 0x00, 0x00, 0x0f, 0x1c, 0xfc, 0x00, 0x70, 0x00, 0x00, 0x07,
   0x00, 0xfc, 0x00, 0x30, 0x00, 0x00, 0x03, 0x00, 0xfc, 0x00, 0x30, 0xf0,
   0x03, 0x03, 0x00, 0xfc, 0x00, 0x10, 0xf8, 0x07, 0x03, 0x00, 0xfc, 0x00,
   0x18, 0x0c, 0x06, 0x06, 0x00, 0xfc, 0x00, 0x18, 0x06, 0x0c, 0x06, 0x00,
   0xfc, 0x00, 0x18, 0x07, 0x1c, 0x06, 0x00, 0xfc, 0x00, 0x78, 0x03, 0x18,
   0x06, 0x00, 0xfc, 0x00, 0xf0, 0x01, 0xb0, 0x07, 0x00, 0xfc, 0x00, 0xc0,
   0x00, 0xf0, 0x01, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0xfc,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xfc};




/****************************************************************************
                                                                           
 Function     : void gui_fill_in_help_formats()

 Description  : This function initializes all the format help texts associated
                to different types.

 Arg(s) In    : none

 Arg(s) Out   : none

 Return(s)    : none
                                                                           
/***************************************************************************/

void gui_fill_in_help_formats()
{
   char           *empty_help=NULL;
   char           *help_format=NULL;
   int            ind_type;

   empty_help = (char *) malloc(5);
   strcpy(empty_help, "   ");

   for ( ind_type = 0;  ind_type < MAX_TACO_TYPES; ind_type++ )
       gui_help_format[ind_type] = empty_help;

   for ( ind_type = 0;  ind_type < MAX_TACO_TYPES; ind_type++ )
   {
      switch (ind_type)
      {
         case D_BOOLEAN_TYPE          : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter one of the two words :\n");
                    strcat(help_format, " true    or    false\n");
                    strcat(help_format, "These values are accepted in case");
                    strcat(help_format, " insensitive manner.\n");
                    strcat(help_format, "Do not enter any extra character ");
                    strcat(help_format, "before or after the word.\n");
                    strcat(help_format, "\nExample : True\n");
                    gui_help_format[ind_type] = help_format;
                    break;

         case D_USHORT_TYPE           : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter an unsigned integer value ");
                    strcat(help_format, "between 0 and 65535.\n");
                    strcat(help_format, "Do not type any non numerical");
                    strcat(help_format, " character other than blanks.\n");
                    strcat(help_format, "\nExample :  123\n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_SHORT_TYPE            : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter a signed integer value ");
                    strcat(help_format, "between -32768 and +32767. \n\n");
                    strcat(help_format, "Do not type any non numerical");
                    strcat(help_format, " character other than blanks, - and +.\n");
                    strcat(help_format, "\nExample :  -123\n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_ULONG_TYPE            : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter an unsigned integer value ");
                    strcat(help_format, "between 0 and 4294967295.\n\n");
                    strcat(help_format, "Do not type any non numerical");
                    strcat(help_format, " character other than blanks.\n");
                    strcat(help_format, "\nExample :  45632\n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_LONG_TYPE             : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter a signed integer value ");
                    strcat(help_format, "between -2147483648 and +2147483647 .\n\n");
                    strcat(help_format, "Do not type any non numerical");
                    strcat(help_format, " character other than blanks, - and +.\n");
                    strcat(help_format, "\nExample :  -45632\n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_FLOAT_TYPE            : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter a signed floating point ");
                    strcat(help_format, "value.\n\n");
                    strcat(help_format, "Enter only valid floating point ");
                    strcat(help_format, "using dot character . as floating");
                    strcat(help_format, " point.\n");
                    strcat(help_format, "\nExample :  -34.50173\n");
                    gui_help_format[ind_type] = help_format;
                    gui_help_format[D_DOUBLE_TYPE] = help_format;
                    break;
         case D_DOUBLE_TYPE           : break;
         case D_STRING_TYPE           : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter a string composed of any ");
                    strcat(help_format, "character between two double quotes");
                    strcat(help_format, " one at the beginning and the other");
                    strcat(help_format, " to end the string.\n");
                    strcat(help_format, "Be careful that the very first ");
                    strcat(help_format, "character of the in put MUST be a ");
                    strcat(help_format, "double quote.\n");
                    strcat(help_format, "\nExample :\"This is my string.\"\n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_INT_FLOAT_TYPE        : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter a signed integer value ");
                    strcat(help_format, "followed by a float value.\n");
                    strcat(help_format, "These two values can be separated by");
                    strcat(help_format, " any number of blank characters.\n");
                    strcat(help_format, "\nExample :  -12876  32.7693 \n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_FLOAT_READPOINT       : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter two float values separated by");
                    strcat(help_format, " any number of blank characters.\n");
                    strcat(help_format, "The first one for set and the ");
                    strcat(help_format, "second one for read.\n");
                    strcat(help_format, "\nExample :  32.7692   32.7693 \n");
                    gui_help_format[ind_type] = help_format;
                    gui_help_format[D_DOUBLE_READPOINT] = help_format;
                    break;
         case D_STATE_FLOAT_READPOINT : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter a signed integer value ");
                    strcat(help_format, "followed by two float values.\n");
                    strcat(help_format, "These three values can be separated ");
                    strcat(help_format, "by any number of blank characters.\n");
                    strcat(help_format, "\nExample : 2  -12.876  32.7693 \n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_LONG_READPOINT        : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter two signed integer values ");
                    strcat(help_format, "separated by any number of blank ");
                    strcat(help_format, "characters.\n");
                    strcat(help_format, "The first one for set and the ");
                    strcat(help_format, "second one for read.\n");
                    strcat(help_format, "\nExample :  -12876  32693 \n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_DOUBLE_READPOINT      : break;
         case D_VAR_CHARARR           : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter a string composed of any ");
                    strcat(help_format, "character.\n");
                    strcat(help_format, "All the characters typed in will be ");
                    strcat(help_format, "given to the device server command. ");
                    strcat(help_format, "Even the white characters.\n");
                    strcat(help_format, "\nExample : This is my Char Array.\n");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_VAR_STRINGARR         : help_format = (char *) malloc(701);
                    strcpy(help_format, "Enter a sequence of strings.\n");
                    strcat(help_format, "Each string MUST BE inside 2 double ");
                    strcat(help_format, "quotes.\nTwo strings of the ");
                    strcat(help_format, "sequence can be separated by zero ");
                    strcat(help_format, "or more characters (any char).\n");
                    strcat(help_format, "\nExample : \"The first string.\"");
                    strcat(help_format, "\"The second.\"  \n , \"The last one.\"");
                    gui_help_format[ind_type] = help_format;
                    break;
         case D_VAR_USHORTARR         : help_format = (char *) malloc(701);
                    strcpy(help_format, "Enter a sequence of unsigned ");
                    strcat(help_format, "integers.\nEach unsigned integer is");
                    strcat(help_format, " followed by one comma and zero or");
                    strcat(help_format, " more white \ncharacters.\n");
                    strcat(help_format, "Be careful that EVEN THE LAST ");
                    strcat(help_format, "integer MUST BE followed by a comma.\n");
                    strcat(help_format, "\nExample : 526 , 76,98,  115,\n");
                    gui_help_format[ind_type] = help_format;
                    gui_help_format[D_VAR_ULONGARR] = help_format;
                    break;
         case D_VAR_SHORTARR         : help_format = (char *) malloc(701);
                    strcpy(help_format, "Enter a sequence of ");
                    strcat(help_format, "integers.\nEach integer is");
                    strcat(help_format, " followed by one comma and zero or");
                    strcat(help_format, " more white \ncharacters.\n");
                    strcat(help_format, "Be careful that EVEN THE LAST ");
                    strcat(help_format, "integer MUST BE followed by a comma.\n");
                    strcat(help_format, "\nExample : -555 , 76,-98,  115,\n");
                    gui_help_format[ind_type] = help_format;
                    gui_help_format[D_VAR_LONGARR] = help_format;
                    break;
         case D_VAR_ULONGARR          : break;
         case D_VAR_LONGARR           : break;
         case D_VAR_FLOATARR         : help_format = (char *) malloc(701);
                    strcpy(help_format, "Enter a sequence of ");
                    strcat(help_format, "float values.\nEach float value is");
                    strcat(help_format, " followed by one comma and zero or");
                    strcat(help_format, " more white \ncharacters.\n");
                    strcat(help_format, "Be careful that EVEN THE LAST ");
                    strcat(help_format, "float MUST BE followed by a comma.\n");
                    strcat(help_format, "\nExample : -1.97 , 76.043,-98.5,  1.5,\n");
                    gui_help_format[ind_type] = help_format;
                    gui_help_format[D_VAR_DOUBLEARR] = help_format;
                    break;
         case D_VAR_DOUBLEARR         : break;
	 case D_MULMOVE_TYPE          : help_format = (char *) malloc(701);
                    strcpy(help_format, "Enter first a sequence of ");
                    strcat(help_format, "8 integers corresponding to ");
                    strcat(help_format, "array field \n\"action[0..7]\". \n");
                    strcat(help_format, "Continue with another sequence of 8 other integers ");
                    strcat(help_format, "corresponding to \narray field \"delay[0..7].\"\n ");
                    strcat(help_format, "Finish with a sequence of 8 floats corresponding ");
	            strcat(help_format, " to array field \n\"position[0..7]\". \n\n ");
                    strcat(help_format, "\nAll of these values will be separated by ");
		    strcat(help_format, "any number of blank characters \nincluding returns.\n\n");
                    gui_help_format[ind_type] = help_format;
                    break;
	 case D_MOTOR_LONG            : help_format = (char *) malloc(501);
                    strcpy(help_format, "Enter two integer values ");
                    strcat(help_format, "separated by any number of blank ");
                    strcat(help_format, "characters.\n");
                    strcat(help_format, "The first one for \"axisnum\" field and the ");
                    strcat(help_format, "second one for \"value\" \nfield.\n");
                    strcat(help_format, "\nExample :  -12876  32693 \n");
                    gui_help_format[ind_type] = help_format;
                    break;
	 case D_MOTOR_FLOAT          : help_format = (char *) malloc(701);
                    strcpy(help_format, "Enter an integer value ");
                    strcat(help_format, "followed by a float value.\n");
		    strcat(help_format, "The integer is \"axisnum\" field and the ");
		    strcat(help_format, "float is \"value\" field.\n");
                    strcat(help_format, "These two values can be separated by");
                    strcat(help_format, " any number of blank characters.\n");
                    strcat(help_format, "\nExample :  -12876  32.7693 \n");
                    gui_help_format[ind_type] = help_format;
                    break;
         default : ;
      }
    
   }

}



/****************************************************************************
                                                                           
 Function     : long gui_init()

 Description  : This function initializes all the internal data strucures
                used by this module.

 Arg(s) In    : none

 Arg(s) Out   : none

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long gui_init()
{
   int            ind_dev;


   for ( ind_dev = 0;  ind_dev < XDEV_MAX_DEV; ind_dev++ )
       gui_dev_array[ind_dev] = NULL;

   gui_fill_in_help_formats();

   gui_current_device = -1;
   gui_selected_cmd = -1;

   return(XDEV_OK);
}





/****************************************************************************
                                                                           
 Function     : void gui_fill_dev_list()

 Description  : This function fills in the items in the list of the selection
                Box associated to the selection of a device to import.

 Arg(s) In    : unsigned int  nb_dev : number of device names in the list.
                char          **dev_names : the array of device names.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_fill_dev_list(unsigned int  nb_dev, char  **dev_names)
{
   XmString      *list_of_xms;
   Widget        dev_sel_box;
   int           ind_item;

 
   dev_sel_box = gui_widget_array[GUI_IMPORT_SELBOX];
   if (nb_dev == 0)
   {
      XtVaSetValues(dev_sel_box, XmNlistItemCount, 0, NULL);
      return;
   };

   list_of_xms = (XmString *) malloc (sizeof(XmString)*nb_dev);
   for (ind_item=0; ind_item < nb_dev; ind_item++)
       list_of_xms[ind_item] = XmStringCreateLtoR ( dev_names[ind_item],
                                                    XmFONTLIST_DEFAULT_TAG );

   XtVaSetValues(dev_sel_box, XmNlistItemCount, nb_dev,
                              XmNlistItems, list_of_xms, NULL);

   for (ind_item=0; ind_item < nb_dev; ind_item++)
       XmStringFree(list_of_xms[ind_item]);
   free(list_of_xms);

}





/****************************************************************************
                                                                           
 Function     : void gui_display_error()

 Description  : This function displays the error message passed through the
                input argument, inside an error popup window.

 Arg(s) In    : char    *err_str : error message to display in the window

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_display_error(char  *err_str)
{

    Widget              error_window;
    XmString            loc_xms;



    error_window = gui_widget_array[GUI_ERROR_WINDOW];

    loc_xms = XmStringCreateLtoR ( err_str, XmFONTLIST_DEFAULT_TAG);

    XtVaSetValues(error_window, XmNmessageString, loc_xms, NULL);
    XtManageChild(error_window);
    XmStringFree(loc_xms);

}







/****************************************************************************
                                                                           
 Function     : void gui_output_history()

 Description  : This function displays the output string passed through the
                input argument, inside a the bottom area of the main paned
                window. In this area all the output argument and error messages
                resulting from the execution of any device server command are
                displayed.

                This function will automatically remove first lines of this
                output text when the number of lines becomes too big and higher
                than a certain limit.

 Arg(s) In    : char    *out_str : output string to add to the window

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_output_history(char  *out_str)
{
    Widget      out_textw;
    char        *content_out, *content2, *new_content;
    int         cur_posit, length_out,
                nb_lines_delete, ind_line, nb_total_lines;



    out_textw = gui_widget_array[GUI_MAIN_RESULT_TEXT];
    content_out = XmTextGetString(out_textw);
    length_out = strlen(content_out);

    if (length_out == 0)
    {
       XtFree(content_out);
       content_out=NULL;
    }

    content2=content_out;
    nb_total_lines = 0;
    while (content2 != NULL)
    {
       nb_total_lines++;
       new_content = strchr(content2, '\n') + 1;
       if (strlen(new_content) == 0)
          content2=NULL;
       else
          content2=new_content;
    };

    if (nb_total_lines > GUI_MAX_NB_OUT_LINES)
    {
       nb_lines_delete = GUI_NB_SCROLL_OUT_LINES;
       content2=content_out;
       for (ind_line=0; ind_line < nb_lines_delete; ind_line++)
       {
           new_content = strchr(content2, '\n') + 1;
           content2 = new_content;
       };
       cur_posit = XmTextGetLastPosition (out_textw);
       XmTextReplace(out_textw, 0, cur_posit, new_content);
    };

    if (content_out != NULL)
       XtFree(content_out);

    cur_posit = XmTextGetLastPosition (out_textw);
    XmTextInsert(out_textw, cur_posit, out_str);
    cur_posit = XmTextGetLastPosition (out_textw);
    XmTextShowPosition(out_textw, cur_posit);

}





/****************************************************************************
                                                                           
 Function     : void gui_error_out()

 Description  : This function displays the error message passed through the
                input argument, in the bottom area of the main paned window.

                This function adds some characters to the initial error message
                (err_str) before displaying it in this area.

 Arg(s) In    : int     dev_ind : the index of the device in the array
                char    *cmd_name : the name of the failed command
                char    *err_str : error message to display in the window
                char    *time_str : the time spent to execute the command,
                                    already converted to string.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_error_out(int  dev_ind, char  *cmd_name, char  *err_str, char  *time_str)
{
    time_t      timer;
    char        *date_buff, date_str[30];
    char        *device_name;
    char        *err_str_copy;
    int         cur_posit, length_err, err_str_ln,
                nb_lines_delete, ind_line, nb_total_lines;




    timer = time(NULL);
    date_buff = ctime(&timer);

    strncpy(date_str, date_buff, 20);
    date_str[20] = '\0';

    device_name = gui_dev_array[dev_ind]->dev_name;

    err_str_ln = strlen(err_str);

    err_str_copy = (char *) malloc(err_str_ln + 501);

    strcpy(err_str_copy, "!!! error !!!   ");
    strcat(err_str_copy, date_str);
    strcat(err_str_copy, "  ");
    strcat(err_str_copy, device_name);
    strcat(err_str_copy, "  ");

    if (cmd_name != NULL)
    {
       strcat(err_str_copy, cmd_name);
       strcat(err_str_copy, "  FAILED");
    }

    if (time_str != NULL)
    {
       strcat(err_str_copy, "  ");
       strcat(err_str_copy, time_str);
    }

    strcat(err_str_copy,"\n    --->");
    strcat(err_str_copy, err_str);
    if ( err_str[err_str_ln - 1] != '\n' )
       strcat(err_str_copy, "\n");

    strcat(err_str_copy, "\n");
    gui_output_history(err_str_copy);

    free(err_str_copy);

}






/****************************************************************************
                                                                           
 Function     : void gui_create_device_icon()

 Description  : create all the widgets necessary to represent a device by
                an icon composed of a RowColomn itself composed of two
                pushbuttons one for the pixmap and the other for the device
                name.

 Arg(s) In    : char    *dev_name : the device name string.
                int     ind_dev  : the index where the widget ids should be
                                   stored in the gui_dev_array.

 Arg(s) Out   : none

 Return(s)    : none
                                                                           
/***************************************************************************/

void gui_create_device_icon( char  *dev_name , int  ind_dev )
{
    Arg           args[25];
    Cardinal      ac=0;
    Boolean       argok=False;
    Widget        mainRC;
    Widget        iconRC;
    Widget        pixmapPB;
    Widget        labelPB;
    XmString      loc_xms;
    Pixel         bg_col, fg_col;
    int           screen_depth;
    

    mainRC = gui_widget_array[GUI_MAIN_ROW_COL];
    XtVaGetValues (mainRC, XmNbackground , &bg_col,
                           XmNforeground , &fg_col, NULL);
    
    if ( (gui_pixmap_loaded == 0) && (gui_icon_pxm == 0) )
    { /*** Create the pixmap to hold the image ***/
        screen_depth = XDefaultDepthOfScreen(XtScreen(mainRC));
        gui_icon_pxm = XCreatePixmapFromBitmapData( XtDisplay(mainRC),
                                        XRootWindowOfScreen(XtScreen(mainRC)),
                                        gui_gearbitmap_bits,
                                        gui_gearbitmap_width,
                                        gui_gearbitmap_height,
                                        fg_col, bg_col, screen_depth);
        gui_pixmap_loaded = 1;
    }
    
    

    ac = 0;
    XtSetArg(args[ac], XmNorientation, XmVERTICAL); ac++;
    XtSetArg(args[ac], XmNnumColumns, 1); ac++;
    XtSetArg(args[ac], XmNpacking, XmPACK_TIGHT); ac++;
    XtSetArg(args[ac], XmNentryVerticalAlignment, XmALIGNMENT_CENTER); ac++;
    XtSetArg(args[ac], XmNspacing, 0); ac++;
    XtSetArg(args[ac], XmNbackground, bg_col); ac++;
    XtSetArg(args[ac], XmNforeground, fg_col); ac++;
    iconRC = XmCreateRowColumn( mainRC, "iconRC", args, ac);
    XtManageChild(iconRC);


    ac = 0;
    XtSetArg(args[ac], XmNalignment, XmALIGNMENT_CENTER); ac++;
    XtSetArg(args[ac], XmNlabelType, XmPIXMAP); ac++;
    XtSetArg(args[ac], XmNlabelPixmap, gui_icon_pxm); ac++;
    XtSetArg(args[ac], XmNbackground, bg_col); ac++;
    XtSetArg(args[ac], XmNforeground, fg_col); ac++;
    pixmapPB = XmCreatePushButton(iconRC, "pixmapPB", args, ac);
    XtAddCallback(pixmapPB, XmNactivateCallback,
                            cb_select_device, (XtPointer) ind_dev);
    XtAddCallback(pixmapPB, XmNactivateCallback,
                            cb_default_action, (XtPointer) NULL);
    XtManageChild(pixmapPB);
    

    loc_xms = XmStringCreateLtoR ( dev_name, XmFONTLIST_DEFAULT_TAG);
    ac = 0;
    XtSetArg(args[ac], XmNlabelString, loc_xms); ac++;
    XtSetArg(args[ac], XmNfontList, 
        CONVERT(mainRC, "-*-helvetica-bold-r-*-*-*-140-75-75-*-*-iso8859-1", 
        XmRFontList, 0, &argok)); if (argok) ac++;
    XtSetArg(args[ac], XmNbackground, bg_col); ac++;
    XtSetArg(args[ac], XmNforeground, fg_col); ac++;
    labelPB = XmCreatePushButton(iconRC, "labelPB", args, ac);
    XtAddCallback(labelPB, XmNactivateCallback,
                           cb_select_device, (XtPointer) ind_dev);
    XtAddCallback(labelPB, XmNactivateCallback,
                            cb_default_action, (XtPointer) NULL);
    XtManageChild(labelPB);

    XtVaSetValues(pixmapPB, XmNalignment, XmALIGNMENT_CENTER, NULL);

    XmStringFree(loc_xms);


    gui_dev_array[ind_dev]->rc = iconRC;
    gui_dev_array[ind_dev]->icon_button = pixmapPB;
    gui_dev_array[ind_dev]->label_button = labelPB;
    
}







/****************************************************************************
                                                                           
 Function     : void gui_add_device()

 Description  : This function creates the widgets assciated to a device :
                     - it's icon's rowcol and pushbuttons
                     - it's output and error window
                and stores all the information about the created widgets
                to a "gui_dev_array" entry associated with the device.

 Arg(s) In    : char   *dev_name : the name of the device.
                int    ind_dev   : the index in the array where the device
                                   should be added.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_add_device(char  *dev_name, int  ind_dev)
{


   gui_dev_array[ind_dev] = (gui_dev_icon_struct *)
                                 malloc ( sizeof(gui_dev_icon_struct));

   gui_dev_array[ind_dev]->dev_name = (char *) malloc(strlen(dev_name)+1);
   strcpy( gui_dev_array[ind_dev]->dev_name , dev_name);

   gui_dev_array[ind_dev]->rc = NULL;
   gui_dev_array[ind_dev]->icon_button = NULL;
   gui_dev_array[ind_dev]->label_button = NULL;

   gui_create_device_icon(dev_name, ind_dev);
}







/****************************************************************************
                                                                           
 Function     : void gui_select_device()

 Description  : This function highlights the icon associated with the device
                whose index is passed as input argument.

 Arg(s) In    : int    ind_dev   : the index of the selected device in the
                                   array.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_select_device(int ind_dev)
{

   if (gui_dev_array[ind_dev] == NULL)
      return;

   if (gui_current_device != -1)
      XtVaSetValues( gui_dev_array[gui_current_device]->rc,
                     XmNborderWidth, 0, NULL);

   XtVaSetValues( gui_dev_array[ind_dev]->rc, XmNborderWidth, 5, NULL);
   gui_current_device = ind_dev;

}







/****************************************************************************
                                                                           
 Function     : void gui_show_cmds()

 Description  : This function fills the list inside the command selection
                window with the list of command names passed as argument.
                Afterwards this function displays the command window.

 Arg(s) In    : char  **cmd_names : the array of command names.
                int   cmd_num     : the number of command names in the array.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_show_cmds(char **cmd_names, int cmd_num)
{
   XmString      *list_of_xms;
   Widget        cmd_sel_box;
   int           ind_item;

 
   cmd_sel_box = gui_widget_array[GUI_COMMAND_SELBOX];
   if (cmd_num == 0)
   {
      XtVaSetValues(cmd_sel_box, XmNlistItemCount, 0, NULL);
      return;
   };

   list_of_xms = (XmString *) malloc (sizeof(XmString)*cmd_num);
   for (ind_item=0; ind_item < cmd_num; ind_item++)
       list_of_xms[ind_item] = XmStringCreateLtoR ( cmd_names[ind_item],
                                                    XmFONTLIST_DEFAULT_TAG);

   XtVaSetValues(cmd_sel_box, XmNlistItemCount, cmd_num,
                              XmNlistItems, list_of_xms, NULL);

   for (ind_item=0; ind_item < cmd_num; ind_item++)
       XmStringFree(list_of_xms[ind_item]);

   free(list_of_xms);

   XtManageChild(cmd_sel_box);

}







/****************************************************************************
                                                                           
 Function     : int gui_get_selected_dev()

 Description  : This function returns the index of the currently selected
                device.

 Arg(s) In    : none.

 Arg(s) Out   : none.

 Return(s)    : -1 if function fails
                the index of the selected device if successful
                                                                           
/***************************************************************************/

int gui_get_selected_dev()
{
    return(gui_current_device);
}






/****************************************************************************
                                                                           
 Function     : void gui_format_arg_out()

 Description  : This function post formats the output string returned by a
                device server command executed by dev_putget_ascii.
                This helps to have a sort of beautiful display of this output
                argument.

 Arg(s) In    : long  out_type  : the type of the output argument
                char  *out_str  : the output argument already converted to str

 Arg(s) Out   : char  **form_out_str : the output string post formatted.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_format_arg_out(long out_type, char *out_str, char **form_out_str)
{
   int     length_to_eoln;
   char    *buf_ptr;
   char    strline[501], ind_beg_str[31], ind_end_str[31];
   int     ind, ind_beg, ind_end;


   switch (out_type)
   {
      case D_INT_FLOAT_TYPE        :
               strcpy(*form_out_str, " state = ");
               length_to_eoln = strcspn(out_str, "\n");
               strncat(*form_out_str, out_str, length_to_eoln);
               strcat(*form_out_str, " , value = ");
               buf_ptr = strstr(out_str, "\n");
               if (buf_ptr != NULL)
               {
                  buf_ptr = buf_ptr + 1;
                  strcat(*form_out_str, buf_ptr);
               }
               break;
      case D_FLOAT_READPOINT       :
               strcpy(*form_out_str, " set = ");
               length_to_eoln = strcspn(out_str, "\n");
               strncat(*form_out_str, out_str, length_to_eoln);
               strcat(*form_out_str, " , read = ");
               buf_ptr = strstr(out_str, "\n");
               if (buf_ptr != NULL)
               {
                  buf_ptr = buf_ptr + 1;
                  strcat(*form_out_str, buf_ptr);
               }
               break;
      case D_STATE_FLOAT_READPOINT :
               strcpy(*form_out_str, " state = ");
               length_to_eoln = strcspn(out_str, "\n");
               strncat(*form_out_str, out_str, length_to_eoln);

               buf_ptr = strstr(out_str, "\n");
               if (buf_ptr != NULL) buf_ptr = buf_ptr + 1;
               strcat(*form_out_str, " , set = ");
               length_to_eoln = strcspn(buf_ptr, "\n");
               strncat(*form_out_str, buf_ptr, length_to_eoln);

               strcat(*form_out_str, " , read = ");
               buf_ptr = strstr(buf_ptr, "\n");
               if (buf_ptr != NULL)
               {
                  buf_ptr = buf_ptr + 1;
                  strcat(*form_out_str, buf_ptr);
               }
               break;
      case D_LONG_READPOINT        :
               strcpy(*form_out_str, " set = ");
               length_to_eoln = strcspn(out_str, "\n");
               strncat(*form_out_str, out_str, length_to_eoln);
               strcat(*form_out_str, " , read = ");
               buf_ptr = strstr(out_str, "\n");
               if (buf_ptr != NULL)
               {
                  buf_ptr = buf_ptr + 1;
                  strcat(*form_out_str, buf_ptr);
               }
               break;
      case D_DOUBLE_READPOINT      :
               strcpy(*form_out_str, " set = ");
               length_to_eoln = strcspn(out_str, "\n");
               strncat(*form_out_str, out_str, length_to_eoln);
               strcat(*form_out_str, " , read = ");
               buf_ptr = strstr(out_str, "\n");
               if (buf_ptr != NULL)
               {
                  buf_ptr = buf_ptr + 1;
                  strcat(*form_out_str, buf_ptr);
               }
               break;
      case D_VAR_CHARARR           : strcpy (*form_out_str, out_str);
                                     break;

      case D_VAR_STRINGARR         : strcpy(*form_out_str, " \n ");
                                     buf_ptr = out_str;
                                     ind = 0;
                                     while (buf_ptr != NULL)
                                     {
                                       sprintf(strline, "                     sequence[%d] = ", ind);
                                       length_to_eoln = strcspn(buf_ptr, "\n");
                                       strncat(strline, buf_ptr, length_to_eoln);
                                       buf_ptr = strstr(buf_ptr, "\n");
                                       if (buf_ptr != NULL)
                                           buf_ptr = buf_ptr + 1;
                                       ind++;
                                       strcat(*form_out_str, strline);
                                       strcat(*form_out_str, " \n");
                                     };
                                     break;
      case D_VAR_USHORTARR         : 
      case D_VAR_SHORTARR          :
      case D_VAR_ULONGARR          :
      case D_VAR_LONGARR           :
      case D_VAR_FLOATARR          :
      case D_VAR_DOUBLEARR         : strcpy(*form_out_str, " \n ");
                                     buf_ptr = out_str;
                                     ind_beg = 0; ind_end = 9;
                                     while (buf_ptr != NULL)
                                     {
                                       sprintf(ind_beg_str, "%d", ind_beg);
                                       sprintf(ind_end_str, "%d", ind_end);
                                       strcpy(strline, "                     sequence[");
                                       strcat(strline, ind_beg_str);
                                       strcat(strline, " .. ");
                                       strcat(strline, ind_end_str);
                                       strcat(strline, "] : ");
                                       
                                       for (ind=ind_beg; ind <= ind_end; ind++)
                                       {
                                        length_to_eoln = strcspn(buf_ptr, "\n");
                                        strncat(strline, buf_ptr, length_to_eoln);
                                        strcat(strline, " , ");
                                        buf_ptr = strstr(buf_ptr, "\n");
                                        if (buf_ptr != NULL)
                                           buf_ptr = buf_ptr + 1;
                                        else
                                        {
                                           ind = ind_end + 1;
                                        }
                                        
                                       }
                                       strcat(*form_out_str, strline);
                                       strcat(*form_out_str, " \n");
                                       ind_beg = ind_beg + 10;
                                       ind_end = ind_end + 10;
                                     };
                                     break;
      default : strcpy (*form_out_str, out_str);
   }
}






/****************************************************************************
                                                                           
 Function     : long gui_get_nb_lines()

 Description  : This function returns the number of lines in the string
		passed as input argument.

 Arg(s) In    : char  *str_lines : the string in which lines are counted.

 Arg(s) Out   : none.

 Return(s)    : The number of lines if succeeded and 0 otherwise.
                                                                           
/***************************************************************************/

long gui_get_nb_lines( char  *str_lines )
{
   char          *buf_ptr;
   long          ind;


   buf_ptr = str_lines;
   ind = 0;
   while (buf_ptr != NULL)
   {
      buf_ptr = strstr(buf_ptr, "\n");
      if (buf_ptr != NULL)
         buf_ptr = buf_ptr + 1;
      ind++;
   };

   return(ind);
}






/****************************************************************************
                                                                           
 Function     : void gui_display_sigs()

 Description  : This function displays the output argument returned by a device
                server command in the bottom part of the main paned window.
                Before displaying the output argument this function rearrange
                the output string for some types of data.

 Arg(s) In    : DevVarStringArray    sig_conf : the config of the signals
                long                 type_out_sigs : the type of the output of the
		                                     command "DevReadSigValues"
                DevVarFloatArray     sig_vals_f : the values to be displayed if floatArray
                DevVarDoubleArray     sig_vals_d : the values to be displayed if doubleArray
                char  *time_str : the time spent to execute the command, already
                                  converted to str.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_display_sigs(DevVarStringArray sig_conf, long type_out_sigs,
                                                  DevVarFloatArray sig_vals_f,
						  DevVarDoubleArray sig_vals_d,
                                                  char  *time_str)
{
   time_t                   timer;
   char                     *date_buff, date_str[30];
   char                     *disp_text;
   char                     val_sig_str[51];
   int                      nb_prop, isig_conf, isig;



   if (type_out_sigs == D_VAR_FLOATARR)
      disp_text = (char *) malloc (201 + (sig_vals_f.length * 80));
   else
      if (type_out_sigs == D_VAR_DOUBLEARR)
         disp_text = (char *) malloc (201 + (sig_vals_d.length * 80));
      else
         disp_text = (char *) malloc(401);
   
   timer = time(NULL);
   date_buff = ctime(&timer);

   strncpy(date_str, date_buff, 20);
   date_str[20] = '\0';

   strcpy(disp_text, date_str);
   strcat(disp_text, "  ");
   strcat(disp_text, gui_dev_array[gui_current_device]->dev_name);
   strcat(disp_text, "  ");
   strcat(disp_text, "DevReadSigValues");
   strcat(disp_text, " ");
   strcat(disp_text, time_str);
   strcat(disp_text, " \n");

   switch (type_out_sigs)
   {
      case D_VAR_FLOATARR :
	   if (sig_vals_f.length <= 0)
	      strcat(disp_text, "   No signal value available.\n");
	   else
	   {
	      nb_prop=atol(sig_conf.sequence[0]);
	      if (sig_vals_f.length != (sig_conf.length -1)/nb_prop)
        	 strcat(disp_text, "   The number of signal values does not correspond to the signal configuration.\n");
	      else
	      {
        	 isig_conf = 1;
		 isig = 0;
		 while ( (isig_conf < sig_conf.length) && (isig < sig_vals_f.length) )
		 {
		    strcat(disp_text, "\t\t");
		    strcat(disp_text, sig_conf.sequence[isig_conf+1]);
		    strcat(disp_text, " = ");
		    sprintf(val_sig_str, sig_conf.sequence[isig_conf+3], sig_vals_f.sequence[isig]);
		    strcat(disp_text, val_sig_str);
		    strcat(disp_text, "  ");
		    strcat(disp_text, sig_conf.sequence[isig_conf+2]);
		    strcat(disp_text, "\n");
		    isig_conf = isig_conf + nb_prop;
		    isig++;
		 }
	      }
	   }
           break;
	   
	   
      case D_VAR_DOUBLEARR :
	   if (sig_vals_d.length <= 0)
	      strcat(disp_text, "   No signal value available.\n");
	   else
	   {
	      nb_prop=atol(sig_conf.sequence[0]);
	      if (sig_vals_d.length != (sig_conf.length -1)/nb_prop)
        	 strcat(disp_text, "   The number of signal values does not correspond to the signal configuration.\n");
	      else
	      {
        	 isig_conf = 1;
		 isig = 0;
		 while ( (isig_conf < sig_conf.length) && (isig < sig_vals_d.length) )
		 {
		    strcat(disp_text, "\t\t");
		    strcat(disp_text, sig_conf.sequence[isig_conf+1]);
		    strcat(disp_text, " = ");
		    sprintf(val_sig_str, sig_conf.sequence[isig_conf+3], sig_vals_d.sequence[isig]);
		    strcat(disp_text, val_sig_str);
		    strcat(disp_text, "  ");
		    strcat(disp_text, sig_conf.sequence[isig_conf+2]);
		    strcat(disp_text, "\n");
		    isig_conf = isig_conf + nb_prop;
		    isig++;
		 }
	      }
	   }
           break;
	   
	   
      default :  strcat(disp_text, "The type of the output of DevReadSigValues is not valid.\n");
   }

   strcat(disp_text, "\n");
   
   gui_output_history(disp_text);

   free(disp_text);
}






/****************************************************************************
                                                                           
 Function     : void gui_display_arg_out()

 Description  : This function displays the output argument returned by a device
                server command in the bottom part of the main paned window.
                Before displaying the output argument this function rearrange
                the output string for some types of data.

 Arg(s) In    : char  *cmd_name : the command name
                char  *out_name : the name of the output argument
                long  out_type  : the type of the output argument
                char  *out_str  : the output argument already converted to str
                char  *time_str : the time spent to execute the command, already
                                  converted to str.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_display_arg_out(char  *cmd_name, char  *out_name, long  out_type,
                                                           char  *out_str,
                                                           char  *time_str)
{
   long                     nb_lines_in_out_str, nb_formatted_lines;
   int                      strout_length;
   time_t                   timer;
   char                     *date_buff, date_str[30];
   char                     *formatted_out, *disp_text;


   nb_lines_in_out_str = gui_get_nb_lines(out_str);
   strout_length = strlen(out_str);


   if (out_type == D_VAR_STRINGARR)
   {
      formatted_out = (char *)
              malloc (strout_length + (nb_lines_in_out_str*60));
      disp_text = (char *)
              malloc (strout_length + 1001 + (nb_lines_in_out_str*60));
   }
   else
   {
      nb_formatted_lines = (nb_lines_in_out_str / 10) + 1;
      formatted_out = (char *) malloc
           (strout_length+(nb_lines_in_out_str*5)+(nb_formatted_lines*60));
      disp_text = (char *) malloc
           (1001+strout_length+(nb_lines_in_out_str*5)+(nb_formatted_lines*60));
   }



   timer = time(NULL);
   date_buff = ctime(&timer);

   strncpy(date_str, date_buff, 20);
   date_str[20] = '\0';

   strcpy(disp_text, date_str);
   strcat(disp_text, "  ");
   strcat(disp_text, gui_dev_array[gui_current_device]->dev_name);
   strcat(disp_text, "  ");
   strcat(disp_text, cmd_name);
   strcat(disp_text, " ");
   strcat(disp_text, time_str);
   strcat(disp_text, " \n");

   if (out_type != D_VOID_TYPE)
   {
       strcat(disp_text, "       ");
       if (out_name == NULL)
          strcat(disp_text, "Out Argument : ");
       else
          if (strlen(out_name) == 0)
             strcat(disp_text, "Out Argument : ");
          else
          {
             strcat(disp_text, out_name);
             strcat(disp_text, " : ");
          }

       gui_format_arg_out(out_type, out_str, &formatted_out);
       strcat(disp_text, formatted_out);
       strcat(disp_text, "\n");
   }
   strcat(disp_text, "\n");

   strout_length = strlen(formatted_out);
   free(formatted_out);

   gui_output_history(disp_text);

   free(disp_text);
}






/****************************************************************************
                                                                           
 Function     : char *gui_get_type_str()

 Description  : This function returns a string representing the name of the
                data type.

 Arg(s) In    : long  type_code   : the type 

 Arg(s) Out   : none.

 Return(s)    : the name of the type.
                                                                           
/***************************************************************************/

char *gui_get_type_str(long type_code)
{
      static char      loc_type_name[101];
      switch (type_code)
      {
         case D_BOOLEAN_TYPE          :
                    strcpy(loc_type_name, "(Boolean)");
                    break;

         case D_USHORT_TYPE           :
                    strcpy(loc_type_name, "(Unsigned Short)");
                    break;
         case D_SHORT_TYPE            :
                    strcpy(loc_type_name, "(Short)");
                    break;
         case D_ULONG_TYPE            :
                    strcpy(loc_type_name, "(Unsigned Long)");
                    break;
         case D_LONG_TYPE             :
                    strcpy(loc_type_name, "(Long)");
                    break;
         case D_FLOAT_TYPE            :
                    strcpy(loc_type_name, "(Float)");
                    break;
         case D_DOUBLE_TYPE           :
                    strcpy(loc_type_name, "(Double)");
                    break;
         case D_STRING_TYPE           :
                    strcpy(loc_type_name, "(String)");
                    break;
         case D_INT_FLOAT_TYPE        :
                    strcpy(loc_type_name, "(IntFloat)");
                    break;
         case D_FLOAT_READPOINT       :
                    strcpy(loc_type_name, "(FloatReadPoint)");
                    break;
         case D_STATE_FLOAT_READPOINT :
                    strcpy(loc_type_name, "(StateFloatReadPoint)");
                    break;
         case D_LONG_READPOINT        :
                    strcpy(loc_type_name, "(LongReadPoint)");
                    break;
         case D_DOUBLE_READPOINT      :
                    strcpy(loc_type_name, "(DoubleReadPoint)");
                    break;
         case D_VAR_CHARARR           :
                    strcpy(loc_type_name, "(Array of Chars)");
                    break;
         case D_VAR_STRINGARR         :
                    strcpy(loc_type_name, "(Array of Strings)");
                    break;
         case D_VAR_USHORTARR         :
                    strcpy(loc_type_name, "(Array of Unsigned Shorts)");
                    break;
         case D_VAR_SHORTARR          :
                    strcpy(loc_type_name, "(Array of Shorts)");
                    break;
         case D_VAR_ULONGARR          :
                    strcpy(loc_type_name, "(Array of Unsigned Longs)");
                    break;
         case D_VAR_LONGARR           :
                    strcpy(loc_type_name, "(Array of Longs)");
                    break;
         case D_VAR_FLOATARR          :
                    strcpy(loc_type_name, "(Array of Floats)");
                    break;
         case D_VAR_DOUBLEARR         :
                    strcpy(loc_type_name, "(Array of Doubles)");
                    break;
	 case D_MULMOVE_TYPE	      :
                    strcpy(loc_type_name, "(DevMulMove)");
                    break;
	 case D_MOTOR_LONG	      :
                    strcpy(loc_type_name, "(DevMotorLong)");
                    break;
	 case D_MOTOR_FLOAT	      :
                    strcpy(loc_type_name, "(DevMotorFloat)");
                    break;

         default : strcpy(loc_type_name, "(Unknown type)");
      }

      return(loc_type_name);

}






/****************************************************************************
                                                                           
 Function     : void gui_display_argin_window()

 Description  : This function displays the input argument dialog window. But
                before displaying the window, the function updates the following
                fields inside this window according to the input parameteres :
                       - The text widget for the help on format
                       - The text widget for the name of the argument
                       - The input text widget for the value initialized to " "

 Arg(s) In    : int   ind_cmd   : index of the selected command (to be saved)
                char  *in_name  : the name of the expected input argument
                long  in_type   : the type of the expected input argument

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_display_argin_window(int ind_cmd, char *in_name, long in_type)
{

   XmTextPosition       cur_posit;
   char                 local_in_name[301];


   gui_selected_cmd = ind_cmd;



   if (in_name != NULL)
   {
      if (strlen(in_name) == 0)
         strcpy(local_in_name, "Input Argument");
      else
         strcpy(local_in_name, in_name);
   }
   else
      strcpy(local_in_name, "Input Argument");

   strcat(local_in_name, "  ");
   strcat(local_in_name, gui_get_type_str(in_type));
   strcat(local_in_name, " : ");

   cur_posit = XmTextGetLastPosition (gui_widget_array[GUI_INARG_NAME_TEXT]);
   XmTextReplace(gui_widget_array[GUI_INARG_NAME_TEXT],
                               0, cur_posit, local_in_name);


   cur_posit = XmTextGetLastPosition (gui_widget_array[GUI_INARG_FORMAT_TEXT]);
   
   if ( in_type >= MAX_TACO_TYPES)
      XmTextReplace( gui_widget_array[GUI_INARG_FORMAT_TEXT], 0, cur_posit,
                     "Type not implemented\n");
   else
      XmTextReplace( gui_widget_array[GUI_INARG_FORMAT_TEXT], 0, cur_posit,
                     gui_help_format[in_type] );



   cur_posit = XmTextGetLastPosition (gui_widget_array[GUI_INARG_VAL_TEXT]);
   XmTextReplace( gui_widget_array[GUI_INARG_VAL_TEXT], 0, cur_posit, " ");

   XtManageChild( gui_widget_array[GUI_INARG_WIN]);
}







/****************************************************************************
                                                                           
 Function     : long gui_get_selected_dev_cmd()

 Description  : This function returns the index of the currently selected
                device and the index of the selected command for that device.

 Arg(s) In    : none.

 Arg(s) Out   : int    *ind_dev : the index of the current device.
                int    *ind_cmd : the index of the selected command.

 Return(s)    : XDEV_OK if succeeded and XDEV_NOTOK otherwise.
                                                                           
/***************************************************************************/

long gui_get_selected_dev_cmd(int *ind_dev, int *ind_cmd)
{
    if (gui_current_device == -1)
       return(XDEV_NOTOK);

    if (gui_selected_cmd == -1)
       return(XDEV_NOTOK);

    *ind_dev = gui_current_device;
    *ind_cmd = gui_selected_cmd;
    return(XDEV_OK);
}









/****************************************************************************
                                                                           
 Function     : void gui_display_dev_info()

 Description  : This function displays the device information in the device
                info window fields and popos up this window.

 Arg(s) In    : char  *dev_name     : the device name.
                char  *dev_class    : the device class.
                char  *dev_type     : the device type.
                char  *server_name  : the name of the server for this device.
                char  *server_host  : the host where the server is running.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_display_dev_info( char  *dev_name, char  *dev_class, char  *dev_type,
                           char  *server_name, char  *server_host )
{

   int         cur_posit;



   cur_posit = XmTextGetLastPosition(gui_widget_array[GUI_DEVINFO_NAME_TEXT]);
   XmTextReplace( gui_widget_array[GUI_DEVINFO_NAME_TEXT],
                  0, cur_posit, dev_name);

   cur_posit = XmTextGetLastPosition(gui_widget_array[GUI_DEVINFO_CLASS_TEXT]);
   XmTextReplace( gui_widget_array[GUI_DEVINFO_CLASS_TEXT],
                  0, cur_posit, dev_class);

   cur_posit = XmTextGetLastPosition(gui_widget_array[GUI_DEVINFO_TYPE_TEXT]);
   XmTextReplace( gui_widget_array[GUI_DEVINFO_TYPE_TEXT],
                  0, cur_posit, dev_type);

   cur_posit = XmTextGetLastPosition(gui_widget_array[GUI_DEVINFO_SERVNAME_TEXT]);
   XmTextReplace( gui_widget_array[GUI_DEVINFO_SERVNAME_TEXT],
                  0, cur_posit, server_name);

   cur_posit = XmTextGetLastPosition(gui_widget_array[GUI_DEVINFO_HOST_TEXT]);
   XmTextReplace( gui_widget_array[GUI_DEVINFO_HOST_TEXT],
                  0, cur_posit, server_host);

   XtManageChild( gui_widget_array[GUI_DEVINFO_BB] );
}







/****************************************************************************
                                                                           
 Function     : void gui_remove_device()

 Description  : This function removes the widgets asscoiated with the device
                and also the entry in the gui_dev_array.

 Arg(s) In    : int    ind_dev   : the index of the device to be removed in the
                                   array.

 Arg(s) Out   : none.

 Return(s)    : none.
                                                                           
/***************************************************************************/

void gui_remove_device(int ind_dev)
{

   if (gui_dev_array[ind_dev] != NULL)
   {
      if (gui_dev_array[ind_dev]->dev_name != NULL)
      {
         free(gui_dev_array[ind_dev]->dev_name);
         gui_dev_array[ind_dev]->dev_name = NULL;
      }
      if (gui_dev_array[ind_dev]->rc != NULL)
      {
         XtDestroyWidget(gui_dev_array[ind_dev]->rc);
         gui_dev_array[ind_dev]->rc = NULL;
         gui_dev_array[ind_dev]->icon_button = NULL;
         gui_dev_array[ind_dev]->label_button = NULL;
      }
      free(gui_dev_array[ind_dev]);
      gui_dev_array[ind_dev] = NULL;
   }

   if (gui_current_device == ind_dev)
   {
      XtUnmanageChild( gui_widget_array[GUI_COMMAND_SELBOX] );
      gui_current_device = -1;
   }

}


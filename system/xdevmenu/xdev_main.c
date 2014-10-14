/*
 * README: Portions of this file are merged at file generation
 * time. Edits can be made *only* in between specified code blocks, look
 * for keywords <Begin user code> and <End user code>.
 */
/*
 * Generated by the ICS Builder Xcessory (BX).
 *
 * Builder Xcessory 4.0
 * Code Generator Xcessory 2.0 (09/09/96)
 *
 */
/*
 * Motif required Headers
 */
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#if (XmVersion >= 1002)
#include <Xm/RepType.h>
#endif
#include <Xm/MwmUtil.h>

/*
 * Globally included information.
 */


/*
 * Headers for classes used in this program
 */

/*
 * Common constant and pixmap declarations.
 */
#include "xdev_const.h"

/*
 * Convenience functions from utilities file.
 */
/* extern void RegisterBxConverters(XtAppContext); */
extern XtPointer CONVERT(Widget, char *, char *, int, Boolean *);
extern XtPointer DOUBLE(double);
extern XtPointer SINGLE(float);
extern void MENU_POST(Widget, XtPointer, XEvent *, Boolean *);
extern Pixmap XPM_PIXMAP(Widget, char**);
extern void SET_BACKGROUND_COLOR(Widget, ArgList, Cardinal*, Pixel);

/* Begin user code block <globals> */
#include <stdlib.h>
#include <stdio.h>
#include "xdev_dev.h"
#include "xdev_gui.h"

extern Widget  gui_widget_array[];


void cb_fill_dev_selbox( Widget , XtPointer , XtPointer );
Widget CreatemainWindow(Widget);
void RegisterBxConverters(XtAppContext);



void xdev_import_start_dev(char *start_devname)
{
   int                             dev_index;
   long                            status;
   char                            *error_str=NULL, *err_disp;


   error_str = NULL;
   status = dev_import_device(start_devname, &dev_index, &error_str);

   if (status == XDEV_NOTOK)
   {
      if (error_str != NULL)
         err_disp = (char *) malloc (strlen(error_str)+501);
      else
         err_disp = (char *) malloc (501);

      strcpy (err_disp, "Cannot import this device.\n");
      strcat(err_disp, "\n");

      if (error_str != NULL) strcat(err_disp, error_str);

      gui_display_error(err_disp);
      if (error_str != NULL)
      {
         free(error_str);
         error_str = NULL;
      };
      free(err_disp);
      return;
   }

   gui_add_device(start_devname, dev_index);
   return;
}


/* End user code block <globals> */

/*
 * Change this line via the Output Application Names Dialog.
 */
#define BX_APP_CLASS "XDevmenu"

int main( int argc, char **argv)
{
    Widget       parent;
    XtAppContext app;
    Arg          args[256];
    Cardinal     ac;
    Boolean      argok=False;
    Widget   topLevelShell;
    Widget   mainWindow;
    
    /* Begin user code block <declarations> */
    char         *start_devname;
    char         *dev_name = NULL;
    int          nb_chars, ind_char, nb_slash;
    /* End user code block <declarations> */
    
    /*
     * The applicationShell is created as an unrealized
     * parent for multiple topLevelShells.  The topLevelShells
     * are created as popup children of the applicationShell.
     * This is a recommendation of Paul Asente & Ralph Swick in
     * _X_Window_System_Toolkit_ p. 677.
     */
    
    parent = XtVaAppInitialize ( &app, BX_APP_CLASS, NULL, 0, 
#ifndef XlibSpecificationRelease
    (Cardinal *) &argc, 
#else
#if (XlibSpecificationRelease>=5)
    &argc, 
#else
    (Cardinal *) &argc, 
#endif
#endif
    argv, NULL, 
    NULL );
    
    RegisterBxConverters(app);
#if (XmVersion >= 1002) 
    XmRepTypeInstallTearOffModelConverter();
#endif
    
    /* Begin user code block <create_shells> */
    start_devname = NULL;
    if (argc > 1)
    {
       dev_name = argv[1];
       nb_chars=strlen(dev_name);
       nb_slash = 0;
       for (ind_char = 0; ind_char < nb_chars; ind_char++)
           if (dev_name[ind_char] == '/')
	      nb_slash++;
	      
       if (nb_slash < 2)
          printf("Bad device name.\n");
       else
       {
         start_devname = (char *) malloc(nb_chars+1);
	 strcpy(start_devname, dev_name);
       }
       
    }

    /* End user code block <create_shells> */
    
    /*
     * Create classes and widgets used in this program. 
     */
    
    /* Begin user code block <create_topLevelShell> */
    /* End user code block <create_topLevelShell> */
    
    ac = 0;
    XtSetArg(args[ac], XmNx, 606); ac++;
    XtSetArg(args[ac], XmNy, -21); ac++;
    XtSetArg(args[ac], XmNwidth, 574); ac++;
    XtSetArg(args[ac], XmNheight, 599); ac++;
    topLevelShell = XtCreatePopupShell("topLevelShell",
        topLevelShellWidgetClass,
        parent,
        args, 
        ac);
    mainWindow = (Widget)CreatemainWindow(topLevelShell);
    XtManageChild(mainWindow);
    XtPopup(XtParent(mainWindow), XtGrabNone);
    
    /* Begin user code block <app_procedures> */
    if ( dev_init() != XDEV_OK )
       exit(-1);
    if ( gui_init() != XDEV_OK )
       exit(-1);
    if ( gui_widget_array[GUI_IMPORT_SELBOX] != NULL)
       cb_fill_dev_selbox( gui_widget_array[GUI_IMPORT_SELBOX], NULL, NULL);
       
    if (start_devname != NULL)
       xdev_import_start_dev(start_devname);
    
    /* End user code block <app_procedures> */
    
    /* Begin user code block <main_loop> */
    /* End user code block <main_loop> */
    
    XtAppMainLoop(app);
    
    /*
     * A return value regardless of whether or not the main loop ends. 
     */
     return(0); 
}
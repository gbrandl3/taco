#include <API.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PanedW.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/MwmUtil.h>
#include <Xm/Xm.h>
#include <X11/cursorfont.h>

#include <greta_cstes.h>


/************************************************************************
 *									*
 *		define some constant					*
 *									*
 ************************************************************************/
  
#define LOWER_ERROR 	-1
#define BAD_DEV_NAME 	0
#define DEV_NAME 	1
#define PSDEV_NAME 	2
#define BAD_DOMAIN 	3
#define NO_RESOURCE 	4
#define NO_PROBLEM 	5
#define NO_CLASS 	6
#define NO_SERV_CLASS 	7
#define NO_DEV_CLASS 	8

#define GR_DEV 		1
#define GR_DEV_DC 	2
#define GR_SERV 	3
#define GR_RES		4
#define GR_LOAD		5

#define DEV_BLOCK	2

#define SLICED 		10
#define SLICEC 		100
#define SLICEQ 		500
#define MAXLINE 	80

#define TAB_SIZE 	10
#define CH_TAB_SIZE 	50

#define MAX_LINE_SIZE	150
#define NB_SPACE	15

#define GR_START_DEV	"sys/start/"



/************************************************************************
 *									*
 *		define some macros					*
 *									*
 ************************************************************************/



#define FREE_STR_ARRAY(A,B) {\
	int im;\
	for (im=0;im<B;im++)\
		free(A[im]);\
	if (B != 0)\
		free(A);\
}


/************************************************************************
 *									*
 *		define some structures					*
 *									*
 ************************************************************************/



typedef struct _ana_input{
	long 		in_type;
	char		**buf;
	FILE		*f;
}ana_input;

	
typedef char ds_class[24]; 
typedef char ds_pname[12]; 
typedef char ResFilterField[24];

typedef struct _dev_window
{
	char 		*devname;
	long 		res_nb;
	Widget 		window;
	Widget 		resotext;
	Widget 		infotext;
	char 		*last_state;
	XmTextPosition 	last_position;
	char 		file[48];
	char 		*dev_save_pathname;
	char 		*chaine_dev_arr[CH_TAB_SIZE];
	long 		chaine_res_nb[CH_TAB_SIZE];
	unsigned long 	pid;
	unsigned long	pn;
	char		host_name[20];
	char		proc_name[40];
	char		pers_name[12];
	long		exported;
}dev_window;

typedef struct _dev_window_dc
{
	char 		*devname;
	long 		res_nb;
	Widget 		window;
	Widget 		resotext;
	Widget 		infotext;
	Widget 		dctext;
	char 		*last_state;
	XmTextPosition 	last_position;
	char 		file[48];
	char 		*dev_save_pathname;
	char 		*chaine_dev_arr[CH_TAB_SIZE];
	long 		chaine_res_nb[CH_TAB_SIZE];
	unsigned long 	pid;
	unsigned long 	pn;
	char		host_name[20];
	char		proc_name[40];
	char		pers_name[12];
	long 		exported;
}dev_window_dc;
	
	
typedef	struct _serv_state
{
	char 		*devs;
	XmTextPosition 	devspos;
	char 		*reso;
	XmTextPosition 	resopos;
}serv_status;
	
typedef struct _serv_window
{
	char 		*servname;
	Widget 		window;
	Widget 		infotext;
	Widget 		devstext;
	Widget 		resotext;
	serv_status 	last_state;
	char 		file[48];
	char 		*serv_save_pathname;
	char 		*chaine_dev_arr[CH_TAB_SIZE];
	unsigned long 	pid;
	unsigned long 	pn;
	char		host_name[20];
	char		proc_name[40];
	char		pers_name[12];
	long		restart;
}serv_window;

typedef struct _res_window
{
	char 		*resfilter;
	Widget 		window;
	Widget 		resotext;
	char 		*last_state;
	XmTextPosition 	last_position;
	char 		file[48];
	char 		*res_save_pathname;
}res_window;

typedef struct _empty_case
{
	int 		val;
	struct 		_empty_case *next;
}empty_case;
	
typedef struct _d_filter
{
	char 		dom[20];
	char 		fam[20];
}d_filter;


 
/************************************************************************
*									*
*	Some function declarations					*
*									*
************************************************************************/



/* In greta_misc.c file */

char *frame(char *title);	
char *arg_version();
long devresfile_create(char *file, char *res_text);
long file_create(char *file, char *res_text);
void loadfile_create(char *file, char *res_text);
char *filetostr(char *file);

/* In greta_sdd.c file */

void init_dev_index();
int new_dev_index();
void create_dev_struct(int , char *, Widget,Widget, Widget);
void free_dev_index(int index);
void dev_remove(int index);
void free_dev_sdd();

void init_serv_index();
int new_serv_index();
void create_serv_struct(int, char *,Widget ,Widget, Widget, Widget);
void save_text(int index);
void restore_text(int index);
void free_serv_index(int index);
void serv_remove(int index);
void free_serv_sdd ();

void init_res_index();
int new_res_index();
void create_res_struct(int, char *, Widget,Widget);
void free_res_index(int index);
void res_remove(int index);
void free_res_sdd();

void init_dev_dc_index();
int new_dev_dc_index();
void create_dev_dc_struct (int, char *, Widget, Widget ,Widget,Widget);
void free_dev_dc_index(int index);
void dev_dc_remove(int index);
void free_dev_dc_sdd();
 
/* In greta_cb.c file */

void DialogCb();
void DevPrepareUpdateCb();
void DevDismissCb();
void PrepareDevDelCb();
void DevPreparePrintCb();
void DevSaveCb();
void DevSaveShowFileSelCb();
void DevUndoCb();
void DevCutCb();
void DevCopyCb();
void DevPasteCb();
void DevDeleteCb();
void DevChainResCb();
void DevRestartCb();
void DevPingCb();
void dev_reso_text_event_hand();

void DevDcPrepareUpdateCb();
void DevDcDismissCb();
void PrepareDevDcDelCb();
void DevDcPreparePrintCb();
void DevDcSaveCb();
void DevDcSaveShowFileSelCb();
void DevDcUndoCb();
void DevDcCutCb();
void DevDcCopyCb();
void DevDcPasteCb();
void DevDcDeleteCb();
void DevDcChainResCb();
void DevDcPingCb();
void DevDcRestartCb();
void devdc_reso_text_event_hand();

void ServPrepareUpdateCb();
void ServDismissCb();
void ServUnregCb();
void PrepareServDelCb();
void ServPreparePrintCb();
void ServSaveCb();
void ServSaveShowFileSelCb();
void ServUndoCb();
void ServCutCb();
void ServCopyCb();
void ServPasteCb();
void ServDeleteCb();
void ServChainResCb();
void ServRestartCb();
void serv_reso_text_event_hand();

void ResPrepareUpdateCb();
void ResDismissCb();
void ResPreparePrintCb();
void ResSaveCb();
void ResSaveShowFileSelCb();
void ResUndoCb();
void ResCutCb();
void ResCopyCb();
void ResPasteCb();
void ResDeleteCb();

void NewServOKCb();
void NewServCancelCb();

void DevRestartSvcOK();

/* In greta_update.c file */
 
void DevManageSecRes(Widget, XtPointer,long ,long *);
void DevUpdateCb(Widget, XtPointer,XtPointer,long);
void ServUpdateCb(Widget, XtPointer ,XtPointer);
void ResUpdateCb(Widget w,XtPointer ,XtPointer );
void LoadUpdate(Widget w, XtPointer , XtPointer );
void passwd_entry_event_hand(Widget, XtPointer ,XEvent *, Boolean *);
void PasswdCancelCb(Widget, XtPointer , XtPointer );
void PasswdOkCb(Widget, XtPointer, XtPointer);

/* In greta_win.c file */

void xsDevDcHdbWindowCreate(Widget, Widget, int , char *);
void xsDeviceWindowCreate(Widget, Widget, int, char *);
void xsServerWindowCreate(Widget, Widget, int, char *);
void xsResourcesWindowCreate(Widget, Widget,int, char *);

/* In greta_xs.c file */

char *xsGetDialogTitle(Widget w);	
void xsSetTitle(Widget w,char *title);	
XmString *xsXmStringTableCreate(char *str_tbl,int nb_tbl, int elt_size);
XmString *etxsXmStringTableCreate(char **str_tbl,long nb_tbl);	
XmString *xsXmStringTableCreateFromPtrTbl(char **str_tbl, int nb_tbl);
void str_ptr_tbl_free(char **str_tbl, int nb_tbl);
char *xsListGetBrowseSelection(Widget lw);
int xsListGetItemCount(Widget lw);
char *xsListGetNthItem(Widget lw, int pos);
void xsTextPrintString(Widget tw, char *s);	
void xsTextFieldSetString(Widget tfw, char *s);
void OpenMsgBox(Widget MsgBox, char *content);
void etxsXmStringTableFree(XmStringTable xmstr_tbl, int xmstr_nb);
void xsSetCursor(Widget w, unsigned int shape);
void xsUnsetCursor(Widget w);	
int LineNbToLRPos(char *text, long *line, long *left, long *right);
int NoBlanksFieldStr(char *strin, char **strout, char **strerr);
void xsTransformToQuestionBox(Widget);
void xsTransformToInfoBox(Widget);
int xsResfileLoad(char *, Widget);
int xsCreateStringForText(char **,long ,char **);
int xsCreateStringForTextServ(char **,long ,char **);

/* In devutil.c */

void devinfo_str(db_devinfo_call *,char *,char *);
long getdc_info(char *, char *);
long gethdb_info(char *,char *);

/* In servutil.c */

long db_getserverdeviceres(char *,char *,db_svcinfo_call *,char **,long,long,long *);
long db_builddevicelist(char *,char *,db_svcinfo_call *,char **,long,long *);
long db_buildservinfo(char *,char *,db_svcinfo_call *,char **,long,long *);

/*
 * Convenience functions from utilities file.
 */
 
extern XtPointer CONVERT();
/* declaration needed to use it as parameter of qsort */
#ifndef __STDC__
extern int strcmp(); 
#endif

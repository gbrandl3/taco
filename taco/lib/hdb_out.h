#ifndef _hdb_out_h
#define _hdb_out_h

#include <hdb.h>
#include <SysNumbers.h>


/**************************************************************************
 *									  *
 *			DEFINE SYMBOLS					  *
 *									  *
 **************************************************************************/
 
/* Some specific define for the library (Most of the HDB define are in hdb.h
   file */
   
#define HDB_NOW			"Now"

#define HDB_ALL_MODES		1
#define HDB_MODE_1		2
#define HDB_MODE_2		4
#define HDB_MODE_3		8
#define HDB_MODE_4		16
#define HDB_MODE_5		32
#define HDB_MODE_6		64
#define HDB_MODE_7		128

#define HDB_SIG_ONLY		1024
#define HDB_SIG_ONLY_MASK	0x0400

#define HDB_ALL_ARRAY		512

#define HDB_DATA_DEFINED	0
#define HDB_DATA_NULL		1
#define HDB_DATA_NOT_DEFINED	2

#define HDB_INACTIVE_MODE	0
#define HDB_ACTIVE_MODE		1

#define HDB_SYNC_MODE		1
#define HDB_ASYNC_MODE		2
#define HDB_CORRELATED_MODE		3
#define HDB_INTERPOLATED_MODE		4

/* Error codes */

#define	HDB_BAD_PARAMETERS			HdbLibBase + 0
#define HDB_WRONG_DATES				HdbLibBase + 1
#define	HDB_FILTER_FLAG				HdbLibBase + 2
#define HDB_NOT_SINGLE_CONF			HdbLibBase + 3
#define HDB_DATA_ON_TAPE			HdbLibBase + 4
#define HDB_NO_DATA_FOUND			HdbLibBase + 5
#define HDB_MEM_ALLOC				HdbLibBase + 6
#define HDB_CANT_CONNECT			HdbLibBase + 7
#define HDB_ALIAS_UNKNOWN			HdbLibBase + 8
#define HDB_NAME_UNKNOWN			HdbLibBase + 9
#define HDB_UNKNOWN_SIGID			HdbLibBase + 10
#define HDB_CANT_GET_M1_DATES   		HdbLibBase + 11
#define HDB_NO_CONF_DATE   			HdbLibBase + 12
#define HDB_TOO_MANY_TAPES   			HdbLibBase + 13
#define HDB_NOT_ARRAY				HdbLibBase + 14
#define HDB_NOT_IN_ARRAY_LIM			HdbLibBase + 15
#define HDB_WRONG_FLAG				HdbLibBase + 16
#define HDB_COMMENT_TOO_LONG			HdbLibBase + 17
#define HDB_TOO_MANY_NETHOST_FOR_DOMAIN		HdbLibBase + 18
#define HDB_NO_DEFAULT_NETHOST			HdbLibBase + 19
#define HDB_CANT_CONVERT_DATE			HdbLibBase + 20
#define HDB_WRONG_TYPE				HdbLibBase + 21
#define HDB_WRONG_LENGTH			HdbLibBase + 22
#define HDB_WRONG_STORAGE_MODE			HdbLibBase + 23
#define HDB_LOW_LEVEL_LIB_FAILED		HdbLibBase + 24
#define HDB_UNKNOWN_GRPID			HdbLibBase + 25
#define HDB_NO_AVG_INFO				HdbLibBase + 26
#define HDB_NO_DEF_INFO				HdbLibBase + 27
#define HDB_NO_TAPE_INFO			HdbLibBase + 28
#define HDB_WRONG_GRP_ELT_NUMBER		HdbLibBase + 29
#define HDB_NOT_SINGLE_DEF			HdbLibBase + 30
#define HDB_NO_GRP_TABLE			HdbLibBase + 31
#define HDB_MAX_GRP_IN_CALL			HdbLibBase + 32
#define HDB_SIG_IS_GROUP_MEMBER			HdbLibBase + 33
#define HDB_SIG_ISNOT_GROUP_MEMBER		HdbLibBase + 34
#define HDB_ONE_SIG_IS_GROUP_MEMBER		HdbLibBase + 35
#define HDB_CANNOT_INTERPOLATE_STRING           HdbLibBase + 36
#define HDB_ARRAY_NOT_SUPPORTED                 HdbLibBase + 37


/**************************************************************************
 *									  *
 *			STRUCTURE DEFINITIONS				  *
 *									  *
 **************************************************************************/

typedef struct _HdbDateInt {
	char		start[HDB_DATE_BUFF];
	char		stop[HDB_DATE_BUFF];
	}HdbDateInt;

typedef struct _HdbSigData {
	void		*data_ptr;
	void		*null_ptr;
	char		**date_str;
	long		*date_long;
	long		conv_flag;
	}HdbSigData;

typedef struct _HdbSigOut {
	long		sig_type;
	long		rec_nb;
	long		array_length;
	HdbSigData	sig_data;
	}HdbSigOut;

typedef struct _HdbConfSigMode1 {
	long		flag;
	long		sample_nb;
	long		sample_date[HDB_MAX_MODE1_SAMPLE];
	char		sample_date_str[HDB_MAX_MODE1_SAMPLE][10];
	}HdbConfSigMode1;
	
typedef HdbSigMode2 HdbConfSigMode2;

typedef struct _HdbConfSigMode3 {
	long		flag;
	long		sub_mode;
	long 		udt;
	long		n;
	}HdbConfSigMode3;

typedef HdbWindowMode4 HdbConfMode4;

typedef struct _HdbConfSigMode4 {
	long		flag;
	long		sub_mode;
	long		udt;
	HdbConfMode4	x;
	HdbConfMode4	y;
	}HdbConfSigMode4;

typedef HdbRefMode5 HdbConfWinMode5;

typedef union _HdbConfRefMode5 {
	char		ch;
	short		sh;
	long		lo;
	float		fl;
	double		db;
	char		str[HDB_MODE5_STR_MAX];
	}HdbConfRefMode5;

typedef struct _HdbConfSigMode5 {
	long		flag;
	long		sub_mode;
	long		udt;
	HdbConfWinMode5	upp_win;
	HdbConfWinMode5	low_win;
	HdbConfRefMode5	ref;
	}HdbConfSigMode5;

typedef HdbSigMode6 HdbConfSigMode6;

typedef HdbSigMode7 HdbConfSigMode7;

typedef struct _HdbSigConf {
	long		sig_type;
	long		filling;
	HdbConfSigMode1	mode1;
	HdbConfSigMode2	mode2;
	HdbConfSigMode3	mode3;
	HdbConfSigMode4	mode4;
	HdbConfSigMode5	mode5;
	HdbConfSigMode6	mode6;
	HdbConfSigMode7 mode7;
	}HdbSigConf;

typedef struct _HdbTape {
	char		tape_start[HDB_DATE_BUFF];
	char		tape_stop[HDB_DATE_BUFF];
	long		tape_num;
	long 		tape_block;
	}HdbTape;

typedef struct _HdbSigInGrp {
	long		group_id;
	char		group_name[HDB_MAX_GRP_NAME_LENGTH];
	}HdbSigInGrp;

typedef struct _HdbVarSigInGrp {
	long 		length;
	HdbSigInGrp	*sequence;
	}HdbVarSigInGrp;
		
typedef struct _HdbSigInfo {
	char		sig_name[HDB_FULL_SIG_NAME_LENGTH];
	char		sig_alias[HDB_ALIAS_MAX];
	long		sig_type;
	long		sig_array_length;
	HdbVarSigInGrp	grp_member;
	}HdbSigInfo;

typedef struct _HdbSig {
	long 		sigid;
	long		array_elt;
	}HdbSig;

typedef struct _HdbMSig {
	long 		length;
	HdbSig		*sequence;
	}HdbMSig;

typedef struct _HdbMSigOut {
	long		rec_nb;
	long 		sig_nb;
	long		*sig_type_arr;
	long 		*array_length_arr;
	HdbSigData	sig_data;
	}HdbMSigOut;

typedef struct _HdbSigComment {
	char		*sigcom1;
	char		*sigcom2;
	char		*sigcom3;
	char		*sigcom4;
	}HdbSigComment;

typedef struct _HdbSigConfHist {
	char		conf_start[HDB_DATE_BUFF];
	char		conf_stop[HDB_DATE_BUFF];
	HdbSigConf	config;
	}HdbSigConfHist;

typedef struct _HdbVarSigConf {
	long		length;
	HdbSigConfHist	*sequence;
	}HdbVarSigConf;

typedef struct _HdbSigAvgHist {
	char		avg_start[HDB_DATE_BUFF];
	char		avg_stop[HDB_DATE_BUFF];
	long		point_nb;
	}HdbSigAvgHist;

typedef struct _HdbVarSigAvg {
	long		length;
	HdbSigAvgHist	*sequence;
	}HdbVarSigAvg;
	
typedef struct _HdbGrpComment {
	char		*grpcom1;
	char		*grpcom2;
	char		*grpcom3;
	char		*grpcom4;
	}HdbGrpComment;
	
typedef HdbConfSigMode1 HdbConfGrpMode1;
	
typedef HdbConfSigMode2 HdbConfGrpMode2;

typedef struct _HdbConfGrgMode4 {
	long		flag;
	long		sub_mode;
	long		udt;
	HdbConfMode4	x;
	HdbConfMode4	y;
	char 		main_sig_name[HDB_FULL_SIG_NAME_LENGTH];
	}HdbConfGrpMode4;

typedef struct _HdbConfGrpMode5 {
	long		flag;
	long		sub_mode;
	long		udt;
	HdbConfWinMode5	upp_win;
	HdbConfWinMode5	low_win;
	HdbConfRefMode5	ref;
	char		main_sig_name[HDB_FULL_SIG_NAME_LENGTH];
	}HdbConfGrpMode5;

typedef HdbConfSigMode6 HdbConfGrpMode6;

typedef HdbConfSigMode7 HdbConfGrpMode7;

typedef struct _HdbGrpConf {
	long		filling;
	HdbConfGrpMode1	mode1;
	HdbConfGrpMode2	mode2;
	HdbConfGrpMode4	mode4;
	HdbConfGrpMode5	mode5;
	HdbConfGrpMode6	mode6;
	HdbConfGrpMode7 mode7;
	}HdbGrpConf;
	
typedef struct _HdbGrpConfHist {
	char		conf_start[HDB_DATE_BUFF];
	char		conf_stop[HDB_DATE_BUFF];
	HdbGrpConf	config;
	}HdbGrpConfHist;

typedef struct _HdbVarGrpConf {
	long		length;
	HdbGrpConfHist	*sequence;
	}HdbVarGrpConf;
	
typedef struct _HdbGrpAvgHist {
	char		avg_start[HDB_DATE_BUFF];
	char		avg_stop[HDB_DATE_BUFF];
	long		point_nb;
	}HdbGrpAvgHist;

typedef struct _HdbVarGrpAvg {
	long		length;
	HdbGrpAvgHist	*sequence;
	}HdbVarGrpAvg;

typedef struct _HdbGrpDef {
	long		sig_nb;
	long		*sigid_array;
	char		**sig_name_array;
	long		*sigtype_array;
	}HdbGrpDef;
	
typedef struct _HdbGrpDefHist {
	char		def_start[HDB_DATE_BUFF];
	char		def_stop[HDB_DATE_BUFF];
	HdbGrpDef	def;
	}HdbGrpDefHist;
	
typedef struct _HdbVarGrpDef {
	long		length;
	HdbGrpDefHist	*sequence;
	}HdbVarGrpDef;

typedef struct _HdbGrpEltInData {
	DevType		sig_type;
	DevArgument	sig_data;
	}HdbGrpEltInData;
	
typedef struct _HdbVarGrpData {
	long 		length;
	HdbGrpEltInData	*sequence;
	}HdbVarGrpData;

typedef struct _HdbGrpEltOutData {
	long		sig_type;
	long 		array_length;
	char		full_sig_name[HDB_FULL_SIG_NAME_LENGTH];
	void		*data_ptr;
	void		*null_ptr;
	}HdbGrpEltOutData;
	
typedef struct _HdbGrpOut {
	long			rec_nb;
	long			sig_nb;
	HdbGrpEltOutData	*grp_data;
	long			*date_long;
	char			**date_str;
	long			conv_flag;
	}HdbGrpOut;

typedef struct _HdbVarSigUse {
	long		length;
	HdbDateInt	*sequence;
	}HdbVarSigUse;

typedef struct _HdbGMUse {
	HdbDateInt	date_int;
	long		group_id;
	long 		grp_member;
	char		group_name[HDB_MAX_GRP_NAME_LENGTH];
	char 		table[HDB_MAX_TABLE_NAME];
	}HdbGMUse;
		
typedef struct _HdbVarGMUse {
	long		length;
	HdbGMUse	*sequence;
	}HdbVarGMUse;
									
/**************************************************************************
 *									  *
 *			FUNCTIONS DECLARATION				  *
 *									  *
 **************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
long hdb_get_alias_list PT_((char ***,long *,long *));
long hdb_get_sig_list PT_((char ***,long *,long *));
long hdb_get_sigid_byalias PT_((char *,long *,long *));
long hdb_get_sigid_byname PT_((char *,long *,long *));
long hdb_get_sig_conf_dates PT_((long,HdbDateInt *,HdbDateInt **,long *,long *));
long hdb_get_sig_conf PT_((long,char *,HdbSigConf *,long *));
long hdb_get_sig_data PT_((long,long,HdbDateInt *,HdbSigOut *,long *));
long hdb_get_msig_data PT_((HdbMSig *,long,HdbDateInt *,HdbMSigOut *,long *));
long hdb_get_sig_elt PT_((long,long,long,HdbDateInt *,HdbSigOut *,long *));
long hdb_free_sig_data PT_((HdbSigOut *,long *));
long hdb_free_msig_data PT_((HdbMSigOut *,long *));
long hdb_date_convert PT_((HdbSigOut *,long *));
long hdb_date_mconvert PT_((HdbMSigOut *,long *));
long hdb_get_sig_record_nb PT_((long,long,HdbDateInt *,long *,long *));
long hdb_get_sig_tape_info PT_((long,HdbDateInt *,HdbTape **,long *,long *));
long hdb_get_sig_info PT_((long,HdbSigInfo *,long *));
long hdb_set_sig_comment PT_((long,HdbSigComment *,long *));
long hdb_get_sig_comment PT_((long,HdbSigComment *,long *));
long hdb_get_domain_list PT_((char ***,long *,long *));
long hdb_get_family_list PT_((char *,char ***,long *,long *));
long hdb_get_member_list PT_((char *,char *,char ***,long *,long *));
long hdb_get_signal_list PT_((char *,char *,char *,char ***,long *,long *));
long hdb_get_sig_conf_history PT_((long,HdbDateInt *,HdbVarSigConf *,long *));
long hdb_date_ux_to_hdb PT_((long,char *,long *));
long hdb_date_hdb_to_ux PT_((char *,long *,long *));
long hdb_get_sig_avg_history PT_((long,HdbDateInt *,HdbVarSigAvg *,long *));
long hdb_store_sig_data PT_((long,long,DevArgument,DevType,long *));
long hdb_get_sig_usage_history PT_((long,HdbDateInt *,HdbVarSigUse *,HdbVarGMUse *,long *));

long hdb_get_grp_list PT_((char ***,long *,long *));
long hdb_get_grpid_byname PT_((char *,long *,long *));
long hdb_get_grp_comment PT_((long,HdbGrpComment *,long *));
long hdb_set_grp_comment PT_((long,HdbGrpComment *,long *));
long hdb_get_grp_conf PT_((long,char *,HdbGrpConf *,long *));
long hdb_get_grp_conf_history PT_((long,HdbDateInt *,HdbVarGrpConf *,long *));
long hdb_get_grp_avg_history PT_((long,HdbDateInt *,HdbVarGrpAvg *,long *));
long hdb_get_grp_avg PT_((long,char *,long *,long *));
long hdb_get_grp_def PT_((long,char *,HdbGrpDef *,long *));
long hdb_get_grp_def_history PT_((long,HdbDateInt *,HdbVarGrpDef *,long *));
long hdb_get_grp_tape_info PT_((long,HdbDateInt *,HdbTape **,long *,long *));
long hdb_store_grp_data PT_((long,long,HdbVarGrpData *,long *));
long hdb_get_grp_record_nb PT_((long,long,HdbDateInt *,long *,long *));
long hdb_get_grp_data PT_((long,long,HdbDateInt *,HdbGrpOut *,long *));
long hdb_free_grp_data PT_((HdbGrpOut *,long *));
long hdb_grp_date_convert PT_((HdbGrpOut *,long *));
long hdb_get_grp_memb_data PT_((long,long,HdbDateInt *,long,HdbSigOut *,long *));
long hdb_disconnect PT_((void));
long hdb_change_connection PT_((char *));

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _hdb_out_h */

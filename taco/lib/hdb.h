#ifndef _hdb_h
#define _hdb_h

/**************************************************************************
 *									  *
 *			DEFINE SYMBOLS					  *
 *									  *
 **************************************************************************/

/* Storage mode definitions */

#define	HDB_MODE_OFF		0
#define	HDB_MODE_ON		1

#define HDB_MODE3_AVERAGE	2
#define HDB_MODE3_MIN		3
#define HDB_MODE3_MAX		4
#define HDB_MODE3_RMS		5

#define HDB_MODE4_WIN_REL	1
#define HDB_MODE4_WIN_ABS	3
#define HDB_MODE4_STRICT	2

#define	HDB_MODE5_GREATER	1
#define	HDB_MODE5_LOWER		2
#define	HDB_MODE5_GREATER_EQUAL	3
#define	HDB_MODE5_LOWER_EQUAL	4
#define	HDB_MODE5_WINDOW	5
#define	HDB_MODE5_STRICT	6

/* Filter flag on storage mode */

#define HDB_FILTER_ALL_BITS	0xffff
#define HDB_FIL_ALL_MODES	0x1
#define HDB_FIL_MODE1		0x2
#define HDB_FIL_MODE2		0x4
#define HDB_FIL_MODE3		0x8
#define HDB_FIL_MODE4		0x10
#define HDB_FIL_MODE5		0x20
#define HDB_FIL_MODE6		0x40
#define HDB_FIL_MODE7		0x80

#define HDB_FIL_ALL_MODES_SHIFT	0
#define HDB_FIL_MODE1_SHIFT	1
#define HDB_FIL_MODE2_SHIFT	2
#define HDB_FIL_MODE3_SHIFT	3
#define HDB_FIL_MODE4_SHIFT	4
#define HDB_FIL_MODE5_SHIFT	5
#define HDB_FIL_MODE6_SHIFT	6
#define HDB_FIL_MODE7_SHIFT	7

/* Reason column value */

#define HDB_REASON_MODE1	1
#define HDB_REASON_MODE2	2
#define HDB_REASON_MODE3	4
#define HDB_REASON_MODE4	8
#define HDB_REASON_MODE5	16
#define HDB_REASON_MODE6	32
#define HDB_REASON_MODE7	64

/* Filler extraction modes */

#define	HDB_EXT_RUN		1
#define HDB_EXT_SHUTDOWN	2
#define HDB_EXT_ALL_THE_TIME	3

/* Signal kind definition */

#define	HDB_SIMPLE		0
#define HDB_GROUP		1
#define	HDB_GROUPS		2
#define HDB_SIMP_GRP		3
#define HDB_SIMP_GRPS		4
#define HDB_UNDEF		5

/* For long term archiving */

#define	HDB_ONE_YEAR		365
#define HDB_ON_LINE_MIN		60
#define HDB_DELETE		0
#define HDB_ARCHIVE		1

/* Some maximun limit */

#define HDB_MAX_SIG_ARRAY	250
#define HDB_MAX_SIG_IN_GRP	250

/* Miscellaneous */

#define	HDB_MAX_MODE1_SAMPLE	100
#define HDB_MIN_UDT		10
#define HDB_MAX_UDT		7200
#define HDB_MIN_N		1
#define HDB_MAX_N		100
#define HDB_MAX_TAPES		100
#define HDB_MAX_MODE6_PARAMS	50

/* For the low level LIB memory management */

#define HDB_SQL_LIB_BLOCK	1000

/* Structure field length */

#define	HDB_DATE_SIZE			19
#define HDB_DATE_BUFF			30
#define HDB_ALIAS_MAX			128
#define HDB_MAX_PAR_DESC		128
#define	HDB_MODE5_STR_MAX		256
#define	HDB_COMMENT_MAX			256
#define	HDB_MAX_COMMENT			4
#define	HDB_MAX_TABLE_NAME		32
#define	HDB_MAX_GRP_NAME_LENGTH		24
#define	HDB_FILLER_NAME_LENGTH		24
#define HDB_MAX_STR_SIZE		512
#define HDB_FULL_SIG_NAME_LENGTH	96
#define HDB_SIG_NAME_LENGTH		24
#define HDB_NETHOST_LENGTH		24
#define HDB_RUN_NAME_LENGTH		25
#define HDB_MODE6_FUNC_NAME		24
#define HDB_DEV_NAME_LENGTH		48
#define HDB_DOMAIN_NAME_LENGTH		8
#define HDB_FAMILY_NAME_LENGTH		20
#define HDB_MEMBER_NAME_LENGTH		20

/* Some specific ORACLE error code */

#define HDB_ORACLE_NO_DATA		100
#define HDB_ORACLE_NO_TABLE		-942
#define HDB_ORACLE_NOT_CONNECTED	-1012
#define HDB_ORACLE_DISCONNECTED		-3114

/* Filler related define */

#define	MACHINE_FILLER			"sys/fill-mcs/1"
#define BL_FILLER			"sys/fill-bl/1"
#define DB_SERVER			"vega"


/**************************************************************************
 *									  *
 *			STRUCTURE DEFINITIONS				  *
 *									  *
 **************************************************************************/

/* For SDT table */

typedef struct _HdbSdtSignalDef {
	char 		sig_name[HDB_SIG_NAME_LENGTH];
	char		nethost[HDB_NETHOST_LENGTH];
	char		dev_name[HDB_DEV_NAME_LENGTH];
	char 		dev_dom[HDB_DOMAIN_NAME_LENGTH];
	char 		dev_fam[HDB_FAMILY_NAME_LENGTH];
	char 		dev_memb[HDB_MEMBER_NAME_LENGTH];
	char 		sig_alias[HDB_ALIAS_MAX];
	char		full_sig_name[HDB_FULL_SIG_NAME_LENGTH];
	DevArgType	sig_type;
	long		sig_array_length;
	u_int		nb_par;
	long		par[3];
	char		par_desc[3][HDB_MAX_PAR_DESC];
	long 		sig_id;
	char		fill_name[HDB_FILLER_NAME_LENGTH];
	long		sig_kind;
	long 		ext_it;
	}HdbSdtSignalDef;

/* For SMT table */

typedef struct _HdbSigMode1 {
	long	flag;
	long	sample_nb;
	long	*sample_date;
	}HdbSigMode1;

typedef struct _HdbSigMode2 {
	long 	flag;
	long	udt;
	}HdbSigMode2;

typedef struct _HdbSigMode3 {
	long 	flag;
	long 	udt;
	long 	n;
	}HdbSigMode3;

typedef union _HdbWindowMode4 {
	long	rel;
	long	abs;
	double 	abs_db;
	}HdbWindowMode4;

typedef struct _HdbSigMode4 {
	long 		flag;
	long 		udt;
	HdbWindowMode4	x;
	HdbWindowMode4 	y;
	}HdbSigMode4;

typedef union _HdbRefMode5 {
	char	ch;
	short	sh;
	long	lo;
	float	fl;
	double	db;
	}HdbRefMode5;

typedef struct _HdbSigMode5 {
	long		flag;
	long		udt;
	HdbRefMode5	upp_win;
	HdbRefMode5	low_win;
	HdbRefMode5	ref;
	char		str_ref[HDB_MODE5_STR_MAX];
	}HdbSigMode5;

typedef struct _HdbSigMode6 {
	long 	flag;
	long 	udt;
	char	func_name[HDB_MODE6_FUNC_NAME];
	long 	par_nb;
	}HdbSigMode6;

typedef struct _HdbSigMode7 {
	long flag;
	}HdbSigMode7;
	
typedef char HdbComment[HDB_MAX_COMMENT][HDB_COMMENT_MAX];

typedef struct _HdbSmtSignalMode {
	long 		sig_id;
	char		start_date[HDB_DATE_BUFF];
	char 		stop_date[HDB_DATE_BUFF];
	HdbSigMode1	mode1;
	HdbSigMode2	mode2;
	HdbSigMode3	mode3;
	HdbSigMode4	mode4;
	HdbSigMode5	mode5;
	HdbSigMode6	mode6;
	HdbSigMode7	mode7;
	long		tape_flag;
	long 		on_line_flag;
	HdbComment	sig_comment;
	}HdbSmtSignalMode;

/* For xAPT table */

typedef struct _HdbObjApt {
	long		obj_id;
	long		on_line_period;
	long		archive_mode;
	}HdbObjApt;
	
/* For GT table */

typedef struct _HdbGtGroup {
	char 	group_name[HDB_MAX_GRP_NAME_LENGTH];
	long	group_id;
	char	filler_name[HDB_FILLER_NAME_LENGTH];
	long	sgrp_flag;
	long 	ext_it;
	}HdbGtGroup;

/* For GDT table */

typedef struct _HdbGdtGroupDef {
	long	group_id;
	char	start_date[HDB_DATE_BUFF];
	char	stop_date[HDB_DATE_BUFF];
	long	sig_id;
	char	sig_table[HDB_MAX_TABLE_NAME];
	long	sig_number;
	}HdbGdtGroupDef;

/* For GMT table */

typedef struct _HdbGrpMode1 {
	long	flag;
	long 	sample_nb;
	long	*sample_date;
	}HdbGrpMode1;

typedef struct _HdbGrpMode2 {
	long	flag;
	long	udt;
	}HdbGrpMode2;

typedef struct _HdbGrpMode4 {
	long 		flag;
	long		udt;
	HdbWindowMode4	x;
	HdbWindowMode4	y;
	long 		comp_sig_id;
	}HdbGrpMode4;

typedef struct _HdbGrpMode5 {
	long		flag;
	long		udt;
	HdbRefMode5	upp_win;
	HdbRefMode5	low_win;
	HdbRefMode5	ref;
	char		str_ref[HDB_MODE5_STR_MAX];
	long		comp_sig_id;
	}HdbGrpMode5;

typedef struct _HdbGrpMode6 {
	long	flag;
	long	udt;
	char	func[HDB_MODE6_FUNC_NAME];
	long	par_nb;
	}HdbGrpMode6;
	
typedef struct _HdbGrpMode7 {
	long	flag;
	}HdbGrpMode7;
	
typedef struct _HdbGmtGroupMode {
	long		group_id;
	char		start_date[HDB_DATE_BUFF];
	char		stop_date[HDB_DATE_BUFF];
	HdbGrpMode1	mode1;
	HdbGrpMode2	mode2;
	HdbGrpMode4	mode4;
	HdbGrpMode5	mode5;
	HdbGrpMode6	mode6;
	HdbGrpMode7	mode7;
	long 		tape_flag;
	long		on_line_flag;
	HdbComment	grp_comment;
	}HdbGmtGroupMode;

typedef struct _HdbRundef {
		char name[HDB_RUN_NAME_LENGTH] ;
		long start ; /* Unix date */
		long end ;   /* Unix date */
	}HdbRundef ;

#endif /* _hdb_h */

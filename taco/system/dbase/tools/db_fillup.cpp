#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>

#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <API.h>

/* For database only */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include "config.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifdef DBM_FETCH_VOID
#include <ndbm.h>
#else
#include <ndbm_ansi.h>
#endif
#ifdef __cplusplus
}
#endif

using namespace std;

/* Function definitions */

static int from_file();
static int from_res();
static int read_file(string);
static void dev_name();
static int read_file(char *);
static int TestLine(char *,char *,int);
static void dev_line(char *,ifstream &, const string, int, char *);
static int dev_name(const string, int);
static int res_line(char *,ifstream &, const string, int , char *);
static int rs_val(string ,int);
static int ask_passwd();
static void create_db();
static void leave();

/* Global variables definitions */

vector<DBM *>	tid;
int line_ptr;
char sec_first = TRUE;
vector<string> 	tblName;
int TblNum = 0;


/****************************************************************************
*                                                                           *
*		Code for db_fillup command                                  *
*                        ---------                                          *
*                                                                           *
*    Command rule : To fill up the static database with the contents of all *
*                   the resource files founded in the directory (and sub-   *
*                   directories) pointed to by the RES_BASE_DIR environment *
*                   variable. Database will be created in the directory     *
*                   defined by DBM_DIR environment variable.                *
*		    The database name is supposed to be given by DBNAME     *
*		    environment variable and the tables (domains) by        *
*		    DBTABLES environment variable.                          *
*                                                                           *
*    Synopsis : db_fillup <0/1>                             		    *
*                                                                           *
****************************************************************************/

int main(int argc,char *argv[])
{
/* Arguments number test */
	if(argc != 2) 
	{
		cerr << "db_fillup usage : db_fillup <data source>" << endl << endl;
		cerr << "     data source parameter values" << endl;
		cerr << "               0: from resource files" << endl;
		cerr << "               1: from previously backuped database" << endl;
		exit(-1);
	}

/* Choose the right function according to data source parameter */
   	switch (int ds = atoi(argv[1])) 
   	{
		case 0 : return from_res();
		case 1 : return from_file();
		default: cout << "Bad value (" << ds << ") for the data source parameter." << endl;
	}
	return 1;
}


/****************************************************************************
*                                                                           *
*		Code for from_file  function                                *
*                        ---------                                          *
*                                                                           *
*    Function rule : To fill up the database from the contents of the       *
*                    backup files.					    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/

static int from_file()
{
    int	iret = 0;

#ifdef DEBUG
    cerr << "From backup file" << endl;
#endif /* DEBUG */

/* Build the right command string for the db_build command */
    if ((iret = system("cp $DBM_DIR/backup/* $DBM_DIR/.")) != 0)
	return iret;
    if ((iret = system("chmod 0664 $DBM_DIR/*.dir")) != 0)
	return iret;
    return system("chmod 0664 $DBM_DIR/*.pag"); 
}


/****************************************************************************
*                                                                           *
*		Code for from_res  function                                 *
*                        --------                                           *
*                                                                           *
*    Function rule : To fill up the database from the contents of the       *
*                    resource files					    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/
static int from_res()
{
	string	res_dir,
    		inter,
    		file_name,
    		temp,
    		base_dir,
    		dbm_dir,
    		dbm_file;
	char	line[256],
     		*ptr,
		*tmp;
	struct shmid_ds buf;
	int 	flags;

/* Get base directory name */

	if ((ptr = getenv("RES_BASE_DIR")) == NULL)
	{
		cerr << "db_fillup : Can't find environment variable RES_BASE_DIR" << endl;
		return (-1);
	}

	res_dir = base_dir = ptr;

/* Change working directory */
	if (chdir(res_dir.c_str()))
	{
		perror ("db_fillup : Can't cd to resource base directory \n");
		return (-1);
	}

/* Find the dbm_database files */        

	if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
	{
		cerr << "db_fillup: Can't find environment variable DBM_DIR" << endl;
		return (-1);
	}
	
	dbm_dir = ptr;
	if (dbm_dir[dbm_dir.length() - 1] != '/')
		dbm_dir += "/";

/* Create the database tables */
	create_db();

	flags = O_RDWR;

/* Open database tables of the database */
	int i = 0;
	for (vector<string>::iterator it = tblName.begin(); it != tblName.end(); it++)
	{
		string dbm_file = dbm_dir + *it;
		DBM	*t;
		t = dbm_open(const_cast<char *>(dbm_file.c_str()), flags, 0666);
		if (t == NULL)
		{
	   		 cerr << "db_fillup : Can't open " << tblName[i] << " table" << endl;
			leave();
		}
		tid.push_back(t);
		i++;
	}

/* Get the contents of this directory */
	FILE	*file;
	if ((file = popen("ls -R1p", "r")) == NULL)
	{
		cerr << "db_fillup : Can't get contents of directory" << endl;
		leave();
	}

/* pipe examination */
	while(fgets(line, sizeof(line), file) != NULL)
	{
		line[strlen(line) - 1] = '\0';		// remove newline
/* Change directory name if it's a new one */
		if (strlen(line) == 0)			// emtpy line
		{
			fgets(line,sizeof(line),file);
	    		line[strlen(line) - 1] = '\0';		// remove newline
	    		temp = string(line, strlen(line) - 1);	// remove colon
//	    		res_dir = base_dir;

#if defined(linux) || defined(sun)
	    		res_dir = "/" + temp;
#endif /* linux, sun */
		    	res_dir = temp;
		}

/* Is it a file ? If yes, build the right file name */

		else if ((strchr(line,(int)'/') == NULL) && ((tmp = strstr(line,".res")) != NULL))
		{
			inter = tmp;
			if (inter == ".res")
			{
				file_name = res_dir + "/" + line;
				if (read_file(file_name))
					leave();
				cout << "Resources from file " << file_name << " inserted" << endl;
			}
		}

	}

/* Close database */
	for (vector<DBM *>::iterator it = tid.begin(); it < tid.end(); ++it)
		dbm_close(*it);
	tid.clear();
	return fclose(file);
}


/****************************************************************************
*                                                                           *
*		Code for read_file function                                 *
*                        ---------                                          *
*                                                                           *
*    Function rule : To read a resource file                                *
*                                                                           *
*    Argin : - The name of the resource file                                *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
static int read_file(const string f_name)
{
	ifstream	fil(f_name.c_str());
	char 	line[256],
    		line1[256];

	cerr << "File name : " << f_name << endl;

/* Open resource file */
	if (!fil.is_open()) 
	{
		cerr << "db_fillup : Couldn't open resource file " << f_name << endl;
		perror("db_fillup");
		return(-1);
	}
	line_ptr = 0;

/*   If the file line is a name definition, call dev_name function. 
     If the file line is a resource value definition, call rs_val function */
	while(!fil.eof()) 
	{
        	fil.getline(line,sizeof(line)); 
		int k;
		line_ptr++;
		if (line[0] == '#') 
			continue;
        	if ((k = strlen(line)) == 0)
			continue;

		int i1;
		for (i1 = 0; i1 < k; i1++) 
			if (line[i1] != ' ')
				break;
		if (i1 == k)
			continue;

		switch(TestLine(line, line1, k)) 
		{
	    		case -1 : 	
				cerr << "db_fillup : Error in resource file " << f_name << endl
		      	   			<< "            at line " << line_ptr << endl;
				leave();
				break;
	    		case 0 :  	
				dev_line(line1,fil,f_name,sizeof(line),line);
				break;
			case 1 : 	
				if (rs_val(line1, 1)) 
				{
			     		cerr << "db_fillup : Error in resource file " << f_name << endl
				  		<< "            at line " << line_ptr << endl;
					leave();
				}
				break;
	    		case 2 : 	
				if (res_line(line1, fil, f_name, sizeof(line), line)) 
				{
			    		cerr << "db_fillup : Error in resource file " << f_name << endl
				 		<< "            at line " << line_ptr << endl;
					leave();
				}
				break;
		}
	}
	fil.close();
	return 0;
}


/****************************************************************************
*                                                                           *
*		Code for TestLine  function                                 *
*                        --------                                           *
*                                                                           *
*    Function rule : To change the line to lower case letters if it is      *
*                    necessary and to return a value which indicate which   *
*                    type of line it is (device definition, simple resource *
*                    definition or resources array definition)              *
*                                                                           *
*    Argin : - A pointer to a buffer where is stored a line of the resource *
*              file 						            *
*	     - A pointer to a buffer where the modified line will be store  *
*            - The length of the original line                              *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    Return value : -1 : Error  					    *
*                    0 : It is a device definition line                     *
*                    1 : It is a simple resource definition line            *
*                    2 : It is definition for an array of resources         *
*                                                                           *
****************************************************************************/

static int TestLine(char *line,char *line1,int k)
{
	char *tmp;
	u_int diff;
    int 	string = 0,
		iret = 1,
		i,
		l1 = 0;

/* Return error in this line is not a definition line */

	if ((tmp = strchr(line,(int)':')) == NULL) 
    {	
	cerr << "No delimiter in definition line ':' found" << endl;
		return(-1);
    }

/* Change all the letters before the : to lower case */

	diff = (u_int)(tmp - line) + 1;
	i = 0;
    for (int j = 0; j < diff; j++)
	{
	if (line[j] != ' ' && line[j] != '\t')
			line1[i++] = tolower(line[j]);
	}
	line1[i] = 0;

/* Is is a device defintion line ? In this case, all the line must be 
   translated to lower case letter */

	if (strstr(line1,"device:") != NULL)
	{
	for (int j = diff; j < k; j++)
		{
	    if (line[j] != ' ' && line[j] != '\t')
				line1[i++] = tolower(line[j]);
		}
		iret = 0;
	}

/* Now it is a resource definition line */

	else
	{

/* If the last character is \ , this is a resource array definition */

		if (line[k - 1] == '\\')
			iret = 2;

	for (int j = diff; j < k; j++)
		{

/* If the " character is detected, set a flag. If the flag is already set,
   reset it */

			if (line[j] == '"')
			{
		string = !string;
				continue;
			}

/* When the string flag is set, copy character from the original line to the
   new one without any modifications */

			if (string)
			{
				line1[i++] = line[j];
				l1 = 1;
				continue;
			}

/* If the , character is detected not in a string definition, this is a
   resource array definition */

			if (line[j] == ',')
			{
				line1[i++] = SEP_ELT;
				iret = 2;
				continue;
			}

/* Remove space and tab */
	    if (line[j] != ' ' && line[j] != '\t')
			{
				line1[i++] = line[j];
				l1 = 1;
			}
		}
	}

/* If an odd number of " character has been detected, it is an error */

	if (string)
    {
	cerr << "incomplete string (missing \"?)" << endl;
		iret = -1;
    }
/* Leave function */

	line1[i] = 0;
	if (strlen(line1) == diff && l1 == 0)
    {
	cerr << "incomplete line ??" << endl;
		iret = -1;
    }
	return(iret);

}


/****************************************************************************
*                                                                           *
*		Code for dev_line function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To extract from a resource file all the informations   *
*                    concerning the device name.                            *
*                                                                           *
*    Argin : - A pointer to a buffer where is stored a line of the resource *
*              file (The first line with the "device" word in it )          *
*            - The pointer to the FILE structure of the resource file       *
*            - Name of the resource file                                    *
*            - The size of the buffer where to stored the next line in the  *
*              resource file which defined device name                      *
*            - A pointer to a buffer where to store the next line in the    *
*              resource file which defined device name                      *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/
static void dev_line(char *line1, ifstream &file, const string f_name, int siz_line, char *line)
{
    string	temp,
		base,
		ptr;
	char *tmp;
    unsigned int diff;
    int 	i,k,j,
    		ind;
    register char *ptr1;


/* Make a copy of the device server network name */
	tmp = strchr(line1,(int)':');
	diff = (u_int)(tmp - line1) + 1;
    base = string(line1, diff);

/* Copy the first line in the result buffer */
    ptr = line1;

/* Following lines examination (discard space or tab at beginning of line) */
    k = strlen(line1);
	while(line1[k - 1] == '\\')
	{
	file.getline(line,siz_line);
		line_ptr++;
		k = strlen(line);
		j = 0;
	for (int i = 0;i < k;i++)
		{
	    if (line[i] != ' ' && line[i] != '\t')
				line1[j++] = tolower(line[i]);
		}
		line1[j] = '\0';
		k = strlen(line1);
	ptr += line1;
	}

/* Fill up db with first device name */

	ind = 1;
    ptr1 = strtok(const_cast<char *>(ptr.c_str()), "\\");
	if (dev_name(ptr1,ind))
	{	
	cerr << "db_fillup : Error in device definition in file " << f_name << endl
	     << "            at line " << line_ptr << endl;
		leave();
	} 

/* Fill up db with the other device names */
    while((ptr1 = strtok(NULL,"\\")) != NULL)
	{
		ind++;
	temp = base  + ptr1;
		if (dev_name(temp,ind))
		{
	    cerr << "db_fillup : Error in device definition in file " << f_name << endl
		 << "            at line " << line_ptr << endl;
			leave();
		} 
	}
}


/****************************************************************************
*                                                                           *
*		Code for dev_name function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To store all the devices names in the NAMES table      *
*                                                                           *
*    Argin : - The address of a buffer where is saved a device name         *
*              definition                                                   *
*              The buffer contains a string which follows this format :     * 
*              Device server network name/device:     device name           *
*	     - The number of the device in the device list		    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
static int dev_name(const string line, int numb)
{
	device dev;
    datum 		ret, 
			key, 
			keyn, 
			key_sto, 
			content;
	unsigned int diff;
    register char 	*temp; 
    string::size_type	pos;
    char 		seqnr[4],
			prgnr[20],
			keyr[MAX_KEY];
    string		lin(line);

    cerr << "dev_name = " << lin << endl;
/* Verify device name syntax */
    if (count(lin.begin(), lin.end(), '/') != 4)
		return(ERR_DEVNAME);

/* Initialize host name, device type and device class */

	strcpy(dev.h_name,"not_exp");
	strcpy(dev.d_type,"unknown");
	strcpy(dev.d_class,"unknown");
	strcpy(dev.proc_name,"unknown");

/* Get device server class */
    if ((pos = lin.find_first_of("/")) > sizeof(dev.ds_class) - 1)
    {
	cerr << "Device server class to long (max. " << (sizeof(dev.ds_class) - 1) << " chars)" << endl; 
	return (ERR_DEVNAME);
    }
    strncpy(dev.ds_class,lin.c_str(), pos);
    dev.ds_class[pos] = '\0';
    lin.erase(0, pos + 1);

/* Get device server name */
    if ((pos = lin.find_first_of("/")) > sizeof(dev.ds_name) - 1)
    {
	cerr << "Device server name to long (max. " << (sizeof(dev.ds_name) - 1) << " chars)" << endl;
	return (ERR_DEVNAME);
    }
    strncpy(dev.ds_name, lin.c_str(), pos);
    dev.ds_name[pos] = '\0';
    lin.erase(0, pos + 1);

/* Get device name */
    pos = lin.find_first_of(":");
    lin.erase(0, pos + 1);
    if (lin.length() > sizeof(dev.d_name) - 1)
    {
	cerr << "Device name to long (max. " << (sizeof(dev.d_name) - 1) << " chars)" << endl; 
		return(ERR_DEVNAME);
    }
    if (lin.empty())
	return(ERR_DEVNAME);
    strcpy(dev.d_name, lin.c_str());

/* Initialize the other columns */

	dev.pn = 0;
	dev.vn = 0;
	dev.indi = numb;
	dev.pid = 0;

#ifdef DEBUG
    cerr  << "Host name : " << dev.h_name << endl
    	<< "Device server class : " << dev.ds_class << endl
    	<< "Device server name : " << dev.ds_name << endl
    	<< "Device name : " << dev.d_name << endl
    	<< "Program number : " << dev.pn << endl
    	<< "Version number : " << dev.vn << endl
    	<< "Device type : " << dev.d_type << endl
    	<< "Device class : " << dev.d_class << endl
    	<< "Device number (in this device list) : " << dev.indi << endl
    	<< "Device server PID : " << dev.pid << endl
    	<< "Device server process name : " << dev.proc_name << endl;
#endif

/* Verify that no device with the same name is already registered in the
   database */

	key_sto.dptr = (char *)malloc(MAX_KEY); 

/* Go through the table to detect any double instance */

    vector<DBM *>::iterator 	t = tid.begin();
    for (key = dbm_firstkey(*t); key.dptr != NULL;key = dbm_nextkey(*t))
	{
		strncpy(keyr,key.dptr,key.dsize);
		keyr[key.dsize] = '\0';
		strncpy(key_sto.dptr, key.dptr, key.dsize);
		key_sto.dsize = key.dsize;
	
	ret = dbm_fetch(*t, key_sto);

		if (ret.dptr == NULL)
		{
	    cerr << "db_fillup: Error in dbm_fetch for " << key.dptr << endl;
			return(ERR_DEVNAME);
		}
		
	char *tmp = ret.dptr;
		temp = strchr(tmp,(int)'|');
		if (temp == NULL)
		{
	    cerr << "db_fillup: No separator in db tuple" << endl;
			return(ERR_DEVNAME);
		}
		diff = (u_int)(temp++ - tmp);
		strncpy(keyr, tmp, diff);
		keyr[diff] = '\0';
		if (strcmp(keyr, dev.d_name) == 0 )
		{
	    cerr << "db_fillup : Can't insert a tuple in NAMES table" << endl
		 << "Device " << dev.d_name << " already defined in the database" << endl;
			return(ERR_DEVNAME);
		}		
	}


/* Insert tuple in NAMES table */
    if ((keyn.dptr = (char *)malloc(line.length())) == NULL)
	{
	cerr << "Error in memory allocation for the key." << endl;
		leave();
	}

	strcpy(keyn.dptr, dev.ds_class);
	strcat(keyn.dptr,"|");
	strcat(keyn.dptr, dev.ds_name);
	strcat(keyn.dptr,"|");
	sprintf(seqnr,"%d",dev.indi);
	strcat(keyn.dptr, seqnr);
	strcat(keyn.dptr, "|");
	keyn.dsize = strlen(keyn.dptr);

	if ((content.dptr = (char *)malloc(MAX_CONT)) == NULL)
	{
	cerr << "Error in memory allocation for the content." << endl;
		leave();
	}
	
	strcpy(content.dptr, dev.d_name);
	strcat(content.dptr,"|");
	strcat(content.dptr, dev.h_name);
	strcat(content.dptr,"|");
	sprintf(prgnr,"%d",dev.pn);
	strcat(content.dptr, prgnr);
	strcat(content.dptr,"|");
	sprintf(prgnr,"%d",dev.vn);
	strcat(content.dptr, prgnr);
	strcat(content.dptr,"|");
	strcat(content.dptr, dev.d_type);
	strcat(content.dptr,"|");
	strcat(content.dptr, dev.d_class);
	strcat(content.dptr,"|");
	sprintf(prgnr,"%d",dev.pid);
	strcat(content.dptr,prgnr);
	strcat(content.dptr,"|");
	strcat(content.dptr,dev.proc_name);
	strcat(content.dptr,"|");

	content.dsize = strlen(content.dptr);	

    int flags = DBM_INSERT,
	i;
    if ((i = dbm_store(*t, keyn, content, flags)) != 0)
	{
	cerr << "db_fillup : Can't insert a" 
	     << ((i == 1) ?  " double " : "n erroneous")
             << " instance in NAMES table: " <<  keyn.dptr << endl;
		free(keyn.dptr);
		free(content.dptr);
		return(ERR_DEVNAME);
	}
	
	free(keyn.dptr);
	free(content.dptr);
	return(0);
}


/****************************************************************************
*                                                                           *
*		Code for res_line function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To extract from a resource file all the informations   *
*                    concerning a resource array			    *
*                                                                           *
*    Argin : - A pointer to a buffer where is stored a line of the resource *
*              file (The first line of the resource array definition)       *
*	     - The pointer to the FILE structure of the resource file       *
*            - Name of the resource file                                    *
*            - The size of the buffer where to stored the next line in the  *
*              resource file which defined the resource array               *
*            - A pointer to a buffer where to store the next line in the    *
*              resource file which defined the resource array               *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/
static int res_line(char *line1, ifstream &file,const string f_name,int siz_line,char *line)
{
	unsigned int diff;
    int 	l_base,ind,
    		string = 0,
    		k1 = 1;
	register char *ptr,*ptr1,*ptr2,*ptr3;
    char 	t_name[80],
    		base[80],
    		temp[80],
    		*tmp,
    		pat[2];

/* Make a copy of the resource array name */

	tmp = strchr(line1,(int)':');
	diff = (u_int)(tmp - line1) + 1;
	strncpy(base,line1,diff);
	base[diff] = 0;
	l_base = diff;

/* Copy the first line in the resulting buffer */
    int k = strlen(line1);
    if ((ptr = (char *)malloc(SIZE)) == NULL) 
    {
	cerr << "Not enough memory, exiting..." << endl;
		leave();
		}
	strcpy(ptr,line1);
    if (line1[k - 1] == '\\') 
		ptr[k - 1] = SEP_ELT;

/* Following line examination */
    while (line1[k - 1] == '\\') 
    {
	file.getline(line,siz_line);
		line_ptr++;
		line[strlen(line) - 1] = 0;

/* Verify the new line is not a simple resource definition */
	if ((ptr2 = strchr(line,(int)':')) != NULL) 
	{
			diff = (u_int)(ptr2 - line);
			strncpy(t_name,line,diff);
			t_name[diff] = 0;
			k = 0;
			NB_CHAR(k,t_name,'/');
	    if (k == 3) 
	    {
				line_ptr--;
				return(ERR_RESVAL);
				    }
						}

		k = strlen(line);
	int j = 0;

/* Remove space and tab characters except if they are between two ".
   Replace the , character by 0x02 except if they are between two ". */
	for (int i = 0; i < k; i++) 
	{
	    if (line[i] == '"') 
	    {
		string = !string;
				continue;
					}
	    if (string) 
	    {
				line1[j++] = line[i];
				continue;
				}
	    if (line[i] == ',') 
	    {
				line1[j++] = SEP_ELT;
				continue;
					}
	    if (line[i] != ' ' && line[i] != '\t')
				line1[j++] = line[i];
				}

/* Error if odd number of " characters */

		if (string)
	{
	    cerr << "unmatched \"" << endl;
			return(ERR_RESVAL);
	}
		line1[j] = 0;
		k = strlen(line1);

/* Add this new line to the result buffer */

		strcat(ptr,line1);
	int l = strlen(ptr);
	if (line1[k - 1] == '\\') 
	    ptr[l - 1] = (ptr[l - 2] == SEP_ELT) ? '\0' : SEP_ELT;		

/* Test to verify that the array (in ascii characters) is not bigger than
   the allocated memory and realloc memory if needed. */
    	if (l > ((k1 * SIZE) - LIM)) 
    	{
	    if ((ptr = (char *)realloc((void *)ptr,(size_t)(l + SIZE))) == 0) 
	    {
		cerr << "Not enought memory, exiting..." << endl;
				leave();
				}
			k1++;
			}
		}

/* Make two copies of the "ptr" string */

	ptr2 = (char*)malloc(strlen(ptr) + 1);
	strcpy(ptr2,ptr);
	ptr3 = (char*)malloc(strlen(ptr) + 1);
	strcpy(ptr3,ptr);

/* Fill up db with the first array element */

	ind = 1;
	pat[0] = SEP_ELT;
	pat[1] = 0;
	ptr1 = strtok(ptr3,pat);
    if (rs_val(ptr1, ind)) 
    {
		base[l_base - 1] = 0;
	cerr << "db_fillup : Error in storing " << base << " array." << endl;
		leave();
			}

/* Fill up the db with the other elements of the resource array */

/* Reinitialize strtok internal pointer */

	ptr1 = strtok(ptr,pat);
    while ((ptr1 = strtok(NULL,pat)) != NULL) 
    {
		ind++;
		strcpy(temp,base);
		strcat(temp,ptr1);
	if (rs_val(temp,ind)) 
	{
			base[l_base - 1] = 0;
	    cerr << "db_fillup : Error in storing " << base << " array." << endl;
			leave();
				}
						}

/* Free memory */

	free(ptr);
	free(ptr2);
	free(ptr3);

/* Leave function */

	return(0);

}


/****************************************************************************
*                                                                           *
*		Code for rs_val function                                    *
*                        ------                                             *
*                                                                           *
*    Function rule : To store resource definition in the static database    *
*                                                                           *
*    Argin : - The address of a buffer where is saved a resource definition *
*              The buffer contains a string which follows this format :     *
*              Resource name (DOMAIN/FAMILY/MEMBER):   resource value       *
*	     - The number of the resource if the resource type is array     *
*	       (must be one if the resource type is not an array)           *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/
static int rs_val(string lin, int ind)
{
	char		indnr[12];
	reso 		res;
	unsigned int 	diff;
	register char 	*temp,
			*tmp;
	DBM 		*tab;
	datum		key, 
			content;
	static int 	res_pas;

	string::size_type	pos = lin.find(':'),
				_pos;

/* Verify that the resource syntax is correct */

	if (pos == string::npos)
	{
		cerr << "No delimiter ':' found" << endl;
		return(ERR_RESVAL);
	}
	string resval = lin.substr(pos + 1);
	string r_name = lin.substr(0, pos);
	if (r_name.length() > (DOMAIN_NAME_LENGTH + FAMILY_NAME_LENGTH + MEMBER_NAME_LENGTH + RES_NAME_LENGTH + 3 - 1))
	{
		cerr << "Resource path to long: (max " 
		     << (DOMAIN_NAME_LENGTH + FAMILY_NAME_LENGTH + MEMBER_NAME_LENGTH + RES_NAME_LENGTH + 3 - 1) 
		     << " characters)" << endl; 
		return(ERR_RESVAL);
	}

	if (count(r_name.begin(), r_name.end(), '/') != 3)
	{
		cerr << "Resource path does not match 'table/family/member/resource'" << endl;
		return(ERR_RESVAL);
	}
/* Get table name */
	_pos = 1 + (pos = r_name.find('/'));
	if (pos > DOMAIN_NAME_LENGTH -1)
	{
		cerr << "Resource name : table name to long (max. " << (DOMAIN_NAME_LENGTH -1) << " chars)" << endl;
		return(ERR_RESVAL);
	}
	string d_name = r_name.substr(0, pos);

/* Get family name */
	pos = r_name.find('/', _pos);
	if (pos - _pos > FAMILY_NAME_LENGTH)
	{
		cerr << "Resource name : family to long (max. " << (FAMILY_NAME_LENGTH - 1) << " chars)" << endl;
		return(ERR_RESVAL);
	}
	strcpy(res.fam, r_name.substr(_pos, (pos - _pos + 1)).c_str());
	_pos = pos + 1;

/* Get member name */
	pos = r_name.find('/', _pos);
	if (pos - _pos > MEMBER_NAME_LENGTH)
	{
		cerr << "Resource name : member to long (max. " << (MEMBER_NAME_LENGTH - 1) << " chars)" << endl; 
		return(ERR_RESVAL);
	}
	strcpy(res.member, r_name.substr(_pos, (pos - _pos + 1)).c_str());
	_pos = pos + 1;

/* Get resource name */
	if (r_name.substr(_pos).length() > RES_NAME_LENGTH - 1)
	{
		cerr << "Resource name : name to long (max. " << (RES_NAME_LENGTH - 1) << " chars)" << endl;
		return(ERR_RESVAL);
	}
	strcpy(res.r_name, r_name.substr(_pos).c_str());
/* Initialise resource number */
	res.indi = ind;

/* Get resource value and leave function if it is initilised to %  */
	if (resval.length() > sizeof(res.r_val) - 1)
	{
		cerr << "Resource : value to long (max. " << (sizeof(res.r_val) - 1) <<" chars)" << endl; 
		return (ERR_RESVAL);
	}
	strcpy(res.r_val, resval.c_str());
	if (res.r_val[0] == '%')
		return(0);

/* If the resource belongs to the security domain, change the | character
   to the classical / character */
	if (d_name == "sec")
	{
		int l = strlen(res.r_name);
		for (int i = 0; i < l; i++)
		{
			if (res.r_name[i] == '|')
				res.r_name[i] = '/';
		}
	}

#ifdef DEBUG
    	cerr << "Table name : " << d_name << endl
    	 	<< "Family name : " << res.fam << endl
    	 	<< "Member name : " << res.member << endl
    	 	<< "Resource name : " << res.r_name << endl
    	 	<< "Resource value : " << res.r_val << endl
    	 	<< "Number of the resource : " << res.indi << endl << endl;
#endif

/* Select the right resource table in database */
	int i = 0;
	vector<string>::iterator it = tblName.begin();
	for (it = tblName.begin(); it != tblName.end(); it++)
	{	
		if (d_name == *it)
		{
			tab = tid[i];
			break;
		}
		i++;
	}

	if (it == tblName.end()) 
	{
		cerr << "Table name '" << d_name << "' not found." << endl;
		return(ERR_RESVAL);
	}

/* If it is a security resource (belonging to SEC domain) and it is the
   first time that a security resource is processed, ask for security
   password */
	if (d_name == "sec")
	{
		if (sec_first == TRUE)
		{
			res_pas = ask_passwd();
                        sec_first = FALSE;
		}

/* Forget Security data if the supplied password was wrong */

                 if (res_pas == -1)
                        return(0);
                                                    }
	
	if ((key.dptr = (char *)malloc(sizeof(res))) == NULL) 
	{
		cerr << "Error in memory allocation for the key." << endl;
		leave();
	}

	strcpy(key.dptr, res.fam);
	strcat(key.dptr, "|");
	strcat(key.dptr, res.member);
	strcat(key.dptr, "|");
	strcat(key.dptr, res.r_name);
	strcat(key.dptr, "|");
	sprintf(indnr,"%d",res.indi);
	strcat(key.dptr, indnr);
	strcat(key.dptr, "|");
	key.dsize = strlen(key.dptr);
	
	content.dptr = res.r_val;
	content.dsize = strlen(res.r_val);

/* Insert tuple in tables */
	int flags = DBM_INSERT;
	if ((i = dbm_store(tab, key, content, flags)) != 0) 
	{
		cerr << "db_fillup : Can't insert a"
	     		<< ((i == 1) ? " double" : "n erroneous")
	     		<< " resource: " << key.dptr << endl;
		free(key.dptr);
		return(ERR_RESVAL);
	}
	free(key.dptr);	
	return(0);
}



/****************************************************************************
*                                                                           *
*		Code for ask_passwd function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To ask for the security passwd if one is defined. This *
*		     function also checks if the passwd typed by the user   *
*		     is the right one					    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : No argout							    *
*                                                                           *
*    This function returns 0 if there is no passwd defined or if the passwd *
*    supplied by the user is the right one. The function returns -1 if the  *
*    passwd typed by the user is not correct				    *
*                                                                           *
****************************************************************************/
static int ask_passwd()
{
	char 	*user_pas,
    		file_pas[40],
    		*db_env;
	string 	f_name;
	struct stat stat_info;

/* Get the database environment variable */

	db_env = getenv("DBM_DIR");

/* Build the security file name */
	f_name = string(db_env) + "/.sec_pass";

/* Is there any password defined for the security table ? */
	if (stat(f_name.c_str(),&stat_info) == -1)
	{
       		if (errno == ENOENT)
                        return(0);
                else
		{
			cerr << "dbm_update : Can't get passwd information" << endl;
                        return(-1);
		}
	}

/* There is one password, so ask it to the user */
	if ((user_pas = (char *)getpass("Security passwd : ")) == NULL)
	{
		cerr << "dbm_update : Can't get user passwd" << endl;
		return(-1);
	}

/* Get the passwd stored in the file */
	ifstream 	file_sec(f_name.c_str());
	if (!file_sec)
	{
        	cerr << "dbm_update : Can't get passwd information" << endl;
                return(-1);
	}
	file_sec.getline(file_pas,sizeof(file_pas));

/* Compare the two passwd */
	if (user_pas == file_pas)
	{
		cerr << "dbm_update : Sorry, bad passwd. The security resources will not be updated" << endl;
		file_sec.close();
		return(-1);
	}

/* Close passwd file */
    	file_sec.close();
	return(0);

}


bool nocase_compare(char c1, char c2)
{
	return toupper(c1) == toupper(c2);
}


/****************************************************************************
*                                                                           *
*		Code for create_db function                                 *
*                        ---------                                          *
*                                                                           *
*    Function rule : To create database tables in CREATE mode and then      *
*    		     close them to be able to open them in RDWR mode.       *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/

static void create_db()
{
	int 	diff,
		ind,
     		names = False,
     		ps_names = False;
	char 	*ptr;
	string	dbm_file;
	mode_t 	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

/* Find the dbm_database table names */        

	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		fprintf(stderr,"db_fillup: Can't find environment variable DBTABLES\n");
		exit(-1);
	}
	string			env_string(ptr);

	tblName.push_back("names");
	tblName.push_back("ps_names");

/* Create database tables of the database definition */
	while(env_string.length())
	{
		string::size_type	pos = env_string.find_first_of(",");
		string t;
		if (pos != std::string::npos)
		{
			t = env_string.substr(0, pos);
			env_string.erase(0, pos + 1);
		}
		else
		{
			t = env_string;
			env_string.erase();
		}
		transform(t.begin(), t.end(), t.begin(), ::tolower);
		if (t.length() > (DOMAIN_NAME_LENGTH -1))
		{
	    		cerr << "Table name '" << t << "' too long in DBTABLES (max. " << (DOMAIN_NAME_LENGTH - 1) << " chars)" << endl;
	    		return;
		}
/* Change the database table names to lowercase letter names and check if there 
   is a names and ps_names tables defined */
		if (t == string("names"))
		{
			names = True;
			continue;	
		}
		if (t == string("ps_names"))
		{
			ps_names = True;
			continue;	
		}
		tblName.push_back(t);
		TblNum++;
		if (TblNum > MAXDOMAIN)
		{
	    		cerr << "Too many domains (" << TblNum << ") were given in the environment variable list, max (" 
				<< MAXDOMAIN << ")" << endl;
			exit(-1);
		}
    	}
		
#ifdef DEBUG
    	for (vector<string>::iterator it = tblName.begin(); it != tblName.end(); it++)
		cerr << "Name = " << *it << endl;
    	for (int i = 0; i < tblName.size(); i++)
		cerr << "Name = " << tblName[i] << endl;
#endif
			
/* Create the dbm_database files */        
	if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
	{
		cerr << "db_fillup: Can't find environment variable DBM_DIR" << endl;
		exit(-1);
	}
    	string	res_dir(ptr);
    	if (res_dir[res_dir.length() - 1]  != '/')
		res_dir += "/";

/* Create database tables of the database definition */

#if defined(linux)
    	int flags = O_CREAT | O_RDWR;
#else
    	int flags = O_CREAT;
#endif

/* Set the file permission mask in order to create db files with access for
   the rest of the world. The dbm_open */

	umask(0);
    	for (vector<string>::iterator it = tblName.begin(); it != tblName.end(); it++)
	{
		DBM	*t;
		dbm_file = res_dir + *it;
		t = dbm_open(const_cast<char *>(dbm_file.c_str()), flags, mode);
		if (t == NULL)
		{
	    		cerr <<"db_fillup : Can't create " << *it << " table" << endl;
			leave();	
		}
		tid.push_back(t);
	} 
	

/* And now close the db_files */
	for (vector<DBM *>::iterator it = tid.begin(); it != tid.end(); it++)
		dbm_close(*it);
	tid.clear();
	return;
}



/****************************************************************************
*                                                                           *
*		Code for leave function                                     *
*                        -----                                              *
*                                                                           *
*    Function rule : To close everything before to exit the process         *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
****************************************************************************/
static void leave()
{
/* Close the database  */
    for (vector<DBM *>::iterator it = tid.begin(); it != tid.end(); it++)
	if (*it != NULL)
	    dbm_close(*it);
/* Exit now */
	exit(-1);
}

/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 ESRF, www.esrf.fr
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
 * File:	db_fillup.cpp
 *
 * Description: To fill up the static database with the contents of all the 
 *		resource files found in the directory (and subdirectories) 
 *		pointed to by the RES_BASE_DIR environment variable. Database 
 *		will be created in the directory defined by DBM_DIR environment 
 *		variable. 
 * 
 * 		The database name is supposed to be given by DBNAME environment 
 *		variable and the tables (domains) by DBTABLES environment variable. 
 *
 * 		Synopsis : db_fillup <0/1>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.14 $
 *
 * Date:        $Date: 2008-04-06 09:07:47 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */
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
#include <gdbm.h>

// Function definitions 

static int 	from_file(void);
static int 	from_res(void);
static int 	read_file(std::string);
static void 	dev_name(void);
static int 	TestLine(char *,char *,int);
static void 	dev_line(char *, std::ifstream &, const std::string, int, char *);
static int 	dev_name(const std::string, int);
static int 	res_line(std::string,  std::ifstream &, const std::string, int , char *);
static int 	rs_val(std::string ,int);
static int 	ask_passwd(void);
static void 	create_db(void);
static void 	leave(void);

// Global variables definitions
std::vector<GDBM_FILE>		tid;
int 				line_ptr;
char 				sec_first = TRUE;
std::vector<std::string> 	tblName;
int 				TblNum = 0;


void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <data source>" << std::endl << std::endl;
	std::cerr << " fills the database from different sources" << std::endl;
	std::cerr << "     data source : 0 from resource files" << std::endl;
	std::cerr << "                   1 from previously backuped database" << std::endl;
	std::cerr << "     options :    -h display this message" << std::endl;
	std::cerr << "                  -v display the current version" << std::endl;
	exit(-1);
}

void version(const char *cmd)
{
	std::cerr << cmd << " version " << VERSION << std::endl;
	exit(0);
}

int main(int argc,char **argv)
{
        extern char     *optarg;
        extern int      optind,
                        opterr,
                        optopt;
        int             c;

// Argument test and domain name modification
        while((c = getopt(argc, argv, "hv")) != -1)
                switch(c)
                {
			case 'v':
				version(argv[0]);
                        case 'h':
                        case '?':
                                usage(argv[0]);
		}

//
// Arguments number test
// 
	if(optind != argc - 1) 
		usage(argv[0]);
//
// Choose the right function according to data source parameter 
//
   	switch (int ds = atoi(argv[optind])) 
   	{
		case 0 : return from_res();
		case 1 : return from_file();
		default: std::cerr << "Bad value (" << ds << ") for the data source parameter." << std::endl;
	}
	return 1;
}


/**
 * To fill up the database from the contents of the backup files.
 */
static int from_file(void)
{
	int	iret = 0;
#ifdef DEBUG
	std::cerr << "From backup file" << std::endl;
#endif /* DEBUG */

// Build the right command string for the db_build command 
	if ((iret = system("cp $DBM_DIR/backup/* $DBM_DIR/.")) != 0)
		return iret;
	if ((iret = system("chmod 0664 $DBM_DIR/*.dir")) != 0)
		return iret;
	return system("chmod 0664 $DBM_DIR/*.pag"); 
}


/**
 * To fill up the database from the contents of the resource files
 */
static int from_res(void)
{
	std::string	res_dir,
    			inter,
    			file_name,
    			temp,
    			base_dir,
    			dbm_dir,
    			dbm_file;
	char		line[256],
     			*ptr,
			*tmp;
	struct shmid_ds buf;
	int 		flags;
//
// Get base directory name 
//
	if ((ptr = getenv("RES_BASE_DIR")) == NULL)
	{
		std::cerr << "db_fillup : Can't find environment variable RES_BASE_DIR" << std::endl;
		return (-1);
	}

	res_dir = base_dir = ptr;
//
// Change working directory 
	if (chdir(res_dir.c_str()))
	{
		perror ("db_fillup : Can't cd to resource base directory \n");
		return (-1);
	}
//
// Find the dbm_database files         
//
	if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
	{
		std::cerr << "db_fillup: Can't find environment variable DBM_DIR" << std::endl;
		return (-1);
	}
	
	dbm_dir = ptr;
	if (dbm_dir[dbm_dir.length() - 1] != '/')
		dbm_dir += "/";
//
// Create the database tables 
	create_db();

	flags = GDBM_WRITER; // O_RDWR;
//
// Open database tables of the database 
	int i = 0;
	for (std::vector<std::string>::iterator it = tblName.begin(); it != tblName.end(); it++)
	{
		std::string dbm_file = dbm_dir + *it;
		GDBM_FILE t = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, 0666, NULL);
		if (t == NULL && gdbm_errno == GDBM_CANT_BE_WRITER)
		{
			flags |= GDBM_NOLOCK;
			t = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, 0666, NULL);
		}		
		if (t == NULL)
		{
	   		std::cerr << "db_fillup (" << gdbm_errno << ") : Can't open table : " << tblName[i] << std::endl;
			leave();
		}
		tid.push_back(t);
		i++;
	}
//
// Get the contents of this directory 
	FILE	*file;
	if ((file = popen("ls -R1pL", "r")) == NULL)
	{
		std::cerr << "db_fillup : Can't get contents of directory" << std::endl;
		leave();
	}
//
// pipe examination 
	while(fgets(line, sizeof(line), file) != NULL)
	{
		line[strlen(line) - 1] = '\0';		// remove newline
// Change directory name if it's a new one */
		if (strlen(line) == 0)			// emtpy line
		{
			fgets(line,sizeof(line),file);
	    		line[strlen(line) - 1] = '\0';		// remove newline
	    		temp = std::string(line, strlen(line) - 1);	// remove colon
//	    		res_dir = base_dir;
		    	res_dir = temp;
		}
//
// Is it a file ? If yes, build the right file name 
//
		else if ((strchr(line,(int)'/') == NULL) && ((tmp = strstr(line,".res")) != NULL))
		{
			inter = tmp;
			if (inter == ".res")
			{
				file_name = res_dir + "/" + line;
				if (read_file(file_name))
					leave();
				std::cout << "Resources from file " << file_name << " inserted" << std::endl;
			}
		}

	}
//
// Close database 
	for (std::vector<GDBM_FILE>::iterator it = tid.begin(); it < tid.end(); ++it)
		gdbm_close(*it);
	tid.clear();
	return fclose(file);
}


/**
 * To read a resource file
 * 
 * @param f_name The name of the resource file
 *
 * @return This function returns 0 if no errors occurs or the error code when
 *    		there is a problem.
 */
static int read_file(const std::string f_name)
{
	std::ifstream	fil(f_name.c_str());
	char 		line[256],
    			line1[256];

	std::cout << "File name : " << f_name << std::endl;
//
// Open resource file 
	if (!fil.is_open()) 
	{
		std::cerr << "db_fillup : Couldn't open resource file " << f_name << std::endl;
		perror("db_fillup");
		return(-1);
	}
	line_ptr = 0;

// If the file line is a name definition, call dev_name function. 
// If the file line is a resource value definition, call rs_val function 
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
				std::cerr << "db_fillup : Error in resource file " << f_name << std::endl
		      	   			<< "            at line " << line_ptr << std::endl;
				leave();
				break;
	    		case 0 :  	
				dev_line(line1,fil,f_name,sizeof(line),line);
				break;
			case 1 : 	
				if (rs_val(line1, 1)) 
				{
			     		std::cerr << "db_fillup : Error in resource file " << f_name << std::endl
				  		<< "            at line " << line_ptr << std::endl;
					leave();
				}
				break;
	    		case 2 : 	
				if (res_line(line1, fil, f_name, sizeof(line), line)) 
				{
			    		std::cerr << "db_fillup : Error in resource file " << f_name << std::endl
				 		<< "            at line " << line_ptr << std::endl;
					leave();
				}
				break;
		}
	}
	fil.close();
	return 0;
}

/**
 * To change the line to lower case letters if it is 
 * necessary and to return a value which indicate which
 * type of line it is (device definition, simple resource
 * definition or resources array definition)
 * 
 * @param line 	A pointer to a buffer where is stored a line of the resource file
 * @param line1	A pointer to a buffer where the modified line will be store 
 * @param k     The length of the original line
 * 
 * @return 
 *	-	-1 : Error 
 *	-        0 : It is a device definition line
 * 	-	 1 : It is a simple resource definition line 
 *      -        2 : It is definition for an array of resources 
 */
static int TestLine(char *line,char *line1,int k)
{
	char 	*tmp;
	u_int 	diff;
	int 	i_string = 0,
		iret = 1,
		i,
		l1 = 0;
//
// Return error in this line is not a definition line 
	if ((tmp = strchr(line,(int)':')) == NULL) 
	{	
		std::cerr << "No delimiter in definition line ':' found" << std::endl;
		return(-1);
	}
//
// Change all the letters before the : to lower case 
	diff = (u_int)(tmp - line) + 1;
	i = 0;
	for (int j = 0; j < diff; j++)
	{
		if (line[j] != ' ' && line[j] != '\t')
			line1[i++] = tolower(line[j]);
	}
	line1[i] = 0;
//
// Is is a device defintion line ? In this case, all the line must be 
// translated to lower case letter 
	if (strstr(line1, "device:") != NULL)
	{
		for (int j = diff; j < k; j++)
		{
			if (line[j] != ' ' && line[j] != '\t')
				line1[i++] = tolower(line[j]);
		}
		iret = 0;
	}
//
// Now it is a resource definition line 
	else
	{
// If the last character is \ , this is a resource array definition 
		if (line[k - 1] == '\\')
			iret = 2;
		for (int j = diff; j < k; j++)
		{
// If the " character is detected, set a flag. If the flag is already set, reset it 
			if (line[j] == '"')
			{
				i_string = !i_string;
				continue;
			}
// When the string flag is set, copy character from the original line to the
// new one without any modifications 
			if (i_string)
			{
				line1[i++] = line[j];
				l1 = 1;
				continue;
			}
// If the , character is detected not in a string definition, this is a
// resource array definition 
			if (line[j] == ',')
			{
				line1[i++] = SEP_ELT;
				iret = 2;
				continue;
			}
// Remove space and tab 
	    		if (line[j] != ' ' && line[j] != '\t')
			{
				line1[i++] = line[j];
				l1 = 1;
			}
		}
	}
// If an odd number of " character has been detected, it is an error 
	if (i_string)
    	{
		std::cerr << "incomplete string (missing \"?)" << std::endl;
		iret = -1;
    	}
// Leave function 
	line1[i] = 0;
	if (strlen(line1) == diff && l1 == 0)
    	{
		std::cerr << "incomplete line ??" << std::endl;
		iret = -1;
    	}
	return(iret);
}


/**
 * To extract from a resource file all the informations concerning the device name.
 *
 * @param line1 	A pointer to a buffer where is stored a line of the resource 
 *              	file (The first line with the "device" word in it )
 * @param file 		The pointer to the FILE structure of the resource file
 * @param f_name 	Name of the resource file
 * @param siz_line      The size of the buffer where to stored the next line in the 
 *              	resource file which defined device name 
 * @param line          A pointer to a buffer where to store the next line in the
 *              	resource file which defined device name
 */
static void dev_line(char *line1, std::ifstream &file, const std::string f_name, int siz_line, char *line)
{
	std::string	temp,
			base,
			ptr;
	char 		*tmp;
	unsigned int 	diff;
	int 		i,
			k,
			j,
    			ind;
    	register char 	*ptr1;

// Make a copy of the device server network name 
	tmp = strchr(line1,(int)':');
	diff = (u_int)(tmp - line1) + 1;
	base = std::string(line1, diff);

// Copy the first line in the result buffer 
	ptr = line1;

// Following lines examination (discard space or tab at beginning of line) 
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

// Fill up db with first device name */
	ind = 1;
	ptr1 = strtok(const_cast<char *>(ptr.c_str()), "\\");
	if (dev_name(ptr1,ind))
	{	
		std::cerr << "db_fillup : Error in device definition in file " << f_name << std::endl
			<< "            at line " << line_ptr << std::endl;
		leave();
	} 
//
// Fill up db with the other device names 
	while((ptr1 = strtok(NULL,"\\")) != NULL)
	{
		ind++;
		temp = base  + ptr1;
		if (dev_name(temp,ind))
		{
	    		std::cerr << "db_fillup : Error in device definition in file " << f_name << std::endl
		 		<< "            at line " << line_ptr << std::endl;
			leave();
		} 
	}
}

/**
 * To store all the devices names in the NAMES table
 * 
 * @param line The address of a buffer where is saved a device name definition 
 *             The buffer contains a string which follows this format :
 *             Device server network name/device:     device name 
 * @param numb The number of the device in the device list
 *
 * @return 	This function returns 0 if no errors occurs or the error code when    
 *    		there is a problem.
 */
static int dev_name(const std::string line, int numb)
{
	device dev;
	datum 		ret, 
			key, 
			keyn, 
			key_sto, 
			content;
	unsigned int 	diff;
	register char 	*temp; 
	std::string::size_type	pos;
	char 		seqnr[4],
			prgnr[20],
			keyr[MAX_KEY];
	std::string		lin(line);

	std::cerr << "dev_name = " << lin << std::endl;
// Verify device name syntax
#ifndef _solaris
        if (std::count(lin.begin(), lin.end(), '/') != 4)
#else
       if (_sol::count(lin.begin(), lin.end(), '/') != 4)
#endif /* _solaris */
		return(ERR_DEVNAME);

// Initialize host name, device type and device class

	strcpy(dev.h_name,"not_exp");
	strcpy(dev.d_type,"unknown");
	strcpy(dev.d_class,"unknown");
	strcpy(dev.proc_name,"unknown");

// Get device server class
	if ((pos = lin.find_first_of("/")) > sizeof(dev.ds_class) - 1)
	{
		std::cerr << "Device server class to long (max. " << (sizeof(dev.ds_class) - 1) << " chars)" << std::endl; 
		return (ERR_DEVNAME);
	}
	strncpy(dev.ds_class,lin.c_str(), pos);
	dev.ds_class[pos] = '\0';
	lin.erase(0, pos + 1);

// Get device server name
	if ((pos = lin.find_first_of("/")) > sizeof(dev.ds_name) - 1)
	{
		std::cerr << "Device server name to long (max. " << (sizeof(dev.ds_name) - 1) << " chars)" << std::endl;
		return (ERR_DEVNAME);
	}
	strncpy(dev.ds_name, lin.c_str(), pos);
	dev.ds_name[pos] = '\0';
	lin.erase(0, pos + 1);

// Get device name
	pos = lin.find_first_of(":");
	lin.erase(0, pos + 1);
	if (lin.length() > sizeof(dev.d_name) - 1)
	{
		std::cerr << "Device name to long (max. " << (sizeof(dev.d_name) - 1) << " chars)" << std::endl; 
		return(ERR_DEVNAME);
	}
	if (lin.empty())
		return(ERR_DEVNAME);
	strcpy(dev.d_name, lin.c_str());

// Initialize the other columns
	dev.pn = 0;
	dev.vn = 0;
	dev.indi = numb;
	dev.pid = 0;

#ifdef DEBUG
	std::cerr  << "Host name : " << dev.h_name << std::endl
		<< "Device server class : " << dev.ds_class << std::endl
		<< "Device server name : " << dev.ds_name << std::endl
		<< "Device name : " << dev.d_name << std::endl
		<< "Program number : " << dev.pn << std::endl
		<< "Version number : " << dev.vn << std::endl
		<< "Device type : " << dev.d_type << std::endl
		<< "Device class : " << dev.d_class << std::endl
		<< "Device number (in this device list) : " << dev.indi << std::endl
		<< "Device server PID : " << dev.pid << std::endl
		<< "Device server process name : " << dev.proc_name << std::endl;
#endif

// Verify that no device with the same name is already registered in the database
	key_sto.dptr = (char *)malloc(MAX_KEY); 

// Go through the table to detect any double instance
	std::vector<GDBM_FILE>::iterator 	t = tid.begin();
	for (key = gdbm_firstkey(*t); key.dptr != NULL;key = gdbm_nextkey(*t, key))
	{
		strncpy(keyr,key.dptr,key.dsize);
		keyr[key.dsize] = '\0';
		strncpy(key_sto.dptr, key.dptr, key.dsize);
		key_sto.dsize = key.dsize;
	
		ret = gdbm_fetch(*t, key_sto);
		if (ret.dptr == NULL)
		{
	    		std::cerr << "db_fillup: Error in gdbm_fetch for " << key.dptr << std::endl;
			return(ERR_DEVNAME);
		}
		
		char *tmp = ret.dptr;
		temp = strchr(tmp,(int)'|');
		if (temp == NULL)
		{
	    		std::cerr << "db_fillup: No separator in db tuple" << std::endl;
			return(ERR_DEVNAME);
		}
		diff = (u_int)(temp++ - tmp);
		strncpy(keyr, tmp, diff);
		keyr[diff] = '\0';
		if (strcmp(keyr, dev.d_name) == 0 )
		{
	    		std::cerr << "db_fillup : Can't insert a tuple in NAMES table" << std::endl
				<< "Device " << dev.d_name << " already defined in the database" << std::endl;
			return(ERR_DEVNAME);
		}		
	}


// Insert tuple in NAMES table
	if ((keyn.dptr = (char *)malloc(line.length())) == NULL)
	{
		std::cerr << "Error in memory allocation for the key." << std::endl;
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
		std::cerr << "Error in memory allocation for the content." << std::endl;
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

	int flags = GDBM_INSERT,
	i;
	if ((i = gdbm_store(*t, keyn, content, flags)) != 0)
	{
		std::cerr << "db_fillup : Can't insert a" 
	     		<< ((i == 1) ?  " double " : "n erroneous")
             		<< " instance in NAMES table: " <<  keyn.dptr << std::endl;
		free(keyn.dptr);
		free(content.dptr);
		return(ERR_DEVNAME);
	}
	free(keyn.dptr);
	free(content.dptr);
	return(0);
}


/**
 * To extract from a resource file all the informations
 * concerning a resource array
 * 
 * @param line1  	A pointer to a buffer where is stored a line of the resource 
 *              	file (The first line of the resource array definition)
 * @param file		The pointer to the FILE structure of the resource file
 * @param f_name 	Name of the resource file
 * @param siz_line 	The size of the buffer where to stored the next line in the
 *              	resource file which defined the resource array
 * @param line          A pointer to a buffer where to store the next line in the
 *              	resource file which defined the resource array
 */
static int res_line(std::string line1, std::ifstream &file, const std::string f_name, int siz_line, char *line)
{
	unsigned int 	diff;
    	int 		l_base,
			ind,
    			i_string = 0,
    			k1 = 1,
			k = 0;
	register char 	*ptr,
			*ptr1,
			*ptr2,
			*ptr3;
    	char 		t_name[80],
    			base[80],
    			temp[80],
    			*tmp,
    			pat[2];
//
// Make a copy of the resource array name
//
	std::string::size_type pos = line1.find(':') + 1;
	if (pos > sizeof(base) - 1)
	{
		std::cerr << "Resource \"" << line1.substr(0, pos) << "\"name is too long : max" 
			<< sizeof(base) << " chars" << std::endl;
		return 1;
	}
	strncpy(base, line1.substr(0, pos).c_str(), pos);
	base[pos] = 0;
	l_base = pos;
//
// Copy the first line in the resulting buffer
	k = line1.length();
	if (k > SIZE)
	{
		std::cerr << "Resource line \"" << line1 << "\" too long. Max " << SIZE << "chars." << std::endl;
		return 1;
	}
	if ((ptr = (char *)malloc(SIZE)) == NULL) 
	{
		std::cerr << "Not enough memory, exiting..." << std::endl;
		leave();
	}
	strcpy(ptr,line1.c_str());
	if (line1[k - 1] == '\\') 
		ptr[k - 1] = SEP_ELT;

// Following line examination
	while (line1[k - 1] == '\\') 
	{
		file.getline(line, siz_line);
		line_ptr++;
		// this chops off the last character! - andy 16nov2005
		//line[strlen(line) - 1] = 0;

// Verify the new line is not a simple resource definition 
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
// Remove space and tab characters except if they are between two ".
// Replace the , character by 0x02 except if they are between two ".
		for (int i = 0; i < k; i++) 
		{
			if (line[i] == '"') 
	    		{
				i_string = !i_string;
				continue;
			}
	    		if (i_string) 
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

// Error if odd number of " characters
		if (i_string)
		{
			std::cerr << "unmatched \"" << std::endl;
			return(ERR_RESVAL);
		}
		line1[j] = 0;
		// length() is not a reliable indicator of string size, 
		// it gives the maximum length this string ever had - andy 16nov2005
		//k = line1.length();
		k = strlen(line1.c_str());

// Add this new line to the result buffer
		strcat(ptr,line1.c_str());
		int l = strlen(ptr);
		if (line1[k - 1] == '\\') 
	    		ptr[l - 1] = (ptr[l - 2] == SEP_ELT) ? '\0' : SEP_ELT;		

// Test to verify that the array (in ascii characters) is not bigger than
// the allocated memory and realloc memory if needed.
    		if (l > ((k1 * SIZE) - LIM)) 
    		{
			if ((ptr = (char *)realloc((void *)ptr,(size_t)(l + SIZE))) == 0) 
	    		{
				std::cerr << "Not enought memory, exiting..." << std::endl;
				leave();
			}
			k1++;
		}
	}

// Make two copies of the "ptr" string 
	ptr2 = (char*)malloc(strlen(ptr) + 1);
	strcpy(ptr2,ptr);
	ptr3 = (char*)malloc(strlen(ptr) + 1);
	strcpy(ptr3,ptr);

// Fill up db with the first array element
	ind = 1;
	pat[0] = SEP_ELT;
	pat[1] = 0;
	ptr1 = strtok(ptr3,pat);
	if (rs_val(ptr1, ind)) 
	{
		base[l_base - 1] = 0;
		std::cerr << "db_fillup : Error in storing " << base << " array." << std::endl;
		leave();
	}

// Fill up the db with the other elements of the resource array
// Reinitialize strtok internal pointer 
	ptr1 = strtok(ptr,pat);
	while ((ptr1 = strtok(NULL,pat)) != NULL) 
	{
		ind++;
		strcpy(temp,base);
		strcat(temp,ptr1);
		if (rs_val(temp,ind)) 
		{
			base[l_base - 1] = 0;
			std::cerr << "db_fillup : Error in storing " << base << " array." << std::endl;
			leave();
		}
	}

// Free memory
	free(ptr);
	free(ptr2);
	free(ptr3);

// Leave function
	return(0);
}

/**
 * To store resource definition in the static database
 *
 * @param lin The  buffer where is saved a resource definition 
 *              The buffer contains a string which follows this format :  
 *              Resource name (DOMAIN/FAMILY/MEMBER):   resource value   
 * @param ind The number of the resource if the resource type is array 
 *	       (must be 1 if the resource type is not an array)      
 */
static int rs_val(std::string lin, int ind)
{
	char		indnr[12];
	reso 		res;
	unsigned int 	diff;
	register char 	*temp,
			*tmp;
	GDBM_FILE 	tab;
	datum		key, 
			content;
	static int 	res_pas;

	std::string::size_type	pos = lin.find(':'),
				_pos;

// Verify that the resource syntax is correct 
	if (pos == std::string::npos)
	{
		std::cerr << "No delimiter ':' found in line " << lin << std::endl;
		return(ERR_RESVAL);
	}
	std::string resval = lin.substr(pos + 1);
	std::string r_name = lin.substr(0, pos);
	if (r_name.length() > (DOMAIN_NAME_LENGTH + FAMILY_NAME_LENGTH + MEMBER_NAME_LENGTH + RES_NAME_LENGTH + 3 - 1))
	{
		std::cerr << "Resource path to long: (max " 
		     << (DOMAIN_NAME_LENGTH + FAMILY_NAME_LENGTH + MEMBER_NAME_LENGTH + RES_NAME_LENGTH + 3 - 1) 
		     << " characters)" << std::endl; 
		return(ERR_RESVAL);
	}

#ifndef _solaris
	if (std::count(r_name.begin(), r_name.end(), '/') != 3)
#else
	if (_sol::count(r_name.begin(), r_name.end(), '/') != 3)
#endif /* _solaris */
	{
		std::cerr << "Resource path does not match 'table/family/member/resource'" << std::endl;
		return(ERR_RESVAL);
	}
// Get table name */
	_pos = 1 + (pos = r_name.find('/'));
	if (pos > DOMAIN_NAME_LENGTH -1)
	{
		std::cerr << "Resource name : table name to long (max. " << (DOMAIN_NAME_LENGTH -1) << " chars)" << std::endl;
		return(ERR_RESVAL);
	}
	std::string d_name = r_name.substr(0, pos);

/* Get family name */
	pos = r_name.find('/', _pos);
	if (pos - _pos > FAMILY_NAME_LENGTH)
	{
		std::cerr << "Resource name : family to long (max. " << (FAMILY_NAME_LENGTH - 1) << " chars)" << std::endl;
		return(ERR_RESVAL);
	}
	strcpy(res.fam, r_name.substr(_pos, (pos - _pos)).c_str());
	_pos = pos + 1;

/* Get member name */
	pos = r_name.find('/', _pos);
	if (pos - _pos > MEMBER_NAME_LENGTH)
	{
		std::cerr << "Resource name : member to long (max. " << (MEMBER_NAME_LENGTH - 1) << " chars)" << std::endl; 
		return(ERR_RESVAL);
	}
	strcpy(res.member, r_name.substr(_pos, (pos - _pos)).c_str());
	_pos = pos + 1;

/* Get resource name */
	if (r_name.substr(_pos).length() > RES_NAME_LENGTH - 1)
	{
		std::cerr << "Resource name : name to long (max. " << (RES_NAME_LENGTH - 1) << " chars)" << std::endl;
		return(ERR_RESVAL);
	}
	strcpy(res.r_name, r_name.substr(_pos).c_str());
/* Initialise resource number */
	res.indi = ind;

/* Get resource value and leave function if it is initilised to %  */
	if (resval.length() > sizeof(res.r_val) - 1)
	{
		std::cerr << "Resource : value to long (max. " << (sizeof(res.r_val) - 1) <<" chars)" << std::endl; 
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
    	std::cerr << "Table name : " << d_name << std::endl
    	 	<< "Family name : " << res.fam << std::endl
    	 	<< "Member name : " << res.member << std::endl
    	 	<< "Resource name : " << res.r_name << std::endl
    	 	<< "Resource value : " << res.r_val << std::endl
    	 	<< "Number of the resource : " << res.indi << std::endl << std::endl;
#endif

/* Select the right resource table in database */
	int i = 0;
	std::vector<std::string>::iterator it = tblName.begin();
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
		std::cerr << "Table name '" << d_name << "' not found." << std::endl;
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
		std::cerr << "Error in memory allocation for the key." << std::endl;
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
	int flags = GDBM_INSERT;
	if ((i = gdbm_store(tab, key, content, flags)) != 0) 
	{
		std::cerr << "db_fillup : Can't insert a"
	     		<< ((i == 1) ? " double" : "n erroneous")
	     		<< " resource: " << key.dptr << std::endl;
		free(key.dptr);
		return(ERR_RESVAL);
	}
	free(key.dptr);	
	return(0);
}



/**
 * To ask for the security passwd if one is defined. This 
 * function also checks if the passwd typed by the user is the right one
 * 
 * @return This function returns 0 if there is no passwd defined or if the passwd 
 * supplied by the user is the right one. The function returns -1 if the  
 * passwd typed by the user is not correct
 */
static int ask_passwd()
{
	char 	*user_pas,
    		file_pas[40],
    		*db_env;
	std::string 	f_name;
	struct stat stat_info;

/* Get the database environment variable */

	db_env = getenv("DBM_DIR");

/* Build the security file name */
	f_name = std::string(db_env) + "/.sec_pass";

/* Is there any password defined for the security table ? */
	if (stat(f_name.c_str(),&stat_info) == -1)
	{
       		if (errno == ENOENT)
                        return(0);
                else
		{
			std::cerr << "gdbm_update : Can't get passwd information" << std::endl;
                        return(-1);
		}
	}

/* There is one password, so ask it to the user */
	if ((user_pas = (char *)getpass("Security passwd : ")) == NULL)
	{
		std::cerr << "gdbm_update : Can't get user passwd" << std::endl;
		return(-1);
	}

/* Get the passwd stored in the file */
	std::ifstream 	file_sec(f_name.c_str());
	if (!file_sec)
	{
        	std::cerr << "gdbm_update : Can't get passwd information" << std::endl;
                return(-1);
	}
	file_sec.getline(file_pas,sizeof(file_pas));

/* Compare the two passwd */
	if (user_pas == file_pas)
	{
		std::cerr << "gdbm_update : Sorry, bad passwd. The security resources will not be updated" << std::endl;
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

/**
 * To create database tables in CREATE mode and then  
 * close them to be able to open them in RDWR mode.       
 */
static void create_db(void)
{
	int 	diff,
		ind,
     		names = False,
     		ps_names = False;
	char 	*ptr;
	std::string	dbm_file;
	mode_t 	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

/* Find the dbm_database table names */        

	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		fprintf(stderr,"db_fillup: Can't find environment variable DBTABLES\n");
		exit(-1);
	}
	std::string			env_string(ptr);

	tblName.push_back("names");
	tblName.push_back("ps_names");

/* Create database tables of the database definition */
	while(env_string.length())
	{
		std::string::size_type	pos = env_string.find_first_of(",");
		std::string t;
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
		std::transform(t.begin(), t.end(), t.begin(), ::tolower);
		if (t.length() > (DOMAIN_NAME_LENGTH -1))
		{
	    		std::cerr << "Table name '" << t << "' too long in DBTABLES (max. " << (DOMAIN_NAME_LENGTH - 1) << " chars)" << std::endl;
	    		return;
		}
/* Change the database table names to lowercase letter names and check if there 
   is a names and ps_names tables defined */
		if (t == std::string("names"))
		{
			names = True;
			continue;	
		}
		if (t == std::string("ps_names"))
		{
			ps_names = True;
			continue;	
		}
		tblName.push_back(t);
		TblNum++;
		if (TblNum > MAXDOMAIN)
		{
	    		std::cerr << "Too many domains (" << TblNum << ") were given in the environment variable list, max (" 
				<< MAXDOMAIN << ")" << std::endl;
			exit(-1);
		}
    	}
		
#ifdef DEBUG
    	for (std::vector<std::string>::iterator it = tblName.begin(); it != tblName.end(); it++)
		std::cerr << "Name = " << *it << std::endl;
    	for (int i = 0; i < tblName.size(); i++)
		std::cerr << "Name = " << tblName[i] << std::endl;
#endif
			
/* Create the dbm_database files */        
	if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
	{
		std::cerr << "db_fillup: Can't find environment variable DBM_DIR" << std::endl;
		exit(-1);
	}
    	std::string	res_dir(ptr);
    	if (res_dir[res_dir.length() - 1]  != '/')
		res_dir += "/";

/* Create database tables of the database definition */

/*
#if defined(linux) || defined(FreeBSD)
    	int flags = GDBM_WRCREAT; // O_CREAT | O_RDWR;
#else
    	int flags = O_CREAT;
#endif
 */
	int flags = GDBM_WRCREAT; /* all platforms use gdbm now - andy 8nov05 */

/* Set the file permission mask in order to create db files with access for
   the rest of the world. The dbm_open */

	umask(0);
    	for (std::vector<std::string>::iterator it = tblName.begin(); it != tblName.end(); it++)
	{
		GDBM_FILE	t;
		dbm_file = res_dir + *it;
		t = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, mode, NULL);
		if (t == NULL)
		{
	    		std::cerr <<"db_fillup : Can't create " << *it << " table" << std::endl;
			leave();	
		}
		tid.push_back(t);
	} 
	

/* And now close the db_files */
	for (std::vector<GDBM_FILE>::iterator it = tid.begin(); it != tid.end(); it++)
		gdbm_close(*it);
	tid.clear();
	return;
}


/**
 * close everything before to exit the process
 */
static void leave(void)
{
/* Close the database  */
	for (std::vector<GDBM_FILE>::iterator it = tid.begin(); it != tid.end(); it++)
		if (*it != NULL)
			gdbm_close(*it);
/* Exit now */
	exit(-1);
}

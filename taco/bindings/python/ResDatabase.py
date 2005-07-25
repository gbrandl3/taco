#
# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright(c) 1994-2005 by European Synchrotron Radiation Facility,
#                     Grenoble, France
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# File:		ResDatabase.py
#
# Project:     	Automatic Beamline Alignment
#
# Description:	Generic interface class to the TACO database.
#       
# Author(s):    J.Meyer
#		$Author: jkrueger1 $
#
# Original:     Mai 2001
#
# Version:	$Revision: 1.3 $
#
# Date:		$Date: 2005-07-25 13:43:42 $
#

""" Generic interface class to the TACO database. """

__author__ = "Jens Meyer, $Author: jkrueger1 $"
__date__ = "$Date: 2005-07-25 13:43:42 $"
__revision__ = "$Revision: 1.3 $"

# Include standard built-in modules.
import types
import string

class ResDatabase:
    """ Class definition """
	
# resource path to be used
    res_path 	= "idxx/aba/test"
# database file name if the TACO database is not used
    db_file_name	= ""
# flag to indicate the usage of the TACO database or a file
    TACO_DB_flag 	= 1
# Reference to imported TACO module
    TacoDeviceObj	= None
	

    def __init__(self, dev_name, file_name=""):
        """
 	Initialise the class with the resource path "Domain/Family/Member" 
        and the database to use.
	The TACO database is the default choice. By specifying a file name 
        the file will be used as the datbase.
	
 	@param dev_name TACO device name as Domain/Family/Member
	@param file_name name of the database file to use. If not specified
                         the TACO database is used
        """
# Create the iresource path to read from the database
        self.res_path = dev_name
	
# If a file name was specified use the file instead of the TACO database
        if file_name != "":
            self.db_file_name = file_name
            self.TACO_DB_flag = 0
            db_file = open (self.db_file_name, "a")
            db_file.close()
            print "Using the file " + self.db_file_name + " as database"
        else:	
# Include the TACO module
            import TacoDevice as Taco
            self.TacoDeviceObj = Taco
            print "Using the TACO database"
			
				
    def __del__ (self):
        """ Destructor of the class """
        print "ResDatabase : called destructor"
		
		
    def read(self, res_name):
        """
 	Reads the value for a given resource name. The value is always returned
        as a string or as a list of string
 	@param res_name resource name 
	@return	Value of resource
        """
# Verify which database is used: TACO or file
        if (self.TACO_DB_flag == 1):
            # Read value from TACO database
            return self.TacoDeviceObj.dev_getresource (self.res_path, res_name)
        else:
            # Open the database file
            db_file = open (self.db_file_name, "r")
			
            # search the corresponding line
            search_line = self.res_path + "/" + res_name + ":"
			
            # Read the file content
            file_text = db_file.readlines()
			
            ret_value = ""
            for i in file_text:
                # Search for the specified resource
                if ( string.find (i, search_line) != -1 ):
                    # get the resource value
                    res_index = string.find (i, ":")
                    ret_value = i[(res_index + 1):]
						
                    # create a list in case of more than one
                    # space or comma separated values		
                    ret_value = string.strip (ret_value)
                    ret_value = string.replace (ret_value, ",", " ")
                    ret_value = string.split (ret_value)
					
                    if len(ret_value) == 0 :
                        ret_value = ""
                    if len(ret_value) == 1 :
                        ret_value = ret_value[0]
                    break
					
            # close the database file		
            db_file.close()
            return ret_value
		
		
    def write (self, res_name, res_value):
        """
 	Writes a resource and its value to the database. An existing resource 
        will be replaced and a new one will be added.
        
 	@param res_name resource name
	@param res_value resource value 
        """
	
        # Verify which database is used: TACO or file
        if (self.TACO_DB_flag == 1):
		
            # To write lists and tuples is still not working
            # with the TACO interface
			
            if type(res_value) != types.ListType or type(res_value) != types.TupleType :
                # Convert the resource value always to a string type
                if ( type(res_value) != types.StringType ):
                    res_value = str (res_value)
					
                # write resource to the TACO database
                self.TacoDeviceObj.dev_putresource(self.res_path, res_name, res_value)
            else:
                print ("Cannot write array resources!!\n")
                print ("Bug in Taco module not yet repaired!\n")
        else:
            # Open the database file for reading
            db_file = open (self.db_file_name, "r")
			
            # Convert the resource value always to a string type
            if ( type(res_value) != types.StringType ):
                res_value = str (res_value)
                res_value = string.replace (res_value, "(", "")
                res_value = string.replace (res_value, ")", "")
                res_value = string.replace (res_value, "[", "")
                res_value = string.replace (res_value, "]", "")
			
            # Read the file, # search an existing resource to replace
            # or add a new resource at the end write the changed file content
            file_text = []
            found     = 0
			
            # search the corresponding line
            search_line = self.res_path + "/" + res_name + ":"
			
            # Read the file content
            file_text = db_file.readlines()
            for i in file_text:
                # Search for the specified resource
                if ( string.find (i, search_line) != -1 ):
                    # replace the resource line
                    list_index = file_text.index(i)
                    file_text[list_index] = self.res_path + "/" + res_name + ":\t" + res_value + "\n"
                    found = 1
                    break
			
            if (found == 0):
                # Add a new resource at the end of the file content
                file_text.append (self.res_path + "/" + res_name + ":\t" + res_value + "\n")
						  
            # close the database file			  		
            db_file.close()
				 
            # write the text back to the database file
            db_file = open (self.db_file_name, "w+")
            db_file.writelines (file_text)
            db_file.close()	
        return
		
		
    def delete (self, res_name):
        """
 	Delete a resource and its value from the database.
 	@param res_name  resource name to delete
        """
	
        # Verify which database is used: TACO or file
		
        if (self.TACO_DB_flag == 1):
            # Delete the resource from the TACO database
            self.TacoDeviceObj.dev_delresource (self.res_path, res_name)	
        else:
            # Open the database file for reading
            db_file = open (self.db_file_name, "r")
			
            # Read the file, search an existing resource to delete,
            # write the changed file content
            file_text = []
			
            # search the corresponding line
            search_line = self.res_path + "/" + res_name + ":"
			
            # Read the file content
            file_text = db_file.readlines()
            for i in file_text:
                # Search for the specified resource
                if ( string.find (i, search_line) != -1 ):
                    # delete the resource line
                    file_text.remove(i)
                    break
					
            # close the database file
            db_file.close()
			
            # write the text back to the file
            db_file = open (self.db_file_name, "w+")
            db_file.writelines (file_text)
            db_file.close()		
        return
		

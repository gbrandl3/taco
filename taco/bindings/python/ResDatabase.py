#############################################################################
#
#	File:		ResDatabase.py
#
#	Project:     	Automatic Beamline Alignement
#
#	Description:	Generic interface class to the TACO database.
#       
#	Author(s):    	J.Meyer
#
#	Original:     	Mai 2001
#
#############################################################################
#
# Include standard built-in modules.
import sys
import types
import string


# Class definition

class ResDatabase:
	
	# resource path to be used
	res_path 	= "idxx/aba/test"
	# database file name if the TACO database is not used
	db_file_name	= ""
	# flag to indicate the usage of the TACO database or a file
	TACO_DB_flag 	= 1
	# Reference to imported TACO module
	TacoDeviceObj	= None
	

	###################################################################
 	# Function:     __init__()
	#
 	# Description:  Initialise the class with the resource path
	#		as "Domain/Family/Member" and the database
	#		to use.
	#		The TACO database is the default choice.
	#		By specifying a file name the file will be
	#		used as the datbase.
	#
 	# Arg(s) In:    dev_name  - TACO device name as Domain/Family/Member
	#		file_name - name of the database file to use.
	#			    If not specified the TACO database
	#			    is used
	#
 	# Arg(s) Out:   none
	###################################################################

	def __init__(self, dev_name, file_name=""):
	
		#
		# Create the iresource path to read from the database
		#
		
		self.res_path = dev_name
	
		# If a file name was specified use the file instead of 
		# the TACO database
			
		if (file_name != ""):
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
			
				
	###################################################################
 	# Function:     __del__()
	#
 	# Description:  Destructor of the class
	#
 	# Arg(s) In:    none
	#
 	# Arg(s) Out:   none
	###################################################################
		
	def __del__ (self):
		print "ResDatabase : called destructor"
		
		
	###################################################################
 	# Function:     read()
	#
 	# Description:  Reads the value for a given resource name.
	#		The value is always returned as a string
	#		or as a list of string
	#
 	# Arg(s) In:    res_name - resource name 
	#
 	# Arg(s) Out:   none
	#
	# Return:	Value of resource
	###################################################################
	
	def read (self, res_name):
		ret_value = ""
		
		# Verify which database is used: TACO or file
		
		if (self.TACO_DB_flag == 1):
			# Read value from TACO database
			ret_value = self.TacoDeviceObj.dev_getresource (self.res_path, res_name)
			
		else:
			# Open the database file
			db_file = open (self.db_file_name, "r")
			
			# search the corresponding line
			search_line = self.res_path + "/" + res_name + ":"
			
			# Read the file content
			file_text = db_file.readlines()
			
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
					
					if ( len(ret_value) == 0 ):
						ret_value = ""
					if ( len(ret_value) == 1 ):
						ret_value = ret_value[0]
					break
					
			# close the database file		
			db_file.close()
		return ret_value
		
		
	###################################################################
 	# Function:     write()
	#
 	# Description:  Writes a resource and its value to the database.
	#		An existing resource will be replaced and a new
	#		one will be added.
	#
 	# Arg(s) In:    res_name  - resource name
	#		res_value - resource value 
	#
 	# Arg(s) Out:   none
	#
	# Return:	none
	###################################################################
			
	def write (self, res_name, res_value):
	
		# Verify which database is used: TACO or file
		
		if (self.TACO_DB_flag == 1):
		
			# To write lists and tuples is still not working
			# with the TACO interface
			
			if ( type(res_value) != types.ListType or
			     type(res_value) != types.TupleType ):
			     
			     	# Convert the resource value always to 
				# a string type
				
			     	if ( type(res_value) != types.StringType ):
					res_value = str (res_value)
					
			     	# write resource to the TACO database
				self.TacoDeviceObj.dev_putresource (self.res_path, res_name, res_value)
			else:
				print ("Cannot write array resources!!\n")
				print ("Bug in Taco module not yet repaired!\n")
		else:
			# Open the database file for reading
			db_file = open (self.db_file_name, "r")
			
			#
			# Convert the resource value always to a string type
			#

			if ( type(res_value) != types.StringType ):
				res_value = str (res_value)
				res_value = string.replace (res_value,"(","")
				res_value = string.replace (res_value,")","")
				res_value = string.replace (res_value,"[","")
				res_value = string.replace (res_value,"]","")
			
			#
			# Read the file,  
			# search an existing resource to replace
			# or add a new resource at the end,
			# write the changed file content
			#
			
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
					file_text[list_index] = \
					     self.res_path + "/" + res_name + \
					     ":\t" + res_value + "\n"
					found = 1
					break
			
			if (found == 0):
				# Add a new resource at the end of the file content
				file_text.append (self.res_path + "/" + res_name + \
					          ":\t" + res_value + "\n")
						  
			# close the database file			  		
			db_file.close()

				 
			# write the text back to the database file
			db_file = open (self.db_file_name, "w+")
			db_file.writelines (file_text)
			db_file.close()	
		return
		
		
	###################################################################
 	# Function:     delete()
	#
 	# Description:  Delete a resource and its value from the database.
	#
 	# Arg(s) In:    res_name  - resource name to delete
	#
 	# Arg(s) Out:   none
	#
	# Return:	none
	###################################################################
		
	def delete (self, res_name):
	
		# Verify which database is used: TACO or file
		
		if (self.TACO_DB_flag == 1):
			
			# Delete the resource from the TACO database
			ret_value = self.TacoDeviceObj.dev_delresource (self.res_path, res_name)	
		else:
			# Open the database file for reading
			db_file = open (self.db_file_name, "r")
			
			#
			# Read the file,  
			# search an existing resource to delete,
			# write the changed file content
			#
			
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
		

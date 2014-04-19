#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <stdio.h>
#include "LabviewClient.h"


void getUserInput(const char* text, char** val_string) {
	char ch;
	int char_count = 0;
	
	printf(text);
	ch = getchar();
	while( (ch != '\n'))  {
		(*val_string)[char_count++] = ch;
		ch = getchar();
	}
	(*val_string)[char_count]='\0';
}


int main(int argc, char **argv) {

	int cmd, status;
	
	char ch;
	int char_count;
	char* ctl_str;
	char* val_str;
	char cntl_string[256];	
   char val_string[256];	
	char cmd_string[256];

	DevVarStringArray* cntl_list;
	LabviewClient* lvc = new LabviewClient();
	if (lvc->InitDevice() != DS_OK) {
		exit(-1);
	}

	ctl_str = new char[256];
	val_str = new char[256];	

	while (1)
	{	printf("\n");
		printf("Select one of the following commands :\n\n");
		printf(" 0. GetViInfo\n");
		printf(" 1. GetControlList\t  2. GetControlInfo\n");
		printf(" 3. GetDoubleValue\t  4. SetDoubleValue\n");
		printf(" 5. GetFloatValue\t  6. SetFloatValue\n");
		printf(" 7. GetShortValue\t  8. SetShortValue\n");
		printf(" 9. GetUShortValue\t 10. SetUShortValue\n");
		printf("11. GetLongValue\t 12. SetLongValue\n");
		printf("13. GetULongValue\t 14. SetULongValue\n");
		printf("15. GetStringValue\t 16. SetStringValue\n");
		printf("17. GetBooleanValue\t 18. SetBooleanValue\n");
		printf("19. Reset\t\t 20. Quit\n");
		printf("21. GetControlType\n");
		printf("Command: ");

		ch = getchar();		
		char_count = 0;
			while( (ch != '\n'))  {
				cmd_string[char_count++] = ch;
				ch = getchar();
			}
			cmd_string[char_count]='\0';

		status = sscanf(cmd_string,"%d",&cmd);

		cntl_list = new DevVarStringArray();

		switch (cmd) 
		{
	   	case 0 : 
				status = lvc->GetViInfo(cntl_list);

				if (status < 0) {
					printf("GetViInfo() failed!!");
				} 
				for(int i=0; i<cntl_list->length; i++) {
					printf("%s \n", cntl_list->sequence[i]);
				}
				 
				break;
	   	case 1 : 
				status = lvc->GetControlList(cntl_list);

				if (status < 0) {
					printf("GetControlList() failed!!");
				} 
				for(int i=0; i<cntl_list->length; i++) {
					printf("Control %d: %s \n",i , cntl_list->sequence[i]);
				}
				 
				break;
			case 2 : 
				getUserInput("Enter Control Name: ", (char**)&ctl_str);

				status = lvc->GetControlInfo((DevString)ctl_str, cntl_list);
				if (status < 0) {
					printf("GetControlInfo() failed!!\n");
				} 
				printf("Control Name:    %s \n", cntl_list->sequence[0]);
				printf("Control Type:    %s \n", cntl_list->sequence[1]);
				printf("Control Value:  %s \n",  cntl_list->sequence[2]);
				break;

			case 3 : {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
			   DevDouble dbl;	
				status = lvc->GetDoubleValue((DevString)ctl_str, &dbl);
				if (status < 0) {
					printf("GetDoubleValue() failed!!\n");
				} 
				printf("Control Name:    %s \n", ctl_str);
				printf("Control Type:    %s \n", "DevDouble");
				printf("Control Value:  %f \n", (DevDouble)dbl);
				}
				break;


			case 4: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter Double Value: ", (char**)&val_str);
				DevDouble d_value = LabviewClient::DevStringToDouble(val_str);
				printf("double: %f\n", (double)d_value);	
				status = lvc->SetDoubleValue((DevString)ctl_str, (DevDouble*)&d_value);
				if (status < 0) {
					printf("SetDoubleValue() failed!!\n");
				} 
				}
				break;

			case 5: { 
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
			   DevFloat flt;	
				status = lvc->GetFloatValue((DevString)ctl_str, &flt);
				if (status < 0) {
					printf("GetFloatValue() failed!!\n");
				} 
				printf("Control Name:    %s \n", ctl_str);
				printf("Control Type:    %s \n", "DevFloat");
				printf("Control Value:  %f \n", (DevFloat)flt);
				}
				break;


			case 6: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter Float Value: " , (char**)&val_str);
				DevFloat fl_value = LabviewClient::DevStringToFloat(val_str);	
				status = lvc->SetFloatValue((DevString)ctl_str, (DevFloat*)&fl_value);
				if (status < 0) {
					printf("SetFloatValue() failed!!\n");
				} 
				}
				break;

			case 7 : { 
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
			   DevShort shrt;	
				status = lvc->GetShortValue((DevString)ctl_str, &shrt);
				if (status < 0) {
					printf("GetShortValue() failed!!\n");
				} 
				printf("Control Name:    %s \n", ctl_str);
				printf("Control Type:    %s \n", "DevShort");
				printf("Control Value:  %d \n", (DevShort)shrt);
				}
				break;

			case 8: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter Short Value: " , (char**)&val_str);
				DevShort s_value = LabviewClient::DevStringToShort(val_str);	
				status = lvc->SetShortValue((DevString)ctl_str, (DevShort*)&s_value);
				if (status < 0) {
					printf("SetShortValue() failed!!\n");
				}
				} 
				break;

			case 9: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
			   DevUShort ushrt;	
				status = lvc->GetUShortValue((DevString)ctl_str, &ushrt);
				if (status < 0) {
					printf("GetUShortValue() failed!!\n");
				} 
				printf("Control Name:    %s \n", ctl_str);
				printf("Control Type:    %s \n", "DevUShort");
				printf("Control Value:  %d \n", (DevUShort)ushrt);
				}
				break;

			case 10: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter UShort Value: " , (char**)&val_str);
				DevUShort us_value = LabviewClient::DevStringToUShort(val_str);	
				
				status = lvc->SetUShortValue((DevString)ctl_str, (DevUShort*)&us_value);
				if (status < 0) {
					printf("SetUShortValue() failed!!\n");
				} 
				}
				break;

			case 11 : {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
			   DevLong lng;	
				status = lvc->GetLongValue((DevString)ctl_str, &lng);
				if (status < 0) {
					printf("GetLongValue() failed!!\n");
				} 
				printf("Control Name:    %s \n", ctl_str);
				printf("Control Type:    %s \n", "DevLong");
				printf("Control Value:  %d \n", (DevLong)lng);
				}
				break;

			case 12: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter Long Value: " , (char**)&val_str);
				DevLong l_value = LabviewClient::DevStringToLong(val_str);	
				
				status = lvc->SetLongValue((DevString)ctl_str, (DevLong*)&l_value);
				if (status < 0) {
					printf("SetLongValue() failed!!\n");
				} 
				}
				break;

			case 13: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);

			   DevULong ul;	
				status = lvc->GetULongValue((DevString)ctl_str, &ul);
				if (status < 0) {
					printf("GetULongValue() failed!!\n");
				} 
				printf("Control Name:    %s \n", ctl_str);
				printf("Control Type:    %s \n", "DevULong");
				printf("Control Value:  %d \n", (DevULong)ul);
				}
				break;

			case 14: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter ULong Value: " , (char**)&val_str);
				DevULong ul_value = LabviewClient::DevStringToULong(val_str);	
				
				status = lvc->SetULongValue((DevString)ctl_str, (DevULong*)&ul_value);
				if (status < 0) {
					printf("SetULongValue() failed!!\n");
				} 
				}
				break;

			case 15: 
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
			
			   {
					DevString str = new char[1024]; 
				
					status = lvc->GetStringValue((DevString)ctl_str, &str);
					if (status < 0) {
						printf("GetStringValue() failed!!\n");
					} 
					printf("Control Name:    %s \n", ctl_str);
					printf("Control Type:    %s \n", "DevString");
					printf("Control Value:  %s \n", (DevString)str);
				}
				
				break;


			case 16:
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter String Value: " , (char**)&val_str);
				{
					DevString* str = new DevString;	
					*str = new char[strlen(val_str)+1];
					strcpy(*str, val_str);
					status = lvc->SetStringValue((DevString)ctl_str, str);
					if (status < 0) {
						printf("SetStringValue() failed!!\n");
					}
				} 
				break;

			case 17 : 
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
			   DevBoolean bl;	
				status = lvc->GetBooleanValue((DevString)ctl_str, &bl);
				if (status < 0) {
					printf("GetBooleanValue() failed!!\n");
				} 
				printf("Control Name:    %s \n", ctl_str);
				printf("Control Type:    %s \n", "DevString");
				printf("Control Value:  %s \n", ((bl)?"TRUE":"FALSE"));
				
				break;


			case 18: {
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				getUserInput("Enter Boolean Value[TRUE or FALSE]: ", (char**)&val_str);
				DevBoolean bl_value = LabviewClient::DevStringToBoolean(val_str);
				
				status = lvc->SetBooleanValue((DevString)ctl_str, (DevBoolean*)&bl_value);
				if (status < 0) {
					printf("SetBooleanValue() failed!!\n");
				} 
				}
				break;
			case 21:
				getUserInput("Enter Control Name: ", (char**)&ctl_str);
				{	
				int type = lvc->GetControlType((DevString)ctl_str);
				switch (type) {
					case D_DOUBLE_TYPE:
						printf("%s is of type: D_DOUBLE_TYPE", ctl_str);
						break;
					case D_FLOAT_TYPE:
						printf("%s is of type: D_FLOAT_TYPE", ctl_str);
						break;
					case D_SHORT_TYPE:
						printf("%s is of type: D_SHORT_TYPE", ctl_str);
						break;
					case D_USHORT_TYPE:
						printf("%s is of type: D_USHORT_TYPE", ctl_str);
						break;
					case D_LONG_TYPE:
						printf("%s is of type: D_LONG_TYPE", ctl_str);
						break;
					case D_ULONG_TYPE:
						printf("%s is of type: D_ULONG_TYPE", ctl_str);
						break;
					case D_STRING_TYPE:
						printf("%s is of type: D_STRING_TYPE", ctl_str);
						break;
					case D_BOOLEAN_TYPE:
						printf("%s is of type: D_BOOLEAN_TYPE", ctl_str);
						break;
					case D_VOID_TYPE:
						printf("%s is of type: D_VOID_TYPE", ctl_str);
						break;
					default:
						printf("%s is of type: D_VOID_TYPE", ctl_str);
						break;
				}
				}
				if (status < 0) {
					printf("SetBooleanValue() failed!!\n");
				} 
				break;

			case 20 :
				exit(0);
			default : break;
		}
	}
	return 0;
}


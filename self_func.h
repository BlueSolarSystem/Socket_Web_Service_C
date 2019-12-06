//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>
#include <ctype.h>
//#include <time.h>


#define TRUE 1 
#define FALSE 0 
#define PORT 10005
//#define PORT 8888 
#define INDEX  "index.html"   //to tell the service which is the index's name 
#define ISspace(x) isspace((int)(x))
/*
	This program is designed to Build a http head for socket service
	So i seperate build function out of the orignal file.
	And rewirite it.mostly this is half finished for response service;
*/

/*
	So How to figure out this problem?


	Let's think it together


					| ---->statu_lines  (This is used for client service to know
					|					 weather it's pack is sended or not and
					|					what is th response)
	HTTP Head  -    |----->headers		(To descibe some statu of service and this http link)
					|
					|----->data  		data

*/



const char OK_string[] = "OK";
const char HTTPVERSION_string[] = "HTTP/1.1";
const char SERVER_string[] = "YMQ's WEB-serv 1.0";
const char CONTENT_Type_string[] = "text/html";
const char CHARSET_string[] = "UTF-8";
const char ERRO_CODE_string[] = "ERRO some where";

int statu_code_static = 200; //To give a statu of return 
char Not_find_char[200];//To give a feedback of find url.
long long length = 0;



/*
*
*		structures
*
*/


typedef struct statu_line   // only give point to fill it's body
{
	char * HTTP_version;
	char * statu_code;
	char * Reason_Phrase;
}STATU_LINE;


typedef struct headers
{
	// All chois above are not a must of it.
	char * Allow; //GET or POST  and so on . To tell the client which method I can respose.
	char * Content_Encoding; // UTF-8  and so on . TO tell the the client which fontset i support.
	char * Content_Length; // which http connect way it support .
	char * Content_Type; // To tell the client which content-type it include.
	char * Date;  // tell the client which time our service is.
	char * Last_Modified; //I'm not support this func
	char * Location; // when it changed ,porvide a new url to reposition the resource. I'm not support this func
	char * Server_type; // tell the client which type of server am I.
	char * Set_Cookie; //I'm not support this func now

}HEADERS;

typedef struct data   //only give point to fill it.   
{
	char* data_char;
	long long file_data_lenth; // only used when it's a file data.
	FILE * file; // only used when it's a file data.
} DATA;

typedef struct HTTP_head  //first need to malloc to fill it's body
{

	STATU_LINE *statu_line;
	HEADERS *headers;
	DATA *data;

}HTTP_HEAD;




/*
*	function voids
*
*
*/

char* get_time();
char* To_string(long long  num);
int split(char * buffer, char  dest_char[]);
int ensure_statu();
HTTP_HEAD * http_send_response(char * file_name);
char * Get_file(char file_name[]);
int Response(char * file_name);
int clear(HTTP_HEAD * head, char * str);




int ensure_statu() //i will finish this func soon
{
	//char * statu="200";
	switch (statu_code_static)
	{
	case 200:
	{
		return 200;
	}break;
	case 404:
	{
		return 404;
	}break;
	case 302:
	{
		return 301;
	}break;
	default:
	{
		return 400;// i can't understand you lol
		break;
	}

	}

	return 200;
}


STATU_LINE * provide_http_statu() //maybe the const char 's problem
{
	int len_ver;
	int len_statu_code;
	int len_Reason_Phrase;

	STATU_LINE	*statu = (STATU_LINE *)malloc(sizeof(STATU_LINE));
	//len_ver=strlen(HTTPVERSION_string);
	//statu->HTTP_version =(char *)malloc(sizeof(char)*len_ver+sizeof(char));
	statu->HTTP_version = HTTPVERSION_string;
	//len_statu_code=strlen(ensure_statu());
	//statu->statu_code=(char*)malloc(sizeof(char)*len_statu_code+sizeof(char));
	statu->statu_code = To_string(ensure_statu());
	//len_Reason_Phrase=strlen(OK_string);
	//statu->Reason_Phrase=(char*)malloc(sizeof(char)*len_Reason_Phrase+sizeof(char));
	statu->Reason_Phrase = OK_string;
	return statu;
}




HTTP_HEAD * http_send_response(char * file_name)
{	//PLz do not forget to free the malloc when release
	HTTP_HEAD *http_head = (HTTP_HEAD*)malloc(sizeof(HTTP_HEAD));
	http_head->statu_line = provide_http_statu();

	http_head->headers = (HEADERS*)malloc(sizeof(HEADERS));
	http_head->headers->Server_type = SERVER_string;
	http_head->headers->Content_Type = CONTENT_Type_string;
	http_head->headers->Content_Encoding = CHARSET_string;


	http_head->headers->Date = get_time();
	http_head->data = (DATA*)malloc(sizeof(DATA));
	http_head->data->data_char = Get_file(file_name);
	return http_head;
}
//HEADERS * provide_http_headers()
//{

//}


int split(char * buffer, char  dest_char[])  //return the file name's length
{
	char * current_buf = buffer;
	char * query_string = NULL;
	int i = 0;
	if (buffer == NULL)
	{
		return NULL;
	}
	dest_char[0] = '.';
	i++;
	

	while (i < 50)
	{

		if (*current_buf == 32)//bugs here  why 0x1 there
		{
			dest_char[i] = '\0';
			break;
		}
		dest_char[i] = *current_buf;
		current_buf++;
		i++;
	}

	if (strcmp(dest_char, "./") == 0)
	{
		i += strncpy(dest_char + 2, INDEX, strlen(INDEX));
		printf("finding %s\n", dest_char);
		return i;
	}


	query_string = dest_char;
	while ((*query_string != '?') && (*query_string != '\0'))
		query_string++;
	if (*query_string == '?')
	{
		
		*query_string = '\0';
		query_string++;
	}


	return i;
}



int clear(HTTP_HEAD * head, char * str)
{
	if (statu_code_static != 404)
	{
		free(head->data->data_char);
	}
	free(head->data);
	free(head->statu_line->statu_code);
	free(head->headers->Date);
	free(head);
	free(str);
}


int Response(char * file_name)
{
	char  file_name2[50];//length limited to 50
	split(file_name, file_name2);
	//HTTP_HEAD * http_head = (HTTP_HEAD *)malloc (sizeof(HTTP_HEAD));
	char * head = (char *)malloc(sizeof(char) * 1024);   //a read size is 1024;
	//char * start_head=head;
	memset(head, 0, 1024);
	HTTP_HEAD * http_head;
	http_head = http_send_response(file_name2);
	//http_head->statu_line->HTTP_version,http_head->statu_line->statu_code);
	strcat(head, http_head->statu_line->HTTP_version);
	strcat(head, " ");
	strcat(head, http_head->statu_line->statu_code);
	strcat(head, " ");
	strcat(head, http_head->statu_line->Reason_Phrase);
	strcat(head, "\r\n");
	strcat(head, "Server:");
	strcat(head, http_head->headers->Server_type);
	strcat(head, "\r\n");
	strcat(head, "Content-Type:");
	strcat(head, http_head->headers->Content_Type);
	strcat(head, "\r\n");
	strcat(head, "Content-Length");
	
	
	strcat(head, To_string(length));
	strcat(head, "\r\n");
	strcat(head, "Date:");
	strcat(head, http_head->headers->Date);
	strcat(head, "\r\n");
	if (length <= 900)
	{
		strcat(head, http_head->data->data_char);
		FILE *fp;
		int write_statu = -1;
		if ((fp = fopen("cache.txt", "wb")) != NULL)/*build a cache file*/
		{
			printf("Create cache successfully.");
			write_statu = fwrite(head, strlen(head) - 1, 1, fp);
			fclose(fp);
			clear(http_head, head);
			if (statu_code_static == 200)
			{
				return 200;
			}
			else
			{
				return 404;
			}

		}
		else
		{
			printf("Create cache failed");
			write_statu = fwrite(ERRO_CODE_string, strlen(ERRO_CODE_string), 1, fp);
			fclose(fp);
			clear(http_head, head);
			return 1;
		}
	}
	else
	{
		
		FILE *fp;
		int write_statu = -1;
		if ((fp = fopen("cache.txt", "wb")) != NULL)/*build a cache file*/
		{
			printf("Create cache successfully.");
			write_statu = fwrite(head, strlen(head) - 1, 1, fp);
			fclose(fp);
			

		}
		else
		{
			printf("Create cache failed");
			write_statu = fwrite(ERRO_CODE_string, strlen(ERRO_CODE_string), 1, fp);
			fclose(fp);
			clear(http_head, head);
			return 1;
		}
		
		if ((fp = fopen("cache.txt", "a+")) != NULL)/*build a cache file*/
		{
			printf("Add file successfully.");
			write_statu = fwrite(http_head->data->data_char, length +1, 1, fp);
			fclose(fp);


		}
		else
		{
			printf("Create cache failed");
			write_statu = fwrite(ERRO_CODE_string, strlen(ERRO_CODE_string), 1, fp);
			fclose(fp);
			clear(http_head, head);
			return 1;
		}

		clear(http_head, head);
		if (statu_code_static == 200)
		{
			return 200;
		}
		else
		{
			return 404;
		}

	}
	
	
	



	
	
	return 3; //erro
}



char* To_string(long long  num)//a limit of 20 bit  test ok
{
	long long num1 = num;
	int bit[20];
	int bit2[20];
	int j = 0;
	char * str = (char*)malloc(sizeof(char) * 20);
	for (int i = 0; i < 20; i++)
	{
		bit2[i] = 0;
		bit[i] = 0;
	}
	while (num1 > 0)
	{

		bit2[j] = num1 % 10;
		num1 /= 10;
		j++;
	}
	for (int i = 19, k = 0; i >= 0; i--)
	{
		if (bit2[i] != 0)
		{
			bit[k] = bit2[i];
			k++;
		}
	}
	for (int i = 0; i < 20; i++)
	{
		if (j > 20)
		{
			printf("TOO LONG num");
			break;
		}
		switch (bit[i])
		{
		case 0:
		{
			str[i] = '0';
		}break;
		case 1:
		{
			str[i] = '1';
		}break;
		case 2:
		{
			str[i] = '2';
		}break;
		case 3:
		{
			str[i] = '3';
		}break;
		case 4:
		{
			str[i] = '4';
		}break;
		case 5:
		{
			str[i] = '5';
		}break;
		case 6:
		{
			str[i] = '6';
		}break;
		case 7:
		{
			str[i] = '7';
		}break;
		case 8:
		{
			str[i] = '8';
		}break;
		case 9:
		{
			str[i] = '9';
		}break;
		default:
		{
			printf("ERRO in strchange");
			break;
		}
		}
		if (i == j)
		{
			str[i] = '\0';
			break;
		}
	}
	//printf("%s",str);
	return str;
}





char* get_time()//here got a servier problem , cause by var "info".
{
	unsigned long rawtime;
	struct tm * info = (struct tm *)malloc(56);//sizeof(struct tm)
	char * Char_time = (char *)malloc(sizeof(char) * 128);
	time(&rawtime);
	info = localtime(&rawtime);
	asm("mov %r10,- 0x18(%rbp) ");
	strftime(Char_time, 128, "Date:%a,%d %b %G %X\r\n", info);
	//printf("格式化的日期 & 时间 : |%s|\n", Char_time);
	return Char_time;
}

char* Get_file(char file_name[])//This is test OK in all.
{
	long long file_len;
	FILE *res_file;
	if(!strcmp(file_name,"/flag") )
	{
		sprintf(Not_find_char, "[Web] The file [%s] is not existed\n", file_name);
		statu_code_static = 404;
		return Not_find_char;
	}
	res_file = fopen(file_name, "rb+");
	int j = 0;
	char * data = NULL;
	if (res_file == NULL)
	{
		//printf("[Web] The file [%s] is not existed\n", file_name);
		sprintf(Not_find_char, "[Web] The file [%s] is not existed\n", file_name);
		statu_code_static = 404;
		return Not_find_char;
	}

	fseek(res_file, 0, SEEK_END);
	file_len = ftell(res_file);
	fseek(res_file, 0, SEEK_SET);
	//this place can record it's length and make it quicker
	data = (char *)malloc(sizeof(char)*file_len + 1);
	fread(data, 1, file_len, res_file);
	statu_code_static = 200;
	length = file_len;
	return data;
}


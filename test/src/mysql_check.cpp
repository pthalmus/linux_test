/*
 * mysql_check.cpp
 *
 *  Created on: 2022. 1. 20.
 *      Author: s9342931
 */
#include<iostream>
#include<mysql/mysql.h>
#include<string.h>

class FFError
{
public:
	std::string Label;

	FFError() { Label = (char*)"Generic Error"; }
	FFError(char* msg) { Label = msg; }
	~FFError() {};
	inline const char* GetMesssage() { return Label.c_str(); }
};

int main()
{
	MYSQL *mysql;
	MYSQL *MYSQL_Connection = NULL;

	std::string hostName = "127.0.0.1";
	std::string userId = "root";
	std::string password = "kiiamm12";
	std::string DB = "GAMEDB";

	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;

	MYSQL_ROW mysqlRow;
	MYSQL_FIELD *mysqlFields;
	my_ulonglong numRows;
	unsigned int numFields;

	MYSQL_Connection = mysql_init(NULL);

	mysql = mysql_real_connect(MYSQL_Connection,hostName.c_str(),userId.c_str(),password.c_str(),DB.c_str(),0,NULL,0);

	if(mysql == NULL)
	{
		throw FFError( (char*)mysql_error(MYSQL_Connection));
	}
	printf("MYSQL Connection info %s\n",mysql_get_host_info(MYSQL_Connection));
	printf("MYSQL Client info %s\n",mysql_get_client_info());
	printf("MYSQL Server info %s\n",mysql_get_server_info(MYSQL_Connection));



	if(mysql_res)
	{
		mysql_free_result(mysql_res);
		mysql_res = NULL;
	}



	try
	{
		std::string sqlSelStatement = "SHOW TABLES";
		mysql_status = mysql_query(MYSQL_Connection, sqlSelStatement.c_str());

		if(mysql_status)
		{
			throw FFError( (char*)mysql_error(MYSQL_Connection));
		}
		else
		{
			mysql_res = mysql_store_result(MYSQL_Connection);
		}
		if(mysql_res)
		{
			numRows = mysql_num_rows(mysql_res);
			numFields = mysql_field_count(MYSQL_Connection);
			numFields = mysql_num_fields(mysql_res);

			printf("Number of rows = %ld , Number of fields = %d \n", numRows, numFields);
		}
		else
		{
			printf("Result set is empty");
		}

		mysqlFields = mysql_fetch_fields(mysql_res);
		for(int jj=0; jj < numFields; jj++)
		{
			printf("%s\t",mysqlFields[jj].name);
		}
		printf("\n");

		while(mysqlRow = mysql_fetch_row(mysql_res))
		{
			for(int ii=0; ii < numFields; ii++)
			{
				printf("%s\t", mysqlRow[ii] ? mysqlRow[ii] : "NULL");
			}
			printf("\n");
		}
		if(mysql_res)
		{
			mysql_free_result(mysql_res);
			mysql_res = NULL;
		}
	}
	catch(FFError e)
	{
		printf("%s\n", e.Label.c_str());
		mysql_close(MYSQL_Connection);
		return 1;
	}
	mysql_close(MYSQL_Connection);

	return 0;
}




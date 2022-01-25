/*
 * mysql_Injection.h
 *
 *  Created on: 2022. 1. 24.
 *      Author: s9342931
 */

#ifndef MYSQL_INJECTION_H_
#define MYSQL_INJECTION_H_

#include<iostream>
#include<mysql/mysql.h>
#include<string.h>

#define MAX_LENGTH 150

class SQLError
{
public:
	std::string Label;

	SQLError() { Label = (char*)"Generic Error"; }
	SQLError(char* msg) { Label = msg; }
	~SQLError() {};
	inline const char* GetMesssage() { return Label.c_str(); }
};


bool make_Room(MYSQL *mysql, int room_Num, char* room_Name, char* room_PWD, int cur_Player, int max_Player, int status )
{
	char sqlStatement[MAX_LENGTH];
	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;

	try{

	}
	catch(SQLError e)
	{
		printf("%s\n", e.Label.c_str());
	}
	sprintf(sqlStatement, "INSERT INTO room VALUES(%d,'%s','%s',%d,%d,%d);",room_Num,room_Name,room_PWD,cur_Player,max_Player,status);
	mysql_status = mysql_query(mysql, sqlStatement);
	return true;
}
bool del_Room(MYSQL *mysql, int room_Num)
{
	char sqlStatement[MAX_LENGTH];
	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;

	sprintf(sqlStatement, "DELETE FROM room WHERE Room_Num = %d;",room_Num);
	mysql_status = mysql_query(mysql, sqlStatement);
	return true;
}

bool reg_Player(MYSQL *mysql, std::string userName, std::string userID)
{
	char sqlStatement[MAX_LENGTH];
	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;

	memset(&sqlStatement,0,sizeof(sqlStatement));

	try{
		sprintf(sqlStatement, "INSERT INTO user(userName,userID) VALUES('%s','%s');",userName.c_str(),userID.c_str());
		mysql_status = mysql_query(mysql, sqlStatement);
		if(mysql_status)
		{
			throw SQLError( (char*)mysql_error(mysql));
		}
		else
		{
			return true;
		}
	}
	catch(SQLError e)
	{
		printf("%s\n", e.Label.c_str());
	}

	return false;
}
bool del_Player(MYSQL *mysql, std::string userID)
{
	char sqlStatement[MAX_LENGTH];
	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;

	memset(&sqlStatement,0,sizeof(sqlStatement));

	try{
		sprintf(sqlStatement, "DELETE FROM user where userID = '%s';",userID.c_str());
		mysql_status = mysql_query(mysql, sqlStatement);
		if(mysql_status)
		{
			throw SQLError( (char*)mysql_error(mysql));
		}
		else
		{
			return true;
		}
	}
	catch(SQLError e)
	{
		printf("%s\n", e.Label.c_str());
	}

	return false;
}
MYSQL_RES* selcet_Player(MYSQL *mysql, std::string userID)
{
	char sqlStatement[MAX_LENGTH];
	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;
	MYSQL_ROW mysqlRow;

	if(mysql_res)
	{
		mysql_free_result(mysql_res);
		mysql_res = NULL;
	}

	memset(&sqlStatement,0,sizeof(sqlStatement));

	try{
		sprintf(sqlStatement, "select * from user where userID = '%s';",userID.c_str());
		mysql_status = mysql_query(mysql, sqlStatement);
		if(mysql_status)
		{
			throw SQLError( (char*)mysql_error(mysql));
		}
		else
		{
			mysql_res = mysql_store_result(mysql);
		}
		if(mysql_res)
		{
			return mysql_res;
		}
		else
		{
			printf("result set is empty!");
		}
	}
	catch(SQLError e)
	{
		printf("%s\n", e.Label.c_str());
	}

	return mysql_res;
}
#endif /* MYSQL_INJECTION_H_ */

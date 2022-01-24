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
bool reg_Player(MYSQL *mysql, char *user_Name, char* UID)
{
	char sqlStatement[MAX_LENGTH];
	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;

	try{
		sprintf(sqlStatement, "INSERT INTO user(user_Name,UID) VALUES('%s','%s');",user_Name,UID);
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
			return true;
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

	return false;
}



#endif /* MYSQL_INJECTION_H_ */

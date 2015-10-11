#include "Connection.h"
#include "Result.h"
#include "Statement.h"

#include <stdio.h>
#include "mariadb/mysql.h"

Connection::Connection()
	: bIsConnected(false)
{
}

Connection::~Connection()
{
	if (bIsConnected)
	{
		Close();
	}
}

bool Connection::Connect(char const* Host, char const* User, char const* Password, char const* DB, unsigned int Port)
{
	MySQL = mysql_init(NULL);
	if (!mysql_real_connect(MySQL, Host, User, Password, DB, Port, NULL, 0))
	{
		fprintf(stderr, "Error: Database Connection Refused\n");
		ShowMySQLError(MySQL, "mysql_real_connect()");
		mysql_close(MySQL);
		return false;
	}

	bIsConnected = true;
	return true;
}

void Connection::Close()
{
	if (bIsConnected)
	{
		mysql_close(MySQL);
		MySQL = nullptr;
	}
}

bool Connection::IsConnected()
{
	return bIsConnected;
}

int Connection::Query(char const* QueryStr, unsigned long Length)
{
	return Query(QueryStr, Length, NULL);
}

int Connection::Query(char const* QueryStr, unsigned long Length, Result* ResultOut)
{
	MYSQL_RES* Result;
	if (mysql_real_query(MySQL, QueryStr, Length))
	{
		ShowMySQLError(MySQL, "mysql_real_query()");
		return -1;
	}

	Result = mysql_store_result(MySQL);
	if (Result)
	{
		if (ResultOut)
		{
			int RetVal = ResultOut->CreateResult(Result);

			if (RetVal == ((my_ulonglong)-1))
			{
				mysql_free_result(Result);
				return -1;
			}
		}

		if (mysql_errno(MySQL))
		{
			ShowMySQLError(MySQL, "mysql_fetch_row()");
			mysql_free_result(Result);
			return -1;
		}

		mysql_free_result(Result);
	}
	else
	{
		if (mysql_errno(MySQL))
		{
			ShowMySQLError(MySQL, "mysql_store_result()");
			return -1;
		}
	}

	my_ulonglong AffectedRows = mysql_affected_rows(MySQL);
	if (AffectedRows == ((my_ulonglong)-1)) return -1;

	return AffectedRows;
}

bool Connection::Execute(Statement* stmt)
{
	return true;
}

void Connection::ShowMySQLError(st_mysql* mysql, char const* call)
{
	fprintf(stderr, "%s: Error(%d) [%s] \"%s\"\n", call,
		mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
}

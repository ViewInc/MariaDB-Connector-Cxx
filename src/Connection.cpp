#include "Connection.h"
#include "Result.h"
#include "Statement.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Transaction.h>
#include "mysql.h"

// Typical Visual Studio mess.
#if defined(_MSC_VER) && _MSC_VER < 1900
#include <stdarg.h>
#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

inline int c99_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap)
{
	int count = -1;

	if (size != 0)
		count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
	if (count == -1)
		count = _vscprintf(format, ap);

	return count;
}

inline int c99_snprintf(char *outBuf, size_t size, const char *format, ...)
{
	int count;
	va_list ap;

	va_start(ap, format);
	count = c99_vsnprintf(outBuf, size, format, ap);
	va_end(ap);

	return count;
}
#endif

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
		MySQL = NULL;
	}
}

bool Connection::IsConnected()
{
	return bIsConnected;
}

bool Connection::SelectDatabase(char const* DatabaseName)
{
    int Failure = mysql_select_db(MySQL, DatabaseName);
    if (Failure)
    {
        ShowMySQLError(MySQL, "mysql_select_db()");
    }

    return Failure == 0;
}

int Connection::Query(char const* QueryStr, unsigned long Length)
{
	return Query(QueryStr, Length, NULL);
}

int Connection::Query(char const* QueryStr, unsigned long Length, Result* ResultOut)
{
	MYSQL_RES* Result;
	my_ulonglong RetVal;
	if (mysql_real_query(MySQL, QueryStr, Length))
	{
		ShowMySQLError(MySQL, "mysql_real_query()");
		return -1;
	}

	Result = mysql_use_result(MySQL);
	if (Result)
	{
		if (ResultOut)
		{
			int RetVal = ResultOut->CreateResult(Result);

			if (RetVal == -1)
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

		RetVal = mysql_num_rows(Result);
		mysql_free_result(Result);
	}
	else
	{
		if (mysql_errno(MySQL))
		{
			ShowMySQLError(MySQL, "mysql_store_result()");
			return -1;
		}

		RetVal = mysql_affected_rows(MySQL);
	}

	if (RetVal == ((my_ulonglong)-1)) return -1;
	return (int)RetVal;
}

bool Connection::GetError(char* Buffer, unsigned long Length)
{
    unsigned int err = mysql_errno(MySQL);
    if (err != 0)
    {
        snprintf(Buffer, Length, "Error(%d) [%s] \"%s\"\n", mysql_errno(MySQL), mysql_sqlstate(MySQL), mysql_error(MySQL));
    }

    return err != 0;
}

int Connection::GetAffectedRows()
{
    my_ulonglong AffectedRows = mysql_affected_rows(MySQL);
    if (AffectedRows == ((my_ulonglong)-1)) return -1;

    return (int)AffectedRows;
}

bool Connection::SetCharacterSet(char const* CharacterSet, char const* Collation)
{
    int Success = mysql_set_character_set(MySQL, CharacterSet);
    if (Collation != NULL)
    {
        char Query[512];
        snprintf(Query, 512, "SET NAMES %s COLLATE %s", CharacterSet, Collation);

        mysql_real_query(MySQL, Query, strlen(Query));
    }

    return Success == 0;
}

char const* Connection::GetStatementInfo()
{
    return mysql_info(MySQL);
}

long Connection::GetLastAutoIncrementID()
{
    return (long)mysql_insert_id(MySQL);
}

bool Connection::Ping()
{
    int Failure = mysql_ping(MySQL);
    if (Failure)
    {
        ShowMySQLError(MySQL, "mysql_ping()");
    }

    return Failure == 0;
}

char const* Connection::GetStatus()
{
    char const* RetVal = mysql_stat(MySQL);
    if (!RetVal)
    {
        ShowMySQLError(MySQL, "mysql_stat()");
    }

    return RetVal;
}

unsigned int Connection::GetWarningCount()
{
    return mysql_warning_count(MySQL);
}

long Connection::EscapeString(char* Query, char const* String)
{
    long RetVal = 0;
    unsigned long Length = strlen(String);

    RetVal = mysql_real_escape_string(MySQL, Query, String, Length);
    if (RetVal == ((unsigned long)-1))
    {
        return -1;
    }

    return RetVal;
}

void Connection::ShowMySQLError(st_mysql* mysql, char const* call)
{
	fprintf(stderr, "%s: Error(%d) [%s] \"%s\"\n", call,
		mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
}

TransactionRef Connection::create_transaction(isolation::level level, bool consistent_snapshot){
    if (!IsConnected()) {
        return TransactionRef();
    }


    return TransactionRef(new Transaction(this, consistent_snapshot));
}

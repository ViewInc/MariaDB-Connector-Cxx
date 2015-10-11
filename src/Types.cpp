#include "Types.h"

#include "mariadb\mysql.h"

int SQLTypeToMySQLType(SQL_TYPE::Value Type)
{
	if (Type == SQL_TYPE::STRING)
	{
		return MYSQL_TYPE_STRING;
	}
	else if (Type == SQL_TYPE::BYTE)
	{
		return MYSQL_TYPE_TINY;
	}
	else if (Type == SQL_TYPE::SHORT)
	{
		return MYSQL_TYPE_SHORT;
	}
	else if (Type == SQL_TYPE::INTEGER)
	{
		return MYSQL_TYPE_LONG;
	}
	else if (Type == SQL_TYPE::LONG)
	{
		return MYSQL_TYPE_LONGLONG;
	}
	else if (Type == SQL_TYPE::UBYTE)
	{
		return MYSQL_TYPE_TINY;
	}
	else if (Type == SQL_TYPE::USHORT)
	{
		return MYSQL_TYPE_SHORT;
	}
	else if (Type == SQL_TYPE::UINTEGER)
	{
		return MYSQL_TYPE_LONG;
	}
	else if (Type == SQL_TYPE::ULONG)
	{
		return MYSQL_TYPE_LONGLONG;
	}
	else if (Type == SQL_TYPE::FLOAT)
	{
		return MYSQL_TYPE_FLOAT;
	}
	else if (Type == SQL_TYPE::DOUBLE)
	{
		return MYSQL_TYPE_DOUBLE;
	}
	else
	{
		return 0;
	}
}

bool SQLTypeIsUnsigned(SQL_TYPE::Value Type)
{
	return (Type == SQL_TYPE::UBYTE
		|| Type == SQL_TYPE::USHORT
		|| Type == SQL_TYPE::UINTEGER
		|| Type == SQL_TYPE::ULONG);
}
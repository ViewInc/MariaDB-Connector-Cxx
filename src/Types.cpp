#include "Types.h"

#include "mariadb/mysql.h"

int SQLTypeToMySQLType(SQL_TYPE::Value Type)
{
	switch (Type)
	{
		case SQL_TYPE::UINT8: return SQL_TYPE::INT8;
		case SQL_TYPE::UINT16: return SQL_TYPE::INT16;
		case SQL_TYPE::UINT24: return SQL_TYPE::INT24;
		case SQL_TYPE::UINT32: return SQL_TYPE::INT32;
		case SQL_TYPE::UINT64: return SQL_TYPE::INT64;
		default: return Type;
	}
}

SQL_TYPE::Value MySQLTypeToSQLType(int Type, bool IsUnsigned)
{
    if (IsUnsigned)
    {
        switch (Type)
        {
            case MYSQL_TYPE_TINY: return SQL_TYPE::UINT8;
            case MYSQL_TYPE_SHORT: return SQL_TYPE::UINT16;
            case MYSQL_TYPE_INT24: return SQL_TYPE::UINT24;
            case MYSQL_TYPE_LONG: return SQL_TYPE::UINT32;
            case MYSQL_TYPE_LONGLONG: return SQL_TYPE::UINT64;
            default: return SQL_TYPE::UNKNOWN;
        }
    }
    else
    {
        return (SQL_TYPE::Value)Type;
    }
}

bool SQLTypeIsUnsigned(SQL_TYPE::Value Type)
{
    return (Type == SQL_TYPE::UINT8
            || Type == SQL_TYPE::UINT16
            || Type == SQL_TYPE::UINT24
            || Type == SQL_TYPE::UINT32
            || Type == SQL_TYPE::UINT64);
}

int SQLTypeLength(SQL_TYPE::Value Type)
{
	switch (Type)
	{
		case SQL_TYPE::INT8: return 1;
		case SQL_TYPE::INT16: return 2;
		case SQL_TYPE::INT24: return 3;
		case SQL_TYPE::INT32: return 4;
		case SQL_TYPE::INT64: return 8;
		case SQL_TYPE::UINT8: return 1;
		case SQL_TYPE::UINT16: return 2;
		case SQL_TYPE::UINT24: return 3;
		case SQL_TYPE::UINT32: return 4;
		case SQL_TYPE::UINT64: return 8;
		case SQL_TYPE::FLOAT: return 4;
		case SQL_TYPE::DOUBLE: return 8;
		case SQL_TYPE::NULLTYPE: return 0;
		case SQL_TYPE::TIMESTAMP: return sizeof(MYSQL_TIME);
		case SQL_TYPE::DATE: return sizeof(MYSQL_TIME);
		case SQL_TYPE::TIME: return sizeof(MYSQL_TIME);
		case SQL_TYPE::DATETIME: return sizeof(MYSQL_TIME);
		case SQL_TYPE::YEAR: return 1;
		case SQL_TYPE::NEWDATE: return sizeof(MYSQL_TIME);
		default: return -1;
	}
}

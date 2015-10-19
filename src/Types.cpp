#include "Types.h"

#include "mariadb/mysql.h"

int SQLTypeToMySQLType(SQL_TYPE::Value Type)
{
    return Type;
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

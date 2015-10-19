#pragma once

/** VS doesn't have the noexcept keyword, yet */
#ifndef _MSC_VER
#	ifndef NOEXCEPT
#		define NOEXCEPT noexcept
#	endif
#else
#	ifndef NOEXCEPT
#		define NOEXCEPT
#	endif
#endif

/** Multiline Strings for query constants */
#define SQL(...) #__VA_ARGS__

struct SQL_TYPE
{
    enum Value
    {
        DECIMAL,
        INT8,
        INT16,
        INT32,
        FLOAT,
        DOUBLE,
        NULLTYPE,
        TIMESTAMP,
        INT64,
        INT24,
        DATE,
        TIME,
        DATETIME,
        YEAR,
        NEWDATE,
        VARCHAR,
        BIT,

        // Additionals.
        UINT8,
        UINT16,
        UINT24,
        UINT32,
        UINT64,

        NEWDECIMAL=246,
        ENUM=247,
        SET=248,
        TINY_BLOB=249,
        MEDIUM_BLOB=250,
        LONG_BLOB=251,
        BLOB=252,
        VAR_STRING=253,
        STRING=254,
        GEOMETRY=255,
        UNKNOWN
    };
};

int SQLTypeToMySQLType(SQL_TYPE::Value Type);
SQL_TYPE::Value MySQLTypeToSQLType(int Type, bool IsUnsigned);
bool SQLTypeIsUnsigned(SQL_TYPE::Value Type);
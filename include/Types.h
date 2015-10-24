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

// Typical Visual Studio mess.
#include <stdio.h>
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

/** Convert SQL_TYPE to MYSQL_VALUE */
int SQLTypeToMySQLType(SQL_TYPE::Value Type);

/** Convert MYSQL_VALUE to SQL_TYPE */
SQL_TYPE::Value MySQLTypeToSQLType(int Type, bool IsUnsigned);

/** Retrieves the signedness of the specified type.
 *
 * @remarks Will return false for types not applicable for signedness.
 * @return Whether it's unsigned.
 */
bool SQLTypeIsUnsigned(SQL_TYPE::Value Type);

/** Retrieves the length for the specified type.
 *
 * @return Will return the correct length, or -1 for error.
 */
int SQLTypeLength(SQL_TYPE::Value Type);

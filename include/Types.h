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
	enum Value {
		STRING,
		BINARY,

		BYTE,
		SHORT,
		INTEGER,
		LONG,

		UBYTE,
		USHORT,
		UINTEGER,
		ULONG,

		FLOAT,
		DOUBLE,

		UNKNOWN
	};
};

int SQLTypeToMySQLType(SQL_TYPE::Value Type);
bool SQLTypeIsUnsigned(SQL_TYPE::Value Type);
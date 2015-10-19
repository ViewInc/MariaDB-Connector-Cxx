#include "Result.h"

#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <new>
#include <limits>
#include "mariadb/mysql.h"

#ifndef ALLOW_CONVERSION
#	define ALLOW_CONVERSION 1
#endif

template<typename Tout, typename Tin>
Tout toInt(Tin in)
{
    Tout retVal = 0;

    if (in > 0)
        retVal = static_cast<Tout>(in & std::numeric_limits<Tout>::max());
    else if (in < 0)
        retVal = static_cast<Tout>(in | std::numeric_limits<Tout>::min());

    return retVal;
}

// Ugly preprocessor hacks.
#define CONVERT_TO(T) \
	else if (DataType == SQL_TYPE::INT8) \
	{ \
		int8_t Original = *(int8_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::INT16) \
	{ \
		int16_t Original = *(int16_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::INT32) \
	{ \
		int32_t Original = *(int32_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::INT64) \
	{ \
		int64_t Original = *(int64_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::UINT8) \
	{ \
		uint8_t Original = *(uint8_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::UINT16) \
	{ \
		uint16_t Original = *(uint16_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::UINT32) \
	{ \
		uint32_t Original = *(uint32_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::UINT64) \
	{ \
		uint64_t Original = *(uint64_t*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::FLOAT) \
	{ \
		float Original = *(float*)Data; \
		RetVal = toInt<T>(Original); \
	} \
	else if (DataType == SQL_TYPE::DOUBLE) \
	{ \
		double Original = *(double*)Data; \
		RetVal = toInt<T>(Original); \
	}

Field::Field(char* MyField, unsigned long MyLength)
{
    Data = malloc(MyLength + 1);
	if (!Data) exit(1);
    memcpy(Data, MyField, MyLength);
    ((char*)Data)[MyLength] = '\0';

    DataLength = MyLength;
    DataType = SQL_TYPE::STRING;
}

Field::Field(SQL_TYPE::Value Type, void* MyField, unsigned long MyLength)
{
	if (Type == SQL_TYPE::STRING)
	{
		Data = malloc(MyLength + 1);
		if (!Data) exit(1);
		memcpy(Data, MyField, MyLength);
		((char*)Data)[MyLength] = '\0';
	}
	else
	{
		Data = malloc(MyLength);
		if (!Data) exit(1);
		memcpy(Data, MyField, MyLength);
	}

	DataLength = MyLength;
	DataType = Type;
}

Field::~Field()
{
	free(Data);
}

Field::Field(Field const& Other)
{
	DataLength = Other.DataLength;
	DataType = Other.DataType;
	if (DataType == SQL_TYPE::STRING)
	{
		Data = malloc(DataLength + 1);
		if (!Data) exit(1);
		memcpy(Data, Other.Data, DataLength);
		((char*)Data)[DataLength] = '\0';
	}
	else
	{
		Data = malloc(DataLength);
		if (!Data) exit(1);
		memcpy(Data, Other.Data, DataLength);
	}
}

Field& Field::operator=(Field const& Other)
{
	// Reset.
	free(Data);

	// Copy.
	DataLength = Other.DataLength;
	DataType = Other.DataType;
	if (DataType == SQL_TYPE::STRING)
	{
		Data = malloc(DataLength + 1);
		if (!Data) exit(1);
		memcpy(Data, Other.Data, DataLength);
		((char*)Data)[DataLength] = '\0';
	}
	else
	{
		Data = malloc(DataLength);
		if (!Data) exit(1);
		memcpy(Data, Other.Data, DataLength);
	}

	return *this;
}

unsigned long Field::GetLength()
{
	return DataLength;
}

SQL_TYPE::Value Field::GetType()
{
	return DataType;
}

char const* Field::AsString()
{
	if (DataType == SQL_TYPE::STRING)
	{
		return (char*)Data;
	}

	return NULL;
}

void const* Field::AsBinary()
{
	return Data;
}

signed char Field::AsByte()
{
	signed char RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (signed char)strtol((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::INT8)
	{
		RetVal = *(signed char*)Data;
	}
#if ALLOW_CONVERSION == 1
    CONVERT_TO(int8_t);
#endif

	return RetVal;
}

signed short Field::AsShort()
{
	signed short RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (signed short)strtol((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::INT16)
	{
		RetVal = *(signed short*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(int16_t);
#endif

	return RetVal;
}

signed int Field::AsInteger()
{
	signed int RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (signed int)strtol((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::INT32)
	{
		RetVal = *(signed int*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(int32_t);
#endif

	return RetVal;
}

signed long Field::AsLong()
{
	signed long RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (signed long)strtol((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::INT64)
	{
		RetVal = *(signed long*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(int64_t);
#endif

	return RetVal;
}

unsigned char Field::AsUnsignedByte()
{
	unsigned char RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (unsigned char)strtoul((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::UINT8)
	{
		RetVal = *(unsigned char*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(uint8_t);
#endif

	return RetVal;
}

unsigned short Field::AsUnsignedShort()
{
	unsigned short RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (unsigned short)strtoul((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::UINT16)
	{
		RetVal = *(unsigned short*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(uint16_t);
#endif

	return RetVal;
}

unsigned int Field::AsUnsignedInteger()
{
	unsigned int RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (unsigned int)strtoul((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::UINT32)
	{
		RetVal = *(unsigned int*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(uint32_t);
#endif

	return RetVal;
}

unsigned long Field::AsUnsignedLong()
{
	unsigned long RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (unsigned long)strtoul((char*)Data, NULL, 10);
	}
	else if (DataType == SQL_TYPE::UINT64)
	{
		RetVal = *(unsigned long*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(uint64_t);
#endif

	return RetVal;
}


float Field::AsFloat()
{
	float RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (float)strtod((char*)Data, NULL);
	}
	else if (DataType == SQL_TYPE::FLOAT)
	{
		RetVal = *(float*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(float);
#endif

	return RetVal;
}

double Field::AsDouble()
{
	double RetVal = 0;
	if (DataType == SQL_TYPE::STRING)
	{
		RetVal = (double)strtod((char*)Data, NULL);
	}
	else if (DataType == SQL_TYPE::DOUBLE)
	{
		RetVal = *(double*)Data;
	}
#if ALLOW_CONVERSION == 1
	CONVERT_TO(double);
#endif

	return RetVal;
}


Row::Row()
	: FieldCount(0)
	, Fields(NULL)
{
}

Row::Row(MYSQL_RES* MyResult, MYSQL_ROW MyRow)
{
	unsigned long* Lengths;
	unsigned int i;

	Lengths = mysql_fetch_lengths(MyResult);
	FieldCount = mysql_num_fields(MyResult);

	Fields = (Field*)malloc(sizeof(Field) * FieldCount);
	if (!Fields) exit(1);

	for (i = 0; i < FieldCount; i++)
	{
		Field* f = new(Fields + i) Field(MyRow[i], Lengths[i]);
		(void)f; // Not used here.
	}
}

Row::~Row()
{
	for (unsigned int i = 0; i < FieldCount; i++)
	{
		Field* f = (Fields + i);
		f->~Field();
	}
	
	free(Fields);
}

Row::Row(Row const& Other)
{
	if (Other.Fields)
	{
		FieldCount = Other.FieldCount;
		Fields = (Field*)malloc(sizeof(Field) * FieldCount);
		if (!Fields) exit(1);

		for (unsigned int i = 0; i < FieldCount; i++)
		{
			Field* fOther = (Other.Fields + i);
			Field* fNew = new(Fields + i) Field(*fOther);
			(void)fNew;
		}
	}
}

Row& Row::operator=(Row const& Other)
{
	// Reset.
	for (unsigned int i = 0; i < FieldCount; i++)
	{
		Field* f = (Fields + i);
		f->~Field();
	}

	free(Fields);

	// Copy.
	if (Other.Fields)
	{
		FieldCount = Other.FieldCount;
		Fields = (Field*)malloc(sizeof(Field) * FieldCount);
		if (!Fields) exit(1);

		for (unsigned int i = 0; i < FieldCount; i++)
		{
			Field* fOther = (Other.Fields + i);
			Field* fNew = new(Fields + i) Field(*fOther);
			(void)fNew;
		}
	}

	return *this;
}

unsigned int Row::GetFieldCount()
{
	return FieldCount;
}

Field* Row::GetField(unsigned int Index)
{
	if (Index >= FieldCount) return NULL;
	return (Fields + Index);
}

RowIter Row::begin()
{
    return RowIter(this, 0);
}

RowIter Row::end()
{
    return RowIter(this, FieldCount);
}

Result::Result()
	: bIsValid(false)
	, RowCount(0)
	, Rows(NULL)
{
}

Result::Result(Result const& Other)
{
	bIsValid = Other.bIsValid;
	if (bIsValid)
	{
		RowCount = Other.RowCount;
		Rows = (Row*)malloc(sizeof(Row) * RowCount);
		if (!Rows) exit(1);

		for (unsigned int i = 0; i < RowCount; i++)
		{
			Row* rOther = (Other.Rows + i);
			Row* rNew = new(Rows + i) Row(*rOther);
			(void)rNew;
		}
	}
}

Result& Result::operator=(Result const& Other)
{
	ResetResult();

	bIsValid = Other.bIsValid;
	if (bIsValid)
	{
		RowCount = Other.RowCount;
		Rows = (Row*)malloc(sizeof(Row) * RowCount);
		if (!Rows) exit(1);

		for (unsigned int i = 0; i < RowCount; i++)
		{
			Row* rOther = (Other.Rows + i);
			Row* rNew = new(Rows + i) Row(*rOther);
			(void)rNew;
		}
	}

	return *this;
}

Result::~Result()
{
	ResetResult();
}

int Result::CreateResult(MYSQL_RES* MyResult)
{
	if (Rows != NULL || bIsValid) return -1;
	MYSQL_ROW MyRow;
	unsigned int i = 0;

	while ((MyRow = mysql_fetch_row(MyResult)))
	{
#pragma warning(suppress: 6308)
		Rows = (Row*)realloc(Rows, sizeof(Row) * (i + 1));
		if (!Rows) exit(1);

		Row* r = new(Rows + i) Row(MyResult, MyRow);
		(void)r; // Value is unused here.

		i++;
	}

	// Error checks are done by the caller.

	RowCount = (unsigned int)mysql_num_rows(MyResult);
	bIsValid = RowCount > 0;
	return RowCount;
}

void Result::ResetResult()
{
	for (unsigned int i = 0; i < RowCount; i++)
	{
		Row* r = (Rows + i);
		r->~Row();
	}

	free(Rows);
	bIsValid = false;
	RowCount = 0;
	Rows = NULL;
}

bool Result::IsValid()
{
	return bIsValid;
}

unsigned int Result::GetRowCount()
{
	return RowCount;
}

Row* Result::GetRow(unsigned int Index)
{
	if (Index >= RowCount) return NULL;
	return (Rows + Index);
}


ResultIter Result::begin()
{
    return ResultIter(this, 0);
}

ResultIter Result::end()
{
    return ResultIter(this, RowCount);
}


RowIter::RowIter(Row* pRow, unsigned int Pos)
{
    this->pRow = pRow;
    this->Pos = Pos;
}

bool RowIter::operator==(RowIter const& other)
{
	return Pos == other.Pos;
}

bool RowIter::operator!=(RowIter const& other)
{
    return !(Pos == other.Pos);
}

Field* RowIter::operator*()
{
    return pRow->GetField(Pos);
}

RowIter const& RowIter::operator++()
{
    ++Pos;
    return *this;
}

ResultIter::ResultIter(Result* pResult, unsigned int Pos)
{
    this->pResult = pResult;
    this->Pos = Pos;
}

bool ResultIter::operator==(ResultIter const& other)
{
	return Pos == other.Pos;
}

bool ResultIter::operator!=(ResultIter const& other)
{
    return !(Pos == other.Pos);
}

Row* ResultIter::operator*()
{
    return pResult->GetRow(Pos);
}

ResultIter const& ResultIter::operator++()
{
    ++Pos;
    return *this;
}

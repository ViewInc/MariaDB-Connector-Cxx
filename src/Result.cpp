#include "Result.h"

#include <stdlib.h>
#include <string.h>
#include <new>
#include "mariadb/mysql.h"

Field::Field(char* MyField, unsigned long MyLength)
{
    Data = malloc(MyLength + 1);
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
		memcpy(Data, MyField, MyLength);
		((char*)Data)[MyLength] = '\0';
	}
	else
	{
		Data = malloc(MyLength);
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
		memcpy(Data, Other.Data, DataLength);
		((char*)Data)[DataLength] = '\0';
	}
	else
	{
		Data = malloc(DataLength);
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
		memcpy(Data, Other.Data, DataLength);
		((char*)Data)[DataLength] = '\0';
	}
	else
	{
		Data = malloc(DataLength);
		memcpy(Data, Other.Data, DataLength);
	}

	return *this;
}

unsigned long Field::GetLength()
{
	return DataLength;
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
	if (DataType == SQL_TYPE::STRING)
	{
		signed char RetVal = (signed char)strtol((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::BYTE)
	{
		return *(signed char*)Data;
	}

	return 0;
}

signed short Field::AsShort()
{
	if (DataType == SQL_TYPE::STRING)
	{
		signed short RetVal = (signed short)strtol((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::SHORT)
	{
		return *(signed short*)Data;
	}

	return 0;
}

signed int Field::AsInteger()
{
	if (DataType == SQL_TYPE::STRING)
	{
		signed int RetVal = (signed int)strtol((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::INTEGER)
	{
		return *(signed int*)Data;
	}

	return 0;
}

signed long Field::AsLong()
{
	if (DataType == SQL_TYPE::STRING)
	{
		signed long RetVal = (signed long)strtol((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::LONG)
	{
		return *(signed long*)Data;
	}

	return 0;
}

unsigned char Field::AsUnsignedByte()
{
	if (DataType == SQL_TYPE::STRING)
	{
		unsigned char RetVal = (unsigned char)strtoul((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::UBYTE)
	{
		return *(unsigned char*)Data;
	}

	return 0;
}

unsigned short Field::AsUnsignedShort()
{
	if (DataType == SQL_TYPE::STRING)
	{
		unsigned short RetVal = (unsigned short)strtoul((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::USHORT)
	{
		return *(unsigned short*)Data;
	}

	return 0;
}

unsigned int Field::AsUnsignedInteger()
{
	if (DataType == SQL_TYPE::STRING)
	{
		unsigned int RetVal = (unsigned int)strtoul((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::UINTEGER)
	{
		return *(unsigned int*)Data;
	}

	return 0;
}

unsigned long Field::AsUnsignedLong()
{
	if (DataType == SQL_TYPE::STRING)
	{
		unsigned long RetVal = (unsigned long)strtoul((char*)Data, NULL, 10);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::ULONG)
	{
		return *(unsigned long*)Data;
	}

	return 0;
}


float Field::AsFloat()
{
	if (DataType == SQL_TYPE::STRING)
	{
		float RetVal = (float)strtod((char*)Data, NULL);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::FLOAT)
	{
		return *(float*)Data;
	}

	return 0;
}

double Field::AsDouble()
{
	if (DataType == SQL_TYPE::STRING)
	{
		double RetVal = (double)strtod((char*)Data, NULL);
		return RetVal;
	}
	else if (DataType == SQL_TYPE::DOUBLE)
	{
		return *(double*)Data;
	}

	return 0;
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
    return RowIter(this, FieldCount - 1);
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
	MYSQL_ROW MyRow;
	unsigned int i;

	RowCount = (unsigned int)mysql_num_rows(MyResult);
	Rows = (Row*)malloc(sizeof(Row) * RowCount);
	if (!Rows) exit(1);

	for (i = 0; i < RowCount; i++)
	{
		if (!(MyRow = mysql_fetch_row(MyResult)))
			break;

		Row* r = new(Rows + i) Row(MyResult, MyRow);
		(void)r; // Value is unused here.
	}

	bIsValid = true;
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
    return ResultIter(this, RowCount - 1);
}


RowIter::RowIter(Row* pRow, unsigned int Pos)
{
    this->pRow = pRow;
    this->Pos = Pos;
}

bool RowIter::operator!=(RowIter const& other)
{
    return Pos != other.Pos;
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

bool ResultIter::operator!=(ResultIter const& other)
{
    return Pos != other.Pos;
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

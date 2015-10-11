#pragma once

#include "Types.h"

/** The data field.
 */
class Field
{
friend class Row;
friend class Statement;

protected:
	explicit Field(char* MyField, unsigned long MyLength);
	explicit Field(SQL_TYPE::Value Type, void* MyField, unsigned long MyLength);
	virtual ~Field();
	Field(Field const& Other);
	Field& operator=(Field const& Other);
	
public:
	unsigned long GetLength();
	
	char const* AsString();
	void const* AsBinary();
	
	signed char AsByte();
	signed short AsShort();
	signed int AsInteger();
	signed long AsLong();
	
	unsigned char AsUnsignedByte();
	unsigned short AsUnsignedShort();
	unsigned int AsUnsignedInteger();
	unsigned long AsUnsignedLong();
	
	float AsFloat();
	double AsDouble();

protected:
	void* Data;
	unsigned long DataLength;
	SQL_TYPE::Value DataType;
};

/** The row.
 */
class Row
{
friend class Result;
friend class Statement;

protected:
	explicit Row();
	explicit Row(struct st_mysql_res* MyResult, char** MyRow);
	virtual ~Row();
	Row(Row const& Other);
	Row& operator=(Row const& Other);

public:
	unsigned int GetFieldCount();
	Field* GetField(unsigned int Index);

protected:
	unsigned int FieldCount;
	Field* Fields;
};

/** The result
 */
class Result
{
friend class Connection;
friend class Statement;

public:
	explicit Result();
	virtual ~Result();
	Result(Result const& Other);
	Result& operator=(Result const& Other);
	
	void ResetResult();

protected:
	int CreateResult(struct st_mysql_res* MyResult);

public:
	bool IsValid();
	unsigned int GetRowCount();
	Row* GetRow(unsigned int Index);

protected:
	bool bIsValid;
	unsigned int RowCount;
	Row* Rows;
};
#pragma once

#include "Types.h"

class Field;
class Row;
class Result;

/** Row Iterator (for range-based loops).
 *
 */
class RowIter
{
friend class Row;

protected:
    RowIter(Row* pRow, unsigned int Pos);

public:
	bool operator==(RowIter const& other);
    bool operator!=(RowIter const& other);
    Field* operator*();
    RowIter const& operator++();

protected:
    Row* pRow;
    unsigned int Pos;
};

/** Resul Iterator (for range-based loops).
 *
 */
class ResultIter
{
friend class Result;

protected:
    ResultIter(Result* pResult, unsigned int Pos);

public:
	bool operator==(ResultIter const& other);
    bool operator!=(ResultIter const& other);
    Row* operator*();
    ResultIter const& operator++();

protected:
    Result* pResult;
    unsigned int Pos;
};

/** The data field.
 *
 * @remarks Queries executed via `Connection::Query()`, the data will *always* be a
 * zero-terminated char array.
 * @remarks Queries executed via prepared statements are in there respective data type.
 * @remarks Trying to get the data as a different type will either return 0 or NULL.
 *
 * @todo Improve the described behaviour above.
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
	/**
	 * @return The length of the data.
	 */
	unsigned long GetLength();

	/**
	 * @return Type of the data.
	 */
	SQL_TYPE::Value GetType();
	
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
	/**
	 * @return The amount of columns (fields) in this row
	 */
	unsigned int GetFieldCount();

	/** Return a field.
	 *
	 * @param Index The index of the field to return.
	 * @return The `Field` or NULL, if Index is out of bounds.
	 * @see Field
	 */
	Field* GetField(unsigned int Index);

    /** Method for the range-for-loop in C++.
     * @remarks Do not use standalone.
     */
    RowIter begin();

    /** Method for the range-for-loop in C++.
     * @remarks Do not use standalone.
     */
    RowIter end();

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
	
	/** Resets the result.
	 *
	 * After resetting a result object, it can be reused.
	 */
	void ResetResult();

	/**
	 * @return Whether the result is valid.
	 */
	bool IsValid();

	/**
	 * @return The row count.
	 */
	unsigned int GetRowCount();

	/** Return a row.
	 *
	 * @param Index The index of the row to return.
	 * @return The `Row` or NULL, if Index is out of bounds.
	 * @see Row
	 */
	Row* GetRow(unsigned int Index);

    /** Method for the range-for-loop in C++.
     * @remarks Do not use standalone.
     */
    ResultIter begin();

    /** Method for the range-for-loop in C++.
     * @remarks Do not use standalone.
     */
    ResultIter end();

protected:
	int CreateResult(struct st_mysql_res* MyResult);

protected:
	bool bIsValid;
	unsigned int RowCount;
	Row* Rows;
};

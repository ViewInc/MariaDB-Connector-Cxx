#include "Statement.h"
#include "Connection.h"
#include "Result.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include "mariadb/mysql.h"

Bind::Bind()
	: Data(nullptr)
	, DataType(SQL_TYPE::UNKNOWN)
	, LengthInput(0)
	, LengthOutput(0)
	, bIsNullOutput(0)
	, bIsInput(true)
{
}

Bind::~Bind()
{
	free(Data);
}

// May re-add them later.
//Bind::Bind(Bind const& Other)
//{
//	// Copy.
//	Data = malloc(Other.LengthInput);
//	if (!Data) exit(1);
//	memcpy(Data, Other.Data, Other.LengthInput);
//
//	DataType = Other.DataType;
//	LengthInput = Other.LengthInput;
//	LengthOutput = Other.LengthOutput;
//	bIsNullOutput = Other.bIsNullOutput;
//	bIsInput = Other.bIsInput;
//}
//
//Bind& Bind::operator=(Bind const& Other)
//{
//	// Reset.
//	free(Data);
//
//	// Copy.
//	Data = malloc(Other.LengthInput);
//	if (!Data) exit(1);
//	memcpy(Data, Other.Data, Other.LengthInput);
//
//	DataType = Other.DataType;
//	LengthInput = Other.LengthInput;
//	LengthOutput = Other.LengthOutput;
//	bIsNullOutput = Other.bIsNullOutput;
//	bIsInput = Other.bIsInput;
//
//	return *this;
//}

void Bind::SetInput(SQL_TYPE::Value Type, void const* Buffer, unsigned long Length)
{
	DataType = Type;
	LengthInput = Length;
	Data = malloc(Length);
	if (!Data) exit(1);
	memcpy(Data, Buffer, Length);

	bIsInput = true;
}

void Bind::SetOutput(SQL_TYPE::Value Type, unsigned long Length)
{
	DataType = Type;
	LengthInput = Length;
	Data = malloc(Length);
	if (!Data) exit(1);
	memset(Data, 0, Length);

	bIsInput = false;
}

void const* Bind::GetData()
{
	return Data;
}

unsigned long Bind::GetLength()
{
	if (bIsInput)
	{
		return LengthInput;
	}
	else
	{
		return LengthOutput;
	}
}

bool Bind::IsNull()
{
	return bIsNullOutput;
}

void Bind::Create(MYSQL_BIND* MyBind)
{
	MyBind->buffer_type = (enum_field_types)SQLTypeToMySQLType(DataType);
	MyBind->buffer = (void*)Data;
	MyBind->buffer_length = LengthInput;

	if (bIsInput)
	{
		MyBind->length = &LengthInput;
		MyBind->is_null = (my_bool*)0;
		MyBind->is_unsigned = SQLTypeIsUnsigned(DataType);
	}
	else
	{
		MyBind->length = &LengthOutput;
		MyBind->is_null = &bIsNullOutput;
	}
}


Statement::Statement()
	: Statement(nullptr)
{
}

Statement::Statement(Connection* ConIn)
	: Query(nullptr)
	, Con(ConIn)
	, MyStatement(nullptr)
	, BindsIn(nullptr)
	, BindsOut(nullptr)
	, NumBindsIn(0)
	, NumBindsOut(0)
	, MyBindsIn(nullptr)
	, MyBindsOut(nullptr)
{
}

Statement::~Statement()
{
	for (unsigned int i = 0; i < NumBindsIn; i++)
	{
		Bind* b = (BindsIn + i);
		b->~Bind();
	}

	for (unsigned int i = 0; i < NumBindsOut; i++)
	{
		Bind* b = (BindsOut + i);
		b->~Bind();
	}

	free(BindsIn);
	free(BindsOut);
	if (MyStatement)
		mysql_stmt_close(MyStatement);
}

// May re-add them later.
//Statement::Statement(Statement const& Other)
//{
//	// Copy.
//	Query = Other.Query;
//	Con = Other.Con;
//
//	if (Other.MyStatement)
//	{
//		MyStatement = (MYSQL_STMT*)malloc(sizeof(MYSQL_STMT));
//		if (!MyStatement) exit(1);
//		memcpy(MyStatement, Other.MyStatement, sizeof(MYSQL_STMT));
//	}
//
//	NumBindsIn = Other.NumBindsIn;
//	NumBindsOut = Other.NumBindsOut;
//	if (Other.BindsIn)
//	{
//		BindsIn = (Bind*)malloc(sizeof(Bind) * NumBindsIn);
//		if (!BindsIn) exit(1);
//
//		for (unsigned int i = 0; i < NumBindsIn; i++)
//		{
//			Bind* bOther = (Other.BindsIn + i);
//			Bind* bNew = new(BindsIn + i) Bind(*bOther);
//			(void)bNew;
//		}
//	}
//	if (Other.BindsOut)
//	{
//		BindsOut = (Bind*)malloc(sizeof(Bind) * NumBindsOut);
//		if (!BindsOut) exit(1);
//
//		for (unsigned int i = 0; i < NumBindsOut; i++)
//		{
//			Bind* bOther = (Other.BindsOut + i);
//			Bind* bNew = new(BindsOut + i) Bind(*bOther);
//			(void)bNew;
//		}
//	}
//
//	Prepare();
//}
//
//Statement& Statement::operator=(Statement const& Other)
//{
//	// Reset.
//	for (unsigned int i = 0; i < NumBindsIn; i++)
//	{
//		Bind* b = (BindsIn + i);
//		b->~Bind();
//	}
//
//	for (unsigned int i = 0; i < NumBindsOut; i++)
//	{
//		Bind* b = (BindsOut + i);
//		b->~Bind();
//	}
//
//	free(BindsIn);
//	free(BindsOut);
//	BindsIn = nullptr;
//	BindsOut = nullptr;
//	if (MyStatement)
//	{
//		mysql_stmt_close(MyStatement);
//		MyStatement = nullptr;
//	}
//
//	// Copy.
//	Query = Other.Query;
//	Con = Other.Con;
//	
//	if (Other.MyStatement)
//	{
//		MyStatement = (MYSQL_STMT*)malloc(sizeof(MYSQL_STMT));
//		if (!MyStatement) exit(1);
//		memcpy(MyStatement, Other.MyStatement, sizeof(MYSQL_STMT));
//	}
//
//	NumBindsIn = Other.NumBindsIn;
//	NumBindsOut = Other.NumBindsOut;
//	if (Other.BindsIn)
//	{
//		BindsIn = (Bind*)malloc(sizeof(Bind) * NumBindsIn);
//		if (!BindsIn) exit(1);
//
//		for (unsigned int i = 0; i < NumBindsIn; i++)
//		{
//			Bind* bOther = (Other.BindsIn + i);
//			Bind* bNew = new(BindsIn + i) Bind(*bOther);
//			(void)bNew;
//		}
//	}
//	if (Other.BindsOut)
//	{
//		BindsOut = (Bind*)malloc(sizeof(Bind) * NumBindsOut);
//		if (!BindsOut) exit(1);
//
//		for (unsigned int i = 0; i < NumBindsOut; i++)
//		{
//			Bind* bOther = (Other.BindsOut + i);
//			Bind* bNew = new(BindsOut + i) Bind(*bOther);
//			(void)bNew;
//		}
//	}
//	
//	Prepare();
//	return *this;
//}

bool Statement::Init(char const* QueryIn)
{
	if (!Con) return false;

	Query = QueryIn;
	MyStatement = mysql_stmt_init(Con->MySQL);
	if (mysql_stmt_prepare(MyStatement, Query, strlen(Query)))
	{
		ShowMySQLStatementError(MyStatement, "mysql_stmt_prepare()");
		return false;
	}

	NumBindsIn = mysql_stmt_param_count(MyStatement);
	if (NumBindsIn > 0)
	{
		BindsIn = (Bind*)malloc(sizeof(Bind) * NumBindsIn);
		if (!BindsIn) exit(1);

		for (unsigned int i = 0; i < NumBindsIn; i++)
		{
			Bind* b = new(BindsIn + i) Bind();
			(void)b;
		}
	}

	NumBindsOut = mysql_stmt_field_count(MyStatement);
	if (NumBindsOut > 0)
	{
		BindsOut = (Bind*)malloc(sizeof(Bind) * NumBindsOut);
		if (!BindsOut) exit(1);

		for (unsigned int i = 0; i < NumBindsOut; i++)
		{
			Bind* b = new(BindsOut + i) Bind();
			(void)b;
		}
	}

	return true;
}

bool Statement::Prepare()
{
	if (!BindsIn || !BindsOut) return false;

	MyBindsIn = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * NumBindsIn);
	if (!BindsIn) exit(1);
	MyBindsOut = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * NumBindsOut);
	if (!BindsOut) exit(1);

	memset(MyBindsIn, 0, sizeof(MYSQL_BIND) * NumBindsIn);
	memset(MyBindsOut, 0, sizeof(MYSQL_BIND) * NumBindsOut);

	unsigned int CountIn = 0;
	for (unsigned int i = 0; i < NumBindsIn; i++)
	{
		Bind* b = (BindsIn + i);
		b->Create(MyBindsIn + CountIn);
		CountIn++;
	}

	unsigned int CountOut = 0;
	for (unsigned int i = 0; i < NumBindsOut; i++)
	{
		Bind* b = (BindsOut + i);
		b->Create(MyBindsOut + CountOut);
		CountOut++;
	}


	if (NumBindsOut > 0)
	{
		if (mysql_stmt_bind_result(MyStatement, MyBindsOut))
		{
			ShowMySQLStatementError(MyStatement, "mysql_stmt_bind_result()");
			return false;
		}
	}
	if (NumBindsIn > 0)
	{
		if (mysql_stmt_bind_param(MyStatement, MyBindsIn))
		{
			ShowMySQLStatementError(MyStatement, "mysql_stmt_bind_param()");
			return false;
		}
	}

	return true;
}

bool Statement::Execute()
{
	if (!Con) return false;

	if (mysql_stmt_execute(MyStatement))
	{
		ShowMySQLStatementError(MyStatement, "mysql_stmt_execute()");
		return false;
	}

	return true;
}

bool Statement::Fetch()
{
	int RetVal = mysql_stmt_fetch(MyStatement);
	return RetVal == 0;
}

int Statement::FetchAll(Result* Res)
{
	// We want to buffer them all at once.
	if (mysql_stmt_store_result(MyStatement))
	{
		ShowMySQLStatementError(MyStatement, "mysql_stmt_store_result()");
		return -1;
	}

	unsigned int NumRows = mysql_stmt_num_rows(MyStatement);
	Res->RowCount = NumRows;
	Res->bIsValid = true;
	Res->Rows = (Row*)malloc(sizeof(Row) * NumRows);
	if (!Res->Rows) exit(1);

	for (unsigned int i = 0; i < NumRows; i++)
	{
		Row* r = new(Res->Rows + i) Row();
		r->FieldCount = NumBindsOut;
		r->Fields = (Field*)malloc(sizeof(Field) * NumBindsOut);
		if (!r->Fields) exit(1);

		mysql_stmt_fetch(MyStatement);
		for (unsigned int j = 0; j < NumBindsOut; j++)
		{
			Bind* out = GetBindOut(j);
			Field* f = new(r->Fields + j) Field(out->DataType, out->Data, out->LengthOutput);
		}
	}

	return NumRows;
}

Bind* Statement::GetBindIn(unsigned int Index)
{
	if (!Con) return nullptr;
	if (Index >= NumBindsIn) return nullptr;

	return (BindsIn + Index);
}

Bind* Statement::GetBindOut(unsigned int Index)
{
	if (!Con) return nullptr;
	if (Index >= NumBindsOut) return nullptr;

	return (BindsOut + Index);
}

void Statement::SetConnection(Connection* ConIn)
{
	Con = ConIn;
}

void Statement::ShowMySQLStatementError(st_mysql_stmt* stmt, char const* call)
{
	fprintf(stderr, "%s: Error(%d) [%s] \"%s\"\n", call,
		mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
}

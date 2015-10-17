#include "Statement.h"
#include "Connection.h"
#include "Result.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include "mariadb/mysql.h"

Bind::Bind()
	: Data(NULL)
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

void Bind::SetInput(SQL_TYPE::Value Type, void const* Buffer, unsigned long Length)
{
	if (Data) free(Data);

	DataType = Type;
	LengthInput = Length;
	Data = malloc(Length);
	if (!Data) exit(1);
	memcpy(Data, Buffer, Length);

	bIsInput = true;
}

void Bind::SetOutput(SQL_TYPE::Value Type, unsigned long Length)
{
	if (Data) free(Data);

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
	return bIsNullOutput == 0;
}

void Bind::Update(MYSQL_BIND* MyBind)
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
    : Query(NULL)
    , Con(NULL)
    , MyStatement(NULL)
    , BindsIn(NULL)
    , BindsOut(NULL)
    , NumBindsIn(0)
    , NumBindsOut(0)
    , MyBindsIn(NULL)
    , MyBindsOut(NULL)
{
}

Statement::Statement(Connection* ConIn)
	: Query(NULL)
	, Con(ConIn)
	, MyStatement(NULL)
	, BindsIn(NULL)
	, BindsOut(NULL)
	, NumBindsIn(0)
	, NumBindsOut(0)
	, MyBindsIn(NULL)
	, MyBindsOut(NULL)
{
}

Statement::~Statement()
{
	FreeBindsIn();
	FreeBindsOut();
	free(MyBindsIn);
	free(MyBindsOut);
	if (MyStatement)
		mysql_stmt_close(MyStatement);
}

bool Statement::Init(char const* QueryIn)
{
	if (!Con) return false;
	if (MyStatement) mysql_stmt_close(MyStatement);

	Query = QueryIn;
	MyStatement = mysql_stmt_init(Con->MySQL);
	if (mysql_stmt_prepare(MyStatement, Query, strlen(Query)))
	{
		ShowMySQLStatementError(MyStatement, "mysql_stmt_prepare()");
		return false;
	}

	NumBindsIn = (unsigned int)mysql_stmt_param_count(MyStatement);
	if (NumBindsIn > 0)
	{
		if (BindsIn) FreeBindsIn();
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
		if (BindsOut) FreeBindsOut();
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

bool Statement::BindIn()
{
    if (!BindsIn) return false;
	if (!MyBindsIn)
	{
		MyBindsIn = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * NumBindsIn);
		if (!MyBindsIn) exit(1);
		memset(MyBindsIn, 0, sizeof(MYSQL_BIND) * NumBindsIn);
	}

	for (unsigned int i = 0; i < NumBindsIn; i++)
	{
		Bind* b = (BindsIn + i);
		b->Update(MyBindsIn + i);
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

bool Statement::BindOut()
{
	if (!BindsOut) return false;
    if (MyBindsOut)
        free(MyBindsOut);

	MyBindsOut = (MYSQL_BIND*)malloc(sizeof(MYSQL_BIND) * NumBindsOut);
	if (!MyBindsOut) exit(1);
	memset(MyBindsOut, 0, sizeof(MYSQL_BIND) * NumBindsOut);

	for (unsigned int i = 0; i < NumBindsOut; i++)
	{
		Bind* b = (BindsOut + i);
		b->Update(MyBindsOut + i);
	}

	if (NumBindsOut > 0)
	{
		if (mysql_stmt_bind_result(MyStatement, MyBindsOut))
		{
			ShowMySQLStatementError(MyStatement, "mysql_stmt_bind_result()");
			return false;
		}
	}

	return true;
}

int Statement::Execute()
{
	if (!Con) return -1;

	if (mysql_stmt_execute(MyStatement))
	{
		ShowMySQLStatementError(MyStatement, "mysql_stmt_execute()");
		return -1;
	}

	my_ulonglong AffectedRows = mysql_stmt_affected_rows(MyStatement);
	if (AffectedRows == (my_ulonglong)-1)
		return -1;

	return (int)AffectedRows;
}

bool Statement::Fetch()
{
	int RetVal = mysql_stmt_fetch(MyStatement);
	return RetVal == 0;
}

int Statement::FetchAll(Result* Res)
{
	if (NumBindsOut < 1) return 0;
	if (Res->Rows != NULL || Res->IsValid()) return -1;

	int NumRows = 0;
	while (!mysql_stmt_fetch(MyStatement))
	{
#pragma warning(suppress: 6308)
		Res->Rows = (Row*)realloc(Res->Rows, sizeof(Row) * (NumRows + 1));
		if (!Res->Rows) exit(1);
		
		Row* r = new(Res->Rows + (NumRows)) Row();
#pragma warning(suppress: 6011)
		r->FieldCount = NumBindsOut;
		r->Fields = (Field*)malloc(sizeof(Field) * NumBindsOut);
		if (!r->Fields) exit(1);

		for (unsigned int i = 0; i < NumBindsOut; i++)
		{
			Bind* out = GetBindOut(i);
			Field* f = new(r->Fields + i) Field(out->DataType, out->Data, out->LengthOutput);
			(void)f;
		}

		NumRows++;
	}

	Res->RowCount = NumRows;
	Res->bIsValid = NumRows > 0;

	return NumRows;
}

Bind* Statement::GetBindIn(unsigned int Index)
{
	if (!Con) return NULL;
	if (Index >= NumBindsIn) return NULL;

	return (BindsIn + Index);
}

Bind* Statement::GetBindOut(unsigned int Index)
{
	if (!Con) return NULL;
	if (Index >= NumBindsOut) return NULL;

	return (BindsOut + Index);
}

void Statement::SetConnection(Connection* ConIn)
{
	Con = ConIn;
}

void Statement::FreeBindsIn()
{
	for (unsigned int i = 0; i < NumBindsIn; i++)
	{
		Bind* b = (BindsIn + i);
		b->~Bind();
	}

	free(BindsIn);
	BindsIn = NULL;
}

void Statement::FreeBindsOut()
{
	for (unsigned int i = 0; i < NumBindsOut; i++)
	{
		Bind* b = (BindsOut + i);
		b->~Bind();
	}

	free(BindsOut);
	BindsOut = NULL;
}

void Statement::ShowMySQLStatementError(st_mysql_stmt* stmt, char const* call)
{
	fprintf(stderr, "%s: Error(%d) [%s] \"%s\"\n", call,
		mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
}

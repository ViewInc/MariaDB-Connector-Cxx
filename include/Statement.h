#pragma once

#include "Types.h"

/** The input / output bindings for prepared statements.
 *
 */
class Bind
{
friend class Statement;

protected:
	explicit Bind();
	virtual ~Bind();
	Bind(Bind const& Other){}
    Bind& operator=(Bind const& Other){return *this;}

public:
	void SetInput(SQL_TYPE::Value Type, void const* Buffer, unsigned long Length);
	void SetOutput(SQL_TYPE::Value Type, unsigned long Length);

	void const* GetData();
	unsigned long GetLength();
	bool IsNull();

protected:
	void Update(struct st_mysql_bind* MyBind);

protected:
	void* Data;
	SQL_TYPE::Value DataType;
	unsigned long LengthInput;
	unsigned long LengthOutput;
	char bIsNullOutput;
	bool bIsInput;
};

class Statement
{
public:
	explicit Statement();
	explicit Statement(class Connection* Con);
	virtual ~Statement();

private:
	Statement(Statement const& Other){}
    Statement& operator=(Statement const& Other){return *this;}

public:
	bool Init(char const* Query);
    bool BindIn();
	bool BindOut();
	int Execute();
	bool Fetch();
	int FetchAll(class Result* Res);

	Bind* GetBindIn(unsigned int Index);
	Bind* GetBindOut(unsigned int Index);

	void SetConnection(class Connection* Con);

protected:
	static void ShowMySQLStatementError(struct st_mysql_stmt* Statement, char const* Call);

protected:
	char const* Query;
	class Connection* Con;
	struct st_mysql_stmt* MyStatement;

	class Bind* BindsIn;
	class Bind* BindsOut;
	unsigned int NumBindsIn;
	unsigned int NumBindsOut;
	struct st_mysql_bind* MyBindsIn;
	struct st_mysql_bind* MyBindsOut;
};
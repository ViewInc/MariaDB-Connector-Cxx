#include "Test.h"

#include "MySQLCXX.h"
#include <stdio.h>
#include <string.h>

bool Test1_Preperation(Connection* Con)
{
	char const* query0 = "DROP DATABASE IF EXISTS `testdb`";
	int AffectedRows0 = Con->Query(query0, strlen(query0));

	char const* query1 = "CREATE DATABASE IF NOT EXISTS `testdb` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci */";
	int AffectedRows1 = Con->Query(query1, strlen(query1));

	char const* query2 = "USE `testdb`";
	int AffectedRows2 = Con->Query(query2, strlen(query2));

	if (AffectedRows0 != -1 && AffectedRows1 != -1 && AffectedRows2 != -1)
	{
		return (AffectedRows0 == 1 && AffectedRows1 == 1) || (AffectedRows1 == 1);
	}

	return false;
}

bool Test2_QueryWithoutOutput(Connection* Con)
{
	char const* query0 = SQL(
		CREATE TABLE IF NOT EXISTS `test` (
			`id` int(10) NOT NULL AUTO_INCREMENT,
			`name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
			`age` tinyint(4) UNSIGNED NOT NULL DEFAULT 0,
			`salary` smallint(5) UNSIGNED NULL DEFAULT 0,
			`savings` int(10) NULL DEFAULT 0,
			`height` float NOT NULL DEFAULT 0,
			`weight` double NOT NULL DEFAULT 0,
			PRIMARY KEY(`id`)
		) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci);
	int AffectedRows0 = Con->Query(query0, strlen(query0));

	char const* query1 = SQL(
		INSERT INTO `test` (`name`, `age`, `salary`, `savings`, `height`, `weight`)
		VALUES('John Doe', 42, 42000, -1337, 1.80, 98.35));
	int AffectedRows1 = Con->Query(query1, strlen(query1));

	return AffectedRows0 != -1 && AffectedRows1 == 1;
}

bool Test3_QueryWithOutput(Connection* Con)
{
	Result Res;
	char const* query0 = SQL(
		SELECT `name`,`age`,`salary`,`savings`,`height`,`weight`
		FROM `test`);
	int AffectedRows0 = Con->Query(query0, strlen(query0), &Res);
	if (AffectedRows0 != 1) return false;
	if (!Res.IsValid()) return false;
	if (Res.GetRowCount() != 1) return false;

	Row* Row0 = Res.GetRow(0);
	if (Row0->GetFieldCount() != 6) return false;

	Field* Name = Row0->GetField(0);
	Field* Age = Row0->GetField(1);
	Field* Salary = Row0->GetField(2);
	Field* Savings = Row0->GetField(3);
	Field* Height = Row0->GetField(4);
	Field* Weight = Row0->GetField(5);

	if (strncmp(Name->AsString(), "John Doe", Name->GetLength()) != 0)
		return false;

	if (Age->AsUnsignedByte() != 42)
		return false;

	if (Salary->AsUnsignedShort() != 42000)
		return false;

	if (Savings->AsInteger() != -1337)
		return false;

	if (Height->AsFloat() != 1.8f)
		return false;

	if (Weight->AsDouble() != 98.35)
		return false;

	return true;
}

bool Test4_PreparedStatement(Connection* Con)
{
	Statement Stmt(Con);
	Stmt.Init("SELECT name,age,salary,savings,height,weight FROM test WHERE id=?");

	int id = 1;
	Stmt.GetBindIn(0)->SetInput(SQL_TYPE::INTEGER, &id, sizeof(int));
	Stmt.GetBindOut(0)->SetOutput(SQL_TYPE::STRING, 255);
	Stmt.GetBindOut(1)->SetOutput(SQL_TYPE::UBYTE, 1);
	Stmt.GetBindOut(2)->SetOutput(SQL_TYPE::USHORT, 2);
	Stmt.GetBindOut(3)->SetOutput(SQL_TYPE::INTEGER, 4);
	Stmt.GetBindOut(4)->SetOutput(SQL_TYPE::FLOAT, 4);
	Stmt.GetBindOut(5)->SetOutput(SQL_TYPE::DOUBLE, 8);
	Stmt.BindIn();
	Stmt.BindOut();
	Stmt.Execute();

	// Fetch All.
	Result Res;
	int AffectedRows0 = Stmt.FetchAll(&Res);
	if (AffectedRows0 != 1) return false;
	if (!Res.IsValid()) return false;
	if (Res.GetRowCount() != 1) return false;

	Row* Row0 = Res.GetRow(0);
	if (Row0->GetFieldCount() != 6) return false;

	Field* Name = Row0->GetField(0);
	Field* Age = Row0->GetField(1);
	Field* Salary = Row0->GetField(2);
	Field* Savings = Row0->GetField(3);
	Field* Height = Row0->GetField(4);
	Field* Weight = Row0->GetField(5);

	if (strncmp(Name->AsString(), "John Doe", Name->GetLength()) != 0)
		return false;

	if (Age->AsUnsignedByte() != 42)
		return false;

	if (Salary->AsUnsignedShort() != 42000)
		return false;

	if (Savings->AsInteger() != -1337)
		return false;

	if (Height->AsFloat() != 1.8f)
		return false;

	if (Weight->AsDouble() != 98.35)
		return false;


    char const* query1 = SQL(
        INSERT INTO `test` (`name`, `age`, `salary`, `savings`, `height`, `weight`)
        VALUES('Jane Doe', 41, 41000, -1338, 1.50, 50.35));
    int AffectedRows1 = Con->Query(query1, strlen(query1));
    if (AffectedRows1 != 1)
        return false;

    int id2 = 2;
    Stmt.GetBindIn(0)->SetInput(SQL_TYPE::INTEGER, &id2, sizeof(int));
	Stmt.BindIn();
	Stmt.Execute();
	Stmt.Fetch();

	if (strncmp((char*)Stmt.GetBindOut(0)->GetData(), "Jane Doe", Stmt.GetBindOut(0)->GetLength()) != 0)
		return false;

	unsigned char* Age1 = (unsigned char*)Stmt.GetBindOut(1)->GetData();
	if (*Age1 != 41)
		return false;

	unsigned short* Salary1 = (unsigned short*)Stmt.GetBindOut(2)->GetData();
	if (*Salary1 != 41000)
		return false;

	int* Savings1 = (int*)Stmt.GetBindOut(3)->GetData();
	if (*Savings1 != -1338)
		return false;

	float* Height1 = (float*)Stmt.GetBindOut(4)->GetData();
	if (*Height1 != 1.5f)
		return false;

	double* Weight1 = (double*)Stmt.GetBindOut(5)->GetData();
	if (*Weight1 != 50.35)
		return false;

	return true;
}

bool Test5_Copy(class Connection* Con)
{
	Statement Stmt1(Con);
	Stmt1.Init("SELECT name FROM test WHERE id=?");

	int id = 1;
	Stmt1.GetBindIn(0)->SetInput(SQL_TYPE::INTEGER, &id, sizeof(int));
	Stmt1.GetBindOut(0)->SetOutput(SQL_TYPE::STRING, 255);
	Stmt1.BindIn();
	Stmt1.BindOut();
	Stmt1.Execute();

	Result Res1;
	Stmt1.FetchAll(&Res1);

	Result Res2 = Res1;
	if (Res1.GetRowCount() != Res2.GetRowCount())
		return false;

	Row* Row1 = Res1.GetRow(0);
	Row* Row2 = Res2.GetRow(0);
	if (Row1 == Row2)
		return false;

	if (Row1->GetFieldCount() != Row2->GetFieldCount())
		return false;

	Field* Name1 = Row1->GetField(0);
	Field* Name2 = Row2->GetField(0);
	if (Name1 == Name2)
		return false;

	if (Name1->GetLength() != Name2->GetLength())
		return false;

	if (strncmp(Name1->AsString(), Name2->AsString(), Name1->GetLength()) != 0)
		return false;

	return true;
}

bool Test6_RangeBasedLoops(class Connection* Con)
{
	Result Res;
	char const* Query = "SELECT name,age FROM test";
	Con->Query(Query, strlen(Query), &Res);

	int i = 0;
	for (Row* R : Res)
	{
		for (Field* F : (*R))
		{
			if (F == NULL) return false;
		}

		if (i == 0)
		{
			if (strncmp(R->GetField(0)->AsString(), "John Doe", R->GetField(0)->GetLength()) != 0)
				return false;

			if (R->GetField(1)->AsUnsignedByte() != 42)
				return false;
		}
		else if (i == 1)
		{
			if (strncmp(R->GetField(0)->AsString(), "Jane Doe", R->GetField(0)->GetLength()) != 0)
				return false;

			if (R->GetField(1)->AsUnsignedByte() != 41)
				return false;
		}

		i++;
	}

	return true;
}

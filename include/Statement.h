#pragma once

#include "Types.h"

/** The input / output bindings for prepared statements.
 *
 * @author Arvid Gerstmann
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
	/** Prepares the binding for input.
	 *
	 * @param Type Type of the input buffer.
	 * @param Buffer Input data.
	 * @param Length Length of the input data.
	 * @remarks The data of specified length will be *copied* into the binding.
	 */
	void SetInput(SQL_TYPE::Value Type, void const* Buffer, unsigned long Length);

	/** Prepares the binding for input.
	*
	* @param Type Type of the input buffer.
	* @param Buffer Input data.
	* @remarks The data of specified length will be *copied* into the binding.
	* @remarks Will guess the length depending on the type, and call `SetOutput` with the
	* correct length.
	* @warning Does not work for any variable length types, like VARCHAR.
	*/
	void SetInput(SQL_TYPE::Value Type, void const* Buffer);

	/** Prepares the binding for output.
	 * 
	 * @param Type Type of the output buffer.
	 * @param Length Maximum length of the output buffer.
	 * @remarks Only lengths bytes will be allocated for the output buffer, anything which
	 * exceeds the length will not be returned.
	 */
	void SetOutput(SQL_TYPE::Value Type, unsigned long Length);

	/** Prepares the binding for output.
	*
	* @param Type Type of the output buffer.
	* @remarks Will guess the length depending on the type, and call `SetOutput` with the
	* correct length.
	* @warning Does not work for any variable length types, like VARCHAR.
	*/
	void SetOutput(SQL_TYPE::Value Type);

	/** Access the buffered data */
	void const* GetData();

	/** Access the length of the buffered data */
	unsigned long GetLength();

	/** Whether the output data is NULL.
	 *
	 * @remarks Only valid for output bindings.
	 */
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

/** The prepared statements.
 *
 * Execution steps:
 *   1. Create prepared statement with a valid `Connection`. Or set it later via `SetConnection()`.
 *   2. Initialize the statement by passing a valid SQL query to `Init()`.
 *   3. Set the values of any parameters using `GetBindIn()`, once done call `BindIn()`.
 *   4. Call `Execute()`.
 *   5. If the statement produces a result set, configure the out buffers via `GetBindOut()`, once done call `BindOut()`.
 *   6. Fetch the data row by row with `Fetch()`, or all at once with `FetchAll()`.
 *   7. Repeat steps 3 through 6, by changing the parameters.
 *
 * @author Arvid Gerstmann
 */
class Statement
{
public:
	/** Constructs a new Statement, without a connection.
	 *
	 * @warning A valid connection has to be set, before use, through `setConnection()`.
	 */
	explicit Statement();

	/** Constructs a new Statement with connection.
	 *
	 * @param Con A valid connection.
	 */
	explicit Statement(class Connection* Con);
	virtual ~Statement();

private:
	Statement(Statement const& Other){}
    Statement& operator=(Statement const& Other){return *this;}

public:
	/** Initialize the Statement.
	 *
	 * Initializes the statement and creates the bindings for the query.
	 *
	 * @remark *Safe* to be called multiple times, though it's not recommended.
	 * @param Query A valid SQL query.
	 * @return Whether the initialization was successfull.
	 */
	bool Init(char const* Query);

	/** Binds the input bindings to the statement.
	 *
	 * @attention All bindings should be setup prior to calling `BindIn()`.
	 * @see GetBindIn()
	 * @return Whether the binding was successfull.
	 */
    bool BindIn();

	/** Binds the output bindings to the statement.
	 *
	 * @attention All bindings should be setup prior to calling `BindOut()`.
	 * @see GetBindOut()
	 * @return Whether the binding was successfull.
	 */
	bool BindOut();

	/** Generates the output binding automatically.
	 *
	 * @return Whether the generation was successfull.
	 */
	bool GenerateBindOut();

	/** Executes the statement.
	 *
	 * @attention All input and output bidings should be setup via `BindIn()` and `BindOut()
	 * prior to calling `Execute()`.
	 * @return Either the affected rows (UPDATE, DELETE, INSERT) or the number of rows in
	 * the result set (SELECT, EXPLAIN, etc). Or -1 for error.
	 */
	int Execute();

	/** Fetches one row of the result.
	 *
	 * The result can be accessed through the output binding.
	 *
	 * @return Whether there has been successfully a row fetched.
	 * @see Bind
	 */
	bool Fetch();

	/** Fetches and buffers all rows of the result.
	 *
	 * The results will be *copied* into the `Result` object.
	 * Only the last result will be available through the output binding.
	 *
	 * @warning Potentially long running operation. Use with caution.
	 * @param Res The `Result` where the data will be copied into.
	 * @return The number of rows fetched. Or -1 for error.
	 */
	int FetchAll(class Result* Res);

	/** Get the output binding for the index.
	 *
	 * @param Index The index of the output.
	 * @return The binding. NULL if no `Connection` is set, or the index is out of bounds.
	 * @see Bind
	 */
	Bind* GetBindIn(unsigned int Index);

	/** Get the input binding for the index.
	*
	* @param Index The index of the input.
	* @return The binding. NULL if no `Connection` is set, or the index is out of bounds.
	* @see Bind
	*/
	Bind* GetBindOut(unsigned int Index);

	/** Set the used connection. */
	void SetConnection(class Connection* Con);

	/** Retrieves the latest error */
	bool GetError(char* Buffer, unsigned long Length);

protected:
	void FreeBindsIn();
	void FreeBindsOut();
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

	bool OutBindsGenerated;
	bool OutBindsBound;
};
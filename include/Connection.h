#pragma once

#include "Types.h"

/**
 * The main class of the MariaDB connection.
 * Create a connection to a database with `Connect()`, and disconnect with `Close()`.
 *
 * @info If the Connection destructor is called, the connection is closed.
 */
class Connection
{
friend class Statement;

public:
	explicit Connection();
	virtual ~Connection();

private:
	Connection(Connection const& cpy){};
	Connection& operator=(Connection const& cpy){};

public:

	/** Connect to the specified server.
	 */
	bool Connect(char const* Host, char const* User, char const* Password, char const* DB, unsigned int Port);

	/** Disconnects from the current connected server.
	 *
	 * Does nothing if not connected to a server.
	 */
	void Close();

	/** Returns the current status of the connection.
	 *
	 * @return Whether connected to MySQL server.
	 */
	bool IsConnected();

	/** Executes a simple query, without expected result.
	 *
	 * @return The affected rows (UPDATE, DELETE, INSERT) or the rows in the result (SELECT),
	 * or -1 on error.
	 */
	int Query(char const* QueryStr, unsigned long Length);

	/** Executes a simple query, with expected results.
	 *
	 * @return The affected rows (UPDATE, DELETE, INSERT) or the rows in the result (SELECT),
	 * or -1 on error.
	 */
	int Query(char const* QueryStr, unsigned long Length, class Result* ResultOut);

	/** Executes a prepared statement.
	 *
	 * @return Whether the query was successfull.
	 */
	bool Execute(class Statement* stmt);

public:


public:
	static void ShowMySQLError(struct st_mysql* MySQL, char const* Call);

protected:
	struct st_mysql* MySQL;
	bool bIsConnected;
};
#pragma once

#include "Types.h"

/** The main class of the MariaDB connection.
 *
 * Create a connection to a database with `Connect()`, and disconnect with `Close()`.
 *
 * @remarks If the Connection destructor is called, the connection is closed.
 * @author Arvid Gerstmann
 */
class Connection
{
friend class Statement;

public:
	explicit Connection();
	virtual ~Connection();

private:
	Connection(Connection const& cpy){};
    Connection& operator=(Connection const& cpy){return *this;};

public:

	/** Connect to the specified server.
	 *
	 * @return Whether the connection attempt was successfull.
	 */
	bool Connect(char const* Host, char const* User, char const* Password, char const* DB, unsigned int Port);

	/** Disconnects from the current connected server.
	 *
	 * @remarks Does nothing if not connected to a server.
	 */
	void Close();

	/** Returns the current status of the connection.
	 *
	 * @return Whether connected to MySQL server.
	 */
	bool IsConnected();

	/** Executes a simple query, without expected result.
	 *
	 * @param QueryStr A valid SQL query.
	 * @param Length The length of the SQL query string.
	 * @return The affected rows (UPDATE, DELETE, INSERT) or the rows in the result (SELECT),
	 * or -1 on error.
	 */
	int Query(char const* QueryStr, unsigned long Length);

	/** Executes a simple query, with expected results.
	 *
	 * @param QueryStr A valid SQL query.
	 * @param Length The length of the SQL query string.
	 * @param ResultOut The `Result` object where the result will be copied to.
	 * @return The affected rows (UPDATE, DELETE, INSERT) or the rows in the result (SELECT),
	 * or -1 on error.
	 */
	int Query(char const* QueryStr, unsigned long Length, class Result* ResultOut);

public:


public:
	static void ShowMySQLError(struct st_mysql* MySQL, char const* Call);

protected:
	struct st_mysql* MySQL;
	bool bIsConnected;
};
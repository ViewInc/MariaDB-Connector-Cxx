#pragma once

#include "Types.h"
#include "Transaction.h"

/** The main class of the MariaDB connection.
 *
 * Create a connection to a database with `Connect()`, and disconnect with `Close()`.
 *
 * @remarks If the Connection destructor is called, the connection is closed.
 * @author Arvid Gerstmann
 */
class Connection {
    friend class Statement;

    friend class Transaction;

public:
    explicit Connection();

    virtual ~Connection();

private:
    Connection(Connection const &cpy) {};

    Connection &operator=(Connection const &cpy) { return *this; };

public:

    /** Connect to the specified server.
     *
     * @return Whether the connection attempt was successfull.
     */
    bool Connect(char const *Host, char const *User, char const *Password, char const *DB, unsigned int Port);

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

    /** Set the default database.
     * The specified database will be used for any subsequent queries.
     *
     * @return Whether the selection was successfull.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-select-db.html
     */
    bool SelectDatabase(char const *DatabaseName);

    /** Executes a simple query, without expected result.
	 *
	 * @param QueryStr A valid SQL query.
	 * @param Length The length of the SQL query string.
	 * @return The affected rows (UPDATE, DELETE, INSERT) or the rows in the result (SELECT),
	 * or -1 on error.
	 */
    int Query(char const *QueryStr, unsigned long Length);

    /** Executes a simple query, with expected results.
     *
     * @param QueryStr A valid SQL query.
     * @param Length The length of the SQL query string.
     * @param ResultOut The `Result` object where the result will be copied to.
     * @return The affected rows (UPDATE, DELETE, INSERT) or the rows in the result (SELECT),
     * or -1 on error.
     */
    int Query(char const *QueryStr, unsigned long Length, class Result *ResultOut);

    /** Retrieves the last error as a formatted string.
     *
     * @param Buffer A char buffer.
     * @param Length The length of the supplied char buffer.
     * @return Wheter an error occured.
     */
    bool GetError(char *Buffer, unsigned long Length);

    /** Retrieves the affected rows of the last query.
     *
     * @return The affected rows (for UPDATE, DELETE, INSERT), or the rows in the result (for SELECT),
     * or -1 on error.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-affected-rows.html
     */
    int GetAffectedRows();

    /** Sets the global character set.
     *
     * @param CharacterSet A valid MySQL character set (eg. utf8, utf8mb4, ascii).
     * @param Collation A valid MySQL collation (eg. utf8_general_ci, utf8mb4_unicode_ci). Can be NULL.
     * in this case, the default collation for the character set will be used.
     * @return Whether the query was successfull.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-set-character-set.html
     */
    bool SetCharacterSet(char const *CharacterSet, char const *Collation);

    /** Retrieves a string providing information about the most recently executed statement.
     *
     * @remarks Only returns a non-null value for INSERT INTO, SELECT, ALTER TABLE, UPDATE.
     * @return String or NULL if no information is available.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-info.html
     */
    char const *GetStatementInfo();

    /** Returns the value generated for an `AUTO_INCREMENT` column by the previous `INSERT` or `UPDATE`.
     *
     * @return 0 if the previous statement does not use an `AUTO_INCREMENT`, otherwise the id.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-insert-id.html
     */
    long GetLastAutoIncrementID();

    /** Checks whether the connection to the server is working.
     *
     * @remarks If auto-reconnect is enabled, `Ping()` will attempt to reconnect.
     * @return Whether the connection is working.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-ping.html
     */
    bool Ping();

    /** Returns a string containing the status of the mysql server.
     *
     * This includes uptime, number of running threads, questions, reloads, open tables etc.
     * @return Status string.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-stat.html
     */
    char const *GetStatus();

    /** Returns the number of errors, warnings, and notes generated after the previous statement.
     *
     * @return Warning count.
     */
    unsigned int GetWarningCount();

    /** Escapes a string to be a legal SQL string, to use in a statement.
     *
     * @remarks MariaDBs C library doesn't have mysql_real_escape_string_quote, that's why 
     * the old mysql_real_escape_string is used here. Keep that in mind.
     * @param Query Valid buffer for the query. Must be atleast strlen(String)*2+1 bytes long.
     * @param String String to escape.
     * @return Length of the encoded string, or -1 on error.
     * @see https://dev.mysql.com/doc/refman/5.7/en/mysql-real-escape-string.html
     */
    long EscapeString(char *Query, char const *String);

    // Commit / rollback support
    //
    TransactionRef create_transaction(isolation::level level = isolation::repeatable_read, bool consistent_snapshot = true);



public:
    static void ShowMySQLError(struct st_mysql *MySQL, char const *Call);

protected:
    struct st_mysql *MySQL;
    bool bIsConnected;
};
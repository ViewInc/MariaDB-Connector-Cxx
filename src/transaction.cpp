//
//  M A R I A D B + +
//
//	Author   : Sylvain Rochette Langlois
//	License  : Boost Software License (http://www.boost.org/users/license.html)
//


#include <Connection.h>
#include <Transaction.h>
#include <vector>
#include <mysql.h>


/*const char* g_isolation_level[] = {
    "SET TRANSACTION ISOLATION LEVEL REPEATABLE READ;",
    "SET TRANSACTION ISOLATION LEVEL READ COMMITTED;",
    "SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;",
    "SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;",
};

const char* g_consistent_snapshot[] = {
    "START TRANSACTION;",
    "START TRANSACTION WITH CONSISTENT SNAPSHOT;",
};*/

//
// Constructor
//
Transaction::Transaction(Connection *connection, bool consistent_snapshot) :
        m_connection(connection) {
}

//
// Destructor
//
Transaction::~Transaction() {
    if (!m_connection)
        return;

    mysql_rollback(m_connection->MySQL);
    cleanup();
}


//
// Cleanup transaction
//
void Transaction::cleanup() {
    for(std::vector<SavePoint *>::iterator it = m_save_points.begin(); it != m_save_points.end(); ++it){
        SavePoint * savePoint = *it;
        savePoint->m_transaction = NULL;
    }
}

//
// Commit the change
//
void Transaction::commit() {
    if (!m_connection)
        return;

    mysql_commit(m_connection->MySQL);
    cleanup();
    m_connection = NULL;
}

//
// Create save point
//
SavePointRef Transaction::create_save_point() {
    if (!m_connection)
        return SavePointRef();

    SavePoint *sp = new SavePoint(this);
    m_save_points.push_back(sp);
    return SavePointRef(sp);
}

//
// Remove save_point
//
void Transaction::remove_save_point(SavePoint *savePoint) {
    m_save_points.erase(std::remove(m_save_points.begin(), m_save_points.end(), savePoint));
}

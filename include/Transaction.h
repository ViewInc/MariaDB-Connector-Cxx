//
//  M A R I A D B + +
//
//	Author   : Sylvain Rochette Langlois
//	License  : Boost Software License (http://www.boost.org/users/license.html)
//

#ifndef _TRANSACTION_HPP_
#define _TRANSACTION_HPP_

#include <vector>
#include "SavePoint.h"

//
// Isolation level
//
namespace isolation
{
    enum level
    {
        repeatable_read = 0,
        read_committed,
        read_uncommitted,
        serializable
    };
}

//
// Transaction only work with connection for a short period of time, it's
// why the connection pointer is not a reference to the shared object.
//
// If a transaction exist while a connection as been closed, your code need to be changed.
//
class Connection;

class SavePoint;

class Transaction {
    friend class Connection;

    friend class SavePoint;

public:
    //
    // Destructor, rollback automatically
    //
    virtual ~Transaction();

    //
    // Commit the change
    //
    void commit();

    //
    // Create save point
    //
    SavePointRef create_save_point();

private:
    //
    // Constructor
    //
    Transaction(Connection *connection, bool consistent_snapshot);

    //
    // Remove save_point
    //
    void remove_save_point(SavePoint *save_point);

    //
    // Cleanup transaction
    //
    void cleanup();

private:
    Connection *m_connection;
    std::vector<SavePoint *> m_save_points;
};

typedef std::shared_ptr<Transaction> TransactionRef;


#endif

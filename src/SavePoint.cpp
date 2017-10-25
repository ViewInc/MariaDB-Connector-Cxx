//
//  M A R I A D B + +
//
//	Author   : Sylvain Rochette Langlois
//	License  : Boost Software License (http://www.boost.org/users/license.html)
//

#include <Connection.h>
#include <SavePoint.h>
#include <Transaction.h>


unsigned long long g_save_point_id = 0;

const char *g_save_point_commands[] = {
        "SAVEPOINT %s",
        "ROLLBACK TO SAVEPOINT %s",
        "RELEASE SAVEPOINT %s"
};

//
// Constructor
//
SavePoint::SavePoint(Transaction *transaction) :
        m_transaction(transaction) {
    char command[32];

    sprintf(command, "SP%llu", ++g_save_point_id);
    m_name = command;
    sprintf(command, g_save_point_commands[0], m_name.c_str());

    m_transaction->m_connection->Query(command, strlen(command));
}

//
// Destructor
//
SavePoint::~SavePoint() {
    if (!m_transaction)
        return;

    char command[32];
    sprintf(command, g_save_point_commands[1], m_name.c_str());

    m_transaction->remove_save_point(this);
    m_transaction->m_connection->Query(command, strlen(command));
}

//
// Commit the change
//
void SavePoint::commit() {
    if (!m_transaction)
        return;

    char command[32];
    sprintf(command, g_save_point_commands[2], m_name.c_str());

    m_transaction->remove_save_point(this);
    m_transaction->m_connection->Query(command, strlen(command));
    m_transaction = NULL;
}

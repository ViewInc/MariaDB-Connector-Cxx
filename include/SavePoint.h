//
//  M A R I A D B + +
//
//	Author   : Sylvain Rochette Langlois
//	License  : Boost Software License (http://www.boost.org/users/license.html)
//

#ifndef _SAVE_POINT_HPP_
#define _SAVE_POINT_HPP_

#include <memory>
#include <string>
#include <Connection.h>

class Connection;
class Transaction;

class SavePoint {
    friend class Connection;

    friend class Transaction;

public:
    //
    // Destructor, rollback automatically
    //
    virtual ~SavePoint();

    //
    // Commit the change
    //
    void commit();

private:
    //
    // Constructor
    //
    SavePoint(Transaction
              *transaction);

private:
    Transaction *m_transaction;
    std::string m_name;
};

typedef std::shared_ptr<SavePoint> SavePointRef;


#endif

/****************************************

数据库操作封装类

****************************************/

#ifndef DATABASE_H
#define DATABASE_H

#include <mysqlx/xapi.h>// for c++ version, its abi cxx11.

using namespace mysqlx;
using namespace std;
class Database
{
    public:
        Database();
        Database(char** opt);
        virtual ~Database();
        mysqlx_session_t* GetSession()
        {
            return sess;
        }
    protected:

    private:
        void InitDb();
        const int MAX_FD = 65535;

        mysqlx_session_t* sess;
        mysqlx_error_t* error;
        mysqlx_result_t* res;
        mysqlx_schema_t *db;
};

#endif // DATABASE_H

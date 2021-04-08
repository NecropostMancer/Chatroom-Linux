#ifndef ENTITY_H
#define ENTITY_H
#include <mysqlx/xapi.h>
#define RESULT_CHECK(R,C) if(!R) \
{ \
    printf("\nError! %s", mysqlx_error_message(C));\
}
#include "Database.h"
class Entity
{
    public:
        Entity();
        virtual ~Entity();
        void connect(Database* db){
            sess = db->GetSession();
        }
    protected:
        mysqlx_error_t* error;
        mysqlx_result_t* res;
        mysqlx_stmt_t* crud;
        mysqlx_schema_t *db;
        mysqlx_table_t *table;
        mysqlx_row_t *row;

        mysqlx_session_t* sess;
    private:



};
#endif // ENTITY_H

#include "Database.h"
#include "Log.h"

#define RESULT_CHECK(R,C) if(!R) \
{ \
    Log::Fatal("\nError! %s", mysqlx_error_message(C));\
}

//currently all methods are actually locked into a single thread.
//docker run -d -p 3306:3306 -p 33060:33060 --name mysqlx -e MYSQL_ROOT_PASSWORD=1234 mysql
//must be 64 bit?
Database::Database()
{
#define DROP // 初始化自动删库
    sess = mysqlx_get_session_from_url("mysqlx://root:1234@127.0.0.1:33060",&error);
    if(!sess)
    {
        Log::Fatal("Error mysqlx:%s",mysqlx_error_message(error));
        return;
    }

#ifdef DROP
    res = mysqlx_sql(sess,
                     "DROP DATABASE `db`",
                     MYSQLX_NULL_TERMINATED);
#endif

    res = mysqlx_sql(sess,
                     "CREATE DATABASE IF NOT EXISTS `db`" \
                     "CHARSET utf8 COLLATE utf8_general_ci",
                     MYSQLX_NULL_TERMINATED);
    RESULT_CHECK(res,sess);

    res = mysqlx_sql(sess,
                     "USE db",
                     MYSQLX_NULL_TERMINATED);

    res = mysqlx_sql(sess,
                     "CREATE TABLE IF NOT EXISTS ver" \
                     "(ver INT)",
                     MYSQLX_NULL_TERMINATED);
    RESULT_CHECK(res,sess);

    res = mysqlx_sql(sess,
                     "CREATE TABLE IF NOT EXISTS user" \
                     "(username VARCHAR(16) PRIMARY KEY, passwd VARCHAR(16)," \
                     "name VARCHAR(16))",
                     MYSQLX_NULL_TERMINATED);
    RESULT_CHECK(res,sess);

    res = mysqlx_sql(sess,
                     "CREATE TABLE IF NOT EXISTS room" \
                     "(roomID INT AUTO_INCREMENT PRIMARY KEY," \
                     " creator VARCHAR(16), name VARCHAR(16)," \
                     " state INT)",
                     MYSQLX_NULL_TERMINATED);
    RESULT_CHECK(res,sess);

    res = mysqlx_sql(sess,
                     "CREATE TABLE IF NOT EXISTS userInRoom" \
                     "(id BIGINT AUTO_INCREMENT PRIMARY KEY, username VARCHAR(16)," \
                     "roomID INT, privilege INT)",
                     MYSQLX_NULL_TERMINATED);
    RESULT_CHECK(res,sess);

    res = mysqlx_sql(sess,
                     "CREATE TABLE IF NOT EXISTS chatlog" \
                     "(id BIGINT AUTO_INCREMENT PRIMARY KEY, timestamp TIMESTAMP," \
                     "username VARCHAR(16), roomID VARCHAR(16))",
                     MYSQLX_NULL_TERMINATED);
    RESULT_CHECK(res,sess);

    db = mysqlx_get_schema(sess,"db",1);

}

Database::~Database()
{
    mysqlx_session_close(sess);
}




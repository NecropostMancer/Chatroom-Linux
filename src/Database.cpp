#include "Database.h"

#include "Log.h"

#include "User.h"

#define RESULT_CHECK(R,C) if(!R) \
{ \
    printf("\nError! %s", mysqlx_error_message(C));\
}

//currently all methods are actually locked into a single thread.
//docker run -d -p 3306:3306 -p 33060:33060 --name mysqlx -e MYSQL_ROOT_PASSWORD=1234 mysql
//must be 64 bit?
Database::Database()
{
    /*
    m_Context = redisConnectUnixWithTimeout("127.0.0.1",{1,500000});
    if(m_Context == NULL || m_Context -> err)
    {
        if(m_Context)
        {
            Log::Fatal(m_Context -> errstr);
            redisFree(m_Context);
        }else{
            Log::Fatal("Can not create connection");
        }

    }*/
    //sess = new Session("mysqlx://user:1234@127.0.0.1:3306/db?ssl-mode=disabled");
#define DROP



    sess = mysqlx_get_session_from_url("mysqlx://root:1234@127.0.0.1:33060",&error);
    if(!sess)
    {
        printf("Error mysqlx:%s",mysqlx_error_message(error));
        return;
    }

    m_UserAlive = new userAlive[MAX_FD];
    m_Livefd = new int[MAX_FD];

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
    //dtor
    //freeReplyObject(m_Reply);
    //redisFree(m_Context);
    //delete sess;
    mysqlx_session_close(sess);
}

bool Database::AddUser(User user)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                         "INSERT INTO db.user " \
                         "(username,passwd,name) VALUES (?,?,?)",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(user.m_UserName.c_str()),
                                    PARAM_STRING(user.m_PasswdSalt.c_str()),
                                    PARAM_STRING(user.m_ShowName.c_str()),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("add user bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);
    return true;
}

User Database::GetUser(std::string username)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                          "SELECT * FROM `user` " \
                          "WHERE username = ?",
                          MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_END);
    User user;
    if(rc != RESULT_OK)
    {
        Log::Error("get user bind failed.");
        return user;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    if((row = mysqlx_row_fetch_one(res)))
    {
        char tmp[50];
        size_t len= sizeof(tmp);
        user.m_UserName = username;
        mysqlx_get_bytes(row,1,0,tmp,&len);
        user.m_PasswdSalt = tmp;
        mysqlx_get_bytes(row,2,0,tmp,&len);
        user.m_ShowName = tmp;

    }
    return user;
}

bool Database::ModifyUserName(std::string username,std::string newname)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                          "UPDATE `user` " \
                          "SET name = ?" \
                          "WHERE username = ?",
                          MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(newname.c_str()),
                                    PARAM_STRING(username.c_str()),
                                    PARAM_END);
    User user;
    if(rc != RESULT_OK)
    {
        Log::Error("mod user bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);
    return true;
}

bool Database::AddRoom(std::string roomname,User user)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                         "INSERT INTO db.room " \
                         "(creator,name,state) VALUES (?,?,0)",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(user.m_UserName.c_str()),
                                    PARAM_STRING(roomname.c_str()),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("add room bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);
    return true;

}
int Database::GetUserJoinedRoom(Room** Roooooom,std::string username, size_t len)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                          "SELECT roomID from `userInRoom`" \
                          "WHERE username = ?" ,
                          MYSQLX_NULL_TERMINATED);// todo:escape % : stackoverflow:how-can-i-with-mysqli-make-a-query-with-like-and-get-all-results
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("get user joined room by name bind failed.");
        return -1;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    size_t i = 0;
    Roooooom[0] = new Room();
    Roooooom[0]->m_roomID = 0;
    Roooooom[0]->m_Name = "DEFAULT_PUBLIC_ROOM";
    Roooooom[0]->m_Creator = "SERVER";
    i++;
    while((row = mysqlx_row_fetch_one(res)))
    {
        Room* room = new Room();
        char tmp[50];
        size_t len= sizeof(tmp);
        long int tmp2;
        mysqlx_get_sint(row,0,&tmp2);
        room->m_roomID = static_cast<int>(tmp2);
        Roooooom[i] = room;
        i++;
        if(i > len){
            break;
        }
    }
    return min(i,len);
}

//delete room* by the caller.
int Database::GetAllRoomByName(Room** Roooooom,std::string name, size_t len)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                          "SELECT r.roomID, r.creator, r.name, u.name FROM `room` r INNER JOIN `user` u " \
                          "ON r.creator = u.username " \
                          "WHERE r.name LIKE CONCAT('%',?,'%') " \
                          "LIMIT ?" ,
                          MYSQLX_NULL_TERMINATED);// todo:escape % : stackoverflow:how-can-i-with-mysqli-make-a-query-with-like-and-get-all-results
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(name.c_str()),
                                    PARAM_UINT(len),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("get room by name bind failed.");
        return -1;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    size_t i = 0;
    while((row = mysqlx_row_fetch_one(res)))
    {
        Room* room = new Room();
        char tmp[50];
        size_t len= sizeof(tmp);
        long int tmp2;
        mysqlx_get_sint(row,0,&tmp2);
        room->m_roomID = static_cast<int>(tmp2);
        mysqlx_get_bytes(row,1,0,tmp,&len);
        room->m_Creator = tmp;//todo:
        mysqlx_get_bytes(row,2,0,tmp,&len);
        room->m_Name = tmp;
        Roooooom[i] = room;
        i++;
        if(i > len){
            break;
        }
    }
    return min(i,len);
}

Room Database::GetRoom(int roomID)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                          "SELECT r.roomID, r.creator, r.name, r.id, u.showname FROM `room` r INNER JOIN `user` u " \
                          "ON r.creator = u.username" \
                          "WHERE roomID = ?",
                          MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_SINT(roomID),
                                    PARAM_END);
    Room room;
    if(rc != RESULT_OK)
    {
        Log::Error("get room by id bind failed.");
        return room;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    if((row = mysqlx_row_fetch_one(res)))
    {
        char tmp[50];
        size_t len= sizeof(tmp);
        room.m_roomID = roomID;
        mysqlx_get_bytes(row,1,0,tmp,&len);
        room.m_Creator = tmp;//todo:
        mysqlx_get_bytes(row,2,0,tmp,&len);
        room.m_Name = tmp;
    }
    return room;
}
//both
bool Database::JoinRoom(std::string username,int roomid,Channel* channel)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                         "INSERT INTO db.userInRoom " \
                         "(username,roomID,privilege) VALUES (?,?,?)",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_SINT(roomid),
                                    PARAM_SINT(0),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("join room bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    if(AliveChannel.find(roomid) == AliveChannel.end()){
        channel = new Channel(roomid);
        AliveChannel.insert(std::pair<int,Channel*>(roomid,channel));
    }
    else{
        channel = (*AliveChannel.find(roomid)).second;
    }
    return true;
}

bool Database::JoinRoom(std::string username,int roomid)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                         "INSERT INTO db.userInRoom " \
                         "(username,roomID,privilege) VALUES (?,?,?)",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_SINT(roomid),
                                    PARAM_SINT(0),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("join room bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    return true;
}


int Database::GetSubscribedChannel(std::string username,Channel** channels,size_t max_len)
{
    Room** start = new Room*[max_len];
    int ret = GetUserJoinedRoom(start,username,max_len);
    Channel * channel;
    for(int i=0;i<ret;i++)
    {
        int roomid = (*(start + i))->m_roomID;
        if(AliveChannel.find(roomid) == AliveChannel.end()){
            channel = new Channel(roomid);
            AliveChannel.insert(std::pair<int,Channel*>(roomid,channel));
        }
        else{
            channel = (*AliveChannel.find(roomid)).second;
        }
        channels[i] = channel;
    }
    delete [] start;
    return ret;
}

bool Database::LeaveRoom(std::string username,int roomid)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                         "DELETE FROM db.userInRoom " \
                         "WHERE username = ? AND roomid = ?",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_SINT(roomid),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("leave room bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);
    return true;
}

bool Database::SetPrivilege(std::string username,int roomid,int level)
{

    std::lock_guard<std::mutex> lock(m_WorkLock);
    return false;
}

int Database::GetPrivilege(std::string username,int roomid)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    return 0;
}

bool Database::AddChatlog(std::string username,int roomid,time_t time)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    return false;
}

ChatLog Database::GetChatlogAfterTime(int roomid,time_t time,int cnt)
{
    std::lock_guard<std::mutex> lock(m_WorkLock);
    return ChatLog();
}




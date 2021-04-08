#include "User.h"

User::User()
{
    //ctor
}

User::~User()
{
    //dtor
}

bool User::AddUser()
{
    crud = mysqlx_sql_new(sess,
                         "INSERT INTO db.user " \
                         "(username,passwd,name) VALUES (?,?,?)",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(m_UserName.c_str()),
                                    PARAM_STRING(m_PasswdSalt.c_str()),
                                    PARAM_STRING(m_ShowName.c_str()),
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
/*
 * @brief 查询用户
 * @param username 用户名:字符串
 * @param
 *
 * @return true = 成功
*/
bool User::GetUser(std::string username)
{
    crud = mysqlx_sql_new(sess,
                          "SELECT * FROM `user` " \
                          "WHERE username = ?",
                          MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("get user bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    if((row = mysqlx_row_fetch_one(res)))
    {
        char tmp[50];
        size_t len= sizeof(tmp);
        m_UserName = username;
        mysqlx_get_bytes(row,1,0,tmp,&len);
        m_PasswdSalt = tmp;
        mysqlx_get_bytes(row,2,0,tmp,&len);
        m_ShowName = tmp;
        return true;
    }else{
        return false;
    }
}
/*
 * @brief 修改用户名
 * @param username 用户名:string
 * @param newname 新名字:string
 *
 * @return true = 成功
*/
bool User::ModifyUserName(std::string newname)
{
    crud = mysqlx_sql_new(sess,
                          "UPDATE `user` " \
                          "SET name = ?" \
                          "WHERE username = ?",
                          MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(newname.c_str()),
                                    PARAM_STRING(m_UserName.c_str()),
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
/*
 * @brief 加入房间
 *
 * @param error错误码
 * @desc
 * @return 加入的房间的channel
*/
Channel* User::JoinRoomC(int roomid,int* error)
{
    crud = mysqlx_sql_new(sess,
                         "INSERT INTO db.userInRoom " \
                         "(username,roomID,privilege) VALUES (?,?,?)",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(m_UserName.c_str()),
                                    PARAM_SINT(roomid),
                                    PARAM_SINT(0),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("join room bind failed.");
        *error = 0;
        return nullptr;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);
    Channel* channel = nullptr;
    if(Channel::AliveChannel.find(roomid) == Channel::AliveChannel.end()){
        channel = new Channel(roomid);
        Channel::AliveChannel.insert(std::pair<int,Channel*>(roomid,channel));
    }
    else{
        channel = (*Channel::AliveChannel.find(roomid)).second;
    }
    return channel;
}

//用于登陆时自动加入channel
int User::GetSubscribedChannel(Channel** channels,Room** AllRoom,size_t max_len)
{
    int ret = 0;
    Channel * channel;
    for(int i=0;i<max_len;i++)
    {
        int roomid = (*(AllRoom + i))->m_roomID;
        if(Channel::AliveChannel.find(roomid) == Channel::AliveChannel.end()){
            channel = new Channel(roomid);
            Channel::AliveChannel.insert(std::pair<int,Channel*>(roomid,channel));
            ret++;
        }
        else{
            channel = (*Channel::AliveChannel.find(roomid)).second;
        }
        channels[i] = channel;
    }
    return ret;
}


bool User::LeaveRoom(int roomid)
{
    crud = mysqlx_sql_new(sess,
                         "DELETE FROM db.userInRoom " \
                         "WHERE username = ? AND roomid = ?",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(m_UserName.c_str()),
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
bool User::SetPrivilege(int roomid,int level)
{
    return false;
}

int User::GetPrivilege(int roomid)
{
    return 0;
}

bool User::AddChatlog(int roomid,time_t time)
{
    return false;
}

ChatLog User::GetChatlogAfterTime(int roomid,time_t time,int cnt)
{
    return ChatLog();
}

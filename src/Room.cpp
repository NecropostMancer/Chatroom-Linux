#include "Room.h"
#include "Log.h"
Room::Room()
{
    //ctor
}

Room::~Room()
{
    //dtor
}
/*
 * @brief 添加房间
 * @param roomname 房间名
 * @param username 用户名
 * @desc
 * @return 新房间的id，在大多数地方为int32t，数据库中为64t
*/
int64_t Room::AddRoom(std::string roomname,std::string username)
{
    crud = mysqlx_sql_new(sess,
                         "INSERT INTO db.room " \
                         "(creator,name,state) VALUES (?,?,0)",
                         MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_STRING(roomname.c_str()),
                                    PARAM_END);
    if(rc != RESULT_OK)
    {
        Log::Error("add room bind failed.");
        return -1;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    crud = mysqlx_sql_new(sess,
                         "SELECT roomID from db.room " \
                         "WHERE creator = ? AND name = ?",
                         MYSQLX_NULL_TERMINATED);
    rc = mysqlx_stmt_bind(crud, PARAM_STRING(username.c_str()),
                                    PARAM_STRING(roomname.c_str()),
                                    PARAM_END);
    res = mysqlx_execute(crud);

    if((row = mysqlx_row_fetch_one(res)))
    {
        int64_t ID = -1;
        mysqlx_get_sint(row,0,&ID);
        return ID;
        m_Name = roomname;
        m_Creator = username;

    }
    return -1;

}
/*
 * @brief 获得user当前加入的所有房间
 * @param Roooom 结果在数组中
 * @param
 * @param len 数组预留的长度
 * @desc 由调用者清理内存(roooom)
 * @return 实际返回的结果数量
*/
int Room::GetUserJoinedRoom(Room** Roooooom,std::string username, size_t len)
{
    crud = mysqlx_sql_new(sess,
                          "SELECT r.roomID, u.name, u.creator from `userInRoom` r INNER JOIN `room` u " \
                          "ON r.roomID = u.roomID "
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
        mysqlx_get_bytes(row,1,0,tmp,&len);
        room->m_Name = tmp;
        mysqlx_get_bytes(row,2,0,tmp,&len);
        room->m_Creator = tmp;
        Roooooom[i] = room;
        i++;
        if(i > len){
            break;
        }
    }
    return min(i,len);
}
/*
 * @brief 以名字查询所有房间
 * @param Roooom 结果在数组中
 * @param
 * @param len 数组预留的长度
 * @desc 由调用者清理内存(roooom)
 * @return 实际返回的结果数量
*/
int Room::GetAllRoomByName(Room** Roooooom,std::string name, size_t len)
{
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
/*
 * @brief 获得id房间
 *
 * @param
 * @desc
 * @return 结果
*/
bool Room::GetRoom()
{
    //std::lock_guard<std::mutex> lock(m_WorkLock);
    crud = mysqlx_sql_new(sess,
                          "SELECT r.roomID, r.creator, r.name, r.state, u.name FROM `room` r INNER JOIN `user` u " \
                          "ON r.creator = u.username " \
                          "WHERE roomID = ?",
                          MYSQLX_NULL_TERMINATED);
    int rc = mysqlx_stmt_bind(crud, PARAM_SINT(m_roomID),
                                    PARAM_END);
    Room room;
    if(rc != RESULT_OK)
    {
        Log::Error("get room by id bind failed.");
        return false;
    }
    res = mysqlx_execute(crud);

    RESULT_CHECK(res,crud);

    if((row = mysqlx_row_fetch_one(res)))
    {
        char tmp[50];
        size_t len= sizeof(tmp);
        mysqlx_get_bytes(row,1,0,tmp,&len);
        m_Creator = tmp;//todo:
        mysqlx_get_bytes(row,2,0,tmp,&len);
        m_Name = tmp;
        int64_t re;
        mysqlx_get_sint(row,3,&re);
        m_Lock = re;
        return true;
    }
    return false;
}
bool Room::UpdateRoom(std::string username,std::string newname,bool lock)
{
    GetRoom();
    std::string mutable_Newname = newname;
    if(m_Creator == username)
    {
        if(newname == "")mutable_Newname = m_Name;
        int state = lock? 1:0;
        crud = mysqlx_sql_new(sess,
                          //"UPDATE `db`.`room` " \
                          //"SET `name` = ? , `state` = ? " \
                          //"WHERE (`roomID` = ?);",
                          "UPDATE `db`.`room` SET `name`= ?,`state` = ? WHERE (`roomID` = ?);",
                          MYSQLX_NULL_TERMINATED);

        int rc = mysqlx_stmt_bind(crud, PARAM_STRING(mutable_Newname.c_str()),
                                        PARAM_SINT(state),
                                        PARAM_SINT(m_roomID),
                                        PARAM_END);

        if(rc != RESULT_OK)
        {
            Log::Error("update room bind failed.");
            return false;
        }
        res = mysqlx_execute(crud);

        RESULT_CHECK(res,crud);
        return true;
    }
    return false;
}

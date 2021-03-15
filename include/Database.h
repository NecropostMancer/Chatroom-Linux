#ifndef DATABASE_H
#define DATABASE_H

#include <mysqlx/xapi.h>// for c++ version, its abi cxx11.
#include <mutex>
#include <map>
#include "User.h"
#include "Room.h"
#include "ChatLog.h"

using namespace mysqlx;
using namespace std;
class Database
{
    public:
        Database();
        virtual ~Database();
        bool AddUser(User user);
        User GetUser(std::string username);
        bool GetUserInRoom(int roomID, bool aliveOnly = true);
        bool ModifyUserName(std::string username,std::string newname);
        bool AddRoom(std::string roomname,User user);
        bool GetAllRoomByName(Room** Roooooom,std::string name, size_t len);
        Room GetRoom(int roomID);
        bool JoinRoom(std::string username,int roomid);
        bool LeaveRoom(std::string username,int roomid);
        bool LockRoom(std::string username,int roomid,bool unlock = false);
        bool SetPrivilege(std::string username,int roomid,int level);
        int GetPrivilege(std::string username,int roomid);
        bool AddChatlog(std::string username,int roomid,time_t time);
        ChatLog GetChatlogAfterTime(int roomid,time_t time,int cnt = -1);
        //following are in-memory storage
        void OpenUser(int socketfd,sockaddr_in addr);
        void CloseUser(int socketfd);

        int GetUserSocket(std::string username);
        bool GetUsersSocket(std::string[] username,int[] result,int maxR = 100);
    protected:

    private:
        //redisContext* m_Context;//warn:not thread-safe
        //redisReply* m_Reply;
        mysqlx_session_t* sess;
        mysqlx_error_t* error;
        mysqlx_result_t* res;
        mysqlx_stmt_t* crud;
        mysqlx_schema_t *db;
        mysqlx_table_t *table;
        mysqlx_row_t *row;
        std::mutex m_WorkLock;

        map<string,user> UsernameToIndentify;
        map<string,string> UsernameToToken;
        constexpr static size_t BUFFER_SIZE = 512;

        struct userAlive
        {
            int m_Sockfd;
            sockaddr_in m_Address;
        }

        userAlive* m_UserAlive;
        int* m_Livefd;

};

#endif // DATABASE_H

#ifndef DATABASE_H
#define DATABASE_H

#include <mysqlx/xapi.h>// for c++ version, its abi cxx11.
#include <mutex>
#include <map>
#include "User.h"
#include "Room.h"
#include "ChatLog.h"
#include <sys/socket.h>
#include <netinet/in.h>

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
        inline void OpenUser(int socketfd,sockaddr_in addr,std::string Username)
        {

            m_UserCount++;
            //m_Livefd[UserCount] = socketfd;
            m_UserAlive[socketfd].m_Sockfd = socketfd;
            m_UserAlive[socketfd].m_Address = addr;
            m_UserAlive[socketfd].m_UserName = Username.c_str();
            if(UsernameToIndentify.insert(std::pair<string,int>(Username,socketfd)).second == true)
            {
                UsernameToIndentify[Username] = socketfd;
            }
        }
        inline void CloseUser(int socketfd)
        {
            m_UserAlive[socketfd] = m_UserAlive[m_Livefd[m_UserCount]];
            //m_Livefd[socketfd] = m_Livefd[m_UserCount];
            m_UserCount--;
        }

        inline int GetUserSocket(std::string username)
        {
            return UsernameToIndentify[username];
        }
        inline bool GetUsersSocket(std::string username[],int length,int result[],int maxR = 100)
        {
            int limit = min(maxR,length);
            if(limit == 0)
            {
                return false;
            }
            for(int i = 0;i<limit;i++)
            {
                result[i] = UsernameToIndentify[username[i]];
            }
            return true;
        }

    protected:

    private:

        const int MAX_FD = 65535;
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

        map<string,int> UsernameToIndentify;
        map<string,string> UsernameToToken;
        constexpr static size_t BUFFER_SIZE = 512;

        struct userAlive
        {
            int m_Sockfd;
            sockaddr_in m_Address;
            const char* m_UserName;
        };

        userAlive* m_UserAlive;
        int* m_Livefd;
        int m_UserCount = 0;

};

#endif // DATABASE_H

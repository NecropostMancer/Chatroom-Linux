/****************************************

用户的数据库实体映射类

****************************************/

#ifndef USER_H
#define USER_H

#include <string>
#include "Entity.h"
#include "Channel.h"
#include "ChatLog.h"
#include "Room.h"

class User : public Entity
{
    public:
        User();
        User(Database& db)
        {
            sess = db.GetSession();
        }

        virtual ~User();

        bool AddUser();
        bool GetUser(std::string username);
        bool GetUserInRoom(int roomID, bool aliveOnly = true);
        bool ModifyUserName(std::string newname);

        Channel* JoinRoomC(int roomid,int* error);
        int GetSubscribedChannel(Channel** channels,Room** AllRoom,size_t max_len);
        bool LeaveRoom(int roomid);
        bool LockRoom(int roomid,bool unlock = false);
        bool SetPrivilege(int roomid,int level);
        int GetPrivilege(int roomid);
        bool AddChatlog(int roomid,time_t time);
        ChatLog GetChatlogAfterTime(int roomid,time_t time,int cnt = -1);

        std::string m_UserName;
        std::string m_PasswdSalt;
        std::string m_ShowName;
    protected:

    private:

        int m_Socket;
        std::string m_Token;
};

#endif // USER_H

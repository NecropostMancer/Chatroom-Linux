/****************************************

用户的数据库实体映射类

****************************************/

#ifndef USER_H
#define USER_H

#include <string>

class Room;

class User
{
    public:
        User();
        User(std::string username,std::string pwds,std::string showname):
            m_UserName(username),m_PasswdSalt(pwds),m_ShowName(showname){};
        virtual ~User();
        void Register();
        void Login();
        void Logout();
        void Join(Room* room);
        void Leave(Room* room);

        void Kick(Room* room,User* other);
        void Lock(Room* room);
        void Say(Room* room);

        std::string m_UserName;
        std::string m_PasswdSalt;
        std::string m_ShowName;
    protected:

    private:

        int m_Socket;
        std::string m_Token;
};

#endif // USER_H

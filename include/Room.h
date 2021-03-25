/****************************************

房间的数据库实体映射

****************************************/


#ifndef ROOM_H
#define ROOM_H


#include "User.h"

class Room
{
    public:
        Room();
        virtual ~Room();
        void Add(User* user);
        void Remove(User* user);

        int m_roomID;
        int m_Lock = 0;
        std::string m_Creator;
        std::string m_Name;
    protected:

    private:


};

#endif // ROOM_H

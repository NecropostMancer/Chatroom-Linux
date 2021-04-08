/****************************************

房间的数据库实体映射

****************************************/


#ifndef ROOM_H
#define ROOM_H

#include "Entity.h"
#include <string>
class Room : public Entity
{
    public:
        Room();
        virtual ~Room();
        bool GetRoom();
        int GetUserJoinedRoom(Room** Roooooom,std::string username, size_t len);//don't forget delete[]
        int64_t AddRoom(std::string roomname,std::string username);
        bool UpdateRoom(std::string username,std::string newname,bool lock);
        int GetAllRoomByName(Room** Roooooom,std::string name, size_t len);//don't forget delete[]

        int m_roomID;
        int m_Lock = 0;
        std::string m_Creator;
        std::string m_Name;
    protected:

    private:


};

#endif // ROOM_H

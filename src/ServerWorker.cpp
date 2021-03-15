#include "ServerWorker.h"
#include <sys/epoll.h>
#include <unistd.h>

#include "Log.h"
Database* ServerWorker::m_db = nullptr;
ServerWorker::ServerWorker()
{

    //ctor
}

ServerWorker::~ServerWorker()
{
    //dtor
}

void ServerWorker::process(const char* data)
{
    m_ReadBuf = data;
    ParseMessage();
    PostResponse(65535);
}


#include "User.pb.h"
#include "Chat.pb.h"
#include "Room.pb.h"
#include "Error.pb.h"

bool ServerWorker::ParseMessage()
{
    RegisterRequest regReq;
    LoginRequest logReq;
    ChangeNameRequest changeName;
    ChatMessageRequest chatIncoming;
    RoomRequest roomReq;
    if(regReq.ParseFromString(m_ReadBuf))
    {
        Log::Debug("RegisterRequest.");
        return true;
    }
    if(logReq.ParseFromString(m_ReadBuf))
    {
        Log::Debug("LoginRequest.");
        return true;
    }
    if(changeName.ParseFromString(m_ReadBuf))
    {
        Log::Debug("ChangeNameRequest.");
        return true;
    }
    if(chatIncoming.ParseFromString(m_ReadBuf))
    {
        Log::Debug("ChatRequest.");
        return true;
    }
    if(roomReq.ParseFromString(m_ReadBuf))
    {
        Log::Debug("RoomRequest.");
        return true;
    }
    Log::Debug("invaild.");
    return false;
}

bool ServerWorker::PostResponse(int len)
{
    return false;
}

bool Register()
{
    return false;
}
bool Login()
{
    return false;
}
bool Logout()
{
    return false;
}
bool ChangeName()
{
    return false;
}
bool JoinRoom()
{
    return false;
}
bool leaveRoom()
{
    return false;
}
bool CreateRoom()
{
    return false;
}
bool LockRoom()
{
    return false;
}
bool KickUser()
{
    return false;
}
bool PromoteUser()
{
    return false;
}
bool Chat()
{
    return false;
}

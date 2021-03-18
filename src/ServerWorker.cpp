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



bool ServerWorker::ParseMessage()
{
    WrapperClientMessage msg;
    if(msg.ParseFromString(m_ReadBuf))
    {
        int msgCase = msg.msg_case();
        Log::Debug("msg case:%d",msgCase);

        switch(msgCase){
            case 0://NO_ONE_OF
                break;
            case 1:
                Chat(msg.chatmessagerequest());
                break;
            case 2:

                break;
            case 3:
                break;
            case 4:
                Login(msg.loginrequest());
                break;
            case 5:
                Register(msg.registerrequest());
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
        }
    }else{
    /*
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
    }*/
        Log::Debug("invaild.");
        return false;
    }
    return true;
}

bool ServerWorker::PostResponse(int len)
{
    int cnt = write(m_Sockfd,m_Out.c_str(),m_Out.length());
    Log::Debug("Write %d bytes.",cnt);
    return true;
}

bool ServerWorker::Register(RegisterRequest req)
{
    WrapperServerMessage res;
    User user = ServerWorker::m_db->GetUser(req.username());
    if(user.m_UserName.length() != 0)
    {
        LoginResponse* error = res.mutable_loginresponse();
        error->mutable_error()->set_code(Error::WRONGAUTH);
        res.SerializeToString(&m_Out);
        return false;

    }else{
        user.m_UserName = req.username();
        user.m_ShowName = req.showname();
        user.m_PasswdSalt = req.password();
        if(ServerWorker::m_db->AddUser(user))
        {
            LoginResponse* succ = res.mutable_loginresponse();
            res.SerializeToString(&m_Out);
            return true;
        }
    }
    Error* error = res.mutable_error();
    error->set_code(Error::INNERERROR);
    res.SerializeToString(&m_Out);

    return false;
}
bool ServerWorker::Login(LoginRequest req)
{
    WrapperServerMessage res;
    User user = ServerWorker::m_db->GetUser(req.username());
    if(user.m_UserName.length() == 0)
    {
        LoginResponse* error = res.mutable_loginresponse();
        error->mutable_error()->set_code(Error::WRONGAUTH);
        res.SerializeToString(&m_Out);
        return false;

    }else{
        m_userName = user.m_UserName = req.username();
        user.m_PasswdSalt = req.password();
        User user = ServerWorker::m_db->GetUser(user.m_UserName);
        if(user.m_UserName.length()!=0 && user.m_PasswdSalt == req.password())
        {
            LoginResponse* succ = res.mutable_loginresponse();
            succ->set_token( user.m_UserName + "@" + "12345");
            ServerWorker::m_db->OpenUser(m_Sockfd,m_Address,req.username());
            res.SerializeToString(&m_Out);
            InitChattingState();
            return true;
        }
    }
    Error* error = res.mutable_error();
    error->set_code(Error::INNERERROR);
    res.SerializeToString(&m_Out);
    return false;
}
bool ServerWorker::Logout()
{
    for(auto pair:m_MyChannel)
    {
        pair.second->Leave(this);
    }
    return true;
}
bool ServerWorker::ChangeName()
{
    return false;
}
bool ServerWorker::JoinRoom(RoomRequest req)
{
    return false;
}
bool ServerWorker::leaveRoom()
{
    return false;
}
bool ServerWorker::CreateRoom()
{
    return false;
}
bool ServerWorker::LockRoom()
{
    return false;
}
bool ServerWorker::KickUser()
{
    return false;
}
bool ServerWorker::PromoteUser()
{
    return false;
}
bool ServerWorker::Chat(ChatMessageRequest)
{
    return false;
}
void ServerWorker::InitChattingState()
{
    Channel* ChannelList[100] = {0};

    int len = ServerWorker::m_db->GetSubscribedChannel(m_userName,ChannelList,100);
    for(int i =0;i<len;i++)
    {
        m_MyChannel.insert(std::pair<int,Channel*>(ChannelList[i]->GetRoomID(),ChannelList[i]));
        ChannelList[i]->Join(this);
    }

}

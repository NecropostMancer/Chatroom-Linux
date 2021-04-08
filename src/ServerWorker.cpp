/*
 * 调用process，传入网络字节流->
 * 处理消息->
 * 结果写入socket
*/


#include "ServerWorker.h"
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

void ServerWorker::process(const char* data, int len)
{
    m_ReadBuf = data;
    ParseMessage(len);
    PostResponse(65535);
}

//以下分发请求

bool ServerWorker::ParseMessage(int len)
{
    WrapperClientMessage msg;
    printf("ServerWorker : Get data(%d bytes) \n",strlen(m_ReadBuf));
    for(int i = 0;i<strlen(m_ReadBuf);i++){
        printf("%d ",m_ReadBuf[i]);
    }
    printf("\n");
    std::string tmp(m_ReadBuf,m_ReadBuf+len);
    if(msg.ParseFromString(tmp))
    {
        int msgCase = msg.msg_case();

        Log::Debug("msg case:%d",msgCase);

        switch(msgCase){
            case 0://NO_ONE_OF
                break;
            case 1://IMPOSSIBLE

                break;
            case 2:
                if(!login)return false;
                Chat(msg.chatmessagerequest());
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
                if(!login)return false;
                ChangeName(msg.changenamerequest());
                break;
            case 7:
                if(!login)return false;
                RoomReq(msg.roomrequest());
                break;
            case 8:
                if(!login)return false;
                DoRoomControl(msg.roomcontrol());
                break;
        }
    }else{
        Log::Debug("invaild.");
        return false;
    }
    return true;
}

bool ServerWorker::RoomReq(RoomRequest req)
{
    WrapperServerMessage res;
    switch(req.operation()){
        case RoomRequest_Operation_CREATE:
            return CreateRoom(req.payload());
            break;
        case RoomRequest_Operation_JOIN:
            return JoinRoom(req.roomid());
            break;
        case RoomRequest_Operation_LEAVE:
            return leaveRoom(req.roomid());
            break;
        case RoomRequest_Operation_QUERY:
            return GetAllRooms();
            break;
        default:
            RoomResponse * rres = res.mutable_roomresponse();
            rres->mutable_error()->set_code(Error::NOTFOUND);
            res.SerializeToString(&m_Out);
            return false;
            break;

    }
    RoomResponse * rres = res.mutable_roomresponse();
    rres->mutable_error()->set_code(Error::INNERERROR);
    res.SerializeToString(&m_Out);
    return false;
}

bool ServerWorker::DoRoomControl(RoomControl req)
{
    WrapperServerMessage res;
    switch(req.operation()){
        case RoomControl_Operation_LOCK:
            return LockRoom(req.roomid(),true);
            break;
        case RoomControl_Operation_UNLOCK:
            return LockRoom(req.roomid(),false);
            break;
        case RoomControl_Operation_KICK:
            return KickUser(req.roomid(),req.payload());
            break;
        case RoomControl_Operation_PROMOTE:
            return PromoteUser(req.roomid(),req.username(),3);
            break;
        default:

            res.mutable_error()->set_code(Error::NOTFOUND);
            res.SerializeToString(&m_Out);
            return false;
            break;
    }
    RoomResponse * rres = res.mutable_roomresponse();
    rres->mutable_error()->set_code(Error::INNERERROR);
    res.SerializeToString(&m_Out);
    return false;
}

//以下为必需过程

//回复
bool ServerWorker::PostResponse(int len)
{
    int cnt = write(m_Sockfd,m_Out.c_str(),m_Out.length());
    Log::Debug("Write %d bytes.",cnt);
    return true;
}
//初始化
void ServerWorker::InitChattingState()
{
    Channel* ChannelList[100] = {0};
    Room* RoomList[100];
    memset(&RoomList,0,sizeof(RoomList));
    Room roomCol;
    roomCol.connect(ServerWorker::m_db);
    int cnt = roomCol.GetUserJoinedRoom(RoomList,m_User.m_UserName,100);
    int len = m_User.GetSubscribedChannel(ChannelList,RoomList,cnt);
    for(int i =0;i<len;i++)
    {
        Log::Debug("id:%d",ChannelList[i]->GetRoomID());
        m_MyChannel->insert(std::pair<int,Channel*>(ChannelList[i]->GetRoomID(),ChannelList[i]));
        ChannelList[i]->Join(this);
    }

}

//以下处理请求

bool ServerWorker::Register(RegisterRequest req)
{
    WrapperServerMessage res;
    m_User.connect(ServerWorker::m_db);
    m_User.GetUser(req.username());

    if(m_User.m_UserName.length() != 0)
    {
        LoginResponse* error = res.mutable_loginresponse();
        error->mutable_error()->set_code(Error::WRONGAUTH);
        res.SerializeToString(&m_Out);
        return false;

    }else{
        m_User.m_UserName = req.username();
        m_User.m_ShowName = req.showname();
        m_User.m_PasswdSalt = req.password();
        if(m_User.AddUser())
        {
            LoginResponse* succ = res.mutable_loginresponse();
            succ->mutable_error()->set_code(Error::SUCCESS);
            res.SerializeToString(&m_Out);
            return true;
        }
    }
    LoginResponse* error = res.mutable_loginresponse();
    error->mutable_error()->set_code(Error::INNERERROR);
    res.SerializeToString(&m_Out);

    return false;
}
bool ServerWorker::Login(LoginRequest req)
{
    WrapperServerMessage res;
    m_User.connect(ServerWorker::m_db);
    m_User.GetUser(req.username());

    if(m_User.m_UserName.length() == 0)
    {
        LoginResponse* error = res.mutable_loginresponse();
        error->mutable_error()->set_code(Error::WRONGAUTH);
        res.SerializeToString(&m_Out);
        return false;

    }else{
        if(m_User.m_PasswdSalt == req.password())
        {

            LoginResponse* succ = res.mutable_loginresponse();
            succ->set_token( m_User.m_ShowName + "@" + "12345");
            succ->set_showname(m_User.m_ShowName);
            succ->mutable_error()->set_code(Error::SUCCESS);

            res.SerializeToString(&m_Out);
            InitChattingState();
            login = true;
            return true;
        }
    }
    LoginResponse* error = res.mutable_loginresponse();
    error->mutable_error()->set_code(Error::INNERERROR);
    res.SerializeToString(&m_Out);
    return false;
}
bool ServerWorker::Logout()
{
    for(auto pair:(*m_MyChannel))
    {
        (pair.second)->Leave(this);
    }
    login = false;
    return true;
}
bool ServerWorker::ChangeName(ChangeNameRequest req)
{
    WrapperServerMessage res;

    if(m_User.m_UserName.length() == 0)
    {
        Error* error = res.mutable_error();
        error->set_code(Error::WRONGAUTH);
        res.SerializeToString(&m_Out);
        return false;

    }else{
        if(m_User.ModifyUserName(req.name()))
        {
            RegularResponse* rres = res.mutable_regularresponse();
            rres->mutable_error()->set_code(Error::SUCCESS);
            res.SerializeToString(&m_Out);
            return true;
        }
    }
    Error* error = res.mutable_error();
    error->set_code(Error::INNERERROR);
    res.SerializeToString(&m_Out);
    return false;
}

bool ServerWorker::GetAllRooms()
{
    WrapperServerMessage res;
    res.mutable_roomresponse();
    RoomResponse room;
    Room* rooms[99];
    Room roomCol;
    roomCol.connect(ServerWorker::m_db);
    int len = roomCol.GetUserJoinedRoom(rooms,m_User.m_UserName,99);

    for(int i =0;i<len;i++)
    {
        RoomResponse* rm = res.add_rooms();

        rm->set_roomid(rooms[i]->m_roomID);
        rm->set_roomname(rooms[i]->m_Name);
        rm->set_creator(rooms[i]->m_Creator);
        delete rooms[i];
    }
    res.SerializeToString(&m_Out);
    return true;
}

//第二个参数意义：这个函数会被其他函数调用，在那时回复缓冲区不应该由这个函数填充。
bool ServerWorker::JoinRoomSub(int roomid,bool doResponse)
{
    WrapperServerMessage res;
    Channel* chan = nullptr;
    int error=255;
    if(m_MyChannel->find(roomid)!=m_MyChannel->end())
    {
        res.mutable_roomresponse()->mutable_error()->set_code(Error::WRONGAUTH);
        return false;
    }

    Room room;
    room.connect(ServerWorker::m_db);
    room.m_roomID = roomid;
    room.GetRoom();

    if(room.m_Lock != 0)
    {
        error = -1;
    }else{
        chan = m_User.JoinRoomC(roomid,&error);
    }
    if(chan != nullptr){
        m_MyChannel->insert(std::pair<int,Channel*>(roomid,chan));
        chan->Join(this);
        if(doResponse)
        {
            res.mutable_roomresponse()->mutable_error()->set_code(Error::SUCCESS);
            res.mutable_roomresponse()->set_roomid(roomid);
            res.SerializeToString(&m_Out);
        }
        return true;
    }else{
        if(doResponse)
        {
            if(error == -1)
            {
                res.mutable_roomresponse()->mutable_error()->set_code(Error::WRONGAUTH);
            }
            else
            {
                res.mutable_roomresponse()->mutable_error()->set_code(Error::NOTFOUND);
            }
            res.SerializeToString(&m_Out);
        }
    }

    return false;
}
bool ServerWorker::JoinRoom(int roomid)
{
    return JoinRoomSub(roomid,true);
}
bool ServerWorker::leaveRoom(int roomid)
{
    auto iter = m_MyChannel->find(roomid);

    WrapperServerMessage res;
    RoomResponse * rres = res.mutable_roomresponse();

    if(iter != m_MyChannel->end())
    {
        m_User.LeaveRoom(roomid);
        rres->set_roomid(roomid);
        rres->mutable_error()->set_code(Error::SUCCESS);
        res.SerializeToString(&m_Out);
        (iter->second)->Leave(this);
        return true;
    }
    rres->set_roomid(roomid);
    rres->mutable_error()->set_code(Error::NOTFOUND);
    res.SerializeToString(&m_Out);
    return false;

}
bool ServerWorker::CreateRoom(std::string name)
{
    WrapperServerMessage res;
    RoomResponse* rres = res.mutable_roomresponse();

    Room room;
    room.connect(ServerWorker::m_db);
    int64_t roomID = room.AddRoom(name,m_User.m_UserName);

    if(roomID != -1)
    {
        bool ret = JoinRoomSub(roomID,false);
        if(ret == true)
        {
            rres->mutable_error()->set_code(Error::SUCCESS);
            rres->set_roomid(roomID);
            rres->set_roomname(name);
            res.SerializeToString(&m_Out);
            return true;
        }
    }
    rres->mutable_error()->set_code(Error::INNERERROR);
    rres->set_roomid(-1);
    rres->set_roomname(name);
    res.SerializeToString(&m_Out);
    return false;
}

bool ServerWorker::LockRoom(int roomid,bool operation)
{
    WrapperServerMessage res;
    RoomResponse* rres = res.mutable_roomresponse();

    Room roomCol;
    roomCol.connect(ServerWorker::m_db);
    roomCol.m_roomID = roomid;

    if(roomCol.UpdateRoom(m_User.m_UserName,"",operation))
    {
        rres->mutable_error()->set_code(Error::SUCCESS);
        rres->set_roomid(roomid);
        rres->set_roomname("locked");
        res.SerializeToString(&m_Out);
        return true;
    }
    rres->mutable_error()->set_code(Error::NOTFOUND);
    rres->set_roomid(-1);
    rres->set_roomname("unlocked");
    res.SerializeToString(&m_Out);
    return false;
}
bool ServerWorker::KickUser(int roomid,std::string username)
{
    return false;
}
bool ServerWorker::PromoteUser(int roomid,std::string username, int toLevel)
{
    return false;
}
bool ServerWorker::Chat(ChatMessageRequest req)
{
    auto iter = m_MyChannel->find(req.roomid());

    WrapperServerMessage res;
    ChatMessageSend * send = res.mutable_chatmessagesend();

    if(iter != m_MyChannel->end())
    {
        send->set_username(req.username());
        send->set_msg(req.msg());
        send->set_roomid(req.roomid());
        send->set_roomorder((iter->second)->GetMessageCount());
        send->set_showname(m_User.m_ShowName);
        send->set_type(1);
        res.SerializeToString(&m_Out);
        (iter->second)->Send(m_Out.c_str(),m_Out.length(),this);
    }
    return true;
}
bool ServerWorker::SendNotice(int type,int roomid,std::string paraA,std::string paraB)
{
}

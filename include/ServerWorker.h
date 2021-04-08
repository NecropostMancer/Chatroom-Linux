/****************************************

定义所有业务逻辑和必要数据结构
和套接字及文件描述符一对一

****************************************/


#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include "ThreadPool.h"
#include "sys/uio.h"
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <sys/epoll.h>
#include <unistd.h>

#include "Database.h"

#include "ServerWorkerInter.h"

#include "User.h"
#include "Room.h"

#include "User.pb.h"
#include "Chat.pb.h"
#include "Room.pb.h"
#include "Error.pb.h"
#include "Wrapper.pb.h"
class ServerWorker : public IServerWorker
{
    public:
        ServerWorker();
        virtual ~ServerWorker();
        void process(const char*,int);
        void Init(int sockfd,sockaddr_in addr){
            m_Sockfd = sockfd;
            m_Address = addr;
            m_MyChannel = new std::map<int,Channel*>();
        }
        void Close()
        {
            Logout();
            delete m_MyChannel;
        }
        virtual void Res(const char* raw,int max_len)//给channel发送聊天消息用
        {
            //m_Out = raw;
            int cnt = write(m_Sockfd,raw,max_len);
            Log::Debug("Write %d bytes.",cnt);
        }
        void ConnectDb(Database &database)
        {
            m_db = &database;
        }
        static Database* m_db;

    protected:

    private:

        bool ParseMessage(int len);
        bool PostResponse(int len);

        bool Register(RegisterRequest);

        bool Login(LoginRequest);
        bool Logout();
        bool ChangeName(ChangeNameRequest);

        bool RoomReq(RoomRequest);
        bool JoinRoom(int roomid);
        bool JoinRoomSub(int roomid,bool doResponse);
        bool leaveRoom(int roomid);
        bool CreateRoom(std::string name);

        bool DoRoomControl(RoomControl);
        bool LockRoom(int roomid,bool operation);
        bool KickUser(int roomid,std::string username);
        bool PromoteUser(int roomid,std::string username,int toLevel);

        bool Chat(ChatMessageRequest req);
        bool SendNotice(int type,int roomid,std::string paraA,std::string paraB);
        void InitChattingState();

        bool GetAllRooms();

        iovec m_iv[2];
        int m_ivCnt;

        const char* m_ReadBuf;
        std::string m_Out;

        int m_Sockfd;
        sockaddr_in m_Address;

        std::map<int,Channel*>* m_MyChannel;

        const char* m_messageStart = "Chatting V0.1";
        const int m_BodyOffset = 4;

        User m_User;
        bool login = false;
};

#endif // SERVERWORKER_H

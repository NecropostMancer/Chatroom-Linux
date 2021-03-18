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
        void process(const char*);
        void Init(int sockfd,sockaddr_in addr){
            m_Sockfd = sockfd;
            m_Address = addr;
            m_MyChannel = new std::map<int,Channel*>();
        }
        void Close()
        {
            m_db->CloseUser(m_Sockfd);

            Logout();
            delete m_MyChannel;
        }
        virtual void Res(const char* raw,int max_len)
        {
            int cnt = write(m_Sockfd,raw,max_len);
            //Log::Debug("Write %d bytes.",cnt);
        }
        void ConnectDb(Database &database)
        {
            m_db = &database;
        }
        static Database* m_db;
    protected:

    private:

        bool ParseMessage();
        bool PostResponse(int len);

        bool Register(RegisterRequest);

        bool Login(LoginRequest);
        bool Logout();
        bool ChangeName();

        bool JoinRoom(RoomRequest req);
        bool leaveRoom();
        bool CreateRoom();

        bool LockRoom();
        bool KickUser();
        bool PromoteUser();

        bool Chat(ChatMessageRequest req);

        void InitChattingState();

        iovec m_iv[2];
        int m_ivCnt;

        const char* m_ReadBuf;
        std::string m_Out;

        int m_Sockfd;
        sockaddr_in m_Address;

        std::map<int,Channel*>* m_MyChannel;
        std::string m_userName;

};

#endif // SERVERWORKER_H

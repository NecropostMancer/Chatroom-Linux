#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include "ThreadPool.h"
#include "sys/uio.h"
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
class ServerWorker
{
    public:
        ServerWorker();
        virtual ~ServerWorker();
        void process(const char* input,const char* output,size_t outputLen);
        void Init(int sockfd,sockaddr_in addr){
            m_Sockfd = sockfd;
            m_Address = addr
        };
        void Close()
        {
            m_UserCount--;
        }
        static int m_UserCount;
    protected:

    private:

        bool ParseMessage();
        bool PostResponse();

        bool Register();

        bool Login();
        bool Logout();
        bool ChangeName();

        bool JoinRoom();
        bool leaveRoom();
        bool CreateRoom();

        bool LockRoom();
        bool KickUser();
        bool PromoteUser();

        bool Chat();

        iovec m_iv[2];
        int m_ivCnt;

        const char* m_ReadBuf;
        const char* m_WriteBuf;

        static std::map<std::string,int>* UsernameToFd;

        constexpr static size_t BUFFER_SIZE = 512;
        int m_Sockfd;
        sockaddr_in m_Address;
        char* m_WriteBuf;
        char m_Buf[BUFFER_SIZE];


};

#endif // SERVERWORKER_H

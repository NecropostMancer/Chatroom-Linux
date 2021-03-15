#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include "ThreadPool.h"
#include "sys/uio.h"
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

#include "Database.h"
class ServerWorker
{
    public:
        ServerWorker();
        virtual ~ServerWorker();
        void process(const char*);
        void Init(int sockfd,sockaddr_in addr){
            m_Sockfd = sockfd;
            m_Address = addr;

        }
        void Close()
        {
            m_db->CloseUser(m_Sockfd);
        }
        void Res(const char* raw,int max_len)
        {

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

        int m_Sockfd;
        sockaddr_in m_Address;

};

#endif // SERVERWORKER_H

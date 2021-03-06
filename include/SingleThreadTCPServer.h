/****************************************

监听网络请求，事务由serverworker处理

****************************************/


#ifndef SINGLETHREADTCPSERVER_H
#define SINGLETHREADTCPSERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <errno.h>

#include "ServerWorker.h"
class SingleThreadTCPServer
{
    public:
        SingleThreadTCPServer();
        virtual ~SingleThreadTCPServer();
        int Start();
        int Init(int port);
        void AssignWorkers(ServerWorker* workers, int workerNum = 65535)
        {
            m_Workers = workers;
        }
    protected:

    private:
        const int BACKLOG = 5;
        const int MAX_EVENT = 10000;
        ServerWorker* m_Workers;
        int m_ListeningSocket;
        bool m_Stop;

        int epollfd;

        int m_Usercnt = 0;

        inline static int SetNonBlocking(int fd)
        {
            int old = fcntl(fd, F_GETFL);
            int new_op = old | O_NONBLOCK;
            fcntl(fd,F_SETFL,new_op);
            return old;
        }

        inline static void Addfd(int epollfd, int fd, bool oneshot)
        {
            epoll_event event;
            event.data.fd = fd;
            event.events = EPOLLIN | EPOLLET;
            if(oneshot)
            {
                event.events |= EPOLLONESHOT;
            }
            epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
            SetNonBlocking(fd);
        }

        inline static void Removefd(int epollfd,int fd)
        {
            epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
            close(fd);
        }

        inline static void Modfd(int epollfd,int fd,int ev)
        {
            epoll_event event;
            event.data.fd = fd;
            event.events=ev | EPOLLET|EPOLLONESHOT|EPOLLRDHUP;
            epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
        }

        inline static void ResetOneshot(int epollfd, int fd)
        {
            epoll_event event;
            event.data.fd = fd;
            event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
            epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
        }

        constexpr static size_t BUFFER_SIZE = 512;
        char m_TmpBuffer[BUFFER_SIZE];

        void OnAccept(int fd,sockaddr_in clientAddress);
        void OnRead();
        //void OnWrite();
        void OnError();
};

#endif // SINGLETHREADTCPSERVER_H

#include "SingleThreadTCPServer.h"

#include "Log.h"


SingleThreadTCPServer::SingleThreadTCPServer()
{
    //ctor
}

SingleThreadTCPServer::~SingleThreadTCPServer()
{
    //dtor
}

int SingleThreadTCPServer::Init(int port)
{
    m_ListeningSocket = socket(PF_INET, SOCK_STREAM, 0);
    int enable = 1;
    setsockopt(m_ListeningSocket,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int));
    sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &address.sin_addr);
    address.sin_port = htons(port);

    int ret = bind(m_ListeningSocket,(sockaddr *)&address,sizeof(address));

    ret = listen(m_ListeningSocket, BACKLOG);
    Log::LogOut("Server will run on port %d.",address.sin_port);
    if(ret == -1){

        Log::Fatal("Can't Listening port, Server Quitting...");
        return -1;

    }


    epollfd = epoll_create(BACKLOG);
    if(epollfd == -1)
    {
        Log::Fatal("Can't make epollfd, Server Quitting...");
        return -1;
    }
    Addfd(epollfd,m_ListeningSocket,false);
    m_Stop = false;
    return 0;
}
#include <iostream>
int SingleThreadTCPServer::Start()
{
    Log::LogOut("Server Start.");


    epoll_event events[MAX_EVENT];
    while(!m_Stop)
    {
        int ret = epoll_wait( epollfd, events, MAX_EVENT, -1);

        //Log::Debug("Get total event(s): %d",ret);
        if (ret < 0)
        {
            Log::Error("Epoll Failed.");
        }
        for (int i=0;i<ret;i++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == m_ListeningSocket)
            {
                sockaddr_in clientAddress;
                socklen_t clientAddressLen = sizeof(clientAddress);

                int cfd = accept(m_ListeningSocket,(sockaddr*)&clientAddress,&clientAddressLen);
                if (cfd < 0)
                {
                    Log::Error(strerror(errno));
                    continue;
                }

                Log::Debug("New connection is coming, open socket:%d.",cfd);
                Addfd(epollfd,cfd,true);

                m_Workers[cfd].Init(cfd,clientAddress);
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                m_Workers[events[i].data.fd].Close();
                close(events[i].data.fd);
                Log::Debug("Connection on socket %d is disconnected.",events[i].data.fd);
                m_Usercnt--;
            }
            else if (events[i].events & EPOLLIN)
            {

                int connfd = events[i].data.fd;
                memset(m_TmpBuffer,0,BUFFER_SIZE);
                int recvNum = recv(connfd, m_TmpBuffer,BUFFER_SIZE-1,0);
                Log::Debug("Get %d length data:%s",recvNum,m_TmpBuffer);
                Modfd(epollfd,connfd,EPOLLIN);
                if(ret < 0)
                {
                    if(errno != EAGAIN)
                    {
                        close(connfd);
                        //move last one to replace closed one
                    }
                }
                else if( ret == 0)
                {
                }
                else
                {
                    //send right after the processing end,
                    //so i decided not to enable epollout manually.
                    //Modfd(epollfd,livefd[j],EPOLLOUT);
                    m_Workers[connfd].process(m_TmpBuffer);
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
            /*
                int connfd = events[i].data.fd;
                if(!users[connfd].write_buf)
                {
                    continue;
                }
                int retcnt = send(connfd,users[connfd].write_buf,strlen(users[connfd].write_buf),0);
                //users[connfd].write_buf = NULL;
                Modfd(epollfd,connfd,EPOLLIN);
                */
            }
            else
            {
                Log::Error("idk whats going on.");
                Log::Error("%d/%d events, events value: %d, fd: %d",i,ret - 1,events[i].events,events[i].data.fd);
            }
        }

    }

    close(m_ListeningSocket);
    return 0;
}

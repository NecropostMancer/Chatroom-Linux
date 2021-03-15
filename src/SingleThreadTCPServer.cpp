#include "SingleThreadTCPServer.h"

#include "Log.h"


#include "ServerWorker.h"

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
    return 0;
}
#include <iostream>
int SingleThreadTCPServer::Start()
{
    Log::LogOut("Server Start.");
    ServerWorker* workers = new ServerWorker[MAX_FD];
    userData* users = new userData[MAX_FD];
    epoll_event events[MAX_EVENT];
    int livefd[MAX_FD] = {};
    while(!m_Stop)
    {
        int ret = epoll_wait( epollfd, events, MAX_EVENT, -1);
        Log::Debug("Lets wait!");
        Log::Debug("Get total event(s): %d",ret);
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
                if (ServerWorker::m_UserCount >= MAX_FD)
                {
                    const char* w = "too many users.";
                    Log::Warning(w);

                    close(cfd);
                    continue;
                }
                Log::Debug("new connection coming");
                Addfd(epollfd,cfd,true);
                workers[cfd].Init(cfd,clientAddress);
                //users[cfd].address = clientAddress;
                m_Usercnt++;
                livefd[m_Usercnt] = cfd;
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                //workers[sockfd].close();
                close(events[i].data.fd);
                m_Usercnt--;
            }
            else if (events[i].events & EPOLLIN)
            {
                int connfd = events[i].data.fd;
                memset(users[connfd].buf,0,BUFFER_SIZE);
                int recvNum = recv(connfd, users[connfd].buf,BUFFER_SIZE-1,0);
                Log::Debug("Get:%s",users[connfd].buf);
                Modfd(epollfd,connfd,EPOLLIN);
                if(ret < 0)
                {
                    if(errno != EAGAIN)
                    {
                        close(connfd);
                        //move last one to replace closed one
                        users[livefd[i]] = users[livefd[m_Usercnt]];
                        livefd[i] = livefd[m_Usercnt];
                        m_Usercnt--;
                    }
                }
                else if( ret == 0)
                {
                }
                else
                {
                    for(int j = 1;j<=m_Usercnt;j++)
                    {
                        if(livefd[j] == connfd)
                        {
                            continue;
                        }
                        Modfd(epollfd,livefd[j],EPOLLOUT);
                        users[livefd[j]].write_buf = users[connfd].buf;
                    }
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                int connfd = events[i].data.fd;
                if(!users[connfd].write_buf)
                {
                    continue;
                }
                int retcnt = send(connfd,users[connfd].write_buf,strlen(users[connfd].write_buf),0);
                users[connfd].write_buf = NULL;
                Modfd(epollfd,connfd,EPOLLIN);
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

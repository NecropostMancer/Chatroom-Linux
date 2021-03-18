#include <iostream>
#include "ThreadPool.h"
#include "Database.h"
using namespace std;

//Database a;

void testAllDB(Database &a)
{
// if this is global will get a segment fault
    //1. user add and modify name.
    a.AddUser(User("1","1","1"));
    a.AddUser(User("2","2","2"));
    a.AddUser(User("3","3","3"));

    a.GetUser("1");
    a.GetUser("4");

    a.AddRoom("blabla",User("1","1","1"));
    Room* room[5] = {0,};
    a.GetAllRoomByName(room,"bla",5);


    a.JoinRoom("1",room[0]->m_roomID);
    a.JoinRoom("2",room[0]->m_roomID);

    a.LeaveRoom("1",room[0]->m_roomID);

    for(int i=0;i<5;i++)
    {
        if(room[i]!=0)
        {
            delete room[i];
        }
    }
}
#include "Log.h"
#include "SingleThreadTCPServer.h"
int main()
{
    Database a;
    //testAllDB(a);
    SingleThreadTCPServer server;
    ServerWorker* worker = new ServerWorker[65535];
    worker[0].ConnectDb(a);
    server.Init(8086);
    server.AssignWorkers(worker);
    server.Start();
    return 0;
}

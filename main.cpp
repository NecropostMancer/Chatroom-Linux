#include <iostream>
#include "ThreadPool.h"
#include "Database.h"
using namespace std;

//Database a;

void testAllDB(Database &a)
{
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

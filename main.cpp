#include <iostream>
#include "Database.h"
using namespace std;
#include "Log.h"
#include "SingleThreadTCPServer.h"
int main(int argc,char** argv)
{
    Database* a;
    if(argc == 4){
        a = new Database(&argv[1]);
    }else{
        a = new Database();
    }

    SingleThreadTCPServer server;
    ServerWorker* worker = new ServerWorker[65535];
    worker[0].ConnectDb(*a);
    server.Init(8086);
    server.AssignWorkers(worker);
    server.Start();
    return 0;
}

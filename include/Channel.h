#ifndef CHANNEL_H
#define CHANNEL_H
#include "ServerWorker.h"
#include <vector>
class Channel
{
    public:
        Channel();
        virtual ~Channel();
        void Send(const char* write, int MAX_LEN)
        {
            for(int i= 0;i<workers.size();i++)
            {
                workers[i]->Res(write,MAX_LEN);
            }
        }
        void Join(ServerWorker* worker){workers.push_back(worker);}
        void Leave(ServerWorker* worker){}
    protected:

    private:
        std::vector<ServerWorker*> workers;
};

#endif // CHANNEL_H

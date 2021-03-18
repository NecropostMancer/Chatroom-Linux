#ifndef CHANNEL_H
#define CHANNEL_H
#include "ServerWorkerInter.h"
#include <vector>
class Channel
{
    public:
        Channel(int roomID):m_RoomID(roomID){};
        virtual ~Channel();
        void Send(const char* write, int MAX_LEN)
        {
            for(int i= 0;i<workers.size();i++)
            {
                workers[i]->Res(write,MAX_LEN);
            }
        }
        void Join(IServerWorker* worker){workers.push_back(worker);}
        void Leave(IServerWorker* worker){}
        void Talk(IServerWorker* worker);
        int GetRoomID(){return m_RoomID;}
    protected:

    private:
        std::vector<IServerWorker*> workers;
        int m_RoomID;
};

#endif // CHANNEL_H

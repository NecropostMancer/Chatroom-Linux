#ifndef CHANNEL_H
#define CHANNEL_H
#include "ServerWorkerInter.h"
#include <vector>

#include "Log.h"
class Channel
{
    public:
        Channel(int roomID):m_RoomID(roomID){m_MessageCnt = 0;};
        virtual ~Channel();
        void Send(const char* write, int MAX_LEN,IServerWorker* caller)
        {
            for(size_t i= 0;i<workers.size();i++)
            {
                if(caller == workers[i])continue;
                workers[i]->Res(write,MAX_LEN);
            }
        }
        void Join(IServerWorker* worker)
        {
            workers.push_back(worker);
            Log::Debug("Channel #%d now has %d ppl.",m_RoomID,workers.size());
        }
        void Leave(IServerWorker* worker)
        {
            for(auto i= workers.begin();i != workers.end();i++)
            {
                if(worker == *i)
                {
                    workers.erase(i);
                    Log::Debug("Channel #%d now has %d ppl.",m_RoomID,workers.size());
                    return;
                }
            }
            return;
        }
        void Talk(IServerWorker* worker);
        int GetMessageCount(){return ++m_MessageCnt;}
        int GetRoomID(){return m_RoomID;}
    protected:

    private:
        std::vector<IServerWorker*> workers;
        int m_RoomID;
        int m_MessageCnt;
};

#endif // CHANNEL_H

/****************************************

聊天频道，也就是被载入到内存的room，worker
通过将自己登记入相对应的channel交流

****************************************/



#ifndef CHANNEL_H
#define CHANNEL_H
#include "ServerWorkerInter.h"
#include <vector>

#include "Log.h"
#include <map>
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

        static std::map<int,Channel*> AliveChannel;
    protected:

    private:
        std::vector<IServerWorker*> workers;
        int m_RoomID;
        int m_MessageCnt;
};

#endif // CHANNEL_H

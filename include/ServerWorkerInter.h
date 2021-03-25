/****************************************

serverWorker的接口，最初目的是避免

channel
serverworker
循环包含

****************************************/

#ifndef SERVERWORKERINTER_H
#define SERVERWORKERINTER_H


class IServerWorker
{
    public:
        IServerWorker() {}
        virtual ~IServerWorker() {}
        virtual void Res(const char* raw,int max_len) = 0;
    protected:

    private:
};

#endif // SERVERWORKERINTER_H

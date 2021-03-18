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

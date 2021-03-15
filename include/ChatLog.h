#ifndef CHATLOG_H
#define CHATLOG_H

#include <string>
class ChatLog
{
    public:
        ChatLog();
        virtual ~ChatLog();

    protected:

    private:
        std::string text;
        std::string username;
        time_t time;
};

#endif // CHATLOG_H

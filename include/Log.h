#ifndef LOG_H
#define LOG_H


class Log
{
    public:

        virtual ~Log();
        static void Init();
        static void SetFilter(int logLevel); // only show higher level log;
        static void Debug(const char*,...); // 0
        static void LogOut(const char*,...); // 1
        static void Warning(const char*,...); // 2
        static void Error(const char*,...);// 3
        static void Fatal(const char*,...);// 255

    protected:

    private:
        Log() = delete;
        static bool m_Inited;
        static unsigned short m_LogLevel;
        static constexpr const char* m_Format[5] =
        {
            "[DEBUG%s]",
            "[LOG%s]",
            "[WARNING%s]",
            "[ERROR%s]",
            "[FATAL%s]"
        };
        static char* m_TimeStamp[12];
};

#endif // LOG_H

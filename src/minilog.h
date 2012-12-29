/*
 * File       : minilog.h
 * Description:
 * Version    : 2011-11-12 Created
 * Author     : buf1024@gmail.com
 */

#ifndef __48SLOTS_MINILOG_H__
#define __48SLOTS_MINILOG_H__

#include <stdarg.h>
#include <stdio.h>

#ifdef _MSC_VER
#  include <windows.h>
#else
#  include <pthread.h>
#endif

#define LOG_DEBUG(...)                                               \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.debug(__VA_ARGS__);                                          \
}while(0)                                                            \

#define LOG_INFO(...)                                                \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.info(__VA_ARGS__);                                           \
}while(0)                                                            \

#define LOG_WARN(...)                                                \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.warn(__VA_ARGS__);                                           \
}while(0)                                                            \

#define LOG_ERROR(...)                                               \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.error(__VA_ARGS__);                                          \
}while(0)                                                            \

#define LOG_FATAL(...)                                               \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.fatal(__VA_ARGS__);                                          \
}while(0)                                                            \

#define SET_FILE_LEVEL(lvl)                                          \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
    log.setFileLvl(Minilog::getLvl(lvl));                            \
}while(0)                                                            \


#define SET_TERM_LEVEL(lvl)                                          \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.setTermLvl(Minilog::getLvl(lvl));                            \
}while(0)                                                            \

#define SET_LOG_LEVEL(fileLvl, termLvl)                              \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
    log.setTermLvl(Minilog::getLvl(fileLvl));                        \
	log.setFileLvl(Minilog::getLvl(termLvl));                        \
}while(0)                                                            \


#define SET_LOG_PATH(path)                                           \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.setPath(path);                                               \
}while(0)                                                            \

#define SET_LOG_FILE_FIXED(szFile)                                   \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.setLogFile(szFile, false);                                   \
}while(0)                                                            \

#define SET_LOG_FILE(szFile)                                         \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.setLogFile(szFile);                                          \
}while(0)                                                            \

#define SET_LOG_BUF_SIZE(size)                                       \
do{                                                                  \
    Minilog& log = Minilog::getInst();                               \
    log.setLogBufSize(size);                                         \
}while(0)                                                            \

#define LOG_FLUSH()                                                  \
do{                                                                  \
	Minilog& log = Minilog::getInst();                               \
	log.flush();                                                     \
}while(0)                                                            \

#define LOG_RELEASE()                                                \
do{                                                                  \
	Minilog::ReleaseRC();                                            \
}while(0)                                                            \

#define LOG_IS_READY() Minilog::getInst().isLogReady()               \

enum LogLevel
{
    ALL     = 0,
    DEBUG   = 100,
    INFO    = 200,
    WARN    = 300,
    ERR     = 400,
    FATAL   = 500,
    OFF     = 600,
    END     = 700
};

class Minilog
{
private:
    Minilog();
public:
    ~Minilog();
public:
    void info(const char* format, ...);
    void debug(const char* format, ...);
    void warn(const char* format, ...);
    void error(const char* format, ...);
    void fatal(const char* format, ...);

    void setFileLvl(LogLevel lvl);
    LogLevel getFileLvl() const;

    void setTermLvl(LogLevel lvl);
    LogLevel getTermLvl() const;

    void setPath(const char* path);
    const char* getPath() const;

	void setLogFile(const char* fileName, bool suffix = true);
	const char* getLogFile() const;

	int setBufSize(unsigned int size);
	unsigned int getBufSize() const;

    void flush();

	void logTerm(LogLevel lvl, const char* msg, int len = -1);
	void logFile(LogLevel lvl, const char* msg, int len = -1);

	int isLogReady() const { return file != 0; }

	static LogLevel getLvl(int lvl);
	static LogLevel getLvl(const char* lvl);

    static Minilog& getInst();
    static void releaseRC();

protected:
    void logMessageV(LogLevel lvl, const char* format, va_list va);
    const char* getHeader(LogLevel lvl);

	void setTermColor(LogLevel lvl);
	void flushMsg(const char* buf, int len);

	int lock();
	int unlock();

private:
    LogLevel fileLvl;
    LogLevel termLvl;

    FILE* file;

    static const int LOG_MAX_PATH = 255;
    char path[LOG_MAX_PATH + 1];
	char fileName[LOG_MAX_PATH + 1];

    unsigned int bufSize;
	char* msgBuf;
	unsigned int idx;

#ifdef _MSC_VER
	HANDLE stdOut;
#else
	pthread_mutex_t mutex;
#endif
};


#endif /* __48SLOTS_MINILOG_H__ */



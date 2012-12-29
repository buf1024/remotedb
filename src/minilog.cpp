/*
 * File       : minilog.h
 * Description:
 * Version    : 2011-11-12 Created
 * Author     : buf1024@gmail.com
 */
#include "minilog.h"
#include <time.h>
#include <assert.h>
#include <string.h>

#ifdef _MSC_VER
#  include <io.h>
#  include <direct.h>
#else
#  include <unistd.h>
#  include <sys/time.h>
#endif

#define WRITE_BUF        2048
#define DEFAULT_BUF_SIZE 4096

Minilog::Minilog()
: fileLvl(ALL)
, termLvl(ALL)
, file(NULL)
, msgBuf(NULL)
{
    memset(path, 0, sizeof(path));
    memset(fileName, 0, sizeof(fileName));


    setBufSize(DEFAULT_BUF_SIZE);

#ifdef _MSC_VER
	stdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
#else
	memset(&mutex, 0, sizeof(mutex)); // PTHREAD_MUTEX_INITIALIZER;
#endif

}

Minilog::~Minilog()
{
	if (file != 0)
	{
		flush();
		fclose(file);
		file = 0;
	}
	if(msgBuf)
	{
	    delete[] msgBuf;
	    msgBuf = 0;
	}
#ifdef _MSC_VER
	CloseHandle(stdOut);
#endif
}
void Minilog::info(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    logMessageV(INFO, format, va);
    va_end(va);
}
void Minilog::debug(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    logMessageV(DEBUG, format, va);
    va_end(va);
}
void Minilog::warn(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    logMessageV(WARN, format, va);
    va_end(va);
}
void Minilog::error(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    logMessageV(ERR, format, va);
    va_end(va);
}
void Minilog::fatal(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    logMessageV(FATAL, format, va);
    va_end(va);
}

void Minilog::setFileLvl(LogLevel lvl)
{
    fileLvl = getLvl(lvl);
}
LogLevel Minilog::getFileLvl() const
{
    return fileLvl;
}
void Minilog::setTermLvl(LogLevel lvl)
{
    termLvl = getLvl(lvl);
}
LogLevel Minilog::getTermLvl() const
{
    return termLvl;
}

void Minilog::setPath(const char* fpath)
{
#ifdef _MSC_VER
    _getcwd(path, Minilog::LOG_MAX_PATH);
#else
    getcwd(path, Minilog::LOG_MAX_PATH);
#endif

	if (path != 0)
	{
	    int nRet = -1;
#ifdef _MSC_VER
		nRet = _access(path, 04); //Read
#else
		nRet = access(path, 04); //Read
#endif
		if (nRet != -1)
		{
			strcpy(path, fpath);
		}
	}
	int nLen = strlen(path);
	if (path[nLen - 1] == '/' || path[nLen - 1] == '\\')
	{
	    path[nLen - 1] = '\0';
	}
}
const char* Minilog::getPath() const
{
    return path;
}

void Minilog::setLogFile(const char* ffile, bool suffix)
{
	if (ffile != 0 && ffile[0] != '\0')
	{
		strcpy(fileName, ffile);
	}

	time_t t;
	time(&t);
	struct tm * pTm = localtime(&t);
	char buf[BUFSIZ] = "";
#ifdef _MSC_VER
	_snprintf(buf, BUFSIZ, "_%04d%02d%02d%02d%02d%02d_%ld.log",
	        pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
	        pTm->tm_hour, pTm->tm_min, pTm->tm_sec,
	        (long)GetCurrentProcessId());
#else
	snprintf(buf, BUFSIZ, "_%04d%02d%02d%02d%02d%02d_%ld.log",
	        pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
	        pTm->tm_hour, pTm->tm_min, pTm->tm_sec,
	        (long)getpid());
#endif
	if (strlen(fileName) == 0)
	{
	    strcpy(fileName, "Minilog");
		strcat(fileName, buf);
	}
	else
	{
		if (suffix)
		{
		    strcat(fileName, buf);
		}
	}
	if (file != 0)
	{
		fflush(file);
		fclose(file);
	}
	strcpy(buf, fileName);
#ifdef _MSC_VER
	strcat(buf, "\\");
#else
	strcat(buf, "/");
#endif

	strcat(buf, fileName);

	file = fopen(buf, "w");
	assert(file != 0);
}
const char* Minilog::getLogFile() const
{
	return fileName;
}

int Minilog::setBufSize(unsigned int size)
{
    if(msgBuf != 0)
    {
        flush();
        delete msgBuf;
        msgBuf = 0;
    }
    bufSize = size;
    msgBuf = new char[size];
    idx = 0;

    return msgBuf != 0;
}
unsigned int Minilog::getBufSize() const
{
    return bufSize;
}

void Minilog::flush()
{
	if(file)
	{
	    flushMsg(msgBuf, idx);
	    idx = 0;
	}
}

void Minilog::flushMsg(const char* buf, int len)
{
    int nTotal = len;
    int nCount = len;
    size_t uIndex = 0;
    while(nTotal > 0){
        if (nTotal > WRITE_BUF)
        {
            nCount = WRITE_BUF;
        }
        fwrite(buf + uIndex, sizeof(char), nCount, file);
        uIndex += nCount;
        nTotal -= nCount;
        nCount = nTotal;
    }
    fflush(file);
}

int Minilog::lock()
{
#ifdef _MSC_VER
#else
    return pthread_mutex_lock(&mutex) == 0;
#endif
}
int Minilog::unlock()
{
#ifdef _MSC_VER
#else
    return pthread_mutex_unlock(&mutex) == 0;
#endif
}

void Minilog::logTerm(LogLevel lvl, const char* msg, int len)
{
	if (lvl >= termLvl)
	{
		if (msg != 0 && msg[0] != '\0')
		{
		    const char* hdr = getHeader(lvl);
			if(len < 0) len = strlen(msg);
#ifdef _MSC_VER
			CONSOLE_SCREEN_BUFFER_INFO csbi = {sizeof(csbi)};
			GetConsoleScreenBufferInfo(
				stdOut, &csbi);
			WORD wAtt = csbi.wAttributes;
			setTermColor(lvl);
			DWORD dwReal;
			WriteConsoleA(stdOut, hdr, strlen(hdr), &dwReal, NULL);
			WriteConsoleA(stdOut, msg, nLen, &dwReal, NULL);
			SetConsoleTextAttribute(stdOut, wAtt);
#else
			setTermColor(lvl);
			printf("%s", hdr);
			printf("%s", msg);
			printf("\033[0m");
#endif
		}
	}
}
void Minilog::logFile(LogLevel lvl, const char* msg, int nLen)
{
	if (lvl >= fileLvl)
	{
		if (file != 0 && msg != 0)
		{
		    const char* hdr = getHeader(lvl);
		    unsigned int nHLen = strlen(hdr);
		    if(nLen == -1) nLen = strlen(msg);
			if(bufSize - idx >= nHLen + nLen)
			{
			    memcpy(msgBuf + idx, hdr, nHLen);
			    idx += nHLen;
			    memcpy(msgBuf + idx, msg, nLen);
			    idx += nLen;
			}
			else
			{
			    flush();
			    if(bufSize - idx >= nHLen + nLen)
			    {
			        memcpy(msgBuf + idx, hdr, nHLen);
			        idx += nHLen;
			        memcpy(msgBuf + idx, msg, nLen);
			        idx += nLen;
			    }
			    else
			    {
			        flushMsg(hdr, nHLen);
			        flushMsg(msg, nLen);
			    }
			}
		}
		else
		{
			assert(false);
		}
	}
}
void Minilog::logMessageV(LogLevel lvl, const char* format, va_list va)
{
    static char szMsgBuf[WRITE_BUF] = "";
	if (lvl >= termLvl || lvl >= fileLvl)
	{
		if (vsnprintf(szMsgBuf, WRITE_BUF - 1, format, va) < 0)
		{
			fatal("Message too long\n");
			return;
		}
	}
	if (lvl >= termLvl)
	{
		logTerm(lvl, szMsgBuf);
	}
	if (lvl >= fileLvl)
	{
		logFile(lvl, szMsgBuf);
	}
}
const char* Minilog::getHeader(LogLevel lvl)
{
    static const int SIZE = 64;
    static char hdr[SIZE - 1] = "";
    switch (lvl) {
    case DEBUG:
        strcpy(hdr, "[D]");
        break;
    case INFO:
        strcpy(hdr, "[I]");
        break;
    case WARN:
        strcpy(hdr, "[W]");
        break;
    case ERR:
        strcpy(hdr, "[E]");
        break;
    case FATAL:
        strcpy(hdr, "[F]");
        break;
    default:
        assert(false);
        strcpy(hdr, "[X]");
        break;
    }

#ifdef _MSC_VER
    SYSTEMTIME st = {0};
    GetLocalTime(&st);
    _snprintf(hdr + 3, SIZE - 4, " [%04d%02d%02d%02d%02d%02d.%ld] ",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond,
            st.wMilliseconds);
#else
    time_t t;
    time(&t);
    struct tm * pTm = localtime(&t);
    struct timeval usec;
    gettimeofday(&usec, 0);
    snprintf(hdr + 3, SIZE - 4, " [%04d%02d%02d%02d%02d%02d.%ld] ",
            pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
            pTm->tm_hour, pTm->tm_min, pTm->tm_sec,
            usec.tv_usec);
#endif

    return hdr;
}


void Minilog::setTermColor(LogLevel lvl)
{
#ifdef _MSC_VER
	WORD dwTextAttr = FOREGROUND_BLUE | FOREGROUND_GREEN |
		FOREGROUND_RED | FOREGROUND_INTENSITY;
	switch(lvl)
	{
	case FATAL:
		dwTextAttr = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case ERR:
		dwTextAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case WARN:
		dwTextAttr = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case INFO:
		dwTextAttr = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case DEBUG:
		break;
	default:
		break;
	}
	SetConsoleTextAttribute(stdOut, dwTextAttr);
#else
	char szBuf[32] = "\033[0m";
	switch(lvl)
	{
	case FATAL:
		strcpy(szBuf, "\033[1;31m");
		break;
	case ERR:
		strcpy(szBuf, "\033[1;35m");
		break;
	case WARN:
		strcpy(szBuf, "\033[1;34m");
		break;
	case INFO:
		strcpy(szBuf, "\033[1;33m");
		break;
	case DEBUG:
		break;
	default:
		break;
	}
	printf("%s", szBuf);
#endif

}


LogLevel Minilog::getLvl(int nLvl)
{
	if (nLvl <100)
	{
		return ALL;
	}
	if (nLvl >= 100 && nLvl < 200)
	{
		return DEBUG;
	}
	if (nLvl >= 200 && nLvl < 300)
	{
		return INFO;
	}
	if (nLvl >= 300 && nLvl < 400)
	{
		return WARN;
	}
	if (nLvl >= 400 && nLvl < 500)
	{
		return ERR;
	}
	if (nLvl >= 500 && nLvl < 600)
	{
		return FATAL;
	}
	return OFF;
}
LogLevel Minilog::getLvl(const char* lvl)
{
	struct _StrLvl
	{
		const char* szStr;
		LogLevel lvl;
	};

	static _StrLvl strLvlMap [] = {
		{"ALL", ALL},
		{"DEBUG", DEBUG},
		{"INFO", INFO},
		{"WARN", WARN},
		{"ERROR", ERR},
		{"FATAL", FATAL},
		{"OFF", OFF}
	};

	if (lvl == 0)
	{
		return ALL;
	}

	for (unsigned i=0; i<sizeof(strLvlMap); i++)
	{
#ifdef _MSC_VER
		if (stricmp(lvl, strLvlMap[i].szStr) == 0)
#else
		if (strcasecmp(lvl, strLvlMap[i].szStr) == 0)
#endif
		{
			return strLvlMap[i].lvl;
		}
	}
	assert(false);
	return ALL;
}

Minilog& Minilog::getInst()
{
/*    if (sm_pInst == 0)
    {
        sm_pInst = new Minilog;
    }
    return *sm_pInst;*/
    static Minilog theLog;
    return theLog;
}
void Minilog::releaseRC()
{
/*    if (sm_pInst != 0)
    {
        delete sm_pInst;
        sm_pInst = 0;
    }*/
}

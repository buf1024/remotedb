/*
 * rdbshm.h
 *
 *  Created on: 2012-12-29
 *      Author: buf1024@gmail.com
 */

#ifndef __48SLOTS_RDBSHM_H__
#define __48SLOTS_RDBSHM_H__

#include <unistd.h>
#include <string>

#define MAX_DB_NAME            256
#define MAX_PATH_NAME          256
#define MAX_ATTACH_PROCESS     512
#define MAX_DB_COUNT           512

struct RDBShmItem
{
    pthread_mutex_t mtx;
    char     dbName[MAX_DB_NAME];
    char     queueIn[MAX_PATH_NAME];
    char     queueOut[MAX_PATH_NAME];
    int      attachCount;
    pid_t    attachList[MAX_ATTACH_PROCESS];
};

struct RDBShm
{
    pid_t pidRdb;
    int   dbCount;
    RDBShmItem db[MAX_DB_COUNT];

};

int createShm(std::string& path);
int destroyShm();
int attachShm(std::string& path);
int detachShm();

RDBShmItem* getShmItem(std::string& name);
pid_t       getRdbPid();

#endif /* __48SLOTS_RDBSHM_H__ */

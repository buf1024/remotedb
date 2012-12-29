/*
 * rdbshm.cpp
 *
 *  Created on: 2012-12-29
 *      Author: buf1024@gmail.com
 */
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include "rdbshm.h"


#define RDB_SHM_ID 0xaf

static int gShmId          = -1;
static RDBShm* gShm        = NULL;
static RDBShmItem* gShmItm = NULL;

int createShm(std::string& path)
{
    return 0;
}
int destroyShm()
{
    return 0;
}
int attachShm(std::string& path)
{
    return 0;
}
int detachShm()
{
    return 0;
}

RDBShmItem* getShmItem(std::string& name)
{
    return 0;
}
pid_t       getRdbPid()
{
    return 0;
}


/*
 * rdb.h
 *
 *  Created on: 2012-12-29
 *      Author: buf1024@gmail.com
 */

#ifndef __48SLOTS_RDB_H__
#define __48SLOTS_RDB_H__
#include <vector>

struct RDBRunInfo
{
    pid_t              pid;
    std::vector<pid_t> route_lst;
    std::vector<int>   send_sock;
};

void usage();
int regSignal();
void handleSignal(int signo);
int service();

#endif /* __48SLOTS_RDB_H__ */

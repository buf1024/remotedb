/*
 * rdbsignal.h
 *
 *  Created on: 2012-12-12
 *      Author: buf1024@gmail.com
 */

#ifndef __48SLOTS_RDBSIGNAL_H__
#define __48SLOTS_RDBSIGNAL_H__

#include <signal.h>

#define REG_SIGNAL(signo, func, restart)                       \
    do{                                                        \
        if(regSignal(signo, func, restart) == SIG_ERR){        \
            return -1;                                         \
        }                                                      \
    }while(0)                                                  \

typedef void (*RDBSignalFunc)(int);
RDBSignalFunc regSignal(int signo, RDBSignalFunc fn, bool restart);

#endif /* __48SLOTS_RDBSIGNAL_H__ */

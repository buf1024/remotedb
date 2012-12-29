/*
 * rdbsignal.cpp
 *
 *  Created on: 2012-12-12
 *      Author: buf1024@gmail.com
 */

#include "rdbsignal.h"
#include <signal.h>

RDBSignalFunc regSignal(int signo, RDBSignalFunc fn, bool restart)
{
    struct sigaction act, oact;
    act.sa_handler = fn;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(restart)    act.sa_flags |= SA_RESTART;

    if (sigaction(signo, &act, &oact) < 0)
        return (RDBSignalFunc)-1;

    return oact.sa_handler;
}


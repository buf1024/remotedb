/*
 * ctrl.cpp
 *
 *  Created on: 2012-12-10
 *      Author: buf1024@gmail.com
 */

#include "rdbconf.h"
#include "rdbutil.h"
#include "rdbsignal.h"
#include "rdbshm.h"
#include "rdb.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RDBConf gRDBConf;
bool    gExit = false;
bool    gUsr  = false;
bool    gCld  = false;

int main(int argc, char **argv)
{
    std::string conf;
    int optch;
    extern char *optarg;
    char optstring[] = "hvc:";
    while ((optch = getopt(argc, argv, optstring)) != -1) {
        switch (optch) {
        case 'h':
            usage();
            exit(0);
        case 'v':
            printf("%s version: %s\n", "RDB", "1.0");
            exit(0);
        case 'c':
            conf = optarg;
            break;
        default:
            break;
        }
    }

    printf("sizeof(RDBShm) = %d\n", sizeof(RDBShm));

    if(conf.empty()){
        printf("Configuration file is empty, using the default configuration\n");
        loadDefConf(gRDBConf);
    }else{
        if(loadConf(conf, gRDBConf) != 0){
            printf("Fail to load configuration file is empty.\n");
            exit(-1);
        }
    }

    if(gRDBConf.daemon) daemonlize();

    //reg signal

    //create share memory

    //start route process

    //wait for all process done

    //start service

    return 0;
}

void usage()
{
    printf("rdb ---- remote db\n\n");
    printf("  -c,   Configuration file  Specific the configuration file\n");
    printf("        If not specific, use the default setting\n");
    printf("  -v,   Print the program version message\n");
    printf("  -h,   Print this help message\n");
}
int regSignal()
{
    REG_SIGNAL(SIGTERM, handleSignal, true); //kill退出
    REG_SIGNAL(SIGINT, handleSignal, true);  //ctrl-c退出
    REG_SIGNAL(SIGUSR1, handleSignal, true); //刷日志
    REG_SIGNAL(SIGUSR2, handleSignal, true); //刷日志
    REG_SIGNAL(SIGHUP, SIG_IGN, true);  //中断重连
    REG_SIGNAL(SIGCHLD, SIG_IGN, false);      //忽略SIGCHLD信号
    REG_SIGNAL(SIGPIPE, SIG_IGN, false);      //忽略SIGPIPE信
    REG_SIGNAL(SIGALRM, SIG_IGN, false);      //忽略SIGALRM信号
    return 0;
}
void handleSignal(int signo)
{
    if(signo == SIGINT || signo == SIGTERM){
        gExit = true;
    }
    if(signo == SIGUSR1 || signo == SIGUSR2){
        gUsr = true;
    }
    if(signo == SIGCHLD){
        gCld = true;
    }
}
int service()
{
    return 0;
}


/*
 * rdbconf.h
 *
 *  Created on: 2012-12-12
 *      Author: buf1024@gmail.com
 */

#ifndef __48SLOTS_RDBCONF_H__
#define __48SLOTS_RDBCONF_H__

#include <string>
#include <list>
#include <stdlib.h>

#define RDBCONF_SEC_COMMON      "common"
#define RDBCONF_LOG_PATH        "log_path"
#define RDBCONF_LOG_FILE_LVL    "log_file_level"
#define RDBCONF_LOG_TERM_LVL    "log_term_level"
#define RDBCONF_LOG_BUFFER      "log_buffer"
#define RDBCONF_LOG_SW_SIZE     "log_switch_size"
#define RDBCONF_LOG_SW_TIME     "log_switch_time"

#define RDBCONF_SHM_PATH        "shm_path"

#define RDBCONF_SEC_RDB         "rdb"
#define RDBCONF_DAEMON          "daemon"
#define RDBCONF_LISTEN_LIST     "listen_list"

#define RDBCONF_ROUTE_COUNT     "route_count"
#define RDBCONF_ROUTE_CLT_COUNT "route_client_count"

#define RDBCONF_ROUTE_FILE      "route_file"

#define RDBCONF_SEC_ROUTE       "route"

#define RDBCONF_STORE_FILE      "store_file"

#define RDBCONF_AUTH_LIST       "auth_list"

#define RDBCONF_SEC_STORE       "store"

#define RDBCONF_STORE_ENGINE    "engine"
#define RDBCONF_STORE_LIB       "engine_lib"
#define RDBCONF_STORE_DATA_DIR  "data_dir"

struct Conf
{
    std::string log_path;
    int         log_file_lvl;
    int         log_term_lvl;
    int         log_buffer;
    size_t      log_switch_size;
    unsigned    log_switch_time;
    std::string shm_path;
};

struct RDBConf : public Conf
{
    bool        daemon;
    std::list<std::pair<std::string, int> > listen_list;
    int         route_count;
    int         route_clt_count;
    std::string route_path;
    std::list<std::string> auth_list;
};

struct RouteConf : public Conf
{
    std::string store_path;
};

struct StoreConf : public Conf
{
    std::string store_engine;
    std::string store_lib;
    std::string store_data_dir;
};

int loadConf(std::string& file, RDBConf& conf);
int loadConf(std::string& file, RouteConf& conf);
int loadConf(std::string& file, StoreConf& conf);

int loadDefConf(RDBConf& conf);
int loadDefConf(RouteConf& conf);
int loadDefConf(StoreConf& conf);


#endif /* __48SLOTS_RDBCONF_H__ */

/*
 * rdbconf.cpp
 *
 *  Created on: 2012-12-12
 *      Author: buf1024@gmail.com
 */

#include "rdbconf.h"
#include "iniconfig.h"
#include "minilog.h"
#include "strutil.h"
#include "cmmhdr.h"
#include <unistd.h>

#define RDBCONF_DEF_PORT 2024

#define RDBCONF_GETSEC(sec, ini, name) \
    do { \
        sec = ini.GetSection(name); \
        if(!sec) { \
            fprintf(stderr, "fail to get section: %s\n", name); \
            return -1; \
        }\
    }while(0)

#define RDBCONF_GETCONF(sec, key, val) \
    do { \
        if(!sec->GetValue(key, val)) { \
            fprintf(stderr, "fail to get item: section=%s, key=%s\n", \
                    sec->GetSectionName().c_str(), key); \
            return -1; \
        }\
    }while(0)

static int loadConf(IniConfig& ini, Conf* conf)
{
    Section* sec = NULL;
    std::string value;
    std::list<std::string> split;

    RDBCONF_GETSEC(sec, ini, RDBCONF_SEC_COMMON);

    RDBCONF_GETCONF(sec, RDBCONF_LOG_PATH, value);
    conf->log_path = value;

    RDBCONF_GETCONF(sec, RDBCONF_LOG_FILE_LVL, value);
    conf->log_file_lvl = Minilog::getLvl(value.c_str());
    if (conf->log_file_lvl == END) {
        fprintf(stderr, "%s \"%s\" not found, set to info\n",
                RDBCONF_LOG_FILE_LVL, value.c_str());
        conf->log_file_lvl = INFO;
    }
    RDBCONF_GETCONF(sec, RDBCONF_LOG_TERM_LVL, value);
    conf->log_term_lvl = Minilog::getLvl(value.c_str());
    if (conf->log_term_lvl == END) {
        fprintf(stderr, "%s \"%s\" not found, set to info\n",
                RDBCONF_LOG_TERM_LVL, value.c_str());
        conf->log_term_lvl = INFO;
    }
    RDBCONF_GETCONF(sec, RDBCONF_LOG_BUFFER, value);
    conf->log_buffer = atoi(value.c_str());
    if (conf->log_buffer < 1024) {
        fprintf(stderr, "%s \"%s\" should greater than 1KB, set to 1KB\n",
                RDBCONF_LOG_SW_SIZE, value.c_str());
        conf->log_buffer = 1024;
    }

    RDBCONF_GETCONF(sec, RDBCONF_LOG_SW_SIZE, value);
    conf->log_switch_size = atoi(value.c_str());
    if (conf->log_switch_size < 1024 * 1024) {
        fprintf(stderr, "%s \"%s\" should greater than 1MB, set to 1MB\n",
                RDBCONF_LOG_SW_SIZE, value.c_str());
        conf->log_switch_size = 1024 * 1024;
    }
    RDBCONF_GETCONF(sec, RDBCONF_LOG_SW_TIME, value);
    if (StdString::Split(value, ":", split) != 2) {
        fprintf(stderr, "%s \"%s\" format not correct\n", RDBCONF_LOG_SW_TIME,
                value.c_str());
        return -1;
    }
    int hour = atoi(split.front().c_str());
    int min = atoi(split.back().c_str());
    if (!(hour >= 0 && hour < 24) || !(min >= 0 && min < 59)) {
        fprintf(stderr, "%s \"%s\" format not correct\n", RDBCONF_LOG_SW_TIME,
                value.c_str());
        return -1;
    }
    conf->log_switch_time = hour * 60 + min;

    RDBCONF_GETCONF(sec, RDBCONF_SHM_PATH, value);
    conf->shm_path = value;

    return 0;
}
static int loadDefConf(Conf* conf)
{
    char path[CMM_PATH_SIZE] = "";
    std::string cur_dir;

    cur_dir = getcwd(path, CMM_PATH_SIZE);
    conf->log_path = cur_dir + "/logs";
    conf->log_file_lvl = INFO;
    conf->log_term_lvl = INFO;
    conf->log_buffer = 1024 * 8; //8K
    conf->log_switch_size = 1024 * 1024 * 10; //10M
    conf->log_switch_time = 0; //00:00
    conf->log_path = cur_dir + "/etc/shm";

    return 0;
}

int loadConf(std::string& file, RDBConf& conf)
{
    IniConfig ini;

    if (!ini.Load(file))
        return -1;

    if (loadConf(ini, (Conf*) &conf) != 0) {
        return -1;
    }

    Section* sec = NULL;
    std::string value;
    std::list<std::string> split;

    RDBCONF_GETSEC(sec, ini, RDBCONF_SEC_RDB);

    RDBCONF_GETCONF(sec, RDBCONF_DAEMON, value);
    conf.daemon = true;
    if (value == "false" || value == "FALSE" || value == "no"
            || value == "NO") {
        conf.daemon = false;
    }
    RDBCONF_GETCONF(sec, RDBCONF_LISTEN_LIST, value);
    if (StdString::Split(value, " ", split) == 0) {
        fprintf(stderr, "%s \"%s\" format not correct, set to %s:%d\n",
                RDBCONF_LISTEN_LIST, value.c_str(), "127.0.0.1",
                RDBCONF_DEF_PORT);
        conf.listen_list.push_back(
                std::make_pair("127.0.0.1", RDBCONF_DEF_PORT));
    } else {
        std::list<std::string> addr_pair;
        for (std::list<std::string>::iterator iter = split.begin();
                iter != split.end(); ++iter) {
            //todo check addr
            if (StdString::Split(*iter, ":", addr_pair) != 2) {
                fprintf(stderr, "%s \"%s\" format not correct\n",
                        RDBCONF_LISTEN_LIST, iter->c_str());
            } else {
                conf.listen_list.push_back(
                        std::make_pair(addr_pair.front(),
                                atoi(addr_pair.back().c_str())));
            }
        }
    }
    if (conf.listen_list.empty()) {
        fprintf(stderr, "%s is empty, set to %s:%d\n", RDBCONF_LISTEN_LIST,
                "127.0.0.1", RDBCONF_DEF_PORT);
        conf.listen_list.push_back(
                std::make_pair("127.0.0.1", RDBCONF_DEF_PORT));
    }
    RDBCONF_GETCONF(sec, RDBCONF_ROUTE_COUNT, value);
    conf.route_count = atoi(value.c_str());
    if (conf.route_count <= 0) {
        fprintf(stderr, "%s \"%s\" format not correct\n", RDBCONF_ROUTE_COUNT,
                value.c_str());
        return -1;
    }
    RDBCONF_GETCONF(sec, RDBCONF_ROUTE_CLT_COUNT, value);
    conf.route_clt_count = atoi(value.c_str());
    if (conf.route_clt_count <= 0) {
        fprintf(stderr, "%s \"%s\" format not correct\n",
                RDBCONF_ROUTE_CLT_COUNT, value.c_str());
        return -1;
    }
    RDBCONF_GETCONF(sec, RDBCONF_ROUTE_FILE, value);
    conf.route_path = value;

    RDBCONF_GETCONF(sec, RDBCONF_AUTH_LIST, value);
    if (StdString::Split(value, " ", split) == 0) {
        conf.auth_list.push_back("*");
    } else {
        for (std::list<std::string>::iterator iter = split.begin();
                iter != split.end(); ++iter) {
            conf.auth_list.push_back(*iter);
        }
    }
    if (conf.auth_list.empty()) {
        conf.auth_list.push_back("*");
    }

    return 0;
}

int loadConf(std::string& file, RouteConf& conf)
{
    IniConfig ini;

    if (!ini.Load(file))
        return -1;

    if (loadConf(ini, (Conf*) &conf) != 0) {
        return -1;
    }
    Section* sec = NULL;
    std::string value;

    RDBCONF_GETSEC(sec, ini, RDBCONF_SEC_ROUTE);
    RDBCONF_GETCONF(sec, RDBCONF_STORE_FILE, value);
    conf.store_path = value;

    return 0;
}
int loadConf(std::string& file, StoreConf& conf)
{
    IniConfig ini;

    if (!ini.Load(file))
        return -1;

    if (loadConf(ini, (Conf*) &conf) != 0) {
        return -1;
    }
    Section* sec = NULL;
    std::string value;
    RDBCONF_GETSEC(sec, ini, RDBCONF_SEC_STORE);

    RDBCONF_GETCONF(sec, RDBCONF_STORE_ENGINE, value);
    conf.store_engine = value;

    RDBCONF_GETCONF(sec, RDBCONF_STORE_LIB, value);
    conf.store_lib = value;

    RDBCONF_GETCONF(sec, RDBCONF_STORE_DATA_DIR, value);
    conf.store_data_dir = value;

    return 0;
}

int loadDefConf(RDBConf& conf)
{

    if(loadDefConf((Conf*)&conf) != 0){
        return -1;
    }

    char path[CMM_PATH_SIZE] = "";
    std::string cur_dir;

    cur_dir = getcwd(path, CMM_PATH_SIZE);

    conf.daemon = true;
    conf.listen_list.push_back(std::make_pair("127.0.0.1", RDBCONF_DEF_PORT));
    conf.route_count = 8;
    conf.route_clt_count = 16;
    conf.route_path = cur_dir + "/route";
    conf.auth_list.push_back("*");

    return 0;
}

int loadDefConf(RouteConf& conf)
{
    if(loadDefConf((Conf*)&conf) != 0){
        return -1;
    }

    char path[CMM_PATH_SIZE] = "";
    std::string cur_dir;
    cur_dir = getcwd(path, CMM_PATH_SIZE);
    conf.store_path = cur_dir + "/store";
    return 0;
}
int loadDefConf(StoreConf& conf)
{
    if(loadDefConf((Conf*)&conf) != 0){
        return -1;
    }

    char path[CMM_PATH_SIZE] = "";
    std::string cur_dir;
    cur_dir = getcwd(path, CMM_PATH_SIZE);
    conf.store_engine = "ldb";
    conf.store_lib = "libldb.so";
    conf.store_data_dir = cur_dir + "/data";

    return 0;
}


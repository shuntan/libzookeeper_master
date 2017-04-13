/*
 * configure_test.cpp
 *
 *  Created on: 2017年3月20日
 *      Author: ShunTan
 */
#include <stdio.h>
#include "src-cpp/zookeeper_helper.h"

/*
 * 测试zookeeper配置同步的问题
 * CConfigureZkRead类为读配置类,用于监视配置变化而发生的运行变化。
 * CConfigureZkWrite类为写配置类，定时修改公共的配置信息,用于触发C端的监视变化。
 * 使用方法:可以在本机运行俩个读和写程式 ./cofigure_test read (and) ./cofigure_test write
 */

const char * g_hosts = "10.143.130.31:2181";
zookeeper::CZookeeperHelper g_zk(g_hosts, 1000, 2048, ZOO_LOG_LEVEL_DEBUG, true);

class CConfigureZkRead: public zookeeper::CWatcherAction
{
public:
    CConfigureZkRead();
    ~CConfigureZkRead();

public:
    virtual void on_node_created(zookeeper::CZookeeperHelper*,  const std::string& path)
    {
        //printf("on_node_created \n");
    }

    virtual void on_nodevalue_changed(zookeeper::CZookeeperHelper*, const std::string& path)
    {
        //printf("on_nodevalue_changed\n");
        if(get(_config))
            printf("config changed !\n");
        clear();
    }

    virtual void on_node_deleted(zookeeper::CZookeeperHelper*, const std::string& path)
    {
        //printf("on_node_deleted\n");
    }

public:
    int  run();
    bool get(std::string& config);

private:
    std::string _config;
};

CConfigureZkRead::CConfigureZkRead()
{
}

CConfigureZkRead::~CConfigureZkRead()
{
}

int CConfigureZkRead::run()
{
    if(get(_config))
    {
        printf("get config success ! \n");
    }
    else
    {
        printf("get config failed ! \n");
        return 0;
    }

    while(true)
    {
        printf("do work with conf[%s] \n", _config.c_str());
        sleep(1);
    }
}

bool CConfigureZkRead::get(std::string& config)
{
    return (g_zk.zookeeper_get("/shit", &config, this) == ZOK) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////////////

class CConfigureZkWrite
{
public:
    CConfigureZkWrite();
    ~CConfigureZkWrite();

public:
    int  run();
    bool set(const std::string& config);
};

CConfigureZkWrite::CConfigureZkWrite()
{
}

CConfigureZkWrite::~CConfigureZkWrite()
{
}

int CConfigureZkWrite::run()
{
    int i = 0;
    std::string config_suffer = "FUCK_YOU_";

    while(true)
    {
        char str[sizeof("065535")]; // 0xFFFF
        snprintf(str, sizeof(str), "%d", i);

        if(set((config_suffer+str)))
        {
            printf("set config[%s] success!\n", (config_suffer+str).c_str() );
            i++;
        }
        else
        {
            return -1;
        }

        sleep(10);
    }
}

bool CConfigureZkWrite::set(const std::string& config)
{
    int errcode;
    errcode = g_zk.zookeeper_set("/shit", config);
    if(errcode == ZNONODE)
    {
        std::string new_path;
        errcode = g_zk.zookeeper_create("/shit", config, &new_path);
        if(errcode != ZOK)
        {
            printf("error:(%s) code[%d]\n", zerror(errcode), errcode);
        }
        else
        {
            return true;
        }
    }
    else
    {
        printf("error:(%s) code[%d]\n", zerror(errcode), errcode);
    }

    return false;
}

/////////////////////////////////////////////////////////////////

extern "C" int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("arguments num error!\n");
        return -1;
    }

    int errcode;
    errcode = g_zk.connect();
    if (errcode == ZOK)
    {
        std::string ip;
        uint16_t port;

        if (g_zk.get_zookeeper_host(&ip, &port))
            printf("client:(%lld)(%s,%d) connect to zookeeper success!\n",g_zk.get_zookeeper_clientid(), ip.c_str(), port);
    }
    else
    {
        printf("error:%s\n", zerror(errcode));
    }

    if(std::string(argv[1]) == "read")
    {
        CConfigureZkRead zr;
        zr.run();
    }
    else if(std::string(argv[1]) == "write")
    {
        CConfigureZkWrite zw;
        zw.run();
    }
    else
    {
        printf("no right argv!\n");
        return -1;
    }

    return 0;
}


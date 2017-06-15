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
 * 使用方法:可以在本机运行俩个读和写程式 ./cofigure_test -r (and) ./cofigure_test -w
 */

const char * g_hosts = "10.143.130.31:2181";
static zookeeper::CZookeeperHelper g_zk(g_hosts, 1000, 2048, ZOO_LOG_LEVEL_DEBUG, true);


////////////////////////////////////读配置端,自动监控配置变化//////////////////////////////////////////////////
class CConfigureZkRead: public zookeeper::CWatcherActionBase
{
public:
    virtual void on_node_created(zhandle_t* zk,  const std::string& path)
    {
        //std::cerr << "on_node_created \n" << std::endl;
    }

    virtual void on_nodevalue_changed(zhandle_t* zk, const std::string& path)
    {
        if(get(_config))
            std::cerr << "config changed !\n" << std::endl;
    }

    virtual void on_node_deleted(zhandle_t* zk, const std::string& path)
    {
        //std::cerr << "on_node_deleted\n" << std::endl;
    }

public:
    int  run()
    {
        if(get(_config))
        {
            std::cerr << "get config success ! \n" << std::endl;
        }
        else
        {
            std::cerr << "get config failed ! \n" << std::endl;
            return 0;
        }

        while(true)
        {
            printf("do work with conf[%s] \n", _config.c_str());
            sleep(1);
        }
    }

    bool get(std::string& config)
    {
        return (g_zk.zookeeper_get("/shit", &config, this) == ZOK) ? true : false;
    }

private:
    std::string _config;
};


////////////////////////////////////写配置端,激活配置变更通知//////////////////////////////////////////////////

class CConfigureZkWrite
{
public:
    int  run()
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
                return 1;
            }

            sleep(10);
        }
    }

    bool set(const std::string& config)
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
};

//////////////////////TEST/////////////////////////////

extern "C" int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Arguments error!" << optarg << std::endl;
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

    int opt;
    while ((opt = getopt(argc, argv, "rw")) != -1)
    {
      switch (opt)
      {
          case 'r':
          {
              CConfigureZkRead zr;
              zr.run();
              break;
          }

          case 'w':
          {
              CConfigureZkWrite zw;
              zw.run();
              break;
          }

          default:
          {
              std::cerr << "Unknown option: " << optarg << std::endl;
              _exit(1);
              break;
          }
      }
    }
    return 0;
}


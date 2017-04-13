/*
 * client_test.cpp
 *
 *  Created on: 2017年2月28日, Tencent Inc.
 *      Author: ShunTan
 */
//主备切换  Master switch
/*
 * 本例子为了测试基于zookeeper的主备切换功能,本机同时开启俩个程式即可看到效果。
 * zookeeper还可应用于负载均衡,配置同步。等各种场景。
 */

#include "cpp/zookeeper_helper.h"

const char * g_hosts = "10.143.130.31:2181";

class CExistsWatcher: public zookeeper::CWatcherAction
{
public:
    virtual void on_node_deleted(zookeeper::CZookeeperHelper* zk, const std::string& path)
    {
        std::string value_path;
        zk->zookeeper_create(path, "shit", &value_path);
        printf("Now, Im' the master \n");
        clear();
    }

    void run()
    {
        while(true)
        {
            if(!is_triggered())
            {
                printf("waiting for ....\n");
                sleep(10);
                continue;
            }

            printf("working ....\n");
            sleep(1);
        }
    }
};

class CExistCompletion: public zookeeper::CAsyncCompletion
{
    virtual void stat_compl(int errcode, const Stat& stat)
    {
        if (errcode != ZOK)
        {
            printf("error:%s\n", zerror(errcode));
        }
        else
        {
            printf("exist async respond success! \n");
        }
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////

extern "C" int main()
{
    int errcode;
    zookeeper::CZookeeperHelper zk(g_hosts, 1000, 2048, ZOO_LOG_LEVEL_DEBUG, true);
    errcode = zk.connect();
    if (errcode == ZOK)
    {
        std::string ip;
        uint16_t port;

        if (zk.get_zookeeper_host(&ip, &port))
            printf("client:(%lld)(%s,%d) connect to zookeeper success!\n",zk.get_zookeeper_clientid(), ip.c_str(), port);
    }
    else
    {
        printf("error:%s\n", zerror(errcode));
    }

    // TEST MULTI
    // 第一次进入确定主备关系
    std::vector<zookeeper::STOption_t> ops;
    std::vector<zookeeper::STResult_t> results;
    ops.push_back(zk.zookeeper_create_op_init("/ShunTan", "shit"));
    errcode = zk.zookeeper_multi(ops, results);

    if (errcode != ZOK)
    {
        if (errcode == ZNODEEXISTS)
        {
            printf("node[%s] exist, Im' the cluster. \n", "/ShunTan");
        }
    }
    else
    {
        printf("create node[%s] success, Im' the master. \n",results[0].value.c_str());
    }

    // TEST ASYNC
    CExistsWatcher exist_watcher;
    zookeeper::CAsyncCompletion async_completion;
    zk.zookeeper_exists("/ShunTan", &async_completion, &exist_watcher);

    // 模拟程序运行/空转
    exist_watcher.run();

    return 0;
}



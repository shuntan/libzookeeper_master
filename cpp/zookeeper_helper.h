/*
 * zookeeper_helper.h
 *
 *  Created on: 2017年2月22日, Tencent Inc.
 *      Author: ShunTan
 */

#ifndef ZOOKEEPER_SHUN_ZOOKEEPER_HELPER_H_
#define ZOOKEEPER_SHUN_ZOOKEEPER_HELPER_H_
#include <zookeeper/zookeeper.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <vector.h>

namespace zookeeper {

/* 编译时使用 zookeeper_mt(多线程库)编译
 * 异步回调函数声明,直接被类CAsyncCompletion回调。
 * 监视回调函数声明,直接被类CWatcherAction回调。
 */

class CZookeeperHelper;
class CAsyncCompletion;
inline void async_completion(int errcode, ACL_vector *acls, Stat *stat, const void *data);
inline void async_completion(int errcode, const char *value, int len, const Stat *stat, const void *data);
inline void async_completion(int errcode, const Stat *stat, const void *data);
inline void async_completion(int errcode, const char *value, const void *data);
inline void async_completion(int errcode, const String_vector *strings, const void *data);
inline void async_completion(int errcode, const String_vector *strings, const Stat *stat, const void *data);
inline void async_completion(int errcode, const void *data);
class CWatcherAction;
inline void active_watcher(zhandle_t *zh, int type, int state, const char *path, void* ctx);

/*
 * 异步调用类,其中包含了异步调用触发状态triggered,当triggered=true时表明回调成功,利用回调的参数进行主动操作。
 * 也可以子类继承,自己定义接口,该类的对象应作为API函数的ASYNC参数指针。
 */
class CAsyncCompletion
{
public:
    CAsyncCompletion();
    virtual ~CAsyncCompletion();

    virtual void acl_compl(int errcode, const std::vector<ACL>& acls, const Stat& stat);

    virtual void data_compl(int errcode, const std::string& value, const Stat& stat);

    virtual void stat_compl(int errcode, const Stat& stat);

    virtual void string_compl(int errcode, const std::string& value);

    virtual void strings_compl(int errcode, const std::vector<std::string>& strings);

    virtual void strings_compl(int errcode, const std::vector<std::string>& strings, const Stat& stat);

    virtual void void_compl(int errcode);

public:
    bool is_triggered() const;

    void clear();   //清除上次异步回调的信息

private:
    volatile bool m_triggered;   // 是否触发
};

/*
 * 监视调用类,其中包含了监视调用触发状态triggered,当triggered=true时表明监视状态发生变化。
 * 也可以子类继承,自己定义接口,在状态发生变化的时刻做相应的操作。该类的对象应作为API函数的Watcher参数指针,为空指针即表明不设置监视。
 */

class CWatcherAction
{
public:
    CWatcherAction();
    virtual ~CWatcherAction();

    /*****连接状态******/
    virtual void on_session_expired(CZookeeperHelper* zookeeper_handler);

    virtual void on_connection_established(CZookeeperHelper* zookeeper_handler);

    virtual void on_connection_lost(CZookeeperHelper* zookeeper_handler);

    virtual void on_associating_refuse(CZookeeperHelper* zookeeper_handler);

    virtual void on_auth_failed(CZookeeperHelper* zookeeper_handler);

    /******监视类型变化*****/
    virtual void on_session_lost(CZookeeperHelper* zookeeper_handler);

    virtual void on_node_created(CZookeeperHelper* zookeeper_handler, const std::string& path);

    virtual void on_nodevalue_changed(CZookeeperHelper* zookeeper_handler, const std::string& path);

    virtual void on_node_deleted(CZookeeperHelper* zookeeper_handler, const std::string& path);

    virtual void on_child_changed(CZookeeperHelper* zookeeper_handler, const std::string& path);

    virtual void on_watching_removed(CZookeeperHelper* zookeeper_handler);


public:
    bool is_connected() const;

    bool is_triggered() const;

    void clear();

private:
    volatile bool m_connected;           // 连接状态
    volatile bool m_triggered;           // 是否触发

public:
    void*  m_data;
};

/*
 * 多重操作选项结构体,可以在CREATE,DELETE,SET,CHECK选择任意一种。
 * 最后封装成一个STOptions数组作为API - zookeeper_multi的参数。
 * STResult为调用返回的结果数组,顺序和 STOption一一对应。
 */
typedef struct STOption
{
    enum ZOPFLAG
    {
        Z_ZOO_NOTIFY = ZOO_NOTIFY_OP, //默认为无类型
        Z_ZOO_CREATE = ZOO_CREATE_OP,
        Z_ZOO_DELETE = ZOO_DELETE_OP,
        Z_ZOO_CHECK  = ZOO_CHECK_OP,
        Z_ZOO_SET    = ZOO_SETDATA_OP
    } type;

    // CREATE
    struct
    {
        std::string path;
        std::string data;
        ACL_vector acl;
        int flags;
    } create_op;

    // DELETE
    struct
    {
        std::string path;
        int version;
    } delete_op;

    // SET
    struct
    {
        std::string path;
        std::string data;
        int version;
    } set_op;

    // CHECK
    struct
    {
        std::string path;
        int version;
    } check_op;

    STOption() :type(Z_ZOO_NOTIFY)
    {

    }

} STOption_t;

typedef struct STResult
{
    int error;
    std::string value;
    Stat stat;

} STResult_t;

/*
 * zookeeper辅助类
 * 分为同步和异步的API,带有CAsyncCompletion参数的为ASYNC异步,不带的为SYNC同步。
 */
class CZookeeperHelper
{
public:

    /* example:
     * hosts = "127.0.0.1:4321,127.0.0.1:1314,...."
     * log_level = "ZOO_LOG_LEVEL_ERROR" or "ZOO_LOG_LEVEL_WARN" or "ZOO_LOG_LEVEL_INFO" or "ZOO_LOG_LEVEL_DEBUG"
     * buffer_length 表示最大一次性接收buffer的大小。
     * enable_logfile = true 表示开启写日志功能。
     */
    CZookeeperHelper(const std::string& hosts, uint16_t timeout = 1000,
            uint16_t buffer_length = 2048, ZooLogLevel log_level =
                    (ZooLogLevel) 0, bool enable_logfile = false);
    ~CZookeeperHelper();

public:
    // @ZOO_OPEN_ACL_UNSAFE(This is a completely open ACL.)
    // @ZOO_READ_ACL_UNSAFE(This ACL gives the world the ability to read.)
    // @ZOO_CREATOR_ALL_ACL(This ACL gives the creators authentication id's all permissions.)

    ZOO_ERRORS connect();

    ZOO_ERRORS reconncet();

    ZOO_ERRORS close();

    bool get_zookeeper_host(std::string* ip, uint16_t* port);

    int64_t get_zookeeper_clientid() const;

    // 基本操作API,上为SYNC,下为ASYNC
    ZOO_ERRORS zookeeper_create(const std::string& path, const std::string& value, std::string* path_value, const ACL_vector& acl_entries = ZOO_OPEN_ACL_UNSAFE, int flags = ZOO_EPHEMERAL);
    ZOO_ERRORS zookeeper_create(const std::string& path, const std::string& value, CAsyncCompletion* completion, const ACL_vector& acl_entries = ZOO_OPEN_ACL_UNSAFE, int flags = ZOO_EPHEMERAL);

    ZOO_ERRORS zookeeper_set(const std::string& path, const std::string& value, int version = -1);
    ZOO_ERRORS zookeeper_set(const std::string& path, const std::string& value, CAsyncCompletion* completion, int version = -1);

    ZOO_ERRORS zookeeper_delete(const std::string& path, int version = -1);
    ZOO_ERRORS zookeeper_delete(const std::string& path, CAsyncCompletion* completion, int version = -1);

    ZOO_ERRORS zookeeper_exists(const std::string& path, CWatcherAction* watcher_action = NULL, Stat* stat = NULL);
    ZOO_ERRORS zookeeper_exists(const std::string& path, CAsyncCompletion* completion, CWatcherAction* watcher_action = NULL);

    ZOO_ERRORS zookeeper_get(const std::string& path, std::string* value, CWatcherAction* watcher_action = NULL, Stat* stat = NULL);
    ZOO_ERRORS zookeeper_get(const std::string& path, CAsyncCompletion* completion, CWatcherAction* watcher_action = NULL);

    ZOO_ERRORS zookeeper_get_children(const std::string& path,std::vector<std::string>* childrens, CWatcherAction* watcher_action = NULL, Stat* stat = NULL);
    ZOO_ERRORS zookeeper_get_children(const std::string& path,CAsyncCompletion* completion, CWatcherAction* watcher_action = NULL);

    ZOO_ERRORS zookeeper_get_acl(const std::string& path, std::vector<ACL>* acls, Stat* stat = NULL);
    ZOO_ERRORS zookeeper_get_acl(const std::string& path, CAsyncCompletion* completion);

    ZOO_ERRORS zookeeper_set_acl(const std::string& path, const ACL_vector& acls, int version = -1);
    ZOO_ERRORS zookeeper_set_acl(const std::string& path, const ACL_vector& acls, CAsyncCompletion* completion, int version = -1);

    // 复合原子操作API
    STOption zookeeper_create_op_init(const std::string& path, const std::string& value, const ACL_vector& acl_entries = ZOO_OPEN_ACL_UNSAFE, int flags = ZOO_EPHEMERAL);
    STOption zookeeper_set_op_init(const std::string& path, const std::string& value, int version = -1);
    STOption zookeeper_delete_op_init(const std::string& path,int version = -1);
    STOption zookeeper_check_op_init(const std::string& path, int version = -1);

    ZOO_ERRORS zookeeper_multi(const std::vector<STOption>& options, std::vector<STResult>& results);
    ZOO_ERRORS zookeeper_multi(const std::vector<STOption>& options, CAsyncCompletion* completion, std::vector<STResult>& results);

private:
    std::string m_hosts;               // zookeeperk服务端集群的host列表
    uint16_t m_timeout;                // 默认接收zookeeper服务端的超时时间(秒s)
    uint16_t m_default_buffer_length;  // 默认创建buffer数据缓存的大小
    clientid_t m_client_id;            // 连接zookeeper的客户端ID
    zhandle_t* m_zk_handle;            // zookeeper的指针句柄
    CWatcherAction* m_watcher_init;    // 监视连接是否成功
    FILE* m_log_file;                  // 开启日志的文件流?
};

}

#endif /* ZOOKEEPER_SHUN_ZOOKEEPER_HELPER_H_ */

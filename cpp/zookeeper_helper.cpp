/*
 * zookeeper_helper.cpp
 *
 *  Created on: 2017年2月22日, Tencent Inc.
 *      Author: ShunTan
 */

#include "zookeeper_helper.h"

namespace zookeeper {

static FILE *open_logfile(const char* log_name)
{
    char name[1024];
    strcpy(name, "LOG-");
    strncpy(name + 4, log_name, sizeof(name) - 4);
#ifdef THREADED
    strcpy(name + strlen(name), "-mt.txt");
#else
    strcpy(name + strlen(name), "-st.txt");
#endif

    FILE *logfile = fopen(name, "a");

    if (logfile == 0)
    {
        fprintf(stderr, "Can't open log file %s!\n", name);
        return 0;
    }

    return logfile;
}

void async_completion(int errcode, ACL_vector *acl, Stat *stat, const void *data)
{
    assert("Completion data is NULL" && data);

    std::vector<ACL> acls;
    for (int i = 0; i < acl->count; i++)
    {
        const ACL& acl_ = acl->data[i];
        //acl.id.id;
        //acl.id.scheme;
        //acl.perms;
        acls.push_back(acl_);
    }

    static_cast<CAsyncCompletion*>((void*) data)->acl_compl(errcode, acls, *stat);
}

void async_completion(int errcode, const char *value, int len, const Stat *stat,
        const void *data)
{
    assert("Completion data is NULL" && data);
    static_cast<CAsyncCompletion*>((void*) data)->data_compl(errcode, value, *stat);
}

void async_completion(int errcode, const Stat *stat, const void *data)
{
    assert("Completion data is NULL" && data);
    static_cast<CAsyncCompletion*>((void*) data)->stat_compl(errcode, *stat);
}

void async_completion(int errcode, const char *value, const void *data)
{
    assert("Completion data is NULL" && data);
    static_cast<CAsyncCompletion*>((void*) data)->string_compl(errcode, value);
}

void async_completion(int errcode, const String_vector *strings, const void *data)
{
    assert("Completion data is NULL" && data);

    std::vector<std::string> strings_;
    for (int i = 0; i < strings->count; i++)
    {
        const std::string& string_ = strings->data[i];
        //acl.id.id;
        //acl.id.scheme;
        //acl.perms;
        strings_.push_back(string_);
    }
    static_cast<CAsyncCompletion*>((void*) data)->strings_compl(errcode, strings_);
}

void async_completion(int errcode, const String_vector *strings, const Stat *stat, const void *data)
{
    assert("Completion data is NULL" && data);

    std::vector<std::string> strings_;
    for (int i = 0; i < strings->count; i++)
    {
        const std::string& string_ = strings->data[i];
        //acl.id.id;
        //acl.id.scheme;
        //acl.perms;
        strings_.push_back(string_);
    }
    static_cast<CAsyncCompletion*>((void*) data)->strings_compl(errcode, strings_, *stat);
}

void async_completion(int errcode, const void *data)
{
    assert("Completion data is NULL" && data);
    static_cast<CAsyncCompletion*>((void*) data)->void_compl(errcode);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void active_watcher(zhandle_t *zh, int type, int state, const char *path,
        void* ctx)
{
    if (zh == NULL || ctx == NULL)
        return;
    CWatcherAction* action = (CWatcherAction*) ctx;

    // 先判断  Zookeeper 连接状态
    if (state == ZOO_EXPIRED_SESSION_STATE)
        action->on_session_expired(static_cast<CZookeeperHelper*>(action->m_data));
    else if (state == ZOO_CONNECTING_STATE)
        action->on_connection_lost(static_cast<CZookeeperHelper*>(action->m_data));
    else if (state == ZOO_CONNECTED_STATE)
        action->on_connection_established(static_cast<CZookeeperHelper*>(action->m_data)); //正常应该返回的状态
    else if (state == ZOO_ASSOCIATING_STATE)
        action->on_associating_refuse(static_cast<CZookeeperHelper*>(action->m_data));
    else if (state == ZOO_AUTH_FAILED_STATE)
        action->on_auth_failed(static_cast<CZookeeperHelper*>(action->m_data));

    if (type == ZOO_SESSION_EVENT)
        action->on_session_lost(static_cast<CZookeeperHelper*>(action->m_data));
    else if (type == ZOO_CHANGED_EVENT)
        action->on_nodevalue_changed(static_cast<CZookeeperHelper*>(action->m_data), path);
    else if (type == ZOO_DELETED_EVENT)
        action->on_node_deleted(static_cast<CZookeeperHelper*>(action->m_data), path);
    else if (type == ZOO_CHILD_EVENT)
        action->on_child_changed(static_cast<CZookeeperHelper*>(action->m_data), path);
    else if (type == ZOO_CREATED_EVENT)
        action->on_node_created(static_cast<CZookeeperHelper*>(action->m_data), path);
    else if (type == ZOO_NOTWATCHING_EVENT)
        action->on_watching_removed(static_cast<CZookeeperHelper*>(action->m_data));
    // TODO: implement for the rest of the event types
    // ...
}

////////////////////////////////////CAsyncCompletion////////////////////////////////////////
CAsyncCompletion::CAsyncCompletion() : m_triggered(false)
{

}

CAsyncCompletion::~CAsyncCompletion()
{
    clear();
}

void CAsyncCompletion::acl_compl(int errcode, const std::vector<ACL>& acls, const Stat& stat)
{
    m_triggered = (true);
}

void CAsyncCompletion::data_compl(int errcode, const std::string& value, const Stat& stat)
{
    m_triggered = (true);
}

void CAsyncCompletion::stat_compl(int errcode, const Stat& stat)
{
    m_triggered = (true);
}

void CAsyncCompletion::string_compl(int errcode, const std::string& value)
{
    m_triggered = (true);
}

void CAsyncCompletion::strings_compl(int errcode, const std::vector<std::string>& strings)
{
    m_triggered = (true);
}

void CAsyncCompletion::strings_compl(int errcode, const std::vector<std::string>& strings, const Stat& stat)
{
    m_triggered = (true);
}

void CAsyncCompletion::void_compl(int errcode)
{
    m_triggered = (true);
}

bool CAsyncCompletion::is_triggered() const
{
    return m_triggered;      // 是否触发
}

void CAsyncCompletion::clear()
{
    m_triggered = false;
}

/////////////////////////////////////////////////CWatcherAction///////////////////////////////////////////////////
CWatcherAction::CWatcherAction() : m_connected(false), m_triggered(false), m_data(NULL)
{

}

CWatcherAction::~CWatcherAction()
{
    clear();
}

void CWatcherAction::on_session_expired(CZookeeperHelper* zookeeper_handler)
{
    // ZOO_EXPIRED_SESSION_STATE;
}

void CWatcherAction::on_connection_established(CZookeeperHelper* zookeeper_handler)
{
    // ZOO_CONNECTED_STATE
    m_connected = true;
}

void CWatcherAction::on_connection_lost(CZookeeperHelper* zookeeper_handler)
{
    // ZOO_CONNECTING_STATE
}

void CWatcherAction::on_associating_refuse(CZookeeperHelper* zookeeper_handler)
{
    // ZOO_ASSOCIATING_STATE;
}

void CWatcherAction::on_auth_failed(CZookeeperHelper* zookeeper_handler)
{
    // ZOO_AUTH_FAILED_STATE;
}

// 监视类型变化
void CWatcherAction::on_session_lost(CZookeeperHelper* zookeeper_handler)
{
    m_triggered = true;
    // ZOO_SESSION_EVENT;
}

void CWatcherAction::on_node_created(CZookeeperHelper* zookeeper_handler, const std::string& path)
{
    m_triggered = true;
    // ZOO_CREATED_EVENT;
}

void CWatcherAction::on_nodevalue_changed(CZookeeperHelper* zookeeper_handler, const std::string& path)
{
    m_triggered = true;
    // ZOO_CHANGED_EVENT;
}

void CWatcherAction::on_node_deleted(CZookeeperHelper* zookeeper_handler, const std::string& path)
{
    m_triggered = true;
    // ZOO_DELETED_EVENT;
}

void CWatcherAction::on_child_changed(CZookeeperHelper* zookeeper_handler, const std::string& path)
{
    m_triggered = true;
    // ZOO_CHILD_EVENT;
}

void CWatcherAction::on_watching_removed(CZookeeperHelper* zookeeper_handler)
{
    m_triggered = true;
    // ZOO_NOTWATCHING_EVENT;
}

bool CWatcherAction::is_connected() const
{
    return m_connected;
}

bool CWatcherAction::is_triggered() const
{
    return m_triggered;
}

void CWatcherAction::clear()
{
    m_connected = (false);
    m_triggered = (false);
}

///////////////////////////////////////////////CZookeeperHelper///////////////////////////////////////////////////
CZookeeperHelper::CZookeeperHelper(const std::string& hosts, uint16_t timeout,
        uint16_t buffer_length, ZooLogLevel log_level, bool enable_logfile) :
        m_zk_handle(NULL), m_watcher_init(NULL)
{
    m_hosts = hosts;
    m_timeout = timeout;
    m_default_buffer_length = buffer_length;

    zoo_set_debug_level(log_level);

    enable_logfile ? m_log_file = open_logfile("zookeeper"): m_log_file = 0;
    zoo_set_log_stream(m_log_file);
}

CZookeeperHelper::~CZookeeperHelper()
{
    close();

    if (m_log_file != NULL)
    {
        fflush(m_log_file);
        fclose(m_log_file);
    }
}

ZOO_ERRORS CZookeeperHelper::connect()
{
    int errcode = ZOK;
    m_watcher_init = new CWatcherAction();
    m_watcher_init->m_data = this;
    // which is good for more even client connection distribution among the quorum peers.
    zoo_deterministic_conn_order(1);

    m_zk_handle = zookeeper_init(m_hosts.c_str(), active_watcher, m_timeout,
            NULL, m_watcher_init, 0);

    if (m_zk_handle == NULL)
    {
        errcode = errno;
        return (ZOO_ERRORS)errcode;
    }
    else
    {
        m_client_id = *(zoo_client_id(m_zk_handle));
    }

    size_t try_times = 0;
    while (!m_watcher_init->is_connected())
    {
        if (try_times > 10)
        {
            errcode = ZNOTHING;
            return (ZOO_ERRORS)errcode;
        }

        usleep(100);
        try_times++;
    }

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::reconncet()
{
    int errcode = ZOK;
    close();

    m_watcher_init = new CWatcherAction();
    m_watcher_init->m_data = this;
    m_zk_handle = zookeeper_init(m_hosts.c_str(), active_watcher, m_timeout,
            &m_client_id, m_watcher_init, 0);

    if (m_zk_handle == NULL)
    {
        errcode = errno;
        return (ZOO_ERRORS)errcode;
    }

    if(ZINVALIDSTATE == is_unrecoverable(m_zk_handle))
    {
        return ZINVALIDSTATE;
    }

    size_t try_times = 0;
    while (!m_watcher_init->is_connected())
    {
        if (try_times > 10)
        {
            errcode = ZNOTHING;
            return (ZOO_ERRORS)errcode;
        }

        usleep(100);
        try_times++;
    }

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::close()
{
    int errcode = zookeeper_close(m_zk_handle);
    m_zk_handle = NULL;

    if (m_watcher_init != NULL)
    {
        delete m_watcher_init;
        m_watcher_init = NULL;
    }

    return (ZOO_ERRORS)errcode;
}

bool CZookeeperHelper::get_zookeeper_host(std::string* ip, uint16_t* port)
{
    struct sockaddr_in *sa = NULL;
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);

    if (zookeeper_get_connected_host(m_zk_handle, &addr, &addr_len) == NULL)
    {
        return false;
    }

    sa = (struct sockaddr_in *) &addr;
    *ip = inet_ntoa(sa->sin_addr);
    *port = ntohs(sa->sin_port);

    return true;
}

int64_t CZookeeperHelper::get_zookeeper_clientid() const
{
    return m_client_id.client_id;
}

/////////////
ZOO_ERRORS CZookeeperHelper::zookeeper_create(const std::string& path,
        const std::string& value, std::string* path_value,
        const ACL_vector& acl_entries, int flags)
{
    int errcode;
    int try_times = 2;
    const char* value_ptr = NULL;
    int buffer_length = m_default_buffer_length, value_length = -1;
    char buffer[buffer_length];
    memset(&buffer, 0, buffer_length);

    if (!value.empty())
    {
        value_ptr = value.c_str();
        value_length = value.size() + 1;
    }

    do
    {
        errcode = zoo_create(m_zk_handle, path.c_str(), value_ptr, value_length,
                &acl_entries, flags, buffer, buffer_length);

        if (ZOK == errcode)
        {
            *path_value = const_cast<char*>(buffer);
            break;
        }
        else if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_create(const std::string& path,
        const std::string& value, CAsyncCompletion* completion,
        const ACL_vector& acl_entries, int flags)
{
    int errcode;
    int try_times = 2;
    const char* value_ptr = NULL;
    int value_length = -1;

    if (!value.empty())
    {
        value_ptr = value.c_str();
        value_length = value.size() + 1;
    }

    do
    {
        errcode = zoo_acreate(m_zk_handle, path.c_str(), value_ptr,
                value_length, &acl_entries, flags, async_completion, completion);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_set(const std::string& path,
        const std::string& value, int version)
{
    int errcode;
    int try_times = 2;
    const char* value_ptr = NULL;
    int value_length = -1;

    if (!value.empty())
    {
        value_ptr = value.c_str();
        value_length = value.size() + 1;
    }

    do
    {
        errcode = zoo_set(m_zk_handle, path.c_str(), value_ptr, value_length,
                version);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_set(const std::string& path,
        const std::string& value, CAsyncCompletion* completion, int version)
{
    int errcode;
    int try_times = 2;
    const char* value_ptr = NULL;
    int value_length = -1;

    if (!value.empty())
    {
        value_ptr = value.c_str();
        value_length = value.size() + 1;
    }

    do
    {
        errcode = zoo_aset(m_zk_handle, path.c_str(), value_ptr, value_length,
                version, async_completion, completion);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_delete(const std::string& path, int version)
{
    int errcode;
    int try_times = 2;
    do
    {
        errcode = zoo_delete(m_zk_handle, path.c_str(), version);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_delete(const std::string& path,
        CAsyncCompletion* completion, int version)
{
    int errcode;
    int try_times = 2;
    do
    {
        errcode = zoo_adelete(m_zk_handle, path.c_str(), version,
                async_completion, completion);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_exists(const std::string& path,
        CWatcherAction* watcher_action, Stat* stat)
{
    int errcode;
    int try_times = 2;
    do
    {
        if (watcher_action == NULL)
        {
            errcode = zoo_exists(m_zk_handle, path.c_str(), 0, stat);
        }
        else
        {
            watcher_action->m_data = this;
            errcode = zoo_wexists(m_zk_handle, path.c_str(), active_watcher,
                    watcher_action, stat);
        }

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_exists(const std::string& path,
        CAsyncCompletion* completion, CWatcherAction* watcher_action)
{
    int errcode;
    int try_times = 2;
    do
    {
        if (watcher_action == NULL)
        {
            errcode = zoo_aexists(m_zk_handle, path.c_str(), 0, async_completion,
                    completion);
        }
        else
        {
            watcher_action->m_data = this;
            errcode = zoo_awexists(m_zk_handle, path.c_str(), active_watcher,
                    watcher_action, async_completion, completion);
        }

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_get(const std::string& path,
        std::string* value, CWatcherAction* watcher_action, Stat* stat)
{
    int errcode , buffer_length = m_default_buffer_length, try_times = 2;
    char buffer[buffer_length];
    memset(&buffer, 0, buffer_length);

    do
    {
        if (watcher_action == NULL)
        {
            errcode = zoo_get(m_zk_handle, path.c_str(), 0, buffer, &buffer_length,
                    stat);
        }
        else
        {
            watcher_action->m_data = this;
            errcode = zoo_wget(m_zk_handle, path.c_str(), active_watcher,
                    watcher_action, buffer, &buffer_length, stat);
        }

        if (ZOK == errcode)
        {
            *value = buffer;
            break;
        }
        else if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_get(const std::string& path,
        CAsyncCompletion* completion, CWatcherAction* watcher_action)
{
    int errcode, try_times = 2;

    do
    {
        if (watcher_action == NULL)
        {
            errcode = zoo_aget(m_zk_handle, path.c_str(), 0, async_completion,
                    completion);
        }
        else
        {
            watcher_action->m_data = this;
            errcode = zoo_awget(m_zk_handle, path.c_str(), active_watcher,
                    watcher_action, async_completion, completion);
        }

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_get_children(const std::string& path,
        std::vector<std::string>* childrens, CWatcherAction* watcher_action,
        Stat* stat)
{
    int errcode, try_times = 2;
    String_vector vec_strings;

    do
    {
        if (watcher_action == NULL)
        {
            errcode = zoo_get_children2(m_zk_handle, path.c_str(), 0, &vec_strings,
                    stat);
        }
        else
        {
            watcher_action->m_data = this;
            errcode = zoo_wget_children2(m_zk_handle, path.c_str(), active_watcher,
                    watcher_action, &vec_strings, stat);
        }

        if (ZOK == errcode)
        {
            for (int i = 0; i < vec_strings.count; i++)
            {
                const std::string& string = vec_strings.data[i];
                childrens->push_back(string);
            }

            /*
             * 需要客户端手动释放内存，否则会引起内存泄漏
             */
            deallocate_String_vector(&vec_strings);
            break;
        }
        else if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_get_children(const std::string& path,
        CAsyncCompletion* completion, CWatcherAction* watcher_action)
{
    int errcode, try_times = 2;

    do
    {
        if (watcher_action == NULL)
        {
            errcode = zoo_aget_children2(m_zk_handle, path.c_str(), 0,
                    async_completion, completion);
        }
        else
        {
            watcher_action->m_data = this;
            errcode = zoo_awget_children2(m_zk_handle, path.c_str(), active_watcher,
                    watcher_action, async_completion, completion);
        }

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_get_acl(const std::string& path,
        std::vector<ACL>* acls, Stat* stat)
{
    int errcode, try_times = 2;
    struct ACL_vector vec_acl;

    do
    {
        errcode = zoo_get_acl(m_zk_handle, path.c_str(), &vec_acl, stat);

        if (ZOK == errcode)
        {
            for (int i = 0; i < vec_acl.count; i++)
            {
                const ACL& acl = vec_acl.data[i];
                //acl.id.id;
                //acl.id.scheme;
                //acl.perms;
                acls->push_back(acl);
            }

            deallocate_ACL_vector(&vec_acl);
            break;
        }
        else if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_get_acl(const std::string& path,
        CAsyncCompletion* completion)
{
    int errcode, try_times = 2;
    do
    {
        errcode = zoo_aget_acl(m_zk_handle, path.c_str(), async_completion,
                completion);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_set_acl(const std::string& path,
        const ACL_vector& acls, int version)
{
    int errcode, try_times = 2;
    struct ACL_vector vec_acl;
    allocate_ACL_vector(&vec_acl, acls.count);

    for (int i = 0; i < acls.count; i++)
    {
        vec_acl.data[i] = acls.data[i];
    }

    do
    {
        errcode = zoo_set_acl(m_zk_handle, path.c_str(), version, &vec_acl);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    deallocate_ACL_vector(&vec_acl);

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_set_acl(const std::string& path,
        const ACL_vector& acls, CAsyncCompletion* completion,
        int version)
{
    int errcode, try_times = 2;
    struct ACL_vector vec_acl;
    allocate_ACL_vector(&vec_acl, acls.count);

    for (int i = 0; i < acls.count; i++)
    {
        vec_acl.data[i] = acls.data[i];
    }

    do
    {
        errcode = zoo_aset_acl(m_zk_handle, path.c_str(), version, &vec_acl,
                async_completion, completion);

        if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    deallocate_ACL_vector(&vec_acl);

    return (ZOO_ERRORS)errcode;
}

STOption CZookeeperHelper::zookeeper_create_op_init(const std::string& path,
        const std::string& value, const ACL_vector& acl_entries, int flags)
{
    STOption op;
    op.type = STOption::Z_ZOO_CREATE;
    op.create_op.path = path;
    op.create_op.data = value;
    op.create_op.acl = acl_entries;
    op.create_op.flags = flags;

    //zoo_create_op_init(&op, path, value, value_length, &acl_entries, flags, NULL, 0);
    return op;
}

STOption CZookeeperHelper::zookeeper_set_op_init(const std::string& path,
        const std::string& value, int version)
{
    STOption op;
    op.type = STOption::Z_ZOO_SET;
    op.set_op.path = path;
    op.set_op.data = value;
    op.set_op.version = version;

    //zoo_set_op_init(&op, path, value, value_length, version, NULL);
    return op;
}

STOption CZookeeperHelper::zookeeper_delete_op_init(const std::string& path,
        int version)
{
    STOption op;
    op.type = STOption::Z_ZOO_DELETE;
    op.delete_op.path = path;
    op.delete_op.version = version;

    //zoo_delete_op_init(&op, path, version);
    return op;
}

STOption CZookeeperHelper::zookeeper_check_op_init(const std::string& path,
        int version)
{
    STOption op;
    op.type = STOption::Z_ZOO_CHECK;
    op.check_op.path = path;
    op.check_op.version = version;

    //zoo_check_op_init(&op, path, version);
    return op;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_multi(const std::vector<STOption>& options,
        std::vector<STResult>& results)
{
    int errcode, try_times = 2;
    zoo_op_t ops_arrary[options.size()];
    zoo_op_result_t ops_results_array[options.size()];

    for (int i = 0; i < static_cast<int>(options.size()); i++)
    {
        zoo_op_t* z_op = &(ops_arrary[i]);
        const STOption* op = &(options[i]);

        switch (op->type)
        {
            case STOption::Z_ZOO_NOTIFY:
            {
                assert("options not defined!");
                break;
            }
            case STOption::Z_ZOO_CREATE:
            {
                const char* value_ptr = NULL;
                int value_length = -1;
                if (!op->create_op.data.empty())
                {
                    value_ptr = op->create_op.data.c_str();
                    value_length = strlen(value_ptr) + 1;
                }

                zoo_create_op_init(z_op, op->create_op.path.c_str(), value_ptr,
                        value_length, &(op->create_op.acl), op->create_op.flags,
                        new char[m_default_buffer_length], m_default_buffer_length);
                break;
            }
            case STOption::Z_ZOO_SET:
            {
                const char* value_ptr = NULL;
                int value_length = -1;
                if (!op->set_op.data.empty())
                {
                    value_ptr = op->create_op.data.c_str();
                    value_length = strlen(value_ptr) + 1;
                }

                zoo_set_op_init(z_op, op->set_op.path.c_str(), value_ptr,
                        value_length, op->set_op.version, new Stat);
                break;
            }
            case STOption::Z_ZOO_DELETE:
            {
                zoo_delete_op_init(z_op, op->set_op.path.c_str(),
                        op->set_op.version);
                break;
            }
            case STOption::Z_ZOO_CHECK:
            {
                zoo_check_op_init(z_op, op->set_op.path.c_str(),
                        op->set_op.version);
                break;
            }
        }
    }

    do
    {
        errcode = zoo_multi(m_zk_handle, static_cast<int>(options.size()),
                ops_arrary, ops_results_array);

        if (ZOK == errcode)
        {
            results.resize(options.size());
            for (int i = 0; i < static_cast<int>(options.size()); i++)
            {
                if (ops_arrary[i].type == ZOO_CREATE_OP)
                {
                    results[i].value = ops_results_array[i].value;
                }

                if (ops_arrary[i].type == ZOO_SETDATA_OP)
                {
                    results[i].stat = *(ops_results_array[i].stat);
                }

                results[i].error = ops_results_array[i].err;
            }
            break;
        }
        else if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    /* 释放内存 */
    for (int i = 0; i < static_cast<int>(options.size()); i++)
    {
        if (ops_arrary[i].type == ZOO_CREATE_OP)
            delete[] ops_arrary[i].create_op.buf;
        if (ops_arrary[i].type == ZOO_SETDATA_OP)
            delete ops_arrary[i].set_op.stat;
    }

    return (ZOO_ERRORS)errcode;
}

ZOO_ERRORS CZookeeperHelper::zookeeper_multi(const std::vector<STOption>& options,
        CAsyncCompletion* completion, std::vector<STResult>& results)
{
    int errcode, try_times = 2;
    zoo_op_t ops_arrary[options.size()];
    zoo_op_result_t ops_results_array[options.size()];

    for (int i = 0; i < static_cast<int>(options.size()); i++)
    {
        zoo_op_t* z_op = &(ops_arrary[i]);
        const STOption* op = &(options[i]);

        switch (op->type)
        {
            case STOption::Z_ZOO_NOTIFY:
            {
                assert("options not defined!");
                break;
            }
            case STOption::Z_ZOO_CREATE:
            {
                const char* value_ptr = NULL;
                int value_length = -1;
                if (!op->create_op.data.empty())
                {
                    value_ptr = op->create_op.data.c_str();
                    value_length = strlen(value_ptr) + 1;
                }

                zoo_create_op_init(z_op, op->create_op.path.c_str(), value_ptr,
                        value_length, &(op->create_op.acl), op->create_op.flags,
                        new char[m_default_buffer_length], m_default_buffer_length);
                break;
            }
            case STOption::Z_ZOO_SET:
            {
                const char* value_ptr = NULL;
                int value_length = -1;
                if (!op->set_op.data.empty())
                {
                    value_ptr = op->create_op.data.c_str();
                    value_length = strlen(value_ptr) + 1;
                }

                zoo_set_op_init(z_op, op->set_op.path.c_str(), value_ptr,
                        value_length, op->set_op.version, new Stat);
                break;
            }
            case STOption::Z_ZOO_DELETE:
            {
                zoo_delete_op_init(z_op, op->set_op.path.c_str(),
                    op->set_op.version);
                break;
            }
            case STOption::Z_ZOO_CHECK:
            {
                zoo_check_op_init(z_op, op->set_op.path.c_str(),
                        op->set_op.version);
                break;
            }
        }
    }

    do
    {
        errcode = zoo_amulti(m_zk_handle, static_cast<int>(options.size()),
                ops_arrary, ops_results_array, async_completion, completion);

        if (ZOK == errcode)
        {
            results.resize(options.size());
            for (int i = 0; i < static_cast<int>(options.size()); i++)
            {
                if (ops_arrary[i].type == ZOO_CREATE_OP)
                {
                    results[i].value = ops_results_array[i].value;
                }

                if (ops_arrary[i].type == ZOO_SETDATA_OP)
                {
                    results[i].stat = *(ops_results_array[i].stat);
                }

                results[i].error = ops_results_array[i].err;
            }
            break;
        }
        else if(ZSESSIONEXPIRED == errcode)
        {
            if(ZOK != reconncet())
                break;
            else
                try_times --;
        }
        else
        {
            break;
        }
    }
    while(try_times > 0);

    /* 释放内存 */
    for (int i = 0; i < static_cast<int>(options.size()); i++)
    {
        if (ops_arrary[i].type == ZOO_CREATE_OP)
            delete[] ops_arrary[i].create_op.buf;
        if (ops_arrary[i].type == ZOO_SETDATA_OP)
            delete ops_arrary[i].set_op.stat;
    }

    return (ZOO_ERRORS)errcode;
}

}



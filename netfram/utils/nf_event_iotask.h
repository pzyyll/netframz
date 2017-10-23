//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_IOTASK_H
#define NETFRAMZ_NF_EVENT_IOTASK_H

#include <functional>
#include <string>
#include "nf_event_config.h"

class EventService;

class IOTaskImpl;

class IOTask {
public:
    typedef std::function<void(EventService *, task_data_t, int)> handle_t;
    typedef IOTaskImpl impl_type;

public:
    IOTask(EventService &es, const int fd);

    IOTask(EventService &es, const int fd, const int mask,
           handle_t read_op = handle_t(),
           handle_t write_op = handle_t());

    ~IOTask();

    /*
     * @Brief prepare depression
     */
    void Bind(handle_t handle);

    int Start();

    int Restart();

    void Stop();

    void SetPrivateData(task_data_t data);

    task_data_t GetPrivateData();

    void SetMask(int mask);

    int GetMask();

    int GetFd();

    std::string GetErr();

    /*
     * @Brief new for add/remove mask
     */
    void AddMask(int mask);

    void RemoveMask(int mask);

    void SetReadHandle(handle_t read_op);

    void SetWriteHandle(handle_t write_op);

private:
    EventService &es_;
    impl_type *pimpl_;
};

#endif //NETFRAMZ_NF_EVENT_IOTASK_H

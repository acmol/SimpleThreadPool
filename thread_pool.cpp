//
// Desc: thread pool impl
// Author: zhangyuncong
// Date: 2018-06-04
//
#include "thread_pool.h"

namespace simple_thread_pool{

void ThreadPool::stop() {
    _io.stop();
    _thread_group.join_all();
    _stopped = true;
}

ThreadPool::~ThreadPool() {
    if (!_stopped) {
        stop();
    }
}

ThreadPool::ThreadPool(int num) : _work(_io) {
    for (int i = 0; i != num; ++i) {
        _thread_group.create_thread([&] {
            _io.run();
        });
    }
}

} // namespace simple_thread_pool
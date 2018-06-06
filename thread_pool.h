//
// Desc: thread pool impl
// Author: zhangyuncong
// Date: 2018-06-04
//
#ifndef SIMPLE_THREAD_POOL_THREAD_POOL_H
#define SIMPLE_THREAD_POOL_THREAD_POOL_H

#include <string>
#include <iostream>
#include <future>
#include <iterator>

#include "boost/asio.hpp"
#include "boost/thread.hpp"

namespace simple_thread_pool {

class ThreadPool {
public:
    ThreadPool(int number);

    template<typename InputIter, typename OutputIter, typename F>
    OutputIter transform(InputIter begin, InputIter end, OutputIter out, F f) {
        typedef std::future<decltype(f(*begin))> Future;
        std::vector<Future> futures;
        for (auto i = begin; i != end; ++i) {
            auto& num = *i;
            futures.emplace_back(this->post([&] {
                return f(num);
            }));
        }
        for (int i = 0; i != futures.size(); ++i) {
            *(out++) = futures[i].get();
        }
        return out;
    }

    template<typename InputIter, typename F>
    void for_each(InputIter begin, InputIter end, F f) {
        typedef std::future<void> Future;
        std::vector<Future> futures;
        for (auto i = begin; i != end; ++i) {
            auto& num = *i;
            futures.emplace_back(this->post([&]{
                f(num);
            }));
        }
        for (int i = 0; i != futures.size(); ++i) {
            futures[i].wait();
        }
    }

    template <typename FuncType>
    std::future<typename std::result_of<FuncType()>::type> post(FuncType&& func)
    {
        typedef typename std::result_of<FuncType()>::type return_type;
        typedef typename std::packaged_task<return_type()> task_type;
        // since post requires that the functions in it are copy-constructible,
        // we use a shared pointer for the packaged_task since it's only movable and non-copyable
        std::shared_ptr<task_type> task = std::make_shared<task_type>(std::move(func));
        std::future<return_type> returned_future = task->get_future();
        _io.post(std::bind(&task_type::operator(),task));
        return returned_future;
    }

    void stop();

    ~ThreadPool();

private:
    boost::thread_group _thread_group;
    boost::asio::io_service _io;
    boost::asio::io_service::work _work;
    bool _stopped = false;
};

} // namespace simple_thread_pool

#endif //SIMPLE_THREAD_POOL_THREAD_POOL_H
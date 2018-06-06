// Copyright 2018, Baidu Inc. All rights reserved.
// Author: Zhang Yuncong

#include "thread_pool.h"

#include <thread>
#include <algorithm>
#include <numeric>
#include "boost/lexical_cast.hpp"

// #include "gtest/gtest.h"

#include <cassert>

#define TEST(a, b) void b()
#define ASSERT_EQ(a, b) assert(a == b)

namespace simple_thread_pool {

TEST(ThreadPoolTestSuite, test_post_future) {
    ThreadPool pool(5);
    int n = 1;
    std::future<int> r1 = pool.post([&]{
        return n;
    });
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    std::future<int> r2 = pool.post([&] {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        return n;
    });
    n = 2;
    ASSERT_EQ(1, r1.get());
    ASSERT_EQ(2, r2.get());
}

TEST(ThreadPoolTestSuite, test_transform) {
    ThreadPool pool(5);
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<std::string> expect;
    std::vector<std::string> real;
    auto to_str = [](int n) -> std::string { return boost::lexical_cast<std::string>(n); };
    std::transform(nums.begin(), nums.end(), std::back_inserter(expect), to_str);
    pool.transform(nums.begin(), nums.end(), std::back_inserter(real), to_str);
    ASSERT_EQ(expect, real);
}

TEST(ThreadPoolTestSuite, test_for_each) {
    ThreadPool pool(5);
    std::vector<int> expect = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> real = {1, 2, 3, 4, 5, 6, 7, 8};
    auto incr = [](int& input) {
        ++input;
    };
    std::for_each(expect.begin(), expect.end(), incr);
    pool.for_each(real.begin(), real.end(), incr);
    ASSERT_EQ(expect, real);
}

TEST(ThreadPoolTestSuite, test_nested_pools) {
    ThreadPool pool1(2);
    ThreadPool pool2(10);
    std::vector<int> nums1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> nums2 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> expect;
    std::vector<int> real;
    auto product_num2_and_accumulate_by_std = [&](int n) {
        std::vector<int> tmp;
        std::transform(nums2.begin(), nums2.end(), std::back_inserter(tmp), [&](int m) {
            return m * n;
        });
        return std::accumulate(tmp.begin(), tmp.end(), 0);
    };
    std::transform(nums1.begin(), nums1.end(), std::back_inserter(expect),
                   product_num2_and_accumulate_by_std);
    auto product_num2_and_accumulate_by_pool = [&](int n) {
        std::vector<int> tmp;
        pool2.transform(nums2.begin(), nums2.end(), std::back_inserter(tmp), [&](int m) {
            return m * n;
        });
        return std::accumulate(tmp.begin(), tmp.end(), 0);
    };
    pool1.transform(nums1.begin(), nums1.end(), std::back_inserter(real),
                    product_num2_and_accumulate_by_pool);
    ASSERT_EQ(expect, real);
}

} //namespace simple_thread_pool

int main(int argc, char **argv)
{
	simple_thread_pool::test_post_future();
	simple_thread_pool::test_transform();
	simple_thread_pool::test_for_each();
	simple_thread_pool::test_nested_pools();
	
	simple_thread_pool::ThreadPool pool(4);

	std::vector<int> nums = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	pool.for_each(nums.begin(), nums.end(), [](int n) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << n << std::endl;
	});
}

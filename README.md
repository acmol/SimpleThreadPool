# SimpleThreadPool

A Simple C++ thread pool implement.

a paralleled transform & for_each algorithm is provided

	ThreadPool pool(4);
	std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};

	// output nums
	pool.for_each(nums.begin(), nums.end(), [](int n) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << n << std::endl;
	});

    // convert nums to string
    auto to_str = [](int n) -> std::string { return boost::lexical_cast<std::string>(n); };
    pool.transform(nums.begin(), nums.end(), std::back_inserter(real), to_str);


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
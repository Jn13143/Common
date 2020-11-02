#include<functional>
#include<chrono>
#include<thread>
#include<atomic>
#include<memory>
#include<mutex>
#include<iostream>
#include<string>
#include<memory>
#include<condition_variable>

using namespace std;
class Timer
{
public:
	Timer() :expired_(true), try_to_expire_(false)
	{

	}

	Timer(const Timer& t) {
		expired_ = t.expired_.load();
		try_to_expire_ = t.try_to_expire_.load();
	}

	~Timer() {
		Expire();
		std::cout << "timer destructed!" << std::endl;
	}

	void StartTimer(int interval, std::function<void()> task) {
		if (expired_ == false)
		{
			std::cout << "timer is currently running, please expire it first..." << std::endl;
			return;
		}

		expired_ = false;
		std::thread([this, interval, task]() {
			while (!try_to_expire_) {
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
				task();
			}

			std::cout << "stop task..." << std::endl;
			{
				std::lock_guard<std::mutex> locker(mutex_);
				expired_ = true;
				expired_cond_.notify_one();
			}

		}).detach();
	}

	void Expire() {
		if (expired_) {
			return;
		}
		if (try_to_expire_) {
			std::cout << "timer is trying to expire, please wait..." << std::endl;
			return;
		}

		try_to_expire_ = true;
		{
			std::unique_lock<std::mutex> locker(mutex_);
			expired_cond_.wait(locker, [this] {return expired_ == true; });
			if (expired_ == true) {
				std::cout << "timer expired!" << std::endl;
				try_to_expire_ = false;
			}
		}

	}

	template<typename callable, class... arguments>
	void SyncWait(int after, callable&& f, arguments&&... args) {
		std::function<typename std::result_of<callable(arguments...)>::type()> task
		(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
		std::this_thread::sleep_for(std::chrono::milliseconds(after));
		task();
	}

	template<typename callable, class... arguments>
	void AsyncWait(int after, callable&& f, arguments&&... args) {
		std::function<typename std::result_of<callable(arguments...)>::type()> task
		(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
		std::thread([after, task]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(after));
			task();
		}).detach();
	}

private:
	std::atomic<bool> expired_;
	std::atomic<bool> try_to_expire_;
	std::mutex mutex_;
	std::condition_variable expired_cond_;
};



void EchoFunc(std::string&& s) {

	std::cout << "test : " << s << endl;

}

int main()
{
	Timer t;

	//t.StartTimer(1000, std::bind(EchoFunc, "hello world!"));
	//std::this_thread::sleep_for(std::chrono::seconds(4));
	//std::cout << "try to expire timer!" << std::endl;
	//t.Expire();

	//while (true)
	//{
	//	//t.StartTimer(1000, std::bind(EchoFunc, "hello world!"));
	//	t.StartTimer(1000, std::bind(Add, 1, 3));
	//}

	//ִֻ��һ�ζ�ʱ����

	//ͬ��

	//t.SyncWait(3000, EchoFunc, "hello world!");
	//int a = 3;

	//�첽
	t.AsyncWait(3000, EchoFunc, "hello c++11!");
	int a = 3;
	std::this_thread::sleep_for(std::chrono::seconds(4));

	return 0;
}
#ifndef __THREADPOOL__H__
#define __THREADPOOL__H__

#include<iostream>
#include<thread>
#include<string>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<vector>
#include<functional>

class ThreadPool//线程池类
{
private:
    std::vector<std::thread> threads;//线程数组
	std::queue<std::function<void()>> tasks;//任务队列
	std::mutex mtx;//互斥锁
	std::condition_variable c_v;//通知
	bool Isstop;//是否关闭线程池
	ThreadPool(const ThreadPool& tdpl) = delete;
	ThreadPool& operator=(const ThreadPool& tdpl) = delete;
private:
    ThreadPool(int numthreads)//创建并初始化线程池，numthreads线程池的线程数量
            :Isstop(false)
        {
            for (int i = 0;i < numthreads;i++)
            {
                threads.emplace_back([this] {
                    while (1)//一直等待任务并完成任务
                    {
                        std::unique_lock<std::mutex> lg(mtx);
                        c_v.wait(lg, [this] {return !tasks.empty() || Isstop;});
                        if (Isstop && tasks.empty())return;
                        std::function<void()> task(std::move(tasks.front()));
                        tasks.pop();
                        lg.unlock();
                        task();
                    }
                    });
            }
        }

	~ThreadPool()//销毁线程池
	{
		{
			std::unique_lock<std::mutex> lg(mtx);
			Isstop = true;	
		}
		c_v.notify_all();
		for (auto& t:threads)
		{
			t.join();
		}
	}
public:
    static ThreadPool& GetInstance()//创建线程池类
        {
            static ThreadPool tdpl(10);
            return tdpl;
        }
    template<class T, class... Args>
    void addqueue(T &&f,Args&&... args)//添加任务到任务队列
	{
		std::function<void()> tk = std::bind(std::forward<T>(f), std::forward<Args>(args)...);
		{
			std::unique_lock<std::mutex> lg(mtx);
			tasks.emplace(std::move(tk));
		}
		c_v.notify_one();
	}

};


#endif
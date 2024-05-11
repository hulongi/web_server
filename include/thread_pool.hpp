#pragma once
#include<iostream>
#include<thread>
#include<memory>
#include<mutex>
#include<string>
#include<condition_variable>
#include<queue>
#include<functional>
#include<vector>
#include "log.h"
using namespace std;
#define MIN_WAIT_TASK 20
#define DEFAULT_THREAD_VARY 10

struct threadpool
{
	mutex lock;      //锁本结构体
	mutex counter;   //忙记录数锁
	mutex mtx;		//count锁
	mutex task_lock;  //任务队列锁

	condition_variable que;		//任务队列条件变量
	condition_variable queue_not_full;
	condition_variable queue_not_empty;
	
	vector<thread> threads_vec;
	thread adjust;
	queue<function<void()>> tasks;

	int min_num;
	int max_num;
	int live_num;
	unsigned int busy_num;
	int count;
	int wait_exit_num;
	int queue_max_size;
	bool shutdown;
};
inline void thread_task(threadpool* pool)
{
	
	while (true)
	{
		
			unique_lock<mutex> lock_emp(pool->lock);
			if (pool->tasks.empty() && !pool->shutdown)//pool->tasks.empty() && !pool->shutdown
			{
				cout << this_thread::get_id() << "\t is waiting" << endl;
				//pool->queue_not_full.notify_one();
				//unique_lock<mutex> lock_emp(pool->lock);
				pool->queue_not_empty.wait(lock_emp, [pool] {
					return !pool->tasks.empty() || pool->shutdown;
					});
			}
			cout << "线程唤醒" << endl;
			LOG("线程唤醒\n");
			if (pool->shutdown && pool->tasks.empty())
				return;
			function<void()>task1(move(pool->tasks.front()));
			pool->tasks.pop();
			lock_emp.unlock();

			pool->queue_not_full.notify_one();

			{
				unique_lock<mutex> lock_counter(pool->counter);//忙线程加一
				pool->busy_num++;
			}
			cout << "执行任务" << endl;
			LOG("执行任务\n");
			task1();	//执行任务

			{
				unique_lock<mutex> lock_counter(pool->counter);//忙线程减一
				pool->busy_num--;
			}
			//pool->queue_not_full.notify_one();
			
		
	}

}

inline void manager(threadpool* pool)
{
	cout << "创建管理线程" << endl;
	LOG("创建管理线程\n");
	int i = 0;
	while (!pool->shutdown)
	{
        this_thread::sleep_for(chrono::milliseconds(10000));
		//Sleep(10000);
		cout << "管理者线程开始管理" << endl;
		LOG("管理者线程开始管理\n");
		pool->lock.lock();
		int queue_size = pool->tasks.size();
		int live_num = pool->live_num;
		pool->lock.unlock();

		pool->counter.lock();
		int busy_num = pool->busy_num;
		pool->counter.unlock();

		//扩容
		pool->lock.lock();
		if (queue_size > pool->min_num && live_num < pool->max_num)
		{
			cout << "开始扩容" << endl;
			LOG("开始扩容\n");
			int add = 0;

			for (i = 0; i < pool->max_num && add < DEFAULT_THREAD_VARY && pool->live_num < pool->max_num;i++)
			{
					pool->threads_vec.emplace_back(move(thread(thread_task, pool)));
					add++;
					pool->live_num++;
			}
		}
		pool->lock.unlock();

		//销毁多余线程
		pool->lock.lock();
		if ((pool->busy_num * 2) < live_num && live_num > pool->min_num)
		{
			cout << "开始销毁" << endl;
			LOG("开始销毁\n");
			int size = pool->threads_vec.size();
			for (i = size - 1; i > 0; i--)
			{
				pool->threads_vec[i].join();
				pool->live_num--;
			}
			for (i = size - 1; i > 0; i--)
			{
				pool->threads_vec.pop_back();
			}
		}
		pool->lock.unlock();
	}
}
class thread_pool
{
private:
	threadpool *pool;
public:
	inline thread_pool(int min_num,int max_num,int queue_size);
	inline ~thread_pool();
	int getcount()
	{
		return pool->count;
	}
	
	thread_pool* get_ptr()
	{
		return this;
	}
	template<class F,class... Args>
	void enqueue(F&& f, Args&&... args)
	{
		function<void()>task = bind(forward<F>(f), forward<Args>(args)...);
		threadpool *p = pool;
		{
			unique_lock<mutex> lock_pool(p->lock);
			//p->counter.lock();
			/*unsigned int busy_num = p->busy_num;
			p->counter.unlock();
			unsigned int size = p->threads_vec.size();*/
			//p->task_lock.lock();
			//int crrunt_queue_size = pool->tasks.size();
			////p->task_lock.unlock();

			//int queue_max_size = pool->queue_max_size;
			p->queue_not_full.wait(lock_pool, [p](){
				return (p->tasks.size()<p->queue_max_size);
				});

			pool->tasks.emplace(move(task));
			lock_pool.unlock();
		}
		pool->queue_not_empty.notify_one();
		cout << "创建一个任务" << endl;
		LOG("创建一个任务");
		p->mtx.lock();
		p->count++;
		p->mtx.unlock();
		//Sleep(1000);
		
	}

};
inline thread_pool::thread_pool(int min_num, int max_num, int queue_size)
{
	
	pool = new threadpool;
	pool->count = 0;
	pool->min_num = min_num;
	pool->max_num = max_num;
	pool->busy_num = 0;
	pool->live_num = min_num;
	pool->wait_exit_num = 0;
	pool->queue_max_size = queue_size;
	pool->shutdown = false;

	pool->threads_vec.reserve(max_num);

	unique_lock<mutex> lock;
	pool->adjust = move(thread(manager, pool));	//创建管理线程

	for (int i = 0; i < min_num; i++)		//创建任务线程
	{
		//pool->threads_vec[i] = move(thread(thread_task, pool));
		pool->threads_vec.emplace_back(move(thread(thread_task, pool)));
		//cout << pool->threads_vec[i].get_id() << endl;
		++pool->count;
	}
	//pool->lock.unlock();

}
inline thread_pool::~thread_pool()
{
	pool->lock.lock();
	pool->shutdown = true;
	for (auto& t : pool->threads_vec)
	{
		t.join();
	}
	pool->lock.unlock();
	delete pool;
}

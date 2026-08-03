/********************************************************************
filename: 	ThreadSafeQueue
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\Util\Concurrent\ThreadSafeQueue.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __THREAD_SAFE_QUEUE_H__
#define __THREAD_SAFE_QUEUE_H__

#include <queue>  
#include <memory>  
#include <mutex>  
#include <condition_variable>  

#define  TIME_INFINITE 0XFFFFFFFF

template<typename T>  
class ThreadSafeQueue
{  
private:  
	std::mutex m_mut;   
	std::queue<T> m_dataQueue;  
	std::condition_variable m_dataCond;  

public:
	void swapContent(ThreadSafeQueue & other)
	{
		std::lock(m_mut, other.m_mut);
		std::lock_guard<decltype(m_mut)> lock1(m_mut, std::adopt_lock);
		std::lock_guard<decltype(other.m_mut)> lock2(other.m_mut, std::adopt_lock);
		using std::swap;
		swap(m_dataQueue, other.m_dataQueue);
		if (!m_dataQueue.empty())
		{
			m_dataCond.notify_all();
		}
		if (!other.m_dataQueue.empty())
		{
			other.m_dataCond.notify_all();
		}
	}

	void push(const T& new_value) 
	{  
		std::lock_guard<std::mutex> lk(m_mut);  
		m_dataQueue.push(new_value);  
		m_dataCond.notify_one();  
	}  

	void pushWithoutNotify(const T& new_value)
	{
		std::lock_guard<std::mutex> lk(m_mut);
		m_dataQueue.push(new_value);
	}

	bool wait_and_pop(T& value, uint64_t time)  
	{  
		std::unique_lock<std::mutex> lk(m_mut); 

		if(time == TIME_INFINITE)
			m_dataCond.wait(lk,[this]{return !m_dataQueue.empty();}); 
		else
			m_dataCond.wait_for(lk, std::chrono::milliseconds(time), [this]{return !m_dataQueue.empty();});

		if(m_dataQueue.empty())
			return false;

		value = m_dataQueue.front();
		m_dataQueue.pop();
		return true;
	}  

	bool try_pop(T& value) 
	{  
		std::lock_guard<std::mutex> lk(m_mut);  
		if(m_dataQueue.empty())  
			return false;  

		value = m_dataQueue.front();
		m_dataQueue.pop();
		 
		return true;  
	}

	size_t size()
	{
		std::lock_guard<std::mutex> lk(m_mut);
		return m_dataQueue.size();
	}

	bool empty()  
	{  
		std::lock_guard<std::mutex> lk(m_mut);  
		return m_dataQueue.empty();  
	}  
};  
#endif
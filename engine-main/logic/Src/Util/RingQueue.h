/********************************************************************
filename: 	RingQueue.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-1-10
*********************************************************************/
#ifndef __RING_QUEUE_HEADER__
#define __RING_QUEUE_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

template<typename T, int MAX_COUNT = 32>
class RingQueue
{
public:
	RingQueue();
	~RingQueue();

	void	push_back(const T& elem);
	T&		push_back_fast();
	T&		push_back_begin(const int offset = 0);
	void	push_back_end(const int count = 1);

	void	pop_front(T& elem);
	T&		peek_front();
	void	drop(const int count = 1);

	int		used() const;
	int		free() const;
	int		capacity() const { return MAX_COUNT; }

private:
	T		m_queue[MAX_COUNT];
	int		m_head;
	int		m_tail;
};

template<typename T, int MAX_COUNT>
RingQueue<T, MAX_COUNT>::RingQueue()
{
	m_head = 0;
	m_tail = 0;
}

template<typename T, int MAX_COUNT>
RingQueue<T, MAX_COUNT>::~RingQueue()
{
}

template<typename T, int MAX_COUNT>
void RingQueue<T, MAX_COUNT>::push_back(const T& elem)
{
	if (free() <= 0)
	{
		assert(false);
		return;
	}
	int tail = m_tail;

	m_queue[tail] = elem;

	//注意对m_tail的修改必须是原子的，这样才能保证线程安全
	tail++;
	if (tail >= MAX_COUNT)
	{
		tail = tail % MAX_COUNT;
	}
	m_tail = tail;
}

template<typename T, int MAX_COUNT>
T& RingQueue<T, MAX_COUNT>::push_back_fast()
{
	T& r = push_back_begin();
	push_back_end();
	return r;
}


template<typename T, int MAX_COUNT>
T& RingQueue<T, MAX_COUNT>::push_back_begin(const int offset)
{
	if (free() <= offset)
	{
		assert(false);
		throw 1;
	}
	int tail = m_tail + offset;
	if (tail >= MAX_COUNT)
		tail = tail % MAX_COUNT;
	return m_queue[tail];
}

template<typename T, int MAX_COUNT>
void RingQueue<T, MAX_COUNT>::push_back_end(int count)
{
	//注意对m_tail的修改必须是原子的，这样才能保证线程安全
	int tail = m_tail;
	tail += count;
	if (tail >= MAX_COUNT)
	{
		tail = tail % MAX_COUNT;
	}
	m_tail = tail;
}


template<typename T, int MAX_COUNT>
void RingQueue<T, MAX_COUNT>::pop_front(T& elem)
{
	if (used() <= 0)
	{
		assert(false);
		return;
	}
	int head = m_head;

	elem = m_queue[head];

	head++;
	if (head >= MAX_COUNT)
	{
		head = head % MAX_COUNT;
	}

	m_head = head;
}

template<typename T, int MAX_COUNT>
T& RingQueue<T, MAX_COUNT>::peek_front()
{
	if (used() <= 0)
	{
		assert(false);
		static T empty;
		return empty;
	}
	return m_queue[m_head];
}

template<typename T, int MAX_COUNT>
void RingQueue<T, MAX_COUNT>::drop(const int count)
{
	if (used() <= 0)
	{
		assert(false);
		return;
	}
	int head = m_head;

	head += count;
	if (head >= MAX_COUNT)
	{
		head = head % MAX_COUNT;
	}

	m_head = head;
}


template<typename T, int MAX_COUNT>
int RingQueue<T, MAX_COUNT>::used() const
{
	const int currentHead = m_head;
	const int currentTail = m_tail;
	int usedLength = 0;
	if (currentHead > currentTail) // is wrapped
	{
		usedLength = (MAX_COUNT - currentHead) + currentTail;
	}
	else
	{
		usedLength = currentTail - currentHead;
	}
	return usedLength;
}

template<typename T, int MAX_COUNT>
int RingQueue<T, MAX_COUNT>::free() const
{
	return MAX_COUNT - used() - 1;
}

}

#endif
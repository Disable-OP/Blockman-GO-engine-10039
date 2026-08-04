/********************************************************************
filename: 	SafeAsyncCall
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\Util\SafeAsyncCall.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/

#ifndef __SafeAsyncCall_H__
#define __SafeAsyncCall_H__
#include "Util/Rwq/readerwriterqueue.h"
#include "Util/Concurrent/ThreadSafeQueue.h"
#include "TypeDef.h"

class SPSCAsyncCall
{
public:
	SPSCAsyncCall(){};
	~SPSCAsyncCall(){};

	void noblockCall(std::function<void(void)> fuc);

	bool realCall();

private:
	moodycamel::ReaderWriterQueue<std::function<void(void)>> m_callList;
};

class SafeAsyncCall
{
public:
	SafeAsyncCall() {};
	~SafeAsyncCall() {};

	void noblockCall(std::function<void(void)> fuc);

	bool realCall();

private:
	ThreadSafeQueue<std::function<void(void)>> m_callList;
};


inline void SafeAsyncCall::noblockCall(std::function<void(void)> fuc)
{
	auto doFuc = [fuc]()
	{
		fuc();
	};

	m_callList.pushWithoutNotify(doFuc);
}

inline bool SafeAsyncCall::realCall()
{
	std::function<void(void)> fuc;
	bool has = m_callList.try_pop(fuc);
	if (has)
	{
		fuc();
	}

	return has;
}


inline void SPSCAsyncCall::noblockCall(std::function<void(void)> fuc)
{
	auto doFuc = [fuc]()
	{
		fuc();
	};

	m_callList.enqueue(doFuc);
}

inline bool SPSCAsyncCall::realCall()
{
	std::function<void(void)> fuc;
	bool has = m_callList.try_dequeue(fuc);
	if (has)
	{
		fuc();
	}

	return has;
}

#endif
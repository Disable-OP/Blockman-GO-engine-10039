#ifndef __FIFO_CACHE_HEADER__
#define __FIFO_CACHE_HEADER__

#include <stdexcept>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	template<typename Key, typename Value>
	class FIFOCache
	{
	private:
		struct CacheEntry;
		using Index = typename map<Key, CacheEntry*>::type;
		using Queue = typename list<CacheEntry>::type;
		struct CacheEntry
		{
			typename Index::iterator indexIter;
			typename Queue::iterator queueIter;
			Value data;
			CacheEntry() : data() {}
			CacheEntry(const Value& data) : data(data) {}
			CacheEntry(Value&& data) : data(std::move(data)) {}
			CacheEntry(const CacheEntry&) = default;
			CacheEntry(CacheEntry&&) = default;
		};

		size_t m_capacity;
		Index m_index;
		Queue m_queue;

		void pop()
		{
			auto& entry = m_queue.front();
			m_index.erase(entry.indexIter);
			m_queue.pop_front();
		}

	public:
		FIFOCache(size_t capacity) : m_capacity(capacity) {}

		void put(const Key& key, Value&& value)
		{
			put(key, value);
		}
		void put(const Key& key, Value& value)
		{
			if (m_queue.size() >= m_capacity)
			{
				pop();
			}
			m_queue.push_back(CacheEntry(std::move(value)));
			auto& newEntry = m_queue.back();
			m_index[key] = &newEntry;
			newEntry.indexIter = m_index.find(key);
			newEntry.queueIter = --m_queue.end();
		}

		Value& at(const Key& key)
		{
			auto iter = m_index.find(key);
			if (iter == m_index.end())
			{
				throw std::out_of_range("not in cache");
			}
			return iter->second->data;
		}

		Value& operator[](const Key& key)
		{
			auto iter = m_index.find(key);
			if (iter == m_index.end())
			{
				put(key, {});
				return at(key);
			}
			else
			{
				return iter->second->data;
			}
		}

		void erase(const Key& key)
		{
			auto iter = m_index.find(key);
			if (iter == m_index.end())
			{
				return;
			}
			auto& entry = *(iter->second);
			m_index.erase(entry.indexIter);
			m_queue.erase(entry.queueIter);
		}

		size_t size()
		{
			return m_queue.size();
		}

		bool empty()
		{
			return m_queue.empty();
		}

		void clear()
		{
			m_index.clear();
			m_queue.clear();
		}

		bool inCache(const Key& key)
		{
			return m_index.find(key) != m_index.end();
		}
	};
}

#endif // !__FIFO_CACHE_HEADER__

#ifndef __LRU_CACHE_HEADER__
#define __LRU_CACHE_HEADER__

#include <stdexcept>
#include <mutex>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	template<typename Key, typename Value>
	class LRUCache
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
		std::mutex m_mutex;

		void pop()
		{
			auto& entry = m_queue.front();
			m_index.erase(entry.indexIter);
			m_queue.pop_front();
		}
		void repush(CacheEntry&& entry)
		{
			repush(entry);
		}
		void repush(CacheEntry& entry)
		{
			m_queue.push_back(std::move(entry));
			auto& newEntry = m_queue.back();
			m_queue.erase(newEntry.queueIter);
			newEntry.indexIter->second = &newEntry;
			newEntry.queueIter = --m_queue.end();
		}
		Value& at(const Key& key)
		{
			auto iter = m_index.find(key);
			if (iter == m_index.end())
			{
				throw std::out_of_range("not in cache");
			}
			repush(*(iter->second));
			return iter->second->data;
		}
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
	public:
		LRUCache(size_t capacity) : m_capacity(capacity) {}

		Value& operator[](const Key& key)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			auto iter = m_index.find(key);
			if (iter == m_index.end())
			{
				put(key, {});
				return at(key);
			}
			else
			{
				repush(*(iter->second));
				return iter->second->data;
			}
		}

		void erase(const Key& key)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

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
			std::unique_lock<std::mutex> lock(m_mutex);

			return m_queue.size();
		}

		bool empty()
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			return m_queue.empty();
		}

		void clear()
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_index.clear();
			m_queue.clear();
		}

		bool inCache(const Key& key)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			return m_index.find(key) != m_index.end();
		}
	};
}

#endif // !__LRU_CACHE_HEADER__

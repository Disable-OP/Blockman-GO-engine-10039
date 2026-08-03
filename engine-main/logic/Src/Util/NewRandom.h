#pragma once
#include "Core.h"
#include <random>
namespace BLOCKMAN
{
	struct RandomCreater
	{
		std::mt19937 *mt = nullptr;
		int useCount = 0;
		long seed = 0;
		
		 long randomSeed()
		{
			std::random_device rd;
			return rd();
		}

		void init()
		{
			if (seed == 0 || useCount > 10000)
			{
				seed = randomSeed();
				useCount = 0;
				if (mt != nullptr)
					delete mt;

				mt = new std::mt19937(seed);
			}
		}

		int random(int begin, int end)
		{
			LordAssert(begin >= 0 && end > 0);

			int rt = (*mt)() % (end - begin);
			rt = rt + begin;

			return rt;
		}
	};

	class NewRandom : public Singleton<NewRandom>, public ObjectAlloc
	{
	public:
		NewRandom()
		{
			m_defaultCreater = std::make_shared<RandomCreater>();
			m_defaultCreater->init();
		}
		int random(int begin, int end, LORD::String createName = "" )
		{
			std::shared_ptr<RandomCreater> useCreater = m_defaultCreater;
			if (!createName.empty())
			{
				if (m_allRandomCreater.find(createName) == m_allRandomCreater.end())
				{
					m_allRandomCreater[createName] = std::make_shared<RandomCreater>();
				}

				useCreater = m_allRandomCreater[createName];
			}

			useCreater->init();
			useCreater->useCount++;

			return useCreater->random(begin, end);
		}

	private:
		std::map<LORD::String, std::shared_ptr<RandomCreater>> m_allRandomCreater;
		std::shared_ptr<RandomCreater> m_defaultCreater;
	};
}
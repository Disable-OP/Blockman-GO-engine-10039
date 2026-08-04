#ifndef __SIGN_TEXTS_CHANGE_BUFFER_HEADER__
#define __SIGN_TEXTS_CHANGE_BUFFER_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class Chunk;
	using ChunkPtr = std::shared_ptr<Chunk>;

	class SignTextsChangeRecorder : public Singleton<SignTextsChangeRecorder>, public ObjectAlloc
	{
	private:
		using BlockPos = Vector3i;
		using Texts = std::array<String, 4>;
		using Records = Vector3iMap<Texts>;

		Records m_changeRecords;

	public:
		void add(const BlockPos& position, const Texts& texts)
		{
			m_changeRecords[position] = texts;
		}

		void applyChanges(const ChunkPtr& chunk) const;
	};
}

#endif // !__SIGN_TEXTS_CHANGE_BUFFER_HEADER__

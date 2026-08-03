#pragma once
#include "Entity/EntityActorCannon.h"
#include "World/World.h"

namespace BLOCKMAN
{
	class EntityActorCannonClient : public EntityActorCannon
	{
		RTTI_DECLARE(EntityActorCannonClient);

	public:
		bool m_hasBeenLaunched = false;

	private:
		bool m_isShowCannonBtn = false;

	public:
		EntityActorCannonClient(World* pWorld, const Vector3& beginPos, const Vector3& fallOnPos);
		~EntityActorCannonClient();

		void onUpdate() override;
	};
}
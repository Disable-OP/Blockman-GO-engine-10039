#ifndef __ENTITY_MERCHANT_HEADER__
#define __ENTITY_MERCHANT_HEADER__

#include "Entity.h"
#include "World/World.h"
#include "Network/protocol/PacketDefine.h"
#include "Util/AboutShopping.h"

namespace BLOCKMAN
{
	class EntityMerchant : public Entity
	{
		RTTI_DECLARE(EntityMerchant);
	private:
		String m_name;
		String m_nameLang = "default.merchant.name";
		vector<CommodityGroup>::type m_commodities;
		bool m_hasUpdated = false;

	public:
		EntityMerchant(World* pWorld) : Entity(pWorld) 
		{
			preventEntitySpawning = true;
		}
		EntityMerchant(World* pWorld, const Vector3& pos) : EntityMerchant(pWorld)
		{
			setPosition(pos);
		}

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		bool canBeCollidedWith() { return true; }

		void setNameLang(const String& nameLang)
		{
			m_nameLang = nameLang;
		}

		String getNameLang() const
		{
			return m_nameLang;
		}

		String getName() const
		{
			return m_name;
		}

		void setName(const String& name)
		{
			m_name = name;
		}

		const CommodityGroup& getGroup(size_t index) const
		{
			return m_commodities[index];
		}

		const vector<CommodityGroup>::type& getCommodities() const
		{
			return m_commodities;
		}

		virtual void resetCommodities(const vector<CommodityGroup>::type& commodities)
		{
			m_commodities = commodities;
		}
		virtual void clearCommodities();
		void onUpdate() override;

		bool isImmuneToExplosions() { return true; }

		bool getHasUpdated() { return m_hasUpdated; }
		void setHasUpdated(bool hasUpdated) { m_hasUpdated = hasUpdated; }
	};
}

#endif // !__ENTITY_MERCHANT_HEADER__

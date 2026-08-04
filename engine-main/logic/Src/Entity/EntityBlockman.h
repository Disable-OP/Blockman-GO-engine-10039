#ifndef __ENTITY_BLOCKMAN_HEADER__
#define __ENTITY_BLOCKMAN_HEADER__

#include "Entity.h"
#include "World/World.h"

namespace BLOCKMAN
{
	class DamageSource;

	enum class EntityBlockmanType {
		BLOCKMAN_NORMAL,
		BLOCKMAN_EMPTY
	};

	class EntityBlockman : public Entity
	{
		RTTI_DECLARE(EntityBlockman);

	private:
		 int matrix[10][2][3]  = 
		 { { { 0, 0, -1 }, { 0, 0, 1 } },
		 { { -1, 0, 0 }, { 1, 0, 0 } },
		 { { -1, -1, 0 }, { 1, 0, 0 } },
		 { { -1, 0, 0 }, { 1, -1, 0 } },
		 { { 0, 0, -1 }, { 0, -1, 1 } },
		 { { 0, -1, -1 }, { 0, 0, 1 } },
		 { { 0, 0, 1 }, { 1, 0, 0 } },
		 { { 0, 0, 1 }, { -1, 0, 0 } },
		 { { 0, 0, -1 }, { -1, 0, 0 } },
		 { { 0, 0, -1 }, { 1, 0, 0 } } };
	
	protected:
		float damage = 0.0f;
		int rollingAmplitude = 0;
		int rollingDirection = 1;
		int displayTile = 0;
		int displayTileData = 0;
		int displayTileOffset = 6;
		bool isHasDisplayTile = false;
		bool isInReverse = false;
		int turnProgress = 0;
		float blockmanYaw = 0.f;
		float blockmanPitch = 0.f;
		float currentMaxSpeed = 0.4f;

		EntityBlockmanType blockmanType = EntityBlockmanType::BLOCKMAN_NORMAL;
		Vector3 blockmanPos = Vector3::ZERO;
		Vector3 velocity = Vector3::ZERO;
		String entityName = "";

	public:
		bool m_isActorChange = true;

	private:
		String m_actorName = "rank.actor";
		String m_nameLang = "";
		String m_actorBody = "";
		String m_actorBodyId = "";

	protected:
		bool inPortal = false;
		int portalCounter = 0;

	public:
		EntityBlockman(World* pWorld) : Entity(pWorld)
		{
			preventEntitySpawning = true;
			setSize(0.98F, 0.7F);
			yOffset = height / 2.0F;
		}
		EntityBlockman(World* pWorld, const Vector3& pos) : EntityBlockman(pWorld)
		{
			setPosition(pos);
			prevPos = pos;
			motion = Vector3::ZERO;
		}

		virtual  ~EntityBlockman();

	protected:
		bool canTriggerWalking() { return false; }
		void moveOnOtherBlock();

	public:
		Box getCollisionBox(Entity* par1Entity);
		Box getBoundingBox() { return boundingBox; }
		bool canBePushed() { return true; }
		float getMountedYOffset() { return height * 0.0f - 0.3f; }
		bool attackEntityFrom(DamageSource* source, float amount);
		void killBlockman(DamageSource* source);
		void performHurtAnimation();
		bool canBeCollidedWith() { return !isDead; }
		void setDead();
		void onUpdate() override;
		void onActivatorRailPass(Vector3i pos, bool receivingPower) {}
		bool isImmuneToExplosions() override { return true; }

		Vector3i getPosOffset(float posX, float posY, float posZ, float offset);
		Vector3i getPos(float posX, float posY, float posZ);
		

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}

		float getShadowSize() { return 0.0f; }
		void applyEntityCollision(Entity* par1Entity);
		void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
		void setVelocity(Vector3 v);
		void setDamage(float damage);
		float getDamage();
		void setRollingAmplitude(int value) { rollingAmplitude = value; }
		int getRollingAmplitude() { return rollingAmplitude; }
		void setRollingDirection(int value) { rollingDirection = value; }
		int getRollingDirection() { return rollingDirection; }
		Block* getDisplayTile();
		Block* getDefaultDisplayTile() { return nullptr; }
		int getDisplayTileData();
		int getDefaultDisplayTileData();
		int getDisplayTileOffset();
		int getDefaultDisplayTileOffset() { return 6; }
		void setDisplayTile(int par1);
		void setDisplayTileData(int par1);
		void setDisplayTileOffset(int par1);

		bool hasDisplayTile() { return isHasDisplayTile; }
		void setHasDisplayTile(bool par1) { isHasDisplayTile = par1; }
		void setBlocmanName(String name) { entityName = name; }
		String getEntityName() override;
		bool isInvNameLocalized() { return entityName.length() > 0; }
		String func_95999_t() { return entityName; };


		bool canKnockedBackByExplosion() { return false; }

		void travelToDimension(int dimension);

	public:
		virtual void setNameLang(String nameLang) { m_nameLang = nameLang; }
		virtual void setActorName(String actorName);
		virtual void setActorBody(String body) { m_actorBody = body; };
		virtual void setActorBodyId(String bodyId) { m_actorBodyId = bodyId; }
		virtual void setCurrentMaxSpeed(float maxSpeed) { currentMaxSpeed = maxSpeed; }

		String getNameLang() const { return m_nameLang; }
		String getActorName() const { return m_actorName; }
		String getActorBody() { return m_actorBody; }
		String getActorBodyId() { return m_actorBodyId; }
		float getCurrentMaxSpeed() { return currentMaxSpeed; }

	public:
		EntityBlockmanType getBlockmanType() { return blockmanType; }
		virtual bool processInitialInteract(EntityPlayer * par1EntityPlayer) { return false; };

		virtual void updateOnTrack(int posX, int posY, int posZ, int blockId, int meta);
		virtual void applyDrag();
		virtual bool checkBlcok(int blockId) { return false;  }
	};
}
#endif

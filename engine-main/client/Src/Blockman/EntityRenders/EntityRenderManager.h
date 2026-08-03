/********************************************************************
filename: 	RenderManager.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-8
*********************************************************************/
#ifndef __ENTITY_RENDER_MANAGER_HEADER__
#define __ENTITY_RENDER_MANAGER_HEADER__

#include "Core.h"
using namespace LORD;

namespace LORD
{
	class ActorObject;
	class ModelEntity;
}

namespace BLOCKMAN
{
	
class Entity;
class EntityItem;
class EntityRender;
class World;
class GameSettings;
class ItemRenderer;
class TextureAtlasRegister;
class EntityLivingBase;
class RenderEntity;
class PackMassRenderable;
class EntityPlayerRenderable;
class EntityTextureModify;

class EntityRenderManager : public Singleton<EntityRenderManager>, public ObjectAlloc
{
public:
	static const int ENTITY_MAX_BATCH = 38;		// 38 is in mcEntity.vs
	typedef map<i32, EntityRender*>::type RendererMap;
	typedef set<EntityRender*>::type RendererSet;
	typedef map<ui64, SkinedRenderable*>::type SkinedRenderableMap;
	typedef list<SkinedRenderable*>::type SkinedRenderableArr;
	typedef vector<Matrix4>::type MatrixStack;
	typedef list<ItemRenderable*>::type ItemTessRenderableArr;
	typedef list<PackMassRenderable*>::type ItemBlockRenderableArr;
	typedef map<EntityLivingBase*, EntityPlayerRenderable*>::type EntityLivingRenderableMap;
	typedef map<Entity*, LORD::ActorObject*>::type EntityActorMap;
	typedef map<EntityItem*, LORD::ModelEntity*>::type MeshItemMap;

protected:
	RendererMap entityRenderMap;
	
	ItemTessRenderableArr		m_itemRenderables;		// for item 
	ItemTessRenderableArr		m_freeItemRenderables;	// for item

	ItemBlockRenderableArr		m_blockRenderables;		// for itemBlock
	ItemBlockRenderableArr		m_fullBlockRenderalbes;	// for itemBlock
	ItemBlockRenderableArr		m_freeBlockRenderables;	// for itemBlock

	FxRenderable*				m_shadowRenderable = nullptr;		// for shaodws.

	EntityLivingRenderableMap	m_LivingRenderables;	// for livingbase, player, mobs, etc.
	EntityActorMap				m_LivingActors;			// for livingbase, player, mobs, etc.
	MeshItemMap					m_meshItems;			// for meshItems.

public:
	/** Reference to the World object. */
	World* world = nullptr;
	RenderEntity* rendererEntity = nullptr;
	/** RenderManager's field for the renderViewEntity */
	ItemRenderer* itemRenderer = nullptr;
	EntityLivingBase* livingPlayer = nullptr;
	EntityLivingBase* pointedEntity = nullptr;
	Vector3 renderPos;
	float playerViewY = 0.f;
	float playerViewX = 0.f;
	/** Reference to the GameSettings object. */
	GameSettings* options = nullptr;
	Vector3 viewerPos;
	static bool field_85095_o;

	Texture* m_BlockTex = nullptr;
	Texture* m_ItemTex = nullptr;
	Texture* m_shadowTex = nullptr;

protected:
	
	void renderBoudingBox(Entity* pEntity, const Vector3& pos, float yaw, float ticks);

public:
	EntityRenderManager();
	~EntityRenderManager();
	
	void beginRender();
	ItemRenderable* getItemRenderable();
	FxRenderable* getShadowRenderable() { return m_shadowRenderable; }
	PackMassRenderable* getPackRenderable(Texture* tex, SKINED_RENDER_PASS rq, int quadNum);
	void endRender();

	EntityRender* getEntityClassRenderObject(int entityClass);
	EntityRender* getEntityRenderObject(Entity* pEntity);

	/** create or retrieval the entityRenderable by living base ptr. */
	bool createEntityRenderable(EntityLivingBase* pLiving);
	bool destroyEntityRenderable(EntityLivingBase* pLiving);
	EntityPlayerRenderable* getEntityRenderable(EntityLivingBase* pLiving);

	bool isItemMeshRender(EntityItem* pEntityMesh);

	/** create or retrieval the actor by livingbase ptr. */
	bool createEntityActor(Entity* pLiving);
	bool destroyEntityActor(Entity* pLiving);
	void destroyAllEntityActor();
	bool refreshEntityActor(Entity* pLiving, String actorName);
	bool restorePlayerActor(Entity* pLiving);
	LORD::ActorObject* getEntityActor(Entity* pLiving);

	/** create or retrivel the mesh item. */
	bool createMeshItem(EntityItem* pEntityItem);
	bool destroyMeshItem(EntityItem* pEntityItem);
	LORD::ModelEntity* getMeshItem(EntityItem* pEntityItem);

	/** Caches the current frame's active render info, including the current World, RenderEngine, GameSettings and
	* FontRenderer settings, as well as interpolated player position, pitch and yaw. */
	void cacheActiveRenderInfo(World* pWorld, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2, GameSettings* setting, float ticks);
	
	/** Will render the specified entity at the specified partial tick time. Args: entity, partialTickTime */
	void renderEntity(Entity* pEntity, float ticks);
	
	/** Renders the specified entity with the passed in position, yaw, and partialTickTime.*/
	void renderEntityWithPosYaw(Entity* pEntity, const Vector3& pos, float yaw, float ticks);
	
	/** World sets this RenderManager's worldObj to the world provided */
	void set(World* pWorld);
	
	float getDistanceToCamera(const Vector3& pos);
	
	/** Returns the font renderer */
	// FontRenderer getFontRenderer() { return fontRenderer; }
	void updateIcons(TextureAtlasRegister* atlas);
};

}

#endif
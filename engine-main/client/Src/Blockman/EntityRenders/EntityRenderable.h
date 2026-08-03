/********************************************************************
filename: 	Render.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-8
*********************************************************************/
#ifndef __ENTITY_RENDERABLE_HEADER__
#define __ENTITY_RENDERABLE_HEADER__

#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{

class EntityLivingBase;
class EntityPlayer;
class EntityRenderPlayer;
class ModelBox;
class TextureAtlas;


// for render player.
class EntityPlayerRenderable : public ObjectAlloc
{
protected:
	static const int ARMOR_QUALITY_COUNT = 5;
	static const String s_armorNamePrefix[ARMOR_QUALITY_COUNT];
	static const int s_armorBoneCount[4];
	// the render param for main model
	SkinedRenderable*	m_mainSkin = nullptr;
	Texture*			m_mainTexture = nullptr;
	Vector2i			m_mainTexSize;
	Vector4*			m_mainBoneMatrix = nullptr;
	int					m_mainBoneCount = 0;

	// the render param for equipment model
	int					m_equipments[InventoryPlayer::ARMOR_INVENTORY_COUNT] = {0};
	int					m_enchantments[InventoryPlayer::ARMOR_INVENTORY_COUNT] = {0};
	SkinedRenderable*	m_equipSkin = nullptr;
	SkinedRenderable*	m_enchantEquipSkin = nullptr;
	TextureAtlas*		m_equipAtlas = nullptr;
	Texture*			m_equipTexture = nullptr;
	Vector2i			m_equipTexSize;
	int					m_equipBoneCount = 0;
	bool				m_equipChanged = false;
	
	EntityPlayer*		m_pPlayer = nullptr;
	
	SKINED_RENDER_PASS  m_pass = SRP_INVALID;
	EntityRenderPlayer* m_render = nullptr;
	Matrix4				m_worldMat;
	Color				m_color;

protected:
	void refreshEquipment();
	void compileEquipGeometry();
	void compileEquipTexture();
	void compileMainGeometry();
	void compileMainTexture();

public:
	EntityPlayerRenderable(EntityPlayer* pLiving, SKINED_RENDER_PASS pass);
	virtual ~EntityPlayerRenderable();
		
	void setColor(const Color& color) { m_color = color; }
	void update(float rdt);
	void updateRender();
	void invalid();
	void getBoneMatrix(int boneID, Matrix4& outMat) const;
	const Matrix4& getWorldMatrix() const { return m_worldMat; }

	// for ui render.
	void updateRender_ui(const Matrix4& world, const Matrix4& viewProj);
};


class RightArmRenderable : public ObjectAlloc
{
protected:
	SkinedRenderable*	m_skin = nullptr;
	Texture*			m_texture = nullptr;
	Vector2i			m_texSize;
	EntityPlayer*		m_player = nullptr;
	bool				m_compile = false;
	Vector4*			m_boneMatrix = nullptr;
	EntityRenderPlayer* m_render = nullptr;
	Matrix4				m_worldMat;
	ModelBox*			m_model = nullptr;
	bool				m_isZombie = false;

protected:
	void compileGeometry();
	void compileTexture();

public:
	RightArmRenderable();
	virtual ~RightArmRenderable();

	void setZombie(bool isZombie);
	bool getisZombie() { return m_isZombie; }

	EntityPlayer* getPlayer() { return m_player; }
	void setPlayer(EntityPlayer* player) { m_player = player; }
	void update(float equipProgress, float rdt);
	void updateRender();
	void invalid();
};

}

#endif
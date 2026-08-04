#include "EntityGunFlameFx.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Render/TextureAtlas.h"
#include "EntityRenders/EntityRenderManager.h"

#include "Block/Block.h"
#include "World/World.h"

#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "Tessolator/TessRenderable.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Setting/GunSetting.h"
#include "Entity/EntityPlayer.h"
#include "cEntity/EntityPlayerSPMP.h"

#include "Tessolator/BatchRenderable.h"
#include "Tessolator/LineRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{

EntityGunFlameFX::EntityGunFlameFX(World* pWorld, const Vector3& pos1, EntityPlayer* shooter)
	: EntityFx(pWorld, pos1, Vector3::ZERO)
	, m_shooter(shooter)
{
	m_color = Color::WHITE;
	
	setSize(0.2F, 0.2F);
	particleScale *= rand->nextFloat() * 0.1F + 0.5F;
	particleAge = 2;
	m_frameIndex = rand->nextInt(3);
	LordAssert(m_shooter);
	if (Blockman::Instance()->m_isShowGunFlameCoordinate)
	{
		m_line = TessManager::Instance()->createLineRenderable(LRP_SELECTED);
	}

	if (shooter == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST)
	{
		m_isFirstPerspective = true;
	}
}

EntityGunFlameFX::~EntityGunFlameFX()
{
	if (Blockman::Instance()->m_isShowGunFlameCoordinate)
	{
		TessManager::Instance()->destroyLineRenderable(m_line);
	}
}

void EntityGunFlameFX::onUpdate()
{
	prevPos = position;

	if (!Blockman::Instance()->m_isShowGunFlameCoordinate && particleAge-- <= 0)
	{
		setDead();
	}
}


void EntityGunFlameFX::renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy)
{
	if (!m_shooter)
		return;

	ItemStackPtr pItem = m_shooter->inventory->getCurrentItem();
	if (!pItem)
		return;

	ItemGun* pGun = dynamic_cast<ItemGun*>(pItem->getItem());
	if (!pGun)
		return;

	const GunSetting* setting = pGun->getGunSetting();
	if (!setting)
		return;

	float frontOff,	rightOff, downOff, scale;
	bool isFirstView = m_shooter == (EntityPlayer*)(Blockman::Instance()->m_pPlayer) &&
		Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST;
	if (isFirstView)
	{
		frontOff = setting->flame_1_frontOff;
		rightOff = setting->flame_1_rightOff;
		downOff = setting->flame_1_downOff;
		scale = setting->flame_1_scale;
	}
	else
	{
		frontOff = setting->flame_3_frontOff;
		rightOff = setting->flame_3_rightOff;
		downOff = setting->flame_3_downOff;
		scale = setting->flame_3_scale;
	}

	if (Blockman::Instance()->m_isShowGunFlameCoordinate)
	{
		std::ifstream infile("scale.txt");
		float flame_1_frontOff, flame_1_rightOff, flame_1_downOff, flame_1_scale;
		float flame_3_frontOff, flame_3_rightOff, flame_3_downOff, flame_3_scale;
		infile >> flame_1_frontOff;
		infile >> flame_1_rightOff;
		infile >> flame_1_downOff;
		infile >> flame_1_scale;
		infile >> flame_3_frontOff;
		infile >> flame_3_rightOff;
		infile >> flame_3_downOff;
		infile >> flame_3_scale;
		infile.close();

		if (isFirstView)
		{
			frontOff = flame_1_frontOff;
			rightOff = flame_1_rightOff;
			downOff = flame_1_downOff;
			scale = flame_1_scale;
		}
		else
		{
			frontOff = flame_3_frontOff;
			rightOff = flame_3_rightOff;
			downOff = flame_3_downOff;
			scale = flame_3_scale;
		}
	}

	float u1 = (7-m_frameIndex) / 8.0F;
	float u2 = u1 + 0.1245f;
	float v1 = 0;
	float v2 = 0.1245f;
	float s = 0.1F * particleScale * scale;

	if (particleIcon != nullptr)
	{
		u1 = particleIcon->getMinU();
		u2 = particleIcon->getMaxU();
		v1 = particleIcon->getMinV();
		v2 = particleIcon->getMaxV();
	}

	// first-view is diff with third-view
	float yaw, pitch;
	if (isFirstView)
	{
		yaw = m_shooter->prevRotationYaw + (m_shooter->rotationYaw - m_shooter->prevRotationYaw) * rdt;
		pitch = m_shooter->prevRotationPitch + (m_shooter->rotationPitch - m_shooter->prevRotationPitch) * rdt;
	}
	else
	{
		yaw = m_shooter->prevRenderYawOffset + (m_shooter->renderYawOffset - m_shooter->prevRenderYawOffset) * rdt;
		pitch = 0.f;
	}

	// adjust the bullet fire flame pos.
	Vector3 v, dir, right, down;
	if (isFirstView)
	{
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		dir = mainCamera->getDirection();
		right = mainCamera->getRight();
		down = -mainCamera->getUp();
		v = mainCamera->getPosition();
	}
	else
	{
		dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
		dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
		dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
		right = Vector3::NEG_UNIT_Y.cross(dir);
		down = Vector3::NEG_UNIT_Y;
		v = m_shooter->getPosition(rdt);
		v.y += m_shooter->getEyeHeight();

		if (m_shooter == (EntityPlayer*)(Blockman::Instance()->m_pPlayer))
		{
			v.y += 0.08f;
		}
	}
	v += dir * frontOff;
	v += right * rightOff;
	v += down * downOff;
	v -= interpPos;

	ui32 color = 0xFFFFFFFF;
	vertices[0].set(v.x - (rx + ryz) * s, v.y - rxz * s, v.z - (rz + rxy) * s, color, 0xFFFFFFFF, u1, v2);
	vertices[1].set(v.x - (rx - ryz) * s, v.y + rxz * s, v.z - (rz - rxy) * s, color, 0xFFFFFFFF, u2, v2);
	vertices[2].set(v.x + (rx + ryz) * s, v.y + rxz * s, v.z + (rz + rxy) * s, color, 0xFFFFFFFF, u2, v1);
	vertices[3].set(v.x + (rx - ryz) * s, v.y - rxz * s, v.z + (rz - rxy) * s, color, 0xFFFFFFFF, u1, v1);

	if (Blockman::Instance()->m_isShowGunFlameCoordinate)
	{
		Vector3 pointO, pointXP, pointYP, pointZP;
		pointO = dir + m_shooter->getPosition(rdt) + m_shooter->getEyeHeight();
		pointXP = pointO + dir;
		pointYP = pointO + right;
		pointZP = pointO + down;
		m_line->beginPolygon();
		LineVertexFmt* points = NULL;
		m_line->getLockPtr_line(2, points);
		points[0].set(pointO.x, pointO.y, pointO.z, 0x000000FF);
		points[1].set(pointXP.x, pointXP.y, pointXP.z, 0x000000FF);
		LineVertexFmt* points2 = NULL;
		m_line->getLockPtr_line(2, points2);
		points2[0].set(pointO.x, pointO.y, pointO.z, 0x00FF0000);
		points2[1].set(pointYP.x, pointYP.y, pointYP.z, 0x00FF0000);
		LineVertexFmt* points3 = NULL;
		m_line->getLockPtr_line(2, points3);
		points3[0].set(pointO.x, pointO.y, pointO.z, 0x0000FF00);
		points3[1].set(pointZP.x, pointZP.y, pointZP.z, 0x0000FF00);
		m_line->endPolygon();
		if (m_line->getPointNum() > 0)
			TessManager::Instance()->addLineRenderable(m_line);
	}
}


}
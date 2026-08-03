#include "ModelBiped.h"
#include "ModelRenderer.h"
#include "ModelBox.h"

#include "EntityRenders/EntityRenderManager.h"

namespace BLOCKMAN
{
	
ModelBiped::ModelBiped(float extends, float yOffset)
	: heldItemLeft(0)
	, heldItemRight(0)
	, isSneak(false)
	, aimedBow(false)
{
	mModelRender = LordNew ModelRenderer(this, "biped");
	
	b_body = mModelRender->addBox(BBI_BODY);
	float scale = 0.0625f;
	extends *= scale;

	b_body->setPosition(Vector3(-4.0F, 0.0F, -2.0F) * scale, Vector3(8.f, 12.f, 4.f) * scale, extends);
	b_body->setTexCoord(Vector2i(16, 16), Vector3i(8, 12, 4));
	b_body->setRotateAxis(Vector3(0.f, 12.f + yOffset, 0.f) * scale);
	b_body->finish();

	b_head = mModelRender->addBox(BBI_HEAD);
	b_head->setPosition(Vector3(-4.0F, 0.0F, -4.0F) * scale, Vector3(8.f, 8.f, 8.f) * scale, extends);
	b_head->setTexCoord(Vector2i(0, 0), Vector3i(8, 8, 8));
	b_head->setRotateAxis(Vector3(0.f, 24.f + yOffset, 0.f) * scale);
	b_head->finish();

	b_arm_r = mModelRender->addBox(BBI_ARM_R);
	b_arm_r->setPosition(Vector3(-1.0F, -10.0F, -2.0F) * scale, Vector3(4.f, 12.f, 4.f) * scale, extends);
	b_arm_r->setTexCoord(Vector2i(40, 16), Vector3i(4, 12, 4));
	b_arm_r->setRotateAxis(Vector3(5.0F, 22.0F + yOffset, 0.0F) * scale);
	b_arm_r->finish();

	b_arm_l = mModelRender->addBox(BBI_ARM_L);
	b_arm_l->setMirror(true);
	b_arm_l->setPosition(Vector3(-3.0F, -10.0F, -2.0F) * scale, Vector3(4.f, 12.f, 4.f) * scale, extends);
	b_arm_l->setTexCoord(Vector2i(40, 16), Vector3i(4, 12, 4));
	b_arm_l->setRotateAxis(Vector3(-5.0F, 22.0F + yOffset, 0.0F) * scale);
	b_arm_l->finish();

	b_leg_r = mModelRender->addBox(BBI_LEG_R);
	b_leg_r->setPosition(Vector3(-4.0F, -12.0F, -2.0F) * scale, Vector3(4.f, 12.f, 4.f) * scale, extends);
	b_leg_r->setTexCoord(Vector2i(0, 16), Vector3i(4, 12, 4));
	b_leg_r->setRotateAxis(Vector3(-0.F, 12.0F + yOffset, 0.0F) * scale);
	b_leg_r->finish();

	b_leg_l = mModelRender->addBox(BBI_LEG_L);
	b_leg_l->setMirror(true);
	b_leg_l->setPosition(Vector3(0.0F, -12.0F, -2.0F) * scale, Vector3(4.f, 12.f, 4.f) * scale, extends);
	b_leg_l->setTexCoord(Vector2i(0, 16), Vector3i(4, 12, 4));
	b_leg_l->setRotateAxis(Vector3(-0.F, 12.0F + yOffset, 0.0F) * scale);
	b_leg_l->finish();

	/*b_swear = mModelRender->addBox(BBI_SWEAR);
	b_swear->setPosition(Vector3(-4.0F, 36.0F, -4.0F) * scale, Vector3(8.f, 8.f, 8.f) * scale, delta + 0.03125f);
	b_swear->setTexCoord(Vector2i(32, 0), Vector3i(8, 8, 8));
	b_swear->setRotateAxis(Vector3(0.f, 0.f + yOffset, 0.f) * scale);
	b_swear->finish();*/
}

ModelBiped::~ModelBiped()
{
	LordSafeDelete(mModelRender);
}

int ModelBiped::getBoneCount()
{
	return mModelRender->getBoxCount();
}

bool ModelBiped::compileSkinMesh(SkinedRenderable* renderer, const Vector2i& pos, const Vector2i& size)
{
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		pBox->render(renderer, pos, size);
	}
	return true;
}

ModelBox* ModelBiped::getModelBox(int i)
{
	LordAssert(i >= 0 && i < mModelRender->getBoxCount());
	return mModelRender->getBox(i);
}

void ModelBiped::updateBones(Vector4* outMatrices)
{
	// fetch all the matrix to destination.
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		pBox->caculateBoneMatrix(outMatrices + i * 3);
	}
}

void ModelBiped::setRotationAngles(float limbSwing, float swingAmount, float rdt, float headYaw, float headPitch, Entity* pEntity)
{
	const float scale = 0.0625;
	// caculate the head m_yaw and pitch.
	b_head->m_yaw = -headYaw * Math::DEG2RAD;
	b_head->m_pitch = -headPitch * Math::DEG2RAD;

	// caculate the arms and legs limbswing. arms * 2.0f,  legs * 1.4f.
	b_arm_r->m_pitch = Math::Cos_tab(limbSwing * 0.6662F) * 2.0F * swingAmount * 0.5F;
	b_arm_l->m_pitch = Math::Cos_tab(limbSwing * 0.6662F + Math::PI) * 2.0F * swingAmount * 0.5F;
	b_arm_r->m_roll = b_arm_l->m_roll = 0.0F;
	b_leg_r->m_pitch = Math::Cos_tab(limbSwing * 0.6662F) * 1.4F * swingAmount;
	b_leg_l->m_pitch = Math::Cos_tab(limbSwing * 0.6662F + Math::PI) * 1.4F * swingAmount;
	b_leg_r->m_yaw = b_leg_l->m_yaw = 0.0f;

	if (heldItemLeft != 0)
	{
		b_arm_l->m_pitch = b_arm_r->m_pitch * 0.5F + (Math::PI * 0.1f) * heldItemLeft;
	}
	if (heldItemRight != 0)
	{
		b_arm_r->m_pitch = b_arm_r->m_pitch * 0.5F + (Math::PI * 0.1f) * heldItemRight;
	}
	b_arm_r->m_yaw = b_arm_l->m_yaw = 0.0f;

	if (swingProgress > 0.f)
	{
		float swings = swingProgress; // [0 ~ 1]
		b_body->m_yaw = Math::Sin_tab(Math::Sqrt(swings) * Math::PI_2) * 0.2F;
		b_arm_r->m_axis.z = -Math::Sin_tab(b_body->m_yaw) * 5.0F * scale;
		b_arm_r->m_axis.x = Math::Cos_tab(b_body->m_yaw) * 5.0F * scale;
		b_arm_l->m_axis.z = Math::Sin_tab(b_body->m_yaw) * 5.0F * scale;
		b_arm_l->m_axis.x = -Math::Cos_tab(b_body->m_yaw) * 5.0F * scale;
		b_arm_r->m_yaw = b_body->m_yaw;
		b_arm_l->m_yaw = b_body->m_yaw;
		b_arm_l->m_pitch += b_body->m_yaw;
		swings = 1.0F - swingProgress;
		swings *= swings;
		swings *= swings;
		swings = 1.0F - swings;
		float baseWings = Math::Sin_tab(swings * Math::PI);
		float headPitch = Math::Sin_tab(swingProgress * Math::PI) * (Math::PI_DIV2 + b_head->m_pitch ) * 0.75F;
		b_arm_r->m_pitch += baseWings * 1.2f + headPitch; 
		b_arm_r->m_yaw -= b_body->m_yaw * 2.0F;
		b_arm_r->m_roll = Math::Sin_tab(swingProgress * Math::PI) * -0.4F;
	}

	if (isSneak)
	{
		b_body->m_pitch = -0.5F;
		b_arm_r->m_pitch += 0.4F;
		b_arm_l->m_pitch += 0.4F;

		// adjust head arm axis.
		b_arm_r->m_axis.y = 20.f * scale;
		b_arm_l->m_axis.y = 20.f * scale;
		b_arm_r->m_axis.z = -3.f * scale;
		b_arm_l->m_axis.z = -3.f * scale;
		b_head->m_axis.y = 22.f * scale;
		b_head->m_axis.z = -4.f * scale;
	}
	else
	{
		b_body->m_pitch = 0.0F;

		b_arm_r->m_axis.y = 22.f * scale;
		b_arm_l->m_axis.y = 22.f * scale;
		b_arm_r->m_axis.z = 0.f * scale;
		b_arm_l->m_axis.z = 0.f * scale;
		b_head->m_axis.y = 24.f * scale;
		b_head->m_axis.z = 0.f * scale;
	}
	// the breath animation for arms.
	b_arm_r->m_roll += Math::Cos_tab(rdt * 0.09F) * 0.05F + 0.05F;
	b_arm_l->m_roll -= Math::Cos_tab(rdt * 0.09F) * 0.05F + 0.05F;
	b_arm_r->m_pitch -= Math::Sin_tab(rdt * 0.067F) * 0.05F;
	b_arm_l->m_pitch += Math::Sin_tab(rdt * 0.067F) * 0.05F;
	
	if (aimedBow)
	{
		float f1 = 0.0F;
		float f2 = 0.0F;
		b_arm_r->m_roll = 0.0F;
		b_arm_l->m_roll = 0.0F;
		b_arm_r->m_yaw = 0.4F  + b_head->m_yaw;
		b_arm_l->m_yaw = -0.4F + b_head->m_yaw + 0.4F;
		b_arm_r->m_pitch = Math::PI_DIV2 + b_head->m_pitch;
		b_arm_l->m_pitch = Math::PI_DIV2 + b_head->m_pitch;
		b_arm_r->m_pitch += f1 * 1.2F - f2 * 0.4F;
		b_arm_l->m_pitch += f1 * 1.2F - f2 * 0.4F;
		b_arm_r->m_roll -= Math::Cos_tab(rdt * 0.09F) * 0.05F + 0.05F;
		b_arm_l->m_roll += Math::Cos_tab(rdt * 0.09F) * 0.05F + 0.05F;
		b_arm_r->m_pitch -= Math::Sin_tab(rdt * 0.067F) * 0.05F;
		b_arm_l->m_pitch += Math::Sin_tab(rdt * 0.067F) * 0.05F;
	}
}

}
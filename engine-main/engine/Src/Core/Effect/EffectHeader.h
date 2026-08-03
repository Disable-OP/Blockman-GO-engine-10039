/********************************************************************
filename: 	Bone.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __EFFECT_EFFECTHEADERS_H__
#define __EFFECT_EFFECTHEADERS_H__

#include "Math/MathLib.h"
#include "Image/Color.h"

namespace LORD
{

	//forward decaleration
	class EffectSystem;						//a system has some layers
	class EffectLayer;						//a layer has some keyframes,special layers like particleLayer has some ParticleControllerSets and a material
	class EffectKeyFrame;					//a keyframe has some controllers
	class EffectParticleControllerSet;		//
	class EffectController;					//
	class EffectMaterial;					//
	class EffectParticle;
	class EffectRenderalbe;
	class DataStream;

	enum EFFECT_RENDER_GROUP
	{
		ERG_GROUPUP = 0,
		ERG_GROUPDOWN,
	};

	enum EFFECT_BLEND_MODE
	{
		AlphaLightMode = 0,
		HighLightMode,
		DistortionMode
	};

	enum EFFECT_LAYER_TYPE
	{
		ELT_2DBillboard = 1,
		ELT_3DBillboard,
		ELT_TRAIL,
		ELT_Column,
		ELT_Paraboloidal,
		ELT_ParticlesPoint,
		ELT_ParticlesCube,
		ELT_ParticlesSphere,
		ELT_ParticlesColumn,
		ELT_ParticlesCruve,
		ELT_Model,
		ELT_Lighting,
		ELT_Rope,
		ELT_2DEditableSurface,
		ELT_3DEditableSurface,
		ELT_Skin,
		ELT_SpriteAnimation,
		ELT_Child,
		ELT_Unknow = 99,
	};

	enum EFFECT_CONTROL_TYPE
	{
		ECT_Translation = 1,
		ECT_RotateSelf,
		ECT_RotateAxis,
		ECT_RotateRevolution,
		ECT_Offcenter,
		ECT_ColorChange,
		ECT_ScaleChange,
		ECT_ScaleNoise,
		ECT_ColorNoise,
		ECT_ColorSet,
		ECT_ScaleSet,
		ECT_Unknow = 99,
	};

	struct EffectVertexFmt
	{
		Vector3 pos;
		ui32 color;
		float tx, ty;
	};

	struct EffectVertexFmt2
	{
		Vector3 pos;
		float tx, ty;
	};

	typedef map<String, String>::type NameValuePairList;
}
#endif
 #include "Core.h"
#include "TessManager.h"
#include "TessMaterial.h"
#include "TessRenderable.h"
#include "TessRenderQueue.h"
#include "SkinedRenderable.h"
#include "SkinedRenderQueue.h"
#include "ItemRenderable.h"
#include "ItemRenderQueue.h"
#include "FxRenderable.h"
#include "FxRenderQueue.h"
#include "BatchRenderable.h"
#include "BatchRenderQueue.h"
#include "LineRenderable.h"
#include "LineRenderQueue.h"
#include "FontRenderable.h"
#include "FontRenderQueue.h"
#include "EnchantRenderQueue.h"
#include "RasterizerHelpRender.h"
#include "TessObject.h"

#include "Object/Exception.h"
#include "Render/Renderer.h"
#include "Render/ShaderProgram.h"
#include "Render/ShaderProgramManager.h"
#include "Scene/SceneManager.h"
#include "Resource/ResourceGroupManager.h"
#include "Render/Material.h"
#include "Render/RenderTarget.h"
#include "Render/TextureManager.h"

namespace LORD
{

// all Tessellator render vertices cast 20M memory
const size_t TessManager::sTessQuadSize = 4096;
const size_t TessManager::sTessFanSize =  128;
const size_t TessManager::sSkinedQuadSize = 1024;
const size_t TessManager::sItemQuadSize = 2048;
const size_t TessManager::sFxQuadSize = 2048;
const size_t TessManager::sLinePointSize = 1024;
const size_t TessManager::sCharQuadSize = 1024;

TessManager::TessManager(void)
	: mRenderaQueueCreated(false)
	, mEnableRender(true)
	, m_commonIndexBuffer(NULL)
	, mTessLineMode(false)
	, mSkinedLineMode(false)
	, mItemLineMode(false)
	, mFxLineMode(false)
	, mFontLineMode(false)
	, m_lastFrame(0.f)
	, m_frameTime(0.1f)
	, m_frameCount(0)
	//, m_mainLightDir(Vector3(0.2f, 1.f, -0.7f).normalizedCopy())
	//, m_mainLightColor(0.6f, 0.6f, 0.6f, 1.f)
	//, m_subLightDir(Vector3(-0.2f, 1.f, 0.7f).normalizedCopy())
	//, m_subLightColor(0.6f, 0.6f, 0.6f, 1.f)
	//, m_ambient(0.4f, 0.4f, 0.4f, 1.f)
{
	// m_fogParam[1] = Vector4(0.5f, 0.5f, 1.0f, 1.0f);
	// m_fogParam[0] = Vector4(10.f, 128.f, 0.9f, 1.f);

	memset(mTessRenderQueues, 0, sizeof(void*) * TRP_COUNT);
	memset(mSkinedRenderQueues, 0, sizeof(void*) * SRP_COUNT);
	memset(mItemRenderQueues, 0, sizeof(void*) * IRP_COUNT);
	memset(mFxRenderQueues, 0, sizeof(void*) * FRP_COUNT);
	memset(mBatchRenderQueues, 0, sizeof(void*) * STRP_COUNT);
	memset(mLineRenderQueue, 0, sizeof(void*) * LRP_COUNT);

	mRasterizerRenderQueue = NULL;
	mEnchantItemRenderQueue = NULL;
	mEnchantSkinedRenderQueue = NULL;
	mFontRenderQueue = NULL;

	m_commonIndexBuffer = NULL;
	m_commonIndexBuffer_line = NULL;
	m_commonIndexBuffer_fan = NULL;
	m_viewBobing.identity();
	m_enchantTexMat[0].identity();
	m_enchantTexMat[1].identity();
}
//-----------------------------------------------------------------------
TessManager::~TessManager(void)
{
	destroyRenderableSet();
}
//-----------------------------------------------------------------------
TessMaterial* TessManager::createMaterial()
{
	TessMaterial* material = LordNew TessMaterial();
	return material;
}

TessMaterial* TessManager::cloneMaterial(TessMaterial* material)
{
	if (!material)
		return 0;

	TessMaterial* cloneMaterial = createMaterial();
	material->copyAttributesTo(cloneMaterial);
	return cloneMaterial;
}

void TessManager::destroyMaterial(TessMaterial* material)
{
	LordSafeDelete(material);
}

SamplerState* TessManager::getSamplerState()
{
	return mTessRenderQueues[TRP_SOLID]->getSamplerState();
}

void TessManager::createTessRenderQueues()
{
	LordAssert(!mTessRenderQueues[TRP_SOLID]);

	TessRenderQueue* rq_solid = LordNew TessRenderQueue(TRP_SOLID);
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Solid");
	rq_solid->setShaderProgram(shaderProgram);
	SamplerState::SamplerDesc sampler_desc;				// sampler
	sampler_desc.minFilter = SamplerState::FO_POINT;
	sampler_desc.magFilter = SamplerState::FO_POINT;
	sampler_desc.mipFilter = SamplerState::FO_LINEAR;
	rq_solid->setSamplerState(sampler_desc);
	SamplerState::SamplerDesc sampler_desc_lightmap;
	sampler_desc_lightmap.minFilter = SamplerState::FO_LINEAR;
	sampler_desc_lightmap.magFilter = SamplerState::FO_LINEAR;
	sampler_desc_lightmap.mipFilter = SamplerState::FO_NONE;
	sampler_desc_lightmap.addrUMode = SamplerState::AM_CLAMP;
	sampler_desc_lightmap.addrVMode = SamplerState::AM_CLAMP;
	rq_solid->setSamplerState_lightmap(sampler_desc_lightmap);
	BlendState::BlendDesc blend_desc;					// blend
	rq_solid->setBlendState(blend_desc);
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	rq_solid->setDepthState(depth_desc);
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	rq_solid->setRasterizerState(rasterizer_desc);
	rq_solid->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_SOLID] = rq_solid;

	TessRenderQueue* rq_solid_nomip = LordNew TessRenderQueue(TRP_SOLID_NOMIP);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Solid");
	rq_solid_nomip->setShaderProgram(shaderProgram);
	sampler_desc.mipFilter = SamplerState::FO_NONE;
	rq_solid_nomip->setSamplerState(sampler_desc);
	rq_solid_nomip->setSamplerState_lightmap(sampler_desc_lightmap);
	rq_solid_nomip->setBlendState(blend_desc);
	rq_solid_nomip->setDepthState(depth_desc);
	rq_solid_nomip->setRasterizerState(rasterizer_desc);
	rq_solid_nomip->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_SOLID_NOMIP] = rq_solid_nomip;

	TessRenderQueue* rq_solid_uv = LordNew TessRenderQueue(TRP_SOLID_UV);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_UV_Anim");
	sampler_desc.mipFilter = SamplerState::FO_LINEAR;
	rq_solid_uv->setShaderProgram(shaderProgram);
	rq_solid_uv->setSamplerState(sampler_desc);
	rq_solid_uv->setSamplerState_lightmap(sampler_desc_lightmap);
	rq_solid_uv->setBlendState(blend_desc);
	rq_solid_uv->setDepthState(depth_desc);
	rq_solid_uv->setRasterizerState(rasterizer_desc);
	rq_solid_uv->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_SOLID_UV] = rq_solid_uv;

	TessRenderQueue* rq_translucent_3d = LordNew TessRenderQueue(TRP_TRANSPARENT_3D);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Solid");
	rq_translucent_3d->setShaderProgram(shaderProgram);
	sampler_desc.mipFilter = SamplerState::FO_NONE;
	sampler_desc.addrUMode = SamplerState::AM_BORDER;
	sampler_desc.addrVMode = SamplerState::AM_BORDER;
	rq_translucent_3d->setSamplerState(sampler_desc);
	rq_translucent_3d->setSamplerState_lightmap(sampler_desc_lightmap);
	blend_desc.colorWriteMask = BlendState::CMASK_NONE;
	rq_translucent_3d->setBlendState(blend_desc);
	blend_desc.colorWriteMask = BlendState::CMASK_ALL;
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
	rq_translucent_3d->setBlendState2(blend_desc);
	depth_desc.bWriteDepth = true;
	depth_desc.bDepthEnable = true;
	rq_translucent_3d->setDepthState(depth_desc);
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = true;
	depth_desc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
	rq_translucent_3d->setDepthState2(depth_desc);
	rasterizer_desc.cullMode = RasterizerState::CULL_NONE;
	rq_translucent_3d->setRasterizerState(rasterizer_desc);
	rq_translucent_3d->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_TRANSPARENT_3D] = rq_translucent_3d;

	TessRenderQueue* rq_translucent_3duv = LordNew TessRenderQueue(TRP_TRANSPARENT_3DUV);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Transparent_UV_Anim");
	rq_translucent_3duv->setShaderProgram(shaderProgram);
	sampler_desc.addrUMode = SamplerState::AM_CLAMP;
	sampler_desc.addrVMode = SamplerState::AM_CLAMP;
	rq_translucent_3duv->setSamplerState(sampler_desc);
	rq_translucent_3duv->setSamplerState_lightmap(sampler_desc_lightmap);
	blend_desc.colorWriteMask = BlendState::CMASK_NONE;
	rq_translucent_3duv->setBlendState(blend_desc);
	blend_desc.colorWriteMask = BlendState::CMASK_ALL;
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
	rq_translucent_3duv->setBlendState2(blend_desc);
	depth_desc.bWriteDepth = true;
	depth_desc.bDepthEnable = true;
	rq_translucent_3duv->setDepthState(depth_desc);
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = true;
	depth_desc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
	rq_translucent_3duv->setDepthState2(depth_desc);
	rasterizer_desc.cullMode = RasterizerState::CULL_NONE;
	rq_translucent_3duv->setRasterizerState(rasterizer_desc);
	rq_translucent_3duv->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_TRANSPARENT_3DUV] = rq_translucent_3duv;

	TessRenderQueue* rq_translucent_2d = LordNew TessRenderQueue(TRP_TRANSPARENT_2D);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Transparent");
	rq_translucent_2d->setShaderProgram(shaderProgram);
	sampler_desc.mipFilter = SamplerState::FO_NONE;
	rq_translucent_2d->setSamplerState(sampler_desc);
	rq_translucent_2d->setSamplerState_lightmap(sampler_desc_lightmap);
	blend_desc.srcBlend = BlendState::BF_DST_COLOR;
	blend_desc.dstBlend = BlendState::BF_SRC_COLOR;
	blend_desc.srcAlphaBlend = BlendState::BF_DST_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
	rq_translucent_2d->setBlendState(blend_desc);
	depth_desc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
	rq_translucent_2d->setDepthState(depth_desc);
	rq_translucent_2d->setRasterizerState(rasterizer_desc);
	rq_translucent_2d->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_TRANSPARENT_2D] = rq_translucent_2d;

	TessRenderQueue* rq_trans_uv = LordNew TessRenderQueue(TRP_TRANSPARENT_UV);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Transparent_UV_Anim");
	rq_trans_uv->setShaderProgram(shaderProgram);
	rq_trans_uv->setSamplerState(sampler_desc);
	rq_trans_uv->setSamplerState_lightmap(sampler_desc_lightmap);
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
	rq_trans_uv->setBlendState(blend_desc);
	depth_desc.depthFunc = DepthStencilState::CF_LESS;
	rq_trans_uv->setDepthState(depth_desc);
	rq_trans_uv->setRasterizerState(rasterizer_desc);
	rq_trans_uv->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_TRANSPARENT_UV] = rq_trans_uv;

	TessRenderQueue* rq_trans_hl = LordNew TessRenderQueue(TRP_TRANSPARENT_HL);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Solid");
	rq_trans_hl->setShaderProgram(shaderProgram);
	sampler_desc.minFilter = SamplerState::FO_POINT;
	sampler_desc.magFilter = SamplerState::FO_POINT;
	sampler_desc.mipFilter = SamplerState::FO_LINEAR;
	rq_trans_hl->setSamplerState(sampler_desc);
	rq_trans_hl->setSamplerState_lightmap(sampler_desc_lightmap);
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_ONE;
	rq_trans_hl->setBlendState(blend_desc);
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = true;
	depth_desc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
	rq_trans_hl->setDepthState(depth_desc);
	rasterizer_desc.depthBias = -0.1f;
	rasterizer_desc.depthBiasFactor = -1.f;
	rq_trans_hl->setRasterizerState(rasterizer_desc);
	rq_trans_hl->setRenderInput(m_tessVertElement);
	mTessRenderQueues[TRP_TRANSPARENT_HL] = rq_trans_hl;
}

void TessManager::createSkinedRenderQueues()
{
	LordAssert(!mSkinedRenderQueues[SRP_SOLID]);

	SkinedRenderQueue* rq_solid = LordNew SkinedRenderQueue(SRP_SOLID);
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcSkined_Solid");
	rq_solid->setShaderProgram(shaderProgram);
	SamplerState::SamplerDesc sampler_desc;				// sampler
	sampler_desc.minFilter = SamplerState::FO_POINT;
	sampler_desc.magFilter = SamplerState::FO_POINT;
	sampler_desc.mipFilter = SamplerState::FO_LINEAR;
	rq_solid->setSamplerState(sampler_desc);
	BlendState::BlendDesc blend_desc;					// blend
	rq_solid->setBlendState(blend_desc);
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	rq_solid->setDepthState(depth_desc);
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	rq_solid->setRasterizerState(rasterizer_desc);
	rq_solid->setRenderInput(m_skinedVertElement);
	mSkinedRenderQueues[SRP_SOLID] = rq_solid;

	LordAssert(!mSkinedRenderQueues[SRP_SOLID_LIGHT]);
	SkinedRenderQueue* rq_light = LordNew SkinedRenderQueue(SRP_SOLID_LIGHT);
	rq_light->setShaderProgram(shaderProgram);
	rq_light->setSamplerState(sampler_desc);
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_ONE;
	blend_desc.dstBlend = BlendState::BF_ONE;
	blend_desc.srcAlphaBlend = BlendState::BF_ONE;
	blend_desc.dstAlphaBlend = BlendState::BF_ONE;
	rq_light->setBlendState(blend_desc);
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = true;
	depth_desc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
	rq_light->setDepthState(depth_desc);
	rq_light->setRasterizerState(rasterizer_desc);
	rq_light->setRenderInput(m_skinedVertElement);
	mSkinedRenderQueues[SRP_SOLID_LIGHT] = rq_light;

	SkinedRenderQueue* rq_solid_nodepth = LordNew SkinedRenderQueue(SRP_SOLID_NODEPTH);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcSkined_Solid");
	rq_solid_nodepth->setShaderProgram(shaderProgram);
	rq_solid_nodepth->setSamplerState(sampler_desc);
	blend_desc.bBlendEnable = false;
	rq_solid_nodepth->setBlendState(blend_desc);
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = false;
	depth_desc.depthFunc = DepthStencilState::CF_LESS;
	rq_solid_nodepth->setDepthState(depth_desc);
	rq_solid_nodepth->setRasterizerState(rasterizer_desc);
	rq_solid_nodepth->setRenderInput(m_skinedVertElement);
	mSkinedRenderQueues[SRP_SOLID_NODEPTH] = rq_solid_nodepth;

	SkinedRenderQueue* rq_translucent = LordNew SkinedRenderQueue(SRP_TRANSPARENT);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcSkined_Transparent");
	rq_translucent->setShaderProgram(shaderProgram);
	rq_translucent->setSamplerState(sampler_desc);
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
	rq_translucent->setBlendState(blend_desc);
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = true;
	rq_translucent->setDepthState(depth_desc);
	rasterizer_desc.cullMode = RasterizerState::CULL_NONE;
	rq_translucent->setRasterizerState(rasterizer_desc);
	rq_translucent->setRenderInput(m_skinedVertElement);
	mSkinedRenderQueues[SRP_TRANSPARENT] = rq_translucent;
}

void TessManager::createItemRenderQueues()
{
	LordAssert(!mItemRenderQueues[IRP_SOLID]);

	BlendState::BlendDesc blend_desc;					// blend
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer

	ItemRenderQueue* rq_solid = LordNew ItemRenderQueue(IRP_SOLID);
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcItem");
	rq_solid->setShaderProgram(shaderProgram);
	rq_solid->setBlendState(blend_desc);
	rq_solid->setDepthState(depth_desc);
	rq_solid->setRasterizerState(rasterizer_desc);
	rq_solid->setRenderInput(m_itemVertElement);
	mItemRenderQueues[IRP_SOLID] = rq_solid;

	ItemRenderQueue* rq_solid_nodepth = LordNew ItemRenderQueue(IRP_SOLID_NODEPTH);
	rq_solid_nodepth->setShaderProgram(shaderProgram);
	rq_solid_nodepth->setBlendState(blend_desc);
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = false;
	rq_solid_nodepth->setDepthState(depth_desc);
	rq_solid_nodepth->setRasterizerState(rasterizer_desc);
	rq_solid_nodepth->setRenderInput(m_itemVertElement);
	mItemRenderQueues[IRP_SOLID_NODEPTH] = rq_solid_nodepth;
}

void TessManager::createFxRenderQueues()
{
	LordAssert(!mFxRenderQueues[FRP_DEFAULT]);

	BlendState::BlendDesc blend_desc;					// blend
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	SamplerState::SamplerDesc sampler_desc;				// sampler

	FxRenderQueue* rq_default = LordNew FxRenderQueue(FRP_DEFAULT);
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Fx");
	rq_default->setShaderProgram(shaderProgram);
	sampler_desc.minFilter = SamplerState::FO_POINT;
	sampler_desc.magFilter = SamplerState::FO_POINT;
	sampler_desc.mipFilter = SamplerState::FO_LINEAR;
	rq_default->setSamplerState(sampler_desc);
	SamplerState::SamplerDesc sampler_desc_lightmap;
	sampler_desc_lightmap.minFilter = SamplerState::FO_LINEAR;
	sampler_desc_lightmap.magFilter = SamplerState::FO_LINEAR;
	sampler_desc_lightmap.mipFilter = SamplerState::FO_NONE;
	sampler_desc_lightmap.addrUMode = SamplerState::AM_CLAMP;
	sampler_desc_lightmap.addrVMode = SamplerState::AM_CLAMP;
	rq_default->setSamplerState_lightmap(sampler_desc_lightmap);
	rq_default->setBlendState(blend_desc);
	rq_default->setDepthState(depth_desc);
	rasterizer_desc.cullMode = RasterizerState::CULL_NONE;
	rq_default->setRasterizerState(rasterizer_desc);
	rq_default->setRenderInput(m_fxVertElement);

	mFxRenderQueues[FRP_DEFAULT] = rq_default;

	FxRenderQueue* rq_alpha = LordNew FxRenderQueue(FRP_ALPHA_CHANEL);
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Fx");
	rq_alpha->setShaderProgram(shaderProgram);
	rq_alpha->setSamplerState(sampler_desc);
	rq_alpha->setSamplerState_lightmap(sampler_desc_lightmap);
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
	blend_desc.dstAlphaBlend = BlendState::BF_ONE;
	rq_alpha->setBlendState(blend_desc);
	depth_desc.bWriteDepth = false;
	rq_alpha->setDepthState(depth_desc);
	rasterizer_desc.cullMode = RasterizerState::CULL_NONE;
	rq_alpha->setRasterizerState(rasterizer_desc);
	rq_alpha->setRenderInput(m_fxVertElement);

	mFxRenderQueues[FRP_ALPHA_CHANEL] = rq_alpha;
}

void TessManager::createBatchRenderQueues()
{
	LordAssert(!mBatchRenderQueues[STRP_SOLID]);

	BlendState::BlendDesc blend_desc;					// blend
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	SamplerState::SamplerDesc sampler_desc;				// sampler

	BatchRenderQueue* rq_soild = LordNew BatchRenderQueue(STRP_SOLID);
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcBatch");
	
	rq_soild->setShaderProgram(shaderProgram);
	rq_soild->setBlendState(blend_desc);
	rq_soild->setDepthState(depth_desc);
	rq_soild->setRasterizerState(rasterizer_desc);
	sampler_desc.minFilter = SamplerState::FO_POINT;
	sampler_desc.magFilter = SamplerState::FO_POINT;
	sampler_desc.mipFilter = SamplerState::FO_LINEAR;
	rq_soild->setSamplerState(sampler_desc);
	rq_soild->setRenderInput(m_batchVertElement);
	mBatchRenderQueues[STRP_SOLID] = rq_soild;
}

void TessManager::createEnchantRenderQueues()
{
	LordAssert(!mEnchantItemRenderQueue);

	mEnchantItemRenderQueue = LordNew EnchantItemRenderQueue();
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcItemEnchant");
	mEnchantItemRenderQueue->setShaderProgram(shaderProgram);
	SamplerState::SamplerDesc sampler_desc;
	sampler_desc.minFilter = SamplerState::FO_LINEAR;
	sampler_desc.magFilter = SamplerState::FO_LINEAR;
	sampler_desc.mipFilter = SamplerState::FO_LINEAR;
	sampler_desc.addrUMode = SamplerState::AM_CLAMP;
	sampler_desc.addrVMode = SamplerState::AM_CLAMP;
	mEnchantItemRenderQueue->setSamplerState(sampler_desc);
	BlendState::BlendDesc blend_desc;					// blend
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_ONE;
	mEnchantItemRenderQueue->setBlendState(blend_desc);
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = true;
	depth_desc.depthFunc = DepthStencilState::CF_EQUAL;
	DepthStencilState::DepthStencilDesc depth_noDepth_desc;
	depth_noDepth_desc.bWriteDepth = false;
	depth_noDepth_desc.bDepthEnable = false;
	mEnchantItemRenderQueue->setDepthState(depth_desc);
	mEnchantItemRenderQueue->setDepthState_noDepth(depth_noDepth_desc);
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	mEnchantItemRenderQueue->setRasterizerState(rasterizer_desc);
	mEnchantItemRenderQueue->setRenderInput(m_itemVertElement);

	LordAssert(!mEnchantSkinedRenderQueue);

	mEnchantSkinedRenderQueue = LordNew EnchantSkinedRenderQueue();
	shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcSkined_Enchant");
	mEnchantSkinedRenderQueue->setShaderProgram(shaderProgram);
	mEnchantSkinedRenderQueue->setSamplerState(sampler_desc);
	mEnchantSkinedRenderQueue->setBlendState(blend_desc);
	mEnchantSkinedRenderQueue->setDepthState(depth_desc);
	mEnchantSkinedRenderQueue->setDepthState_noDepth(depth_noDepth_desc);
	mEnchantSkinedRenderQueue->setRasterizerState(rasterizer_desc);
	mEnchantSkinedRenderQueue->setRenderInput(m_skinedVertElement);
}

void TessManager::createFontRenderQueue()
{
	LordAssert(!mFontRenderQueue);

	FontRenderQueue* rq_font = LordNew FontRenderQueue();
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("mcFont");
	rq_font->setShaderProgram(shaderProgram);
	SamplerState::SamplerDesc sampler_desc;
	sampler_desc.minFilter = SamplerState::FO_LINEAR;
	sampler_desc.magFilter = SamplerState::FO_LINEAR;
	sampler_desc.mipFilter = SamplerState::FO_NONE;
	sampler_desc.addrUMode = SamplerState::AM_CLAMP;
	sampler_desc.addrVMode = SamplerState::AM_CLAMP;
	rq_font->setSamplerState(sampler_desc);
	BlendState::BlendDesc blend_desc;					// blend
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
	blend_desc.dstAlphaBlend = BlendState::BF_ONE;
	rq_font->setBlendState(blend_desc);
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = true;
	rq_font->setDepthState(depth_desc);
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	rq_font->setRasterizerState(rasterizer_desc);
	rq_font->setRenderInput(m_fontVertElement);
	mFontRenderQueue = rq_font;
}

void TessManager::createRasterizerRenderQueue()
{
	LordAssert(!mRasterizerRenderQueue);

	RasterizerRenderQueue* rq_raster = LordNew RasterizerRenderQueue();
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Line");
	rq_raster->setShaderProgram(shaderProgram);
	BlendState::BlendDesc blend_desc;					// blend
	blend_desc.bBlendEnable = true;
	blend_desc.bBlendEnable = true;
	blend_desc.srcBlend = BlendState::BF_SRC_ALPHA;
	blend_desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	blend_desc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
	blend_desc.dstAlphaBlend = BlendState::BF_ONE;
	rq_raster->setBlendState(blend_desc);
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	depth_desc.bWriteDepth = false;
	depth_desc.bDepthEnable = false;
	rq_raster->setDepthState(depth_desc);
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	rasterizer_desc.cullMode = RasterizerState::CULL_NONE;
	rq_raster->setRasterizerState(rasterizer_desc);
	rq_raster->setRenderInput(m_lineVertElement);
	mRasterizerRenderQueue = rq_raster;
}

void TessManager::createLineRenderQueue()
{
	LordAssert(!mLineRenderQueue[LRP_SELECTED]);

	LineRenderQueue* rq_line = LordNew LineRenderQueue();
	ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Line");
	rq_line->setShaderProgram(shaderProgram);
	BlendState::BlendDesc blend_desc;					// blend
	rq_line->setBlendState(blend_desc);
	DepthStencilState::DepthStencilDesc depth_desc;		// depth
	depth_desc.bWriteDepth = true;
	depth_desc.bDepthEnable = true;
	rq_line->setDepthState(depth_desc);
	RasterizerState::RasterizerDesc rasterizer_desc;	// rasterizer
	rasterizer_desc.lineWidth = 2;
	rq_line->setRasterizerState(rasterizer_desc);
	rq_line->setRenderInput(m_lineVertElement);
	mLineRenderQueue[LRP_SELECTED] = rq_line;

	LordAssert(!mLineRenderQueue[LRP_VIRSUALHELP]);

	rq_line = LordNew LineRenderQueue();
	rq_line->setShaderProgram(shaderProgram);
	rq_line->setBlendState(blend_desc);
	depth_desc.bWriteDepth = true;
	depth_desc.bDepthEnable = true;
	rq_line->setDepthState(depth_desc);
	rasterizer_desc.lineWidth = 1;
	rq_line->setRasterizerState(rasterizer_desc);
	rq_line->setRenderInput(m_lineVertElement);
	mLineRenderQueue[LRP_VIRSUALHELP] = rq_line;
}

void TessManager::createRenderableSet()
{
	if (mRenderaQueueCreated)
		return;

	RenderInput::VertexElement posElm, colorElm, uvElm, uv4Elm, lightmapUVElm, normalElm, boneElm;
	posElm.semantic = RenderInput::VS_POSITION;
	posElm.pixFmt = PF_RGB32_FLOAT;
	colorElm.semantic = RenderInput::VS_COLOR;
	colorElm.pixFmt = PF_RGBA8_UNORM;
	uvElm.semantic = RenderInput::VS_TEXCOORD;
	uvElm.pixFmt = PF_RG32_FLOAT;
	uv4Elm.semantic = RenderInput::VS_TEXCOORD;
	uv4Elm.pixFmt = PF_RGBA32_FLOAT;
	lightmapUVElm.semantic = RenderInput::VS_TEXCOORD1;
	lightmapUVElm.pixFmt = PF_BGRA8_UNORM;
	normalElm.semantic = RenderInput::VS_NORMAL;
	normalElm.pixFmt = PF_RGB32_FLOAT;
	boneElm.semantic = RenderInput::VS_BLENDINDICES;
	boneElm.pixFmt = PF_R32_FLOAT;
	
	RenderInput::VertexElement posElmEx, normalElmEx, uvElmEx, colorElmEx;
	posElmEx.semantic = RenderInput::VS_POSITION;
	posElmEx.pixFmt = PF_RGBA8_UINT;
	normalElmEx.semantic = RenderInput::VS_NORMAL;
	normalElmEx.pixFmt = PF_RGBA8_UINT;
	uvElmEx.semantic = RenderInput::VS_TEXCOORD;
	uvElmEx.pixFmt = PF_RG16_UINT;
	colorElmEx.semantic = RenderInput::VS_COLOR;
	colorElmEx.pixFmt = PF_RGBA8_UINT;

	m_tessVertElement.push_back(posElmEx);
	m_tessVertElement.push_back(uvElmEx);
	m_tessVertElement.push_back(colorElmEx);

	m_skinedVertElement.push_back(posElmEx);
	m_skinedVertElement.push_back(uvElmEx);
	m_skinedVertElement.push_back(colorElmEx);

	m_itemVertElement.push_back(posElmEx);
	m_itemVertElement.push_back(uvElmEx);
	m_itemVertElement.push_back(colorElm);

	m_batchVertElement.push_back(posElm);
	m_batchVertElement.push_back(uvElm);
	m_batchVertElement.push_back(colorElm);

	m_lineVertElement.push_back(posElm);
	m_lineVertElement.push_back(colorElm);

	m_fontVertElement.push_back(posElm);
	m_fontVertElement.push_back(colorElm);
	m_fontVertElement.push_back(uvElm);

	m_fxVertElement.push_back(posElm);
	m_fxVertElement.push_back(colorElm);
	m_fxVertElement.push_back(uvElm);
	m_fxVertElement.push_back(lightmapUVElm);
	
	createTessRenderQueues();
	createSkinedRenderQueues();
	createBatchRenderQueues();
	createItemRenderQueues();
	createFxRenderQueues();
	createEnchantRenderQueues();
	createLineRenderQueue();
	createRasterizerRenderQueue();
	createFontRenderQueue();
	createCommonIndexBuffer();
	
	mRenderaQueueCreated = true;
}

void TessManager::createCommonIndexBuffer()
{
	ui16* indexSysMemory = (ui16*)(LordMalloc(sTessQuadSize * 6 * sizeof(ui16)));
	Buffer idxBuff(sTessQuadSize * 6 * sizeof(ui16), indexSysMemory);
	m_commonIndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, idxBuff);
	
	// fill the common index buffer.
	for (int i = 0; i < sTessQuadSize; ++i)
	{
		indexSysMemory[i * 6 + 0] = i * 4 + 0;
		indexSysMemory[i * 6 + 1] = i * 4 + 2;
		indexSysMemory[i * 6 + 2] = i * 4 + 1;

		indexSysMemory[i * 6 + 3] = i * 4 + 0;
		indexSysMemory[i * 6 + 4] = i * 4 + 3;
		indexSysMemory[i * 6 + 5] = i * 4 + 2;
	}

	m_commonIndexBuffer->updateSubData(0, idxBuff);
	LordFree(indexSysMemory);

	indexSysMemory = (ui16*)(LordMalloc(sTessQuadSize * sizeof(ui16) * 8));
	Buffer indexBuff_line(sTessQuadSize * 8 * sizeof(ui16), indexSysMemory);
	m_commonIndexBuffer_line = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, indexBuff_line);
	for (int i = 0; i < sTessQuadSize; ++i)
	{
		indexSysMemory[i * 8 + 0] = i * 4 + 0;
		indexSysMemory[i * 8 + 1] = i * 4 + 1;

		indexSysMemory[i * 8 + 2] = i * 4 + 1;
		indexSysMemory[i * 8 + 3] = i * 4 + 2;

		indexSysMemory[i * 8 + 4] = i * 4 + 2;
		indexSysMemory[i * 8 + 5] = i * 4 + 3;

		indexSysMemory[i * 8 + 6] = i * 4 + 3;
		indexSysMemory[i * 8 + 7] = i * 4 + 0;
	}
	m_commonIndexBuffer_line->updateSubData(0, indexBuff_line);
	LordFree(indexSysMemory);

	indexSysMemory = (ui16*)(LordMalloc(sTessFanSize * 3 * sizeof(ui16)));
	Buffer indexBuff_fan(sTessFanSize * 3 * sizeof(ui16), indexSysMemory);
	m_commonIndexBuffer_fan = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, indexBuff_fan);
	for (int i = 0; i < sTessFanSize; ++i)
	{
		indexSysMemory[i * 3 + 0] = 0;
		indexSysMemory[i * 3 + 1] = i + 1;
		indexSysMemory[i * 3 + 2] = i + 2;
	}
	m_commonIndexBuffer_fan->updateSubData(0, indexBuff_fan);
	LordFree(indexSysMemory);
}

void TessManager::destroyRenderableSet()
{
	if (!mRenderaQueueCreated)
		return;
	
	m_tessVertElement.clear();
	m_skinedVertElement.clear();
	m_itemVertElement.clear();
	m_batchVertElement.clear();
	m_lineVertElement.clear();
	m_fxVertElement.clear();

	for (int i = 0; i < STRP_COUNT; ++i)
	{
		LordSafeDelete(mBatchRenderQueues[i]);
	}

	for (int i = 0; i < FRP_COUNT; ++i)
	{
		LordSafeDelete(mFxRenderQueues[i]);
	}

	for (int i = 0; i < IRP_COUNT; ++i)
	{
		LordSafeDelete(mItemRenderQueues[i]);
	}

	for (int i = 0; i < SRP_COUNT; ++i)
	{
		LordSafeDelete(mSkinedRenderQueues[i]);
	}

	for (int i = 0; i < TRP_COUNT; ++i)
	{
		LordSafeDelete(mTessRenderQueues[i]);
	}
	
	for (int i = 0; i < LRP_COUNT; ++i)
	{
		LordSafeDelete(mLineRenderQueue[i]);
	}

	LordSafeDelete(mRasterizerRenderQueue);
	LordSafeDelete(mEnchantItemRenderQueue);
	LordSafeDelete(mEnchantSkinedRenderQueue);
	LordSafeDelete(mFontRenderQueue);

	LordSafeDelete(m_commonIndexBuffer);
	LordSafeDelete(m_commonIndexBuffer_line);
	LordSafeDelete(m_commonIndexBuffer_fan);

	mRenderaQueueCreated = false;
}

TessRenderable* TessManager::createTessRenderable(Texture* texture, TESS_RENDER_PASS rg)
{
	LordAssert(texture);
	LordAssert(rg > TRP_INVALID);
	LordAssert(rg < TRP_COUNT);

	TessRenderable* pRenderable = LordNew TessRenderable();
	pRenderable->setQuadPoolSize(sTessQuadSize);
	if (mTessLineMode)
		pRenderable->setIndexBuffer(m_commonIndexBuffer_line);
	else
		pRenderable->setIndexBuffer(m_commonIndexBuffer);

	pRenderable->setMaterialParam(texture, rg);
	
	return pRenderable;
}

SkinedRenderable* TessManager::createSkinedRenderable(Texture* texture, SKINED_RENDER_PASS rg)
{
	LordAssert(texture);
	LordAssert(rg > SRP_INVALID);
	LordAssert(rg < SRP_COUNT);

	SkinedRenderable* pRenderable = NULL;
	if (rg == SRP_ENCHANT || rg == SRP_ENCHANT_NODEPTH)
		pRenderable = LordNew EnchantSkinedRenderable();
	else
		pRenderable = LordNew SkinedRenderable();

	pRenderable->setQuadPoolSize(sSkinedQuadSize);
	if (mSkinedLineMode)
		pRenderable->setIndexBuffer(m_commonIndexBuffer_line);
	else
		pRenderable->setIndexBuffer(m_commonIndexBuffer);

	pRenderable->setMaterialParam(texture, rg);
	return pRenderable;
}

BatchRenderable* TessManager::createBatchRenderable(Texture* texture, BATCH_RENDER_PASS rg)
{
	LordAssert(texture);
	LordAssert(rg > STRP_INVALID);
	LordAssert(rg < STRP_COUNT);

	BatchRenderable* pRenderable = LordNew BatchRenderable();
	pRenderable->setQuadPoolSize(sSkinedQuadSize);
	if (mSkinedLineMode)
		pRenderable->setIndexBuffer(m_commonIndexBuffer_line);
	else
		pRenderable->setIndexBuffer(m_commonIndexBuffer);

	pRenderable->setMaterialParam(texture, rg);
	return pRenderable;
}

LineRenderable* TessManager::createLineRenderable(LINE_RENDER_PASS rg)
{
	LineRenderable* pRenderable = LordNew LineRenderable(rg);
	pRenderable->setPointPoolSize(sLinePointSize);
	
	return pRenderable;
}

RasterizerRenderable* TessManager::createRasterizerRenderable()
{
	RasterizerRenderable* pRenderable = LordNew RasterizerRenderable();
	pRenderable->setQuadPoolSize(sTessQuadSize);
	if (mTessLineMode)
		pRenderable->setIndexBuffer(m_commonIndexBuffer_line);
	else
		pRenderable->setIndexBuffer(m_commonIndexBuffer);

	return pRenderable;
}

FontRenderable* TessManager::createFontRenderable()
{
	FontRenderable* pRenderable = LordNew FontRenderable();
	pRenderable->setQuadPoolSize(sCharQuadSize);
	if (mFontLineMode)
		pRenderable->setIndexBuffer(m_commonIndexBuffer_line);
	else
		pRenderable->setIndexBuffer(m_commonIndexBuffer);
	return pRenderable;
}

ItemRenderMesh* TessManager::createItemRenderMesh(ITEM_RENDER_PASS rg)
{
	LordAssert(rg > IRP_INVALID);
	LordAssert(rg < IRP_COUNT);

	ItemRenderMesh* pItemMesh = LordNew ItemRenderMesh();
	pItemMesh->setQuadPoolSize(sItemQuadSize);
	if (mItemLineMode)
		pItemMesh->setIndexBuffer(m_commonIndexBuffer_line);
	else
		pItemMesh->setIndexBuffer(m_commonIndexBuffer);

	return pItemMesh;
}

FxRenderable* TessManager::createFxRenderable(Texture* texture, FX_RENDER_PASS rg)
{
	LordAssert(rg > FRP_INVALID);
	LordAssert(rg < FRP_COUNT);

	FxRenderable* pRenderable = LordNew FxRenderable();
	pRenderable->setQuadPoolSize(sFxQuadSize);
	if (mFxLineMode)
		pRenderable->setIndexBuffer(m_commonIndexBuffer_line);
	else
		pRenderable->setIndexBuffer(m_commonIndexBuffer);
	pRenderable->setMaterialParam(texture, rg);
	return pRenderable;
}

void TessManager::destroyTessRenderable(TessRenderable* renderable)
{
	LordAssert(renderable);
	LordDelete(renderable);
}

void TessManager::destroySkinedRenderable(SkinedRenderable* renderable)
{
	LordAssert(renderable);
	LordDelete(renderable);
}

void TessManager::destroyBatchRenderable(BatchRenderable* renderable)
{
	LordAssert(renderable);
	LordDelete(renderable);
}

void TessManager::destroyLineRenderable(LineRenderable* renderable)
{
	LordAssert(renderable);
	LordDelete(renderable);
}

void TessManager::destroyRasterizerRenderalbe(RasterizerRenderable* renderable)
{
	LordAssert(renderable);
	LordDelete(renderable);
}

void TessManager::destroyItemRenderMesh(ItemRenderMesh* pItemMesh)
{
	LordAssert(pItemMesh);
	LordDelete(pItemMesh);
}

void TessManager::destroyFxRenderable(FxRenderable* renderable)
{
	LordAssert(renderable);
	LordDelete(renderable);
}

void TessManager::destroyFontRenderable(FontRenderable* renderable)
{
	LordAssert(renderable);
	LordDelete(renderable);
}

void TessManager::beginRender()
{
	for (int i = 0; i < TRP_COUNT; ++i)
	{
		mTessRenderQueues[i]->beginRender();
	}
	for (int i = 0; i < SRP_COUNT; ++i)
	{
		if (mSkinedRenderQueues[i])
			mSkinedRenderQueues[i]->beginRender();
	}
	for (int i = 0; i < IRP_COUNT; ++i)
	{
		if (mItemRenderQueues[i])
			mItemRenderQueues[i]->beginRender();
	}
	for (int i = 0; i < FRP_COUNT; ++i)
	{
		if (mFxRenderQueues[i])
			mFxRenderQueues[i]->beginRender();
	}
	for (int i = 0; i < STRP_COUNT; ++i)
	{
		if (mBatchRenderQueues[i])
			mBatchRenderQueues[i]->beginRender();
	}

	mRasterizerRenderQueue->beginRender();

	for (int i = 0; i < LRP_COUNT; ++i)
	{
		if (mLineRenderQueue[i])
			mLineRenderQueue[i]->beginRender();
	}
	mEnchantItemRenderQueue->beginRender();
	mEnchantSkinedRenderQueue->beginRender();
	mFontRenderQueue->beginRender();
}

void TessManager::endRender1(const Matrix4* viewproj, bool renderPrev)
{
	if (!mEnableRender)
	{
		beginRender();
		return;
	}

	SceneManager::Instance()->refleshMainPositon();

	if (renderPrev)
	{
		for (TessObjectArr::iterator it = m_prevObjects.begin(); it != m_prevObjects.end(); ++it)
		{
			(*it)->render();
		}
	}

	// for render all the solid-block renderables.
	for (int i = 0; i < TRP_TRANSPARENT_3D; ++i)
	{
		mTessRenderQueues[i]->endRender(mTessLineMode, viewproj);
	}

	for (int i = 0; i < FRP_COUNT; ++i)
	{
		if (mFxRenderQueues[i])
			mFxRenderQueues[i]->endRender(mFxLineMode, viewproj);
	}

	// render all the skined renderables.
	for (int i = 0; i < SRP_TRANSPARENT; ++i)
	{
		if (mSkinedRenderQueues[i])
			mSkinedRenderQueues[i]->endRender(mSkinedLineMode, viewproj);
	}

	for (int i = 0; i < STRP_COUNT; ++i)
	{
		if (mBatchRenderQueues[i])
			mBatchRenderQueues[i]->endRender(mItemLineMode, viewproj);
	}
	// render all the actor. powered by echo-engine. manually inserted.
}

void TessManager::endRender2(const Matrix4* viewproj, bool renderPrev)
{
	// then render transparent-block 3d renderables. only write z.
	// change this renderQueue to z
	for (int i = TRP_TRANSPARENT_3D; i <= TRP_TRANSPARENT_3DUV; ++i)
	{
		mTessRenderQueues[i]->changeCurrentState(false);
		mTessRenderQueues[i]->endRender(mTessLineMode, viewproj);
		// change this renderQueue to transparent mode.
		mTessRenderQueues[i]->changeCurrentState(true);
	}

	for (int i = 0; i < IRP_SOLID_NODEPTH; ++i)
	{
		if (mItemRenderQueues[i])
			mItemRenderQueues[i]->endRender(mItemLineMode, viewproj);
	}

	// render all the transparent-block renderables.
	for (int i = TRP_TRANSPARENT_3D; i < TRP_COUNT; ++i)
	{
		mTessRenderQueues[i]->endRender(mTessLineMode, viewproj);
	}

	mFontRenderQueue->endRender(mFontLineMode, viewproj);

	// render the line.
	mLineRenderQueue[LRP_SELECTED]->endRender(viewproj);

	mTessRenderQueues[TRP_TRANSPARENT_HL]->endRender(mTessLineMode, viewproj);

	for (int i = SRP_SOLID_NODEPTH; i < SRP_COUNT; ++i)
	{
		if (mSkinedRenderQueues[i])
			mSkinedRenderQueues[i]->endRender(mSkinedLineMode, viewproj);
	}
	for (int i = IRP_SOLID_NODEPTH; i < IRP_COUNT; ++i)
	{
		if (mItemRenderQueues[i])
			mItemRenderQueues[i]->endRender(mItemLineMode, viewproj);
	}
	mRasterizerRenderQueue->endRender(mTessLineMode, viewproj);

	for (TessObjectArr::iterator it = m_postObjects.begin(); it != m_postObjects.end(); ++it)
	{
		(*it)->render();
	}

	// render the enchantment equipments,tools,weapons.
	mEnchantItemRenderQueue->endRender(mItemLineMode, viewproj);
	mEnchantSkinedRenderQueue->endRender(mSkinedLineMode, viewproj);

	mSkinedRenderQueues[SRP_TRANSPARENT]->endRender(mItemLineMode, viewproj);

	mLineRenderQueue[LRP_VIRSUALHELP]->endRender(viewproj);

	Renderer::BGCOLOR = SceneManager::Instance()->getFogColor();
	beginRender();
}

void TessManager::addTessRenderable(TessRenderable* renderable)
{
	TESS_RENDER_PASS pass = renderable->getRenderGroup();
	mTessRenderQueues[pass]->addRenderable(renderable);
}

void TessManager::addSkinedRenderable(SkinedRenderable* renderable)
{
	SKINED_RENDER_PASS pass = renderable->getRenderGroup();
	mSkinedRenderQueues[pass]->addRenderable(renderable);
}

void TessManager::addRasterizerRenderable(RasterizerRenderable* renderable)
{
	mRasterizerRenderQueue->addRenderable(renderable);
}

void TessManager::addLineRenderable(LineRenderable* renderable)
{
	LINE_RENDER_PASS pass = renderable->getRenderGroup();
	mLineRenderQueue[pass]->addRenderable(renderable);
}

void TessManager::addItemRenderalbe(ItemRenderable* renderable)
{
	ITEM_RENDER_PASS pass = renderable->getRenderGroup();
	mItemRenderQueues[pass]->addRenderable(renderable);
}

void TessManager::addEnchantRenderable(ItemRenderable* renderable)
{
	mEnchantItemRenderQueue->addRenderable(renderable);
}

void TessManager::addEnchantRenderable(SkinedRenderable* renderable)
{
	mEnchantSkinedRenderQueue->addRenderable(renderable);
}

void TessManager::addFxRenderable(FxRenderable* renderable)
{
	FX_RENDER_PASS pass = renderable->getRenderGroup();
	mFxRenderQueues[pass]->addRenderable(renderable);
}

void TessManager::addFontRenderable(FontRenderable* renderable)
{
	mFontRenderQueue->addRenderable(renderable);
}

void TessManager::addBatchRenderable(BatchRenderable* renderable)
{
	BATCH_RENDER_PASS pass = renderable->getRenderGroup();
	mBatchRenderQueues[pass]->addRenderable(renderable);
}

void TessManager::tick(float dt)
{
	m_lastFrame += dt;
	while (m_lastFrame > m_frameTime)
	{
		m_lastFrame -= m_frameTime;
		m_frameCount++;
	}
}

void TessManager::setLightMap(Texture* lightmap)
{
	m_lightMap = lightmap;
	for (int i = 0; i < TRP_COUNT; ++i)
		mTessRenderQueues[i]->setLightmap(lightmap);
}


TessObject* TessManager::createTessObject_Sky()
{
	TessObject* pResult = LordNew TessObject_Sky(StringUtil::BLANK, this);
	pResult->init();
	m_prevObjects.push_back(pResult);
	return pResult;
}

TessObject* TessManager::createTessObject_Cloud()
{
	TessObject* pResult = LordNew TessObject_Cloud(StringUtil::BLANK, this);
	pResult->init();
	m_prevObjects.push_back(pResult);
	return pResult;
}

TessObject* TessManager::createTessObject_Sun()
{
	TessObject* pResult = LordNew TessObject_Sun(StringUtil::BLANK, this);
	pResult->init();
	m_prevObjects.push_back(pResult);
	return pResult;
}

TessObject* TessManager::createTessObject_Raster()
{
	TessObject* pResult = LordNew TessObject_Sun(StringUtil::BLANK, this);
	pResult->init();
	m_postObjects.push_back(pResult);
	return pResult;
}

TessObject* TessManager::createTessObject_Mark()
{
	TessObject* pResult = LordNew TessObject_Sun(StringUtil::BLANK, this);
	pResult->init();
	m_postObjects.push_back(pResult);
	return pResult;
}

TessObject* TessManager::createTessObject_Glow()
{
	TessObject* pResult = LordNew TessObject_Glow(StringUtil::BLANK, this);
	pResult->init();
	m_prevObjects.push_back(pResult);
	return pResult;
}

TessObject* TessManager::createTessObject_PoisionCircle()
{
	TessObject* pResult = LordNew TessObject_PoisonCircle(StringUtil::BLANK, this);
	pResult->init();
	m_postObjects.push_back(pResult);
	return pResult;
}

void TessManager::destroyPrevObject(TessObject* tessQuad)
{
	TessObjectArr::iterator it = std::find(m_prevObjects.begin(), m_prevObjects.end(), tessQuad);
	if (it == m_prevObjects.end())
		return;
	LordDelete(*it);
	m_prevObjects.erase(it);
}

void TessManager::destroyPostObject(TessObject* tessQuad)
{
	TessObjectArr::iterator it = std::find(m_postObjects.begin(), m_postObjects.end(), tessQuad);
	if (it == m_postObjects.end())
		return;
	LordDelete(*it);
	m_postObjects.erase(it);
}

}

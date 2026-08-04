/********************************************************************
filename: 	TessManager.h
file path:	dev\engine\Src\Core\Tessellator\

version:	1
author:		ajohn
company:	supernano
datatime:   2016-11-29
*********************************************************************/

#ifndef __LORD_TESS_MANAGER_H__
#define __LORD_TESS_MANAGER_H__

#include "Core.h"

#include "Object/Singleton.h"
#include "Util/StringUtil.h"
#include "Object/Singleton.h"
#include "Render/RenderState.h"
#include "Render/RenderQueue.h"

namespace LORD
{
class TessRenderQueue;
class SkinedRenderQueue;
class LineRenderQueue;
class RasterizerRenderQueue;
class ItemRenderQueue;
class FxRenderQueue;
class BatchRenderQueue;
class EnchantItemRenderQueue;
class EnchantSkinedRenderQueue;
class FontRenderQueue;
class TessManager;
class TessObject;

typedef list<TessObject*>::type TessObjectArr;

class  LORD_CORE_API TessManager : public Singleton<TessManager>, public ObjectAlloc
{
public:
	static inline TessManager* Instance() { return ms_pSingleton; }

	TessManager(void);
	~TessManager(void);

	TessMaterial* createMaterial();
	TessMaterial* cloneMaterial(TessMaterial* material);
	void destroyMaterial(TessMaterial* material);

	TessObject* createTessObject_Sky();
	TessObject* createTessObject_Cloud();
	TessObject* createTessObject_Sun();
	TessObject* createTessObject_Raster();
	TessObject* createTessObject_Mark();
	TessObject* createTessObject_Glow();
	TessObject* createTessObject_PoisionCircle();
	void destroyPrevObject(TessObject* tessQuad);
	void destroyPostObject(TessObject* tessQuad);

	SamplerState* getSamplerState();
	GPUBuffer*	getIndexBuffer() { return m_commonIndexBuffer;	}
	GPUBuffer*  getIndexBuffer_line() { return m_commonIndexBuffer_line; }
	GPUBuffer*  getIndexBuffer_fan() { return m_commonIndexBuffer_fan; }

	void createRenderableSet();
	void destroyRenderableSet();
	void setLineMode(bool linemode) { mTessLineMode = linemode; }
	bool isLineMode() { return mTessLineMode; }

	TessRenderable* createTessRenderable(Texture* texture, TESS_RENDER_PASS rg);
	void destroyTessRenderable(TessRenderable* renderable);

	SkinedRenderable* createSkinedRenderable(Texture* texture, SKINED_RENDER_PASS rg);
	void destroySkinedRenderable(SkinedRenderable* renderable);

	BatchRenderable* createBatchRenderable(Texture* texture, BATCH_RENDER_PASS rg);
	void destroyBatchRenderable(BatchRenderable* renderable);

	ItemRenderMesh* createItemRenderMesh(ITEM_RENDER_PASS rg);
	void destroyItemRenderMesh(ItemRenderMesh* pItemMesh);
	
	FxRenderable* createFxRenderable(Texture* texture, FX_RENDER_PASS rg);
	void destroyFxRenderable(FxRenderable* renderable);

	LineRenderable* createLineRenderable(LINE_RENDER_PASS rg);
	void destroyLineRenderable(LineRenderable* renderable);

	RasterizerRenderable* createRasterizerRenderable();
	void destroyRasterizerRenderalbe(RasterizerRenderable* renderable);

	FontRenderable* createFontRenderable();
	void destroyFontRenderable(FontRenderable* renderable);

	void beginRender();
	void endRender1(const Matrix4* viewproj = NULL, bool renderPrev = true);
	void endRender2(const Matrix4* viewproj = NULL, bool renderPrev = true);

	void addTessRenderable(TessRenderable* renderable);
	void addSkinedRenderable(SkinedRenderable* renderable);
	void addLineRenderable(LineRenderable* renderable);
	void addRasterizerRenderable(RasterizerRenderable* renderable);
	void addItemRenderalbe(ItemRenderable* renderable);
	void addEnchantRenderable(ItemRenderable* renderable);
	void addEnchantRenderable(SkinedRenderable* renderable);
	void addFxRenderable(FxRenderable* renderable);
	void addFontRenderable(FontRenderable* renderable);
	void addBatchRenderable(BatchRenderable* renderable);
	TessRenderQueue* getTessRenderQueue(TESS_RENDER_PASS pass) { return mTessRenderQueues[pass]; }
	SkinedRenderQueue* getSkinedRenderQueue(SKINED_RENDER_PASS pass) { return mSkinedRenderQueues[pass]; }
	ItemRenderQueue* getItemRenderQueue(ITEM_RENDER_PASS pass) { return mItemRenderQueues[pass]; }
	FxRenderQueue* getFxRenderQueue(FX_RENDER_PASS pass) { return mFxRenderQueues[pass]; }
	FontRenderQueue* getFontRenderQueue() { return mFontRenderQueue; }
	BatchRenderQueue* getBatchRenderQueue(BATCH_RENDER_PASS pass) { return mBatchRenderQueues[pass]; }
	LineRenderQueue* getLineRenderQueue(LINE_RENDER_PASS pass) { return mLineRenderQueue[pass]; }
	RasterizerRenderQueue* getRasterizerRenderQueue() { return mRasterizerRenderQueue; }
	EnchantItemRenderQueue* getEnchantItemRenderQueue() { return mEnchantItemRenderQueue; }
	EnchantSkinedRenderQueue* getEnchantSkinedRenderQueue() { return mEnchantSkinedRenderQueue; }

	void setEnableRender(bool _val) { mEnableRender = _val; }
	bool getEnableRender() const { return mEnableRender; }
	void setLightMap(Texture* lightmap);
	void setViewBobing(const Matrix4& mat) { m_viewBobing = mat; }
	void setEnchantTexMat(int idx, const Matrix4& mat) { LordAssert(idx == 0 || idx == 1); m_enchantTexMat[idx] = mat; }

	const Matrix4& getViewBobing() const { return m_viewBobing; }
	const Matrix4& getEnchantTexMat(int idx) const { LordAssert(idx == 0 || idx == 1); return m_enchantTexMat[idx]; }

	void tick(float dt);
	int getFrameCount() const { return m_frameCount; }

protected:
	void createCommonIndexBuffer();
	void createTessRenderQueues();
	void createSkinedRenderQueues();
	void createItemRenderQueues();
	void createFxRenderQueues();
	void createBatchRenderQueues();
	void createLineRenderQueue();
	void createRasterizerRenderQueue();
	void createEnchantRenderQueues();
	void createFontRenderQueue();

public:
	static const size_t sTessQuadSize;
	static const size_t sTessFanSize;
	static const size_t sSkinedQuadSize;
	static const size_t sItemQuadSize;
	static const size_t sFxQuadSize;
	static const size_t sLinePointSize;
	static const size_t sCharQuadSize;

protected:
	bool	mRenderaQueueCreated;
	bool	mEnableRender;
	bool    mTessLineMode;
	bool	mSkinedLineMode;
	bool	mFxLineMode;
	bool	mFontLineMode;
	bool	mItemLineMode;
	
	RenderInput::VertexElementList m_tessVertElement;		// MC³¡¾°
	RenderInput::VertexElementList m_skinedVertElement;		//
	RenderInput::VertexElementList m_itemVertElement;
	RenderInput::VertexElementList m_fxVertElement;
	RenderInput::VertexElementList m_batchVertElement;
	RenderInput::VertexElementList m_lineVertElement;
	RenderInput::VertexElementList m_fontVertElement;

	TessRenderQueue*			mTessRenderQueues[TRP_COUNT];
	SkinedRenderQueue*			mSkinedRenderQueues[SRP_COUNT];
	ItemRenderQueue*			mItemRenderQueues[IRP_COUNT];
	FxRenderQueue*				mFxRenderQueues[FRP_COUNT];
	BatchRenderQueue*		mBatchRenderQueues[STRP_COUNT];
	LineRenderQueue*			mLineRenderQueue[LRP_COUNT];
	RasterizerRenderQueue*		mRasterizerRenderQueue;
	EnchantItemRenderQueue*		mEnchantItemRenderQueue;
	EnchantSkinedRenderQueue*	mEnchantSkinedRenderQueue;
	FontRenderQueue*			mFontRenderQueue;
	
	// the common index buffer. for all the renderables are stored quad. use the same index buffer.
	float		m_lastFrame;
	float		m_frameTime;
	int			m_frameCount;
	GPUBuffer*	m_commonIndexBuffer;
	GPUBuffer*	m_commonIndexBuffer_line;
	GPUBuffer*  m_commonIndexBuffer_fan;
	Texture*	m_lightMap;
	
	Matrix4		m_viewBobing;
	Matrix4		m_enchantTexMat[2];

	// for Tess_utils
	TessObjectArr	m_prevObjects;
	TessObjectArr	m_postObjects;
};

}
#endif
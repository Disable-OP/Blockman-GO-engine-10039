#include "TessObject.h"
#include "TessManager.h"
#include "Scene/SceneManager.h"
#include "Math/LordMath.h"
#include "Render/ShaderProgramManager.h"

namespace LORD
{

const size_t TessObject::sBuffSize = 128;

TessObject::TessObject(const String& name, TessManager* manager)
	: mManger(manager)
	, mName(name)
	, mParent(NULL)
	, mRenderInput(NULL)
	, mVertexBuffer(NULL)
	, mbTransparent(false)
	, mbAlphaAdd(false)
	, mColorMask(false)
	, mTestDepth(false)
	, mWriteDepth(false)
	, mDoubleFace(true)
	, mProgram(NULL)
	, mSysMemoryVertices(NULL)
	, mVertexSize(0)
	, mQuadNum(0)
	, mQuadPoolSize(0)
	, mLineMode(false)
	, mColor(Color::WHITE)
{
}

void TessObject::init()
{
	createMaterial();
	refleshMaterial();
}

TessObject::~TessObject()
{
	clear();
	LordSafeDelete(mMaterial);
}

void TessObject::attachTo(SceneNode* node)
{
	mParent = node;
	mMaterial->modifyShaderParam("matWorld", SPT_MAT4, (node != NULL) ? (void*)node->getWorldMatrixPtr() : (void*)&Matrix4::IDENTITY);
}

void TessObject::beginPolygon_clone(TessObject* prhs)
{
	clear();
	caculateVertexSize();

	mQuadPoolSize = prhs->mQuadPoolSize;
	mQuadNum = prhs->mQuadNum;
	mAABBLocal = prhs->mAABBLocal;
	mSysMemoryVertices = (float*)LordMalloc(mQuadPoolSize * 4 * mVertexSize);
	memcpy(mSysMemoryVertices, prhs->mSysMemoryVertices, mQuadNum * 4 * mVertexSize);
}

void TessObject::beginPolygon()
{
	clear();
	caculateVertexSize();

	mQuadPoolSize = sBuffSize;
	mSysMemoryVertices = (float*)LordMalloc(mQuadPoolSize * 4 * mVertexSize);
	mQuadNum = 0;
	
	mAABBLocal.reset();
}

void TessObject::setQuadPos(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3 vec4)
{
	if (mQuadPoolSize == mQuadNum)
	{
		float* newBuffer = (float*)LordMalloc(mVertexSize * 4 * (mQuadPoolSize + sBuffSize));
		memcpy(newBuffer, mSysMemoryVertices, mVertexSize * 4 * mQuadPoolSize);
		LordFree(mSysMemoryVertices);
		mSysMemoryVertices = newBuffer;
		mQuadPoolSize += sBuffSize;
	}

	setQuadPos_impl(vec1, vec2, vec3, vec4);
		
	mAABBLocal.addPoint(vec1);
	mAABBLocal.addPoint(vec2);
	mAABBLocal.addPoint(vec3);
	mAABBLocal.addPoint(vec4);
	mQuadNum++;
}

void TessObject::clear()
{
	LordSafeFree(mSysMemoryVertices);
	mQuadNum = 0;
	mVertexSize = 0;
	mQuadPoolSize = 0;
	LordSafeDelete(mVertexBuffer);
	LordSafeDelete(mRenderInput);
	mAABBLocal.reset();
}

void TessObject::render()
{
	if (mParent && mRenderInput && mParent->isVisible())
	{
		mMaterial->active();
		//if (m_type == 2)
		//	LordLogInfo("========> mark renderable rendering");
		LORD::Renderer::Instance()->render(mRenderInput);
	}
}

void TessObject::setTransparent(bool bTransparent)
{
	mbTransparent = bTransparent;
	refleshMaterial();
}

void TessObject::setTestDepth(bool testDetph)
{
	mTestDepth = testDetph;
	refleshMaterial();
}

void TessObject::setWriteDepth(bool writeDepth)
{
	mWriteDepth = writeDepth;
	refleshMaterial();
}

void TessObject::setColorMask(bool maskAll)
{
	mColorMask = maskAll;
	refleshMaterial();
}

void TessObject::setDoubleFace(bool doubleFace)
{
	mDoubleFace = doubleFace;
	refleshMaterial();
}

void TessObject::refleshBlendState()
{
	// BlendState use default
	BlendState::BlendDesc desc = mMaterial->getBlendState()->getDesc();
	if (mbTransparent)
	{
		desc.bBlendEnable = true;
		desc.srcBlend = BlendState::BF_SRC_ALPHA;
		desc.dstBlend = mbAlphaAdd? BlendState::BF_ONE : BlendState::BF_INV_SRC_ALPHA;
		desc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
		desc.dstAlphaBlend = mbAlphaAdd ? BlendState::BF_ONE : BlendState::BF_INV_SRC_ALPHA;
	}
	else
	{
		desc.bBlendEnable = false;
		desc.srcBlend = BlendState::BF_ONE;
		desc.dstBlend = BlendState::BF_ZERO;
		desc.srcAlphaBlend = BlendState::BF_ONE;
		desc.dstAlphaBlend = BlendState::BF_ZERO;
	}

	if (mColorMask)
	{
		desc.colorWriteMask = BlendState::CMASK_NONE;
	}
	else
	{
		desc.colorWriteMask = BlendState::CMASK_ALL;
	}
	mMaterial->setBlendState(desc);
}

void TessObject::refleshDepthState()
{
	DepthStencilState::DepthStencilDesc desc = mMaterial->getDepthState()->getDesc();
	if (mTestDepth && mWriteDepth)
	{
		desc.bDepthEnable = true;
		desc.bWriteDepth = true;
		desc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
		mMaterial->setDepthState(desc);
	}
	else if (mTestDepth && (!mWriteDepth))
	{
		desc.bDepthEnable = true;
		desc.bWriteDepth = false;
		desc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
		mMaterial->setDepthState(desc);
	}
	else if ((!mTestDepth) && mWriteDepth)
	{
		desc.bDepthEnable = false;
		desc.bWriteDepth = true;
		mMaterial->setDepthState(desc);
	}
	else
	{
		desc.bDepthEnable = false;
		desc.bWriteDepth = false;
		mMaterial->setDepthState(desc);
	}
}

void TessObject::refleshRasterizerState()
{
	RasterizerState::RasterizerDesc desc = mMaterial->getRasterizerState()->getDesc();
	if (mDoubleFace)
		desc.cullMode = RasterizerState::CULL_NONE;
	else
		desc.cullMode = RasterizerState::CULL_BACK;
	mMaterial->setRasterizerState(desc);
}

void TessObject::refleshMaterial()
{
	refleshBlendState();
	refleshDepthState();
	refleshRasterizerState();
}

TessObject_Sky::TessObject_Sky(const String& name, TessManager* manager)
	: TessObject(name, manager)
{}

void TessObject_Sky::createMaterial()
{
	mProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Sky");
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	mMaterial = LordNew Material;
	mMaterial->setShaderProgram(mProgram);
	mMaterial->beginShaderParams(4);
	mMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)&Matrix4::IDENTITY);
	mMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&pCamera->getViewProjMatrix());
	mMaterial->setShaderParam("ambient", SPT_VEC4, (void*)&mColor);
	mMaterial->setShaderParam("fogParam", SPT_VEC4, (void*)SceneManager::Instance()->getFogParamVec4Ptr(), 3);
	mMaterial->endShaderParams();
}

void TessObject_Sky::caculateVertexSize()
{
	mVertexSize = sizeof(TESSOBJ_SKY_FMT);
}

void TessObject_Sky::setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4)
{
	TESSOBJ_SKY_FMT* pFmt = (TESSOBJ_SKY_FMT*)(mSysMemoryVertices + mQuadNum * 4 * mVertexSize / sizeof(float));
	pFmt[0].pos = vec1;
	pFmt[1].pos = vec2;
	pFmt[2].pos = vec3;
	pFmt[3].pos = vec4;
}

void TessObject_Sky::endPolygon()
{
	Buffer vertBuff(mQuadNum * 4 * mVertexSize, mSysMemoryVertices);
	mVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_CPU_READ, vertBuff);
	LordSafeFree(mSysMemoryVertices);

	RenderInput::VertexElementList vertElms(1);
	vertElms[0].semantic = RenderInput::VS_POSITION;
	vertElms[0].pixFmt = PF_RGB32_FLOAT;

	mRenderInput = Renderer::Instance()->createRenderInput(vertElms, mProgram);
	mRenderInput->bindGPUBuffers(mVertexBuffer, mVertexSize, mManger->getIndexBuffer(), sizeof(ui16));
	mRenderInput->setVertexCount(mQuadNum * 4);
	mRenderInput->setIndexCount(mQuadNum * 6);
}

TessObject_Cloud::TessObject_Cloud(const String& name, TessManager* manager)
	: TessObject(name, manager)
	, mAmbient(Color::WHITE)
{
	mbTransparent = true;
	mTestDepth = true;
	mWriteDepth = false;
}

TessObject_Cloud::~TessObject_Cloud()
{
	for (SubSetModels::iterator it = m_models.begin(); it != m_models.end(); ++it)
	{
		SubsetModel& model = *it;
		LordSafeDelete(model.vertexBuffer);
		LordSafeFree(model.vertexMemory);
		model.quadNum = 0;
	}
	m_models.clear();
}

void TessObject_Cloud::createMaterial()
{
	mProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Cloud");
	Camera* pCamera = SceneManager::Instance()->getMainCamera();

	mMaterial = LordNew Material;
	mMaterial->setShaderProgram(mProgram);
	mMaterial->beginShaderParams(4);
	mMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)&Matrix4::IDENTITY);
	mMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&pCamera->getViewProjMatrix());
	mMaterial->setShaderParam("fogParam", SPT_VEC4, (void*)SceneManager::Instance()->getForParamFarVec4Ptr(), 3);
	mMaterial->setShaderParam("ambient", SPT_VEC4, (void*)&mColor, 1);
	mMaterial->endShaderParams();
}

void TessObject_Cloud::setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4)
{
	int modelIdx = mQuadNum / TessManager::sTessQuadSize;
	int quadIdx = mQuadNum % TessManager::sTessQuadSize;
	if (m_models.size() == modelIdx)
	{
		SubsetModel model;
		model.quadNum = 0;
		model.vertexMemory = (float*)LordMalloc(TessManager::sTessQuadSize * mVertexSize * 4);
		Buffer vertBuff(TessManager::sTessQuadSize * 4 * mVertexSize, mSysMemoryVertices);
		model.vertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_CPU_READ, vertBuff);
		m_models.push_back(model);
	}
	SubsetModel& model = m_models[modelIdx];
	TESSOBJ_CLOUD_FMT* pFmt = (TESSOBJ_CLOUD_FMT*)(model.vertexMemory + quadIdx * 4 * mVertexSize / sizeof(float));
	pFmt[0].pos = vec1;
	pFmt[1].pos = vec2;
	pFmt[2].pos = vec3;
	pFmt[3].pos = vec4;
	model.quadNum++;
}

void TessObject_Cloud::setQuadColor(int c1, int c2, int c3, int c4)
{
	int modelIdx = (mQuadNum-1) / TessManager::sTessQuadSize;
	int quadIdx = (mQuadNum-1) % TessManager::sTessQuadSize;
	SubsetModel& model = m_models[modelIdx];
	TESSOBJ_CLOUD_FMT* pFmt = (TESSOBJ_CLOUD_FMT*)(model.vertexMemory + quadIdx * 4 * mVertexSize / sizeof(float));
	pFmt[0].color = c1;
	pFmt[1].color = c2;
	pFmt[2].color = c3;
	pFmt[3].color = c4;
}

void TessObject_Cloud::caculateVertexSize()
{
	mVertexSize = sizeof(TESSOBJ_CLOUD_FMT);
}

void TessObject_Cloud::render()
{
	if (mParent && mRenderInput && mParent->isVisible() && mQuadNum > 0)
	{

		int modelIndex = (mQuadNum - 1) / TessManager::sTessQuadSize;
        
		// int modelIndex = mQuadNum / TessManager::sTessQuadSize;
        
		GPUBuffer* indexBuff = mLineMode ? mManger->getIndexBuffer_line() : mManger->getIndexBuffer();

		BlendState::BlendDesc blendDesc;
		blendDesc.colorWriteMask = BlendState::CMASK_NONE;
		DepthStencilState::DepthStencilDesc depthDesc;
		depthDesc.bDepthEnable = true;
		depthDesc.bWriteDepth = true;
		mMaterial->setBlendState(blendDesc);
		mMaterial->setDepthState(depthDesc);
		mMaterial->active();
		for (int i = 0; i < (int)m_models.size(); ++i)
		{
			SubsetModel& model = m_models[i];
			if (model.quadNum == 0)
				continue;
			mRenderInput->bindGPUBuffers(model.vertexBuffer, mVertexSize, indexBuff, sizeof(ui16));
			mRenderInput->setVertexCount(model.quadNum * 4);
			mRenderInput->setIndexCount(mLineMode ? (model.quadNum * 8) : (model.quadNum * 6));
			LORD::Renderer::Instance()->render(mRenderInput);
		}

		blendDesc.reset();
		blendDesc.bBlendEnable = true;
		blendDesc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
		blendDesc.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
		mMaterial->setBlendState(blendDesc);
		depthDesc.reset();
		depthDesc.bWriteDepth = false;
		depthDesc.bDepthEnable = true;
		depthDesc.depthFunc = DepthStencilState::CF_LESS_EQUAL;
		mMaterial->setDepthState(depthDesc);
		mMaterial->active();
		for (int i = 0; i < (int)m_models.size(); ++i)
		{
			SubsetModel& model = m_models[i];
			if (model.quadNum == 0)
				continue;
			mRenderInput->bindGPUBuffers(model.vertexBuffer, mVertexSize, indexBuff, sizeof(ui16));
			mRenderInput->setVertexCount(model.quadNum * 4);
			mRenderInput->setIndexCount(mLineMode ? (model.quadNum * 8) : (model.quadNum * 6));
			LORD::Renderer::Instance()->render(mRenderInput);
		}
	}
}

void TessObject_Cloud::beginPolygon()
{
	mQuadNum = 0;
	mVertexSize = 0;
	mQuadPoolSize = 65536;  // avoid TessObject::addQuadVertex to malloc vertex memory.
	caculateVertexSize();
	mAABBLocal.reset();
	for (size_t i = 0; i < m_models.size(); ++i)
	{
		m_models[i].quadNum = 0;
	}
}

void TessObject_Cloud::endPolygon()
{
	for (SubSetModels::iterator it = m_models.begin(); it != m_models.end(); ++it)
	{
		SubsetModel& model = *it;
		if (model.quadNum == 0)
			continue;
		Buffer buff(model.quadNum * 4 * mVertexSize, model.vertexMemory);
		model.vertexBuffer->updateSubData(0, buff);
	}

	if (!mRenderInput)
	{
		RenderInput::VertexElementList vertElms(2);
		vertElms[0].semantic = RenderInput::VS_POSITION;
		vertElms[0].pixFmt = PF_RGB32_FLOAT;
		vertElms[1].semantic = RenderInput::VS_COLOR;
		vertElms[1].pixFmt = PF_RGBA8_UNORM;

		mRenderInput = Renderer::Instance()->createRenderInput(vertElms, mProgram);
		mRenderInput->setTopologyType(mLineMode ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	}
}

TessObject_PoisonCircle::TessObject_PoisonCircle(const String& name, TessManager* manager)
	: TessObject(name, manager)
	, mTexture(NULL)
{
	mbTransparent = true;
	mbAlphaAdd = false;
	mTestDepth = true;
	mWriteDepth = false;
}

TessObject_PoisonCircle::~TessObject_PoisonCircle()
{
	TextureManager::Instance()->releaseResource(mTexture);
	mTexture = NULL;
}

void TessObject_PoisonCircle::createMaterial()
{
	mProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Sun"); // use Tess_Sun.
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	mMaterial = LordNew Material;
	mMaterial->setShaderProgram(mProgram);
	mMaterial->beginShaderParams(4);
	mMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)&Matrix4::IDENTITY);
	mMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&pCamera->getViewProjMatrix());
	mMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
	mMaterial->setShaderParam("ambient", SPT_VEC4, (void*)&mColor);
	mMaterial->endShaderParams();
}

void TessObject_PoisonCircle::onUpdate(float rdt, i64 currentTime)
{
	if (!isStart() || m_speed == 0)
	{
		return;
	}
	if (m_startUpdateTime == 0)
	{
		m_startUpdateTime = currentTime;
		return;
	}
	Real lbX = 0, lbY = 0, rtX = 0, rtY = 0;
	
	Real distance = m_speed * (currentTime - m_startUpdateTime) / 1000;

	lbX = Math::Min(m_poisonCircle[0].x + distance, m_safeCircle[0].x);
	lbY = Math::Min(m_poisonCircle[0].y + distance, m_safeCircle[0].y);

	rtX = Math::Max(m_poisonCircle[1].x - distance, m_safeCircle[1].x);
	rtY = Math::Max(m_poisonCircle[1].y - distance, m_safeCircle[1].y);

	Vector2 points[4] = { { lbX, lbY },{ lbX, rtY },{ rtX, rtY },{ rtX, lbY } };
	setPoints(points);
	reflesh(rdt);
}

void TessObject_PoisonCircle::setTexture(const String& name)
{
	LordAssert(!mTexture);
	LordAssert(mMaterial);
	mTexture = TextureManager::Instance()->createTexture(name);
	mTexture->prepare();
	mTexture->load();
	SamplerState::SamplerDesc desc = mMaterial->getSamplerState(0)->getDesc();
	desc.minFilter = SamplerState::FO_LINEAR;
	desc.magFilter = SamplerState::FO_LINEAR;
	desc.mipFilter = SamplerState::FO_LINEAR;
	desc.addrUMode = SamplerState::AM_WRAP;
	desc.addrVMode = SamplerState::AM_WRAP;
	mMaterial->setSamplerState(0, desc);
	mMaterial->activeTexture(0, mTexture);
}

void TessObject_PoisonCircle::setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4)
{
	TESSOBJ_POISON_CIRCLE_FMT* pFmt = (TESSOBJ_POISON_CIRCLE_FMT*)(mSysMemoryVertices + mQuadNum * 4 * mVertexSize / sizeof(float));
	pFmt[0].pos = vec1;
	pFmt[1].pos = vec2;
	pFmt[2].pos = vec3;
	pFmt[3].pos = vec4;
}

void TessObject_PoisonCircle::setQuadUV(const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Vector2& uv4)
{
	TESSOBJ_POISON_CIRCLE_FMT* pFmt = (TESSOBJ_POISON_CIRCLE_FMT*)(mSysMemoryVertices + (mQuadNum - 1) * 4 * mVertexSize / sizeof(float));
	pFmt[0].uv = uv1;
	pFmt[1].uv = uv2;
	pFmt[2].uv = uv3;
	pFmt[3].uv = uv4;
}

void TessObject_PoisonCircle::setPoints(const Vector2* points)
{
	m_points[0] = points[0];
	m_points[1] = points[1];
	m_points[2] = points[2];
	m_points[3] = points[3];

}

void TessObject_PoisonCircle::reflesh(float rdt)
{
	// rebuild the tessObj.
	Vector3 top[4], btm[4];
	top[0].x = m_points[0].x;	top[0].y = m_maxHeight;	top[0].z = m_points[0].y;
	top[1].x = m_points[1].x;	top[1].y = m_maxHeight;	top[1].z = m_points[1].y;
	top[2].x = m_points[2].x;	top[2].y = m_maxHeight;	top[2].z = m_points[2].y;
	top[3].x = m_points[3].x;	top[3].y = m_maxHeight;	top[3].z = m_points[3].y;

	btm[0].x = m_points[0].x;	btm[0].y = m_minHeight;	btm[0].z = m_points[0].y;
	btm[1].x = m_points[1].x;	btm[1].y = m_minHeight;	btm[1].z = m_points[1].y;
	btm[2].x = m_points[2].x;	btm[2].y = m_minHeight;	btm[2].z = m_points[2].y;
	btm[3].x = m_points[3].x;	btm[3].y = m_minHeight;	btm[3].z = m_points[3].y;

	m_innerTime += rdt;
	beginPolygon();
	setQuadPos(btm[0], btm[1], top[1], top[0]);
	setQuadUV(
		Vector2(btm[0].z / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[1].z / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[1].z / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[0].z / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime);
	setQuadPos(btm[1], btm[2], top[2], top[1]);
	setQuadUV(
		Vector2(btm[1].x / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[2].x / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[2].x / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[1].x / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime);
	setQuadPos(btm[2], btm[3], top[3], top[2]);
	setQuadUV(
		Vector2(btm[2].z / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[3].z / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[3].z / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[2].z / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime);
	setQuadPos(btm[3], btm[0], top[0], top[3]);
	setQuadUV(
		Vector2(btm[3].x / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[0].x / m_texUVLen, m_minHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[0].x / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime,
		Vector2(btm[3].x / m_texUVLen, m_maxHeight / m_texUVLen) + m_texUVAnim * m_innerTime);
	endPolygon();

}

void TessObject_PoisonCircle::setPoisonCircleRange(vector<Vector2>::type range)
{
	m_safeCircle[0] = Vector2(range[0].x, range[0].y);
	m_safeCircle[1] = Vector2(range[1].x, range[1].y);
	m_poisonCircle[0] = Vector2(range[2].x, range[2].y);
	m_poisonCircle[1] = Vector2(range[3].x, range[3].y);
	m_isStart = true;
	m_startUpdateTime = 0;
}

void TessObject_PoisonCircle::caculateVertexSize()
{
	mVertexSize = sizeof(TESSOBJ_POISON_CIRCLE_FMT);
}

void TessObject_PoisonCircle::endPolygon()
{
	Buffer vertBuff(mQuadNum * 4 * mVertexSize, mSysMemoryVertices);
	mVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_CPU_READ, vertBuff);
	LordSafeFree(mSysMemoryVertices);

	RenderInput::VertexElementList vertElms(2);
	vertElms[0].semantic = RenderInput::VS_POSITION;
	vertElms[0].pixFmt = PF_RGB32_FLOAT;
	vertElms[1].semantic = RenderInput::VS_TEXCOORD;
	vertElms[1].pixFmt = PF_RG32_FLOAT;

	mRenderInput = Renderer::Instance()->createRenderInput(vertElms, mProgram);
	GPUBuffer* indexBuff = mLineMode ? mManger->getIndexBuffer_line() : mManger->getIndexBuffer();
	mRenderInput->bindGPUBuffers(mVertexBuffer, mVertexSize, indexBuff, sizeof(ui16));
	mRenderInput->setVertexCount(mQuadNum * 4);
	mRenderInput->setIndexCount(mLineMode ? (mQuadNum * 8) : (mQuadNum * 6));
	mRenderInput->setTopologyType(mLineMode ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
}

TessObject_Sun::TessObject_Sun(const String& name, TessManager* manager)
	: TessObject(name, manager)
	, mTexture(NULL)
{
	mbTransparent = true;
	mbAlphaAdd = true;
	mTestDepth = false;
	mWriteDepth = false;
}

TessObject_Sun::~TessObject_Sun()
{
	TextureManager::Instance()->releaseResource(mTexture);
	mTexture = NULL;
}

void TessObject_Sun::createMaterial()
{
	mProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Sun");
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	mMaterial = LordNew Material;
	mMaterial->setShaderProgram(mProgram);
	mMaterial->beginShaderParams(4);
	mMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)&Matrix4::IDENTITY);
	mMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&pCamera->getViewProjMatrix());
	mMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
	mMaterial->setShaderParam("ambient", SPT_VEC4, (void*)&mColor);
	mMaterial->endShaderParams();
}

void TessObject_Sun::setTexturePtr(Texture* pTex)
{
	LordAssert(!mTexture);
	LordAssert(mMaterial);

	mTexture = pTex;

	BlendState::BlendDesc blend = mMaterial->getBlendState()->getDesc();
	blend.bBlendEnable = false;
	mMaterial->setBlendState(blend);

	DepthStencilState::DepthStencilDesc desc = mMaterial->getDepthState()->getDesc();
	desc.bDepthEnable = false;
	desc.bWriteDepth = false;
	desc.depthFunc = DepthStencilState::CF_ALWAYS;
	mMaterial->setDepthState(desc);
	mMaterial->activeTexture(0, mTexture);
}

void TessObject_Sun::setTexture(const String& name)
{
	LordAssert(!mTexture);
	LordAssert(mMaterial);
	mTexture = TextureManager::Instance()->createTexture(name);
	mTexture->prepare();
	mTexture->load();
		
	SamplerState::SamplerDesc desc = mMaterial->getSamplerState(0)->getDesc();
	desc.minFilter = SamplerState::FO_POINT;
	desc.magFilter = SamplerState::FO_POINT;
	desc.mipFilter = SamplerState::FO_POINT;
	mMaterial->setSamplerState(0, desc);
	mMaterial->activeTexture(0, mTexture);
}

void TessObject_Sun::setTexture2(const String& name)
{
	LordAssert(!mTexture);
	LordAssert(mMaterial);
	mTexture = TextureManager::Instance()->createTexture(name);
	mTexture->prepare();
	mTexture->load();

	m_type = 2;
	//LordLogInfo("Mark Texture is loaded [%d]", mTexture->isLoaded() ? 1 : 0 );

	BlendState::BlendDesc desc = mMaterial->getBlendState()->getDesc();
	desc.bBlendEnable = true;
	desc.srcBlend = BlendState::BF_SRC_ALPHA;
	desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
	desc.srcAlphaBlend = BlendState::BF_INV_DST_ALPHA;
	desc.dstAlphaBlend = BlendState::BF_ONE;
	mMaterial->setBlendState(desc);

	DepthStencilState::DepthStencilDesc depthdesc = mMaterial->getDepthState()->getDesc();
	depthdesc.bDepthEnable = true;
	depthdesc.bWriteDepth = true;
	mMaterial->setDepthState(depthdesc);

	mMaterial->activeTexture(0, mTexture);
}

void TessObject_Sun::setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4)
{
	TESSOBJ_SUN_FMT* pFmt = (TESSOBJ_SUN_FMT*)(mSysMemoryVertices + mQuadNum * 4 * mVertexSize / sizeof(float));
	pFmt[0].pos = vec1;
	pFmt[1].pos = vec2;
	pFmt[2].pos = vec3;
	pFmt[3].pos = vec4;
}

void TessObject_Sun::setQuadUV(const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Vector2& uv4)
{
	TESSOBJ_SUN_FMT* pFmt = (TESSOBJ_SUN_FMT*)(mSysMemoryVertices + (mQuadNum - 1) * 4 * mVertexSize / sizeof(float));
	pFmt[0].uv = uv1;
	pFmt[1].uv = uv2;
	pFmt[2].uv = uv3;
	pFmt[3].uv = uv4;
}

void TessObject_Sun::caculateVertexSize()
{
	mVertexSize = sizeof(TESSOBJ_SUN_FMT);
}

void TessObject_Sun::endPolygon()
{
	Buffer vertBuff(mQuadNum * 4 * mVertexSize, mSysMemoryVertices);
	mVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_CPU_READ, vertBuff);
	LordSafeFree(mSysMemoryVertices);

	RenderInput::VertexElementList vertElms(2);
	vertElms[0].semantic = RenderInput::VS_POSITION;
	vertElms[0].pixFmt = PF_RGB32_FLOAT;
	vertElms[1].semantic = RenderInput::VS_TEXCOORD;
	vertElms[1].pixFmt = PF_RG32_FLOAT;

	mRenderInput = Renderer::Instance()->createRenderInput(vertElms, mProgram);
	GPUBuffer* indexBuff = mLineMode ? mManger->getIndexBuffer_line() : mManger->getIndexBuffer();
	mRenderInput->bindGPUBuffers(mVertexBuffer, mVertexSize, indexBuff, sizeof(ui16));
	mRenderInput->setVertexCount(mQuadNum * 4);
	mRenderInput->setIndexCount(mLineMode ? (mQuadNum * 8) : (mQuadNum * 6));
	mRenderInput->setTopologyType(mLineMode ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
}

TessObject_Glow::TessObject_Glow(const String& name, TessManager* manager)
	: TessObject(name, manager)
	, mVertexCount(0)
{
	mbTransparent = true;
	mbAlphaAdd = false;
	mTestDepth = false;
	mWriteDepth = false;
}

TessObject_Glow::~TessObject_Glow()
{
}

void TessObject_Glow::createMaterial()
{
	mProgram = ShaderProgramManager::Instance()->getShaderProgram("Tess_Glow");
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	mMaterial = LordNew Material;
	mMaterial->setShaderProgram(mProgram);
	mMaterial->beginShaderParams(3);
	mMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)&Matrix4::IDENTITY);
	mMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&pCamera->getViewProjMatrix());
	mMaterial->setShaderParam("ambient", SPT_VEC4, (void*)&mColor);
	mMaterial->endShaderParams();
}

void TessObject_Glow::caculateVertexSize()
{
	mVertexSize = sizeof(TESSOBJ_GLOW_FMT);
}

void TessObject_Glow::setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4)
{
	LordAssert(false);
}

void TessObject_Glow::addVertex(float x, float y, float z, ui32 color)
{
	TESSOBJ_GLOW_FMT* pFmt = (TESSOBJ_GLOW_FMT*)(mSysMemoryVertices + mVertexCount * mVertexSize / sizeof(float));
	pFmt->pos.x = x;
	pFmt->pos.y = y;
	pFmt->pos.z = z;
	pFmt->color = color;
	mVertexCount++;
}

void TessObject_Glow::endPolygon()
{
	Buffer vertBuff(mVertexCount * mVertexSize, mSysMemoryVertices);
	mVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_CPU_READ, vertBuff);
	LordSafeFree(mSysMemoryVertices);

	RenderInput::VertexElementList vertElms(2);
	vertElms[0].semantic = RenderInput::VS_POSITION;
	vertElms[0].pixFmt = PF_RGB32_FLOAT;
	vertElms[1].semantic = RenderInput::VS_COLOR;
	vertElms[1].pixFmt = PF_RGBA8_UNORM;

	mRenderInput = Renderer::Instance()->createRenderInput(vertElms, mProgram);
	GPUBuffer* indexBuff = mManger->getIndexBuffer_fan();
	mRenderInput->bindGPUBuffers(mVertexBuffer, mVertexSize, indexBuff, sizeof(ui16));
	mRenderInput->setVertexCount(mVertexCount);
	mRenderInput->setIndexCount((mVertexCount-2)*3);
	mRenderInput->setTopologyType(RenderInput::TT_TRIANGLELIST);
	mVertexCount = 0; // for next beginPolygon.
}

}

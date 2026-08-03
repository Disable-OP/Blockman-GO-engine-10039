#include "Core.h"
#include "QueryObject.h"
#include "Render/Renderer.h"
#include "Render/RenderStage.h"
#include "Render/ShaderProgram.h"
#include "Render/ShaderProgramManager.h"
#include "Scene/SceneManager.h"
#include "Object/Exception.h"
#include "Render/GPUBuffer.h"
#include "Scene/SceneNode.h"
#include "Scene/Camera.h"
#include "Render/Material.h"
#include "Geom/Ray.h"

namespace LORD
{

	void QueryManualRender::render(bool uiRender)
	{
		m_pQueryObjectManager->render();
	}

	const size_t QueryObject::sBuffSize = 100;

	QueryObject::QueryObject(const String& name, QueryObjectManager* manager)
		: mManger(manager)
		, mName(name)
		, mAABBLocal()
		, mParent(NULL)
		, mRenderInput(NULL)
		, mVertexBuffer(NULL)
		, mIndexBuffer(NULL)
		, mTexture(NULL)
		, mSysMemoryVertices(NULL)
		, mSysMemoryIndices(NULL)
		, mVerticesLockPtr(NULL)
		, mIndicesLockPtr(NULL)
		, mVertexSize(0)
		, mVerticesNum(0)
		, mVerticesBuffSize(0)
		, mIndicesNum(0)
		, mIndicesBuffSize(0)
		, mQueryFlag(0)
		, mb2DMode(false)
		, mbTransparent(false)
		, mPolygonType(QO_WIREFRAME)
		, mTestDepth(false)
		, mWriteDepth(false)
		, mbInitialized(false)
		, mbHasIndices(false)
		, mbVisible(true)
		, m_pUserData(NULL)
	{
		ShaderProgram* program = mManger->getProgram();
		Camera* pCamera = mb2DMode ? SceneManager::Instance()->getGUICamera() : SceneManager::Instance()->getMainCamera();
		mMaterial = LordNew Material;
		mMaterial->setShaderProgram(program);
		mMaterial->beginShaderParams(2);
		mMaterial->setShaderParam("worldMat", SPT_MAT4, (void*)&Matrix4::IDENTITY);
		mMaterial->setShaderParam("viewProjMat", SPT_MAT4, (void*)&pCamera->getViewProjMatrix());
		mMaterial->endShaderParams();
		refleshMaterial();
	}

	QueryObject::~QueryObject()
	{
		clear();
		LordDelete mMaterial;
	}

	void QueryObject::attachTo(SceneNode* node)
	{
		mParent = node;
		mMaterial->modifyShaderParam("worldMat", SPT_MAT4, (node != NULL) ? (void*)node->getWorldMatrixPtr() : (void*)&Matrix4::IDENTITY);
	}

	void QueryObject::setTexture(Texture* texture)
	{
		mTexture = texture;
	}

	void QueryObject::setVisible(bool bvisible)
	{
		mbVisible = bvisible;
	}

	void QueryObject::beginPolygon(QueryObjectType type)
	{
		clear();

		mPolygonType = type;
		mVertexSize = sizeof(VFTMT1);

		mVerticesBuffSize = sBuffSize;
		mSysMemoryVertices = (float*)LordMalloc(mVerticesBuffSize*mVertexSize);
		mVerticesLockPtr = mSysMemoryVertices;
		mVerticesNum = 0;

		mIndicesBuffSize = sBuffSize;
		mSysMemoryIndices = (ui16*)LordMalloc(mIndicesBuffSize*sizeof(ui16));
		mIndicesLockPtr = mSysMemoryIndices;
		mIndicesNum = 0;
		mbHasIndices = false;

		mbInitialized = true;
		mAABBLocal.reset();
	}

	void QueryObject::setPosition(const Vector3& vec)
	{
		if(mVerticesBuffSize == mVerticesNum)
		{
			float* newBuffer = (float*)LordMalloc(mVertexSize*(mVerticesBuffSize+sBuffSize));
			memcpy(newBuffer, mSysMemoryVertices, mVertexSize*mVerticesBuffSize);
			LordFree(mSysMemoryVertices);
			mSysMemoryVertices = newBuffer;
			mVerticesBuffSize += sBuffSize;
			mVerticesLockPtr = mSysMemoryVertices + mVertexSize/sizeof(float)*mVerticesNum;
		}
		*mVerticesLockPtr++ = vec.x;
		*mVerticesLockPtr++ = vec.y;
		*mVerticesLockPtr++ = vec.z;

		mAABBLocal.addPoint(vec);
		mVerticesNum++;
	}

	void QueryObject::setPosition(Real x, Real y, Real z)
	{
		setPosition(Vector3(x, y, z));
	}

	void QueryObject::setColor(Color color)
	{
		ui32 colour = color.getABGR();
		setColor(colour);
	}

	void QueryObject::setColor(ui32 color)
	{
		ui32* pcolor = (ui32*)mVerticesLockPtr++;
		*pcolor = color;
	}

	void QueryObject::setColor(float r, float g, float b, float a/* =1.f */)
	{
		setColor(Color(r,g,b,a));
	}

	void QueryObject::setTextureUV(Real tu, Real tv)
	{
		*mVerticesLockPtr++ = tu;
		*mVerticesLockPtr++ = tv;
	}

	void QueryObject::setIndex(ui16 idx)
	{
		if(mIndicesBuffSize == mIndicesNum)
		{
			ui16* newBuffer = (ui16*)LordMalloc(sizeof(ui16)*(mIndicesBuffSize+sBuffSize));
			memcpy(newBuffer, mSysMemoryIndices, sizeof(ui16)*mIndicesBuffSize);
			LordFree(mSysMemoryIndices);
			mSysMemoryIndices = newBuffer;
			mIndicesBuffSize += sBuffSize;
			mIndicesLockPtr = mSysMemoryIndices + mIndicesNum;
		}
		*mIndicesLockPtr++ = idx;
		mIndicesNum++;
		mbHasIndices = true;
	}

	void QueryObject::setTriangle(ui16 idx1, ui16 idx2, ui16 idx3)
	{
		setIndex(idx1);
		setIndex(idx2);
		setIndex(idx3);
	}

	void QueryObject::setQuad(ui16 idx1, ui16 idx2, ui16 idx3, ui16 idx4)
	{
		setTriangle(idx1, idx2, idx3);
		setTriangle(idx3, idx4, idx1);
	}

	void QueryObject::endPolygon()
	{
		// flush data to Gpu buffer.
		Buffer vertBuff(mVerticesNum * mVertexSize, mSysMemoryVertices);
		mVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_CPU_READ, vertBuff);

		if(mbHasIndices)
		{
			Buffer indBuff(mIndicesNum*sizeof(ui16), mSysMemoryIndices);
			mIndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_CPU_READ, indBuff);
		}

		RenderInput::VertexElementList vertElms(2);
		vertElms[0].semantic = RenderInput::VS_POSITION;
		vertElms[0].pixFmt = PF_RGB32_FLOAT;

		vertElms[1].semantic = RenderInput::VS_COLOR;
		vertElms[1].pixFmt = PF_RGBA8_UNORM;


		ShaderProgram* program = mManger->getProgram();
		mRenderInput = Renderer::Instance()->createRenderInput(vertElms, program);
		if(mPolygonType == QO_WIREFRAME)
			mRenderInput->setTopologyType(RenderInput::TT_LINELIST);
		mRenderInput->bindGPUBuffers(mVertexBuffer, mVertexSize, mIndexBuffer, sizeof(ui16));
		mRenderInput->setVertexCount(mVerticesNum);
		if(mbHasIndices)
			mRenderInput->setIndexCount(mIndicesNum);
	}

	void QueryObject::set2DMode(bool b2DMode)
	{
		mb2DMode = b2DMode;

		Camera* pCamera = b2DMode ? SceneManager::Instance()->getGUICamera() : SceneManager::Instance()->getMainCamera();
		mMaterial->modifyShaderParam("viewProjMat", SPT_MAT4, (void*)&pCamera->getViewProjMatrix());
	}

	void QueryObject::setTransparent(bool bTransparent)
	{
		mbTransparent = bTransparent;
		refleshBlendState();
	}

	void QueryObject::clear()
	{
		if(mbInitialized)
		{
			LordSafeFree(mSysMemoryVertices);
			mVerticesNum = 0;
			mVerticesLockPtr = NULL;
			mVertexSize = 0;
			mVerticesBuffSize = 0;
			LordSafeFree(mSysMemoryIndices);
			mIndicesNum = 0;
			mIndicesLockPtr = NULL;
			mIndicesBuffSize = 0;
			LordSafeDelete(mVertexBuffer);
			LordSafeDelete(mIndexBuffer);
			LordSafeDelete(mRenderInput);
			mAABBLocal.reset();
		}
	}

	void QueryObject::setTestDepth(bool testDetph)
	{
		mTestDepth = testDetph;
		refleshDepthState();
	}

	void QueryObject::setWriteDepth(bool writeDepth)
	{
		mWriteDepth = writeDepth;
		refleshDepthState();
	}

	void QueryObject::render()
	{
		if (mParent && mRenderInput && mParent->isVisible() && mbVisible)
		{
			mMaterial->active();

			LORD::Renderer::Instance()->render(mRenderInput);
		}
	}

	void QueryObject::createSphere(Real r, int longitude, int latitude, const Color& color)
	{
		clear();
		beginPolygon(QO_SOLIDQUAD);
		Real mAngleLongitude;
		Real mAngleLatitude = -Math::PI_DIV2;
		Real mSegmentLongitude = Math::PI_2/longitude;
		Real mSegmentLatitude = Math::PI/(latitude-1);
		Real mRadius = r;
		float x, y, z, projectxz;

		for(i32 i=0; i<latitude; ++i)
		{
			mAngleLongitude = 0.0f;
			for(i32 j=0; j<longitude; ++j)
			{
				y = mRadius * Math::Sin(mAngleLatitude);
				projectxz = mRadius * Math::Cos(mAngleLatitude);
				x = projectxz * Math::Cos(mAngleLongitude);
				z = projectxz * Math::Sin(mAngleLongitude);

				setPosition(x, y, z);
				setColor(color.r, color.g, color.b, color.a);
				mAngleLongitude += mSegmentLongitude;
			}
			mAngleLatitude += mSegmentLatitude;
		}

		for(i32 i=0; i<latitude-1; ++i)
		{
			for(i32 j=0; j<longitude; ++j)
			{
				if(j == longitude-1)
				{
					setIndex(i*(longitude)+j);
					setIndex((i+1)*(longitude)+j);
					setIndex((i+1)*(longitude));
					setIndex(i*(longitude)+j);
					setIndex((i+1)*(longitude));
					setIndex(i*(longitude));
				}
				else
				{
					setIndex(i*(longitude)+j);
					setIndex((i+1)*(longitude)+j);
					setIndex((i+1)*(longitude)+j+1);
					setIndex(i*(longitude)+j);
					setIndex((i+1)*(longitude)+j+1);
					setIndex(i*(longitude)+j+1);
				}
			}
		}
		endPolygon();
	}

//-----------------------------------------------------------------------------
	// assigned to weijianwei .
	/*
		   3--------2
		  /|       /|
		 / |      / |
		7--------6  |
		|  0-----|--1
		| /      | /
		|/       |/
		4--------5

		vMin (0)
		vMax (6)
		*/
	void QueryObject::createCube(Real r, const Color& color)
	{
		clear();
		beginPolygon(QO_SOLIDQUAD);
		setPosition(-r,-r,-r);
		setColor(color.r, color.g, color.b, color.a);
		setPosition(r,-r,-r);
		setColor(color.r, color.g, color.b, color.a);
		setPosition(r,-r,r);
		setColor(color.r, color.g, color.b, color.a);
		setPosition(-r,-r,r);
		setColor(color.r, color.g, color.b, color.a);
		setPosition(-r,r,-r);
		setColor(color.r, color.g, color.b, color.a);
		setPosition(r,r,-r);
		setColor(color.r, color.g, color.b, color.a);
		setPosition(r,r,r);
		setColor(color.r, color.g, color.b, color.a);
		setPosition(-r,r,r);
		setColor(color.r, color.g, color.b, color.a);

		setQuad(0,1,2,3);
		setQuad(4,5,6,7);
		setQuad(1,2,6,5);
		setQuad(0,3,7,4);
		setQuad(4,5,1,0);
		setQuad(2,3,7,6);

		endPolygon();
	}
	//-----------------------------------------------------------------------------
	void QueryObject::createCubeFrame(Real r, const Color& color, int segment)
	{
		clear();
		beginPolygon(QO_WIREFRAME);

		Real segmentX = r*2.0f / (segment-1);
		Real segmentY = r*2.0f / (segment-1);
		Real segmentZ = r*2.0f / (segment-1);

		Vector3 beginPos;
		//top
		beginPos.x = -r;
		beginPos.y = r;
		beginPos.z = r;
		int i,j,k;

		for(i=0; i<segment; ++i)
		{
			for(j=0; j<segment; ++j)
			{
				setPosition(beginPos.x + segmentX*j, beginPos.y, beginPos.z - segmentZ*i);
				setColor(color.r, color.g, color.b, color.a);
			}
		}
		//front
		beginPos.x = -r;
		beginPos.y = r;
		beginPos.z = -r;
		for(i=0; i<segment; ++i)
		{
			for(j=0; j<segment; ++j)
			{
				setPosition(beginPos.x + segmentX*j, beginPos.y - segmentY*i, beginPos.z);
				setColor(color.r, color.g, color.b, color.a);
			}
		}
		//right
		beginPos.x = r;
		beginPos.y = r;
		beginPos.z = -r;
		for(i=0; i<segment; ++i)
		{
			for(j=0; j<segment; ++j)
			{
				setPosition(beginPos.x, beginPos.y - segmentY*i, beginPos.z + segmentZ*j);
				setColor(color.r, color.g, color.b, color.a);
			}
		}
		//back
		beginPos.x = r;
		beginPos.y = r;
		beginPos.z = r;
		for(i=0; i<segment; ++i)
		{
			for(j=0; j<segment; ++j)
			{
				setPosition(beginPos.x - segmentX*j,beginPos.y - segmentY*i,beginPos.z);
				setColor(color.r, color.g, color.b, color.a);
			}
		}
		//left
		beginPos.x = -r;
		beginPos.y = r;
		beginPos.z = r;
		for(i=0; i<segment; ++i)
		{
			for(j=0; j<segment; ++j)
			{
				setPosition(beginPos.x, beginPos.y - segmentY*i, beginPos.z - segmentZ*j);
				setColor(color.r, color.g, color.b, color.a);
			}
		}
		//bottom
		beginPos.x = -r;
		beginPos.y = -r;
		beginPos.z = -r;
		for(i=0; i<segment; ++i)
		{
			for(j=0; j<segment; ++j)
			{
				setPosition(beginPos.x + segmentX * j,	beginPos.y,	beginPos.z + segmentZ * i);
				setColor(color.r, color.g, color.b, color.a);
			}
		}

		ui16 indexface = 0;
		for(k=0; k<6; k++)
		{
			for(i=0; i<segment; i++)
			{
				for(j=0; j<segment-1; j++)
				{
					setIndex(indexface + i*segment + j);
					setIndex(indexface + i*segment + j+1);
				}
			}
			for(j=0; j<segment; j++)
			{
				for(i=0; i<segment-1; i++)
				{
					setIndex(indexface + i*segment + j);
					setIndex(indexface + (i+1)*segment + j);
				}
			}
			indexface += segment*segment;
		}
		endPolygon();
	}
	//-----------------------------------------------------------------------------
	void QueryObject::createSphereFrame(Real r, int longitude, int latitude, const Color& color)
	{
		clear();
		beginPolygon(QO_WIREFRAME);
		Real mAngleLongitude;
		Real mAngleLatitude = -Math::PI_DIV2;
		Real mSegmentLongitude = Math::PI_2/longitude;
		Real mSegmentLatitude = Math::PI/(latitude-1);
		Real mRadius = r;
		float x, y, z, projectxz;

		for(i32 i=0; i<latitude; ++i)
		{
			mAngleLongitude = 0.0f;
			for(i32 j=0; j<longitude; ++j)
			{
				y = mRadius * Math::Sin(mAngleLatitude);
				projectxz = mRadius * Math::Cos(mAngleLatitude);
				x = projectxz * Math::Cos(mAngleLongitude);
				z = projectxz * Math::Sin(mAngleLongitude);

				setPosition(x, y, z);
				setColor(color.r, color.g, color.b, color.a);
				mAngleLongitude += mSegmentLongitude;
			}
			mAngleLatitude += mSegmentLatitude;
		}

		for(i32 i=0; i<latitude; ++i)
		{
			for(i32 j=0; j<longitude-1; ++j)
			{
				setIndex(i * longitude + j);
				setIndex(i * longitude + j+1);
			}
			setIndex(i * longitude + longitude - 1);
			setIndex(i * longitude);
		}

		for(i32 i=0; i<longitude; ++i)
		{
			for(i32 j=0; j<latitude-1; ++j)
			{
				setIndex(j * longitude + i);
				setIndex((j+1)*longitude + i);
			}
		}
		endPolygon();
	}
	//-----------------------------------------------------------------------------
	void QueryObject::create3DCircle(float r, int segment, const Color& colorx, const Color& colory, const Color& colorz)
	{
		clear();
		beginPolygon(QO_WIREFRAME);

		for(int i=0; i<segment; ++i)
		{
			float x = Math::Sin(float(i)/float(segment)*Math::PI_2);
			float y = Math::Cos(float(i)/float(segment)*Math::PI_2);
			setPosition(0.f, x*r, y*r);
			setColor(colorx.r, colorx.g, colorx.b, colorx.a);
			setPosition(x*r, 0.f, y*r);
			setColor(colory.r, colory.g, colory.b, colory.a);
			setPosition(x*r, y*r, 0.f);
			setColor(colorz.r, colorz.g, colorz.b, colorz.a);
		}
		for(int j=0;j<3;j++)
		{
			for(int i=0; i<segment; ++i)
			{
				setIndex(i*3 + j); 
				if(i==segment-1)
					setIndex(j);
				else
					setIndex((i+1)*3+j);
			}
		}
		endPolygon();
	}
	//-----------------------------------------------------------------------------
	void QueryObject::createCylinderFrame(Real r, Real h, int longitude, int latitude, const Color& color)
	{
		clear();
		beginPolygon(QO_WIREFRAME);
		Real mAngleLongitude;
		//Real mAngleLatitude = -Math::PI_DIV2;
		Real mSegmentLongitude = Math::PI_2/longitude;
		Real mSegmentLatitude = h*2.0f/(latitude-1);
		Real mRadius = r;
		float x, y, z;
		y = h;
		for(i32 i=0; i<latitude; ++i)
		{
			mAngleLongitude = 0.0f;
			for(i32 j=0; j<longitude; ++j)
			{
				x = mRadius * Math::Cos(mAngleLongitude);
				z = mRadius * Math::Sin(mAngleLongitude);

				setPosition(x, y, z);
				setColor(color.r, color.g, color.b, color.a);

				mAngleLongitude += mSegmentLongitude;
			}
			y -= mSegmentLatitude; 
		}

		for(i32 i=0; i<latitude; ++i)
		{
			for(i32 j=0; j<longitude-1; ++j)
			{
				setIndex(i * longitude + j);
				setIndex(i * longitude + j+1);
			}
			setIndex(i * longitude + longitude - 1);
			setIndex(i * longitude);
		}

		for(i32 i=0; i<longitude; ++i)
		{
			for(i32 j=0; j<latitude-1; ++j)
			{
				setIndex(j * longitude + i);
				setIndex((j+1)*longitude + i);
			}
		}
		endPolygon();
	}

	void QueryObject::createAABBFrame( const Box& box, const Color& color )
	{
		clear();
		beginPolygon(QO_WIREFRAME);

		/*
		   4--------7
		  /|       /|
		 / |      / |
		0--------3  |
		|  6-----|--5
		| /      | /
		|/       |/
		2--------1

		vMin (6)
		vMax (3)
		*/

		for (size_t i = 0; i <= Box::CORNER_FRT; ++i)
		{
			setPosition(box.getCorner(Box::Corner(i)));
			setColor(color);
		}

		// set index
		setIndex(0);
		setIndex(3);
		setIndex(3);
		setIndex(1);
		setIndex(1);
		setIndex(2);
		setIndex(2);
		setIndex(0);

		setIndex(4);
		setIndex(7);
		setIndex(7);
		setIndex(5);
		setIndex(5);
		setIndex(6);
		setIndex(6);
		setIndex(4);

		setIndex(0);
		setIndex(4);
		setIndex(2);
		setIndex(6);
		setIndex(3);
		setIndex(7);
		setIndex(1);
		setIndex(5);

		endPolygon();
	}

	void QueryObject::create2DCircle( float r, int segment, const Color& color )
	{
		clear();
		beginPolygon(QO_WIREFRAME);

		for(int i=0; i<segment; ++i)
		{
			float x = Math::Sin(float(i)/float(segment)*Math::PI_2);
			float y = Math::Cos(float(i)/float(segment)*Math::PI_2);

			setPosition(x*r, 0.5f, y*r);
			setColor(color);
		}

		for (int i = 0; i < segment; ++i)
		{
			setIndex(i);
			if ( i == segment-1)
			{
				setIndex(0);
			}
			else
			{
				setIndex(i+1);
			}
		}
		endPolygon();
	}

	void QueryObject::refleshBlendState()
	{
		// BlendState use default
		BlendState::BlendDesc desc = mMaterial->getBlendState()->getDesc();
		if (mbTransparent)
		{
			desc.bBlendEnable = true;
			desc.srcBlend = BlendState::BF_SRC_ALPHA;
			desc.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			desc.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
			desc.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
		}
		else
		{
			BlendState::BlendDesc desc = mMaterial->getBlendState()->getDesc();
			desc.bBlendEnable = false;
			desc.srcBlend = BlendState::BF_ONE;
			desc.dstBlend = BlendState::BF_ZERO;
			desc.srcAlphaBlend = BlendState::BF_ONE;
			desc.dstAlphaBlend = BlendState::BF_ZERO;
		}
		mMaterial->setBlendState(desc);
	}

	void QueryObject::refleshDepthState()
	{
		DepthStencilState::DepthStencilDesc desc = mMaterial->getDepthState()->getDesc();
		if (mTestDepth && mWriteDepth)
		{
			desc.bDepthEnable = true;
			desc.bWriteDepth = true;
			mMaterial->setDepthState(desc);
		}
		else if (mTestDepth && (!mWriteDepth))
		{
			desc.bDepthEnable = true;
			desc.bWriteDepth = false;
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

	void QueryObject::refleshRasterizerState()
	{
		// Rasterizer State
		RasterizerState::RasterizerDesc desc = mMaterial->getRasterizerState()->getDesc();
		desc.cullMode = RasterizerState::CULL_NONE;
		mMaterial->setRasterizerState(desc);
	}

	void QueryObject::refleshMaterial()
	{
		refleshBlendState();
		refleshDepthState();
		refleshRasterizerState();
	}

	QueryObjectManager* QueryObjectManager::Instance()
	{
		return ms_pSingleton;
	}

	QueryObjectManager::QueryObjectManager()
		: mWireFrameProgram(NULL)
		, mbRender(true)
	{
		mQueryManualRender.m_pQueryObjectManager = this;
	}

	QueryObjectManager::~QueryObjectManager()
	{
		destroy();
		QueryObjectMapIt it;
		for(it=mQueryObjectMap.begin(); it!=mQueryObjectMap.end(); ++it)
		{
			LordDelete it->second;
		}
	}

	QueryObject* QueryObjectManager::createQueryObject(const String& name)
	{
		LordAssert(mQueryObjectMap.find(name) == mQueryObjectMap.end());

		QueryObject* pResult = LordNew QueryObject(name, this);
		mQueryObjectMap[name] = pResult;
		return pResult;
	}

	void QueryObjectManager::destroyQueryObject(QueryObject* obj)
	{
		destroyQueryObject(obj->mName);
	}

	void QueryObjectManager::destroyQueryObject(const String& name)
	{
		QueryObjectMapIt it = mQueryObjectMap.find(name);
		if(it != mQueryObjectMap.end())
		{
			LordDelete it->second;
			mQueryObjectMap.erase(it);
		}
	}

	void QueryObjectManager::destroyAllQueryObject()
	{
		for(QueryObjectMapIt it=mQueryObjectMap.begin(); it!=mQueryObjectMap.end(); ++it)
		{
			LordDelete it->second;
		}
		mQueryObjectMap.clear();
	}

	void QueryObjectManager::Initialize()
	{
		SceneManager::Instance()->getRenderQueue("HelperForEditor")->setManalRenderBegin(&mQueryManualRender);

		mWireFrameProgram = ShaderProgramManager::Instance()->getShaderProgram("HelperForEditor");
	}

	void QueryObjectManager::destroy()
	{
	}

	ShaderProgram* QueryObjectManager::getProgram()
	{
		return mWireFrameProgram;
	}

	void QueryObjectManager::render()
	{
		if (!mbRender)
			return;

		QueryObjectMapIt it;
		for(it=mQueryObjectMap.begin(); it!=mQueryObjectMap.end(); ++it)
		{
			it->second->render();
		}
	}
	
	QueryObject* QueryObjectManager::RayQueryObject(const Ray& ray, ui32 flag)
	{		
		// check every object to find.
		float disMin;
		QueryObject* pResult = NULL;
		QueryObject* pTest;
		QueryObjectMapIt it;
		for(it=mQueryObjectMap.begin(); it!=mQueryObjectMap.end(); ++it)
		{
			pTest = it->second;
			if(pTest->mQueryFlag != flag)
				continue;
			if(pTest->mParent == NULL)
				continue;
			Matrix4 inverseObjMat = pTest->mParent->getWorldMatrix();
			inverseObjMat = inverseObjMat.detInverse();

			Ray rayInObject = ray;
			rayInObject.transform(inverseObjMat);
			float dis;
			HitInfo info;
			if(rayInObject.hitBox(pTest->mAABBLocal, dis, info))
			{
				if(pResult==NULL || (pResult&&(disMin > dis)))
				{
					pResult = pTest;
					disMin = dis;
				}
			}
		}
		return pResult;
	}

	void QueryObjectManager::EnableRender( bool bRender )
	{
		mbRender = bRender;
	}

	bool QueryObjectManager::isEnableRender() const
	{
		return mbRender;
	}

}

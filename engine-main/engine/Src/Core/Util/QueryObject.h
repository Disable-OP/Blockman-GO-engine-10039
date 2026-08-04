/********************************************************************
filename: 	QueryObject.h
file path:	dev\engine\Src\Core\Util\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_QUERYOBJECT_H__
#define __LORD_QUERYOBJECT_H__

#include "Core.h"
#include "Geom/Box.h"


namespace LORD
{
	class QueryObjectManager;
	class BlendState;
	class RasterizerState;
	class DepthStencilState;

	class  LORD_CORE_API QueryManualRender : public ManualRenderListener
	{
	public:
		virtual void render(bool uiRender);
		QueryObjectManager* m_pQueryObjectManager;
	};

	class LORD_CORE_API QueryObject : public ObjectAlloc
	{
		friend class QueryObjectManager;
		struct VFTMT1
		{
			float x, y, z;
			ui32 color;
		};

	public:
		enum QueryObjectType
		{
			QO_WIREFRAME = 1,
			QO_SOLIDQUAD,
			QO_UNKNOW,
		};
	public:
		virtual ~QueryObject();

		void attachTo(SceneNode* node);
		inline bool isAttached() const { return mParent != NULL; }
		inline SceneNode* getParent() { return mParent; }
		void setTexture(Texture* texture);
		void setVisible(bool bvisible);

		void beginPolygon(QueryObjectType type);
		void setPosition(const Vector3& vec);
		void setPosition(Real x, Real y, Real z);
		void setColor(Color color);
		void setColor(ui32 color);
		void setColor(float r, float g, float b, float a=1.f);
		void setTextureUV(Real tu, Real tv);
		void setIndex(ui16 idx);
		void setTriangle(ui16 idx1, ui16 idx2, ui16 idx3);
		void setQuad(ui16 idx1, ui16 idx2, ui16 idx3, ui16 idx4);
		void endPolygon();
		void clear();
		inline void setQueryFlag(ui32 flag) { mQueryFlag = flag; }
		void set2DMode(bool b2DMode);
		void setTransparent(bool bTransparent);

		void setTestDepth(bool testDetph);
		void setWriteDepth(bool writeDepth);

		inline GPUBuffer* getVertexBuffer() { return mVertexBuffer; }
		inline GPUBuffer* getIndexBuffer() { return mIndexBuffer; }
		inline ui16 getVertexSize() { return mVertexSize; }
		inline ui32 getVerticesNum() { return mVerticesNum; }
		inline ui16 getIndicesNum() { return mIndicesNum; }

		inline const String& getName() const { return mName; }

		void render();

		inline void setUserData(void* pData){ m_pUserData = pData; }
		inline void* getUserData() const{ return m_pUserData; }
		/// some function for editor.
		void createSphere(Real r, int longitude, int latitude, const Color& color);
		void createCube(Real r, const Color& color);

		void createCubeFrame(Real r, const Color& color, int segment);
		void createAABBFrame(const Box& box, const Color& color);
		void createSphereFrame(Real r, int longitude, int latitude, const Color& color);
		void createCylinderFrame(Real r, Real h, int longitude, int latitude, const Color& color);
		void create3DCircle(float r, int segment, const Color& colorx, 
			const Color& colory, const Color& colorz);

		void create2DCircle(float r, int segment, const Color& color);

		inline float* getVertextBufferCache() { return mSysMemoryVertices; }

		inline ui16*  getIndexBufferCache() { return mSysMemoryIndices; }

		void refleshBlendState();
		void refleshDepthState();
		void refleshRasterizerState();
		void refleshShaderParams();
		void refleshMaterial();
	protected:
		QueryObject(const String& name, QueryObjectManager* manager);

		static const size_t sBuffSize;

		bool				mb2DMode;
		bool				mbTransparent;
		bool				mTestDepth;
		bool				mWriteDepth;
		bool				mbInitialized;
		bool				mbHasIndices;
		bool				mbVisible;

		ui16				mVertexSize;
		ui16				mIndicesNum;
		ui32				mVerticesNum;
		ui32				mVerticesBuffSize;
		ui32				mIndicesBuffSize;
		ui32				mQueryFlag;
		QueryObjectType		mPolygonType;
		
		QueryObjectManager* mManger;
		SceneNode*			mParent;
		Material*			mMaterial;
		RenderInput*		mRenderInput;
		GPUBuffer*			mVertexBuffer;
		GPUBuffer*			mIndexBuffer;
		Texture*			mTexture;
		float*				mSysMemoryVertices;
		ui16*				mSysMemoryIndices;
		float*				mVerticesLockPtr;
		ui16*				mIndicesLockPtr;

		Box					mAABBLocal;

		String				mName;

		void* m_pUserData;
	};

	class LORD_CORE_API QueryObjectManager : public ObjectAlloc, public Singleton<QueryObjectManager>
	{
		friend class QueryObject;
	public:
		QueryObjectManager();
		~QueryObjectManager();

		static QueryObjectManager* Instance();

		void Initialize();
		void destroy();

		QueryObject* createQueryObject(const String& name);
		void destroyQueryObject(const String& name);
		void destroyQueryObject(QueryObject* obj);
		void destroyAllQueryObject();
		ShaderProgram* getProgram();
		void EnableRender(bool bRender);
		bool isEnableRender() const;
		QueryObject* RayQueryObject(const Ray& ray, ui32 flag);
		void render();
		typedef map<String, QueryObject*>::type QueryObjectMap;
		typedef QueryObjectMap::iterator QueryObjectMapIt;

	protected:
		QueryObjectMap mQueryObjectMap;
		ShaderProgram*		mWireFrameProgram;
		DepthStencilState::DepthStencilDesc	mDepthState[4];
		RasterizerState::RasterizerDesc	mRasterizerState;
		bool				mbRender;
		QueryManualRender mQueryManualRender;
	};
}

#endif
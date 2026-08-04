/********************************************************************
filename: 	tessObject.h
file path:	dev\engine\Src\Core\Tessellator\

version:	1
author:		ajohn
company:	supernano
datatime:   2016-12-15
*********************************************************************/

#ifndef __LORD_TESS_OBJECT_H__
#define __LORD_TESS_OBJECT_H__

#include "Core.h"

namespace LORD
{

class TessManager;

class LORD_CORE_API TessObject : public ObjectAlloc
{
	friend class TessManager;

public:
	virtual ~TessObject();
	void attachTo(SceneNode* node);
	inline bool isAttached() const { return mParent != NULL; }
	inline SceneNode* getParent() { return mParent; }

	void init();
	void setLineMode(bool isLineMode) { mLineMode = isLineMode; }
	void setTransparent(bool bTransparent);
	void setTestDepth(bool testDetph);
	void setWriteDepth(bool writeDepth);
	void setColorMask(bool maskAll);
	void setDoubleFace(bool doubleFace);
	void refleshBlendState();
	void refleshDepthState();
	void refleshRasterizerState();
	void refleshMaterial();
	void setColor(const Color& color) { mColor = color; }

	void beginPolygon_clone(TessObject* prhs);
	virtual void beginPolygon();
	void setQuadPos(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3 vec4);
	virtual void setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4) = 0;
	virtual void setQuadColor(int c1, int c2, int c3, int c4) {}
	virtual void setQuadUV(const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Vector2& uv4) {}
	virtual void endPolygon() = 0;
	void clear();
	

	inline GPUBuffer* getVertexBuffer() { return mVertexBuffer; }
	inline ui16 getVertexSize() { return mVertexSize; }
	inline ui32 getVerticesNum() { return mQuadNum * 4; }
	inline ui32 getQuadNum() { return mQuadNum; }

	inline const String& getName() const { return mName; }

	virtual void render();
	
protected:
	TessObject(const String& name, TessManager* manager);
	virtual void createMaterial() = 0;
	virtual void caculateVertexSize() = 0;

protected:
	static const size_t sBuffSize;

	bool				mLineMode;
	ui16				mVertexSize;
	ui32				mQuadNum;
	ui32				mQuadPoolSize;

	bool				mbTransparent;
	bool				mbAlphaAdd;
	bool				mColorMask;
	bool				mTestDepth;
	bool				mWriteDepth;
	bool				mDoubleFace;

	ShaderProgram*		mProgram;
	TessManager*		mManger;
	SceneNode*			mParent;
	Material*			mMaterial;
	RenderInput*		mRenderInput;
	GPUBuffer*			mVertexBuffer;
	float*				mSysMemoryVertices;
	Color				mColor;
	Box					mAABBLocal;
	
	int					m_type = 0;
	String				mName;
};

class LORD_CORE_API TessObject_Sky : public TessObject
{
	friend class TessManager;
	struct TESSOBJ_SKY_FMT
	{
		Vector3 pos;
	};

public:
	virtual void endPolygon();

protected:
	TessObject_Sky(const String& name, TessManager* manager);
	virtual void createMaterial();
	virtual void caculateVertexSize();
	virtual void setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4);
};

class LORD_CORE_API TessObject_Cloud : public TessObject
{
	friend class TessManager;
	struct TESSOBJ_CLOUD_FMT
	{
		Vector3 pos;
		int color;
	};

	struct SubsetModel
	{
		GPUBuffer*	vertexBuffer;
		float*		vertexMemory;
		int			quadNum;
	};
	typedef vector<SubsetModel>::type SubSetModels;

public:
	virtual ~TessObject_Cloud();
	virtual void beginPolygon();
	virtual void endPolygon();
	virtual void setQuadColor(int c1, int c2, int c3, int c4);
	virtual void render();

	void setAmbient(const Color& color) { mAmbient = color; }

protected:
	SubSetModels m_models;
	Color mAmbient;

	TessObject_Cloud(const String& name, TessManager* manager);
	virtual void createMaterial();
	virtual void caculateVertexSize();
	virtual void setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4);
};

class LORD_CORE_API TessObject_Sun : public TessObject
{
	friend class TessManager;
	struct TESSOBJ_SUN_FMT
	{
		Vector3 pos;
		Vector2 uv;
	};
public:
	virtual ~TessObject_Sun();
	void setTexturePtr(Texture* pTex);
	void setTexture(const String& name);
	void setTexture2(const String& name);
	virtual void endPolygon();
	virtual void setQuadUV(const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Vector2& uv4);
	
protected:
	TessObject_Sun(const String& name, TessManager* manager);
	virtual void createMaterial();
	virtual void caculateVertexSize();
	virtual void setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4);

protected:
	Texture* mTexture;
};

class LORD_CORE_API TessObject_Glow : public TessObject
{
	friend class TessManager;
	struct TESSOBJ_GLOW_FMT
	{
		Vector3 pos;
		ui32  color;
	};

public:
	virtual ~TessObject_Glow();
	virtual void endPolygon();
	virtual void setQuadColor(ui32 c1, ui32 c2, ui32 c3, ui32 c4) {}
	void addVertex(float x, float y, float z, ui32 color);

protected:
	TessObject_Glow(const String& name, TessManager* manager);
	virtual void createMaterial();
	virtual void caculateVertexSize();
	virtual void setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4);

protected:
	int					mVertexCount;
};

class LORD_CORE_API TessObject_PoisonCircle : public TessObject
{
	friend class TessManager;
	struct TESSOBJ_POISON_CIRCLE_FMT
	{
		Vector3 pos;
		Vector2 uv;
	};

public:
	virtual ~TessObject_PoisonCircle();
	void setTexture(const String& name);
	virtual void endPolygon();
	virtual void setQuadUV(const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Vector2& uv4);

	void onUpdate(float rdt, i64 currentTime);

	// init
	void setHeight(float minh, float maxh) { m_minHeight = minh, m_maxHeight = maxh; }
	void setUVLength(float uvlen) { m_texUVLen = uvlen; }
	void setUVAnimation(const Vector2& uvAnim) { m_texUVAnim = uvAnim; }

	// call every frame to move the circle.
	// points  {0.f,0.f}, {0.f,32.f}, {32.f,32.f}, {32.f,0.f}
	void setPoints(const Vector2* points);
	void reflesh(float rdt);

	void setPoisonCircleRange(vector<Vector2>::type range);
	void setPoisonSpeed(float speed) { m_speed = speed; }

	Vector3 getPoisonCircleStartPosition() { return Vector3(m_points[0].x, 0 , m_points[0].y);}
	Vector3 getPoisonCircleEndPosition() { return Vector3(m_points[2].x, 0, m_points[2].y); }

	Vector3 getSafeCircleEndPosition() { return Vector3(m_safeCircle[1].x, 0 , m_safeCircle[1].y); }
	Vector3 getSafeCircleStartPosition() { return Vector3(m_safeCircle[0].x, 0, m_safeCircle[0].y); }

	bool isStart() { return m_isStart; }

protected:
	TessObject_PoisonCircle(const String& name, TessManager* manager);
	virtual void createMaterial();
	virtual void caculateVertexSize();
	virtual void setQuadPos_impl(const Vector3& vec1, const Vector3& vec2, const Vector3& vec3, const Vector3& vec4);

protected:
	Texture * mTexture = nullptr;
	
	float m_innerTime = 0.f;
	float m_minHeight = 16.f;
	float m_maxHeight = 128.f;
	float m_texUVLen = 4.f;
	
	float m_speed = 0.0f;

	bool m_isStart = false;

	i64 m_startUpdateTime = 0;

	Vector2 m_texUVAnim = { 0.f, 0.f };
	Vector2 m_points[4];

	Vector2 m_safeCircle[2];
	Vector2 m_poisonCircle[2];

};

}

#endif
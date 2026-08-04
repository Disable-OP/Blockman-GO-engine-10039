#include "Core.h"
#include "SkinedRenderable.h"
#include "TessMaterial.h"
#include "BatchRenderable.h"

#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{

SkinedRenderable::SkinedRenderable()
	: m_VertexSysMemory(NULL)
	, m_VertexBuffer(NULL)
	, m_quadPoolSize(0)
	, m_quadNum(0)
	, m_IndexBuffer(NULL)
	, m_Texture(NULL)
	, m_RenderGroup(SRP_SOLID)
	, m_boneMatrices(NULL)
	, m_boneCount(0)
	, m_worldMat(NULL)
	//, m_extraSkyBlockLight(1.f, 1.f, 1.f, 1.f)
	, m_extraVertexColor(1.f, 1.f, 1.f, 1.f)
	, m_viewSpace(false)
{
}

SkinedRenderable::~SkinedRenderable()
{
	destroyVertexPool();
}

void SkinedRenderable::setQuadPoolSize(ui16 quadSize)
{
	m_quadPoolSize = quadSize;
	m_quadNum = 0;
}

void SkinedRenderable::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	m_quadNum = 0;
	m_quadPoolSize = 0;
}

void SkinedRenderable::beginPolygon()
{
	if (!m_VertexSysMemory)
	{
		m_VertexSysMemory = (float*)(LordMalloc(m_quadPoolSize * 4 * sizeof(SkinedVertexFmt)));
		LordAssert(m_VertexSysMemory);
	}
	LordSafeDelete(m_VertexBuffer);
	m_quadNum = 0;
}

void SkinedRenderable::endPolygon(bool needBatch)
{
	LordAssert(m_VertexSysMemory);
	LordAssert(!m_VertexBuffer);

	Buffer vertBuff(m_quadNum * 4 * sizeof(SkinedVertexFmt), m_VertexSysMemory);
	m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);

	if(!needBatch)
		LordSafeFree(m_VertexSysMemory);
}

void SkinedRenderable::setMaterialParam(Texture* texture, SKINED_RENDER_PASS group)
{
	m_Texture = texture;
	m_RenderGroup = group;
}

void SkinedRenderable::batch(BatchVertexFmt* pDst)
{
	/* shader:
	vec3 blockPos = (inPosition.xyz / 127.0 - 1.0) * 8.0;
	texCoord = inTexCoord / 2048.0;
	vec3 color = inColor.rgb / 255.0;
	color *= extraColor.rgb;
	int BoneIndices = int(inColor.a);
	vec3 vPos = MulBone4(vec4(blockPos, 1.0), BoneIndices);
	vec4 vWorldPos = matWorld * vec4(vPos, 1.0);
	*/
	LordAssert(m_VertexSysMemory);
	SkinedVertexFmt* pSrc = (SkinedVertexFmt*)m_VertexSysMemory;
	Vector3 pos;
	Vector2 tex;
	Color   color;
	int idx;
	Matrix4 matw = *m_worldMat;
	for (int i = 0; i < m_quadNum * 4; ++i)
	{
		pos = pSrc[i].GetPositionEx();
		tex = pSrc[i].GetTexcoord();
		color = pSrc[i].GetLightColorEx() * m_extraVertexColor;
		idx = pSrc[i].getIndex();
		Vector4 vec4Pos(pos, 1.f);
		pos.x = vec4Pos.dot(m_boneMatrices[3 * idx + 0]);
		pos.y = vec4Pos.dot(m_boneMatrices[3 * idx + 1]);
		pos.z = vec4Pos.dot(m_boneMatrices[3 * idx + 2]);
		vec4Pos.setVec3(pos, 1.f);
		pos = vec4Pos * matw;

		pDst[i].pos = pos;
		pDst[i].uv = tex;
		pDst[i].color = color.getABGR();
	}

}

}

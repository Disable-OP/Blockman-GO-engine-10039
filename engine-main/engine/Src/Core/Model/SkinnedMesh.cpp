#include "Core.h"
#include "SkinnedMesh.h"
#include "Util/PathUtil.h"
#include "Render/TextureManager.h"
#include "Render/Renderer.h"
#include "Resource/DataStream.h"
#include "Image/PixelFormat.h"
#include "Render/GPUBuffer.h"
#include "Scene/TextureSet.h"

namespace LORD
{
	//---------------------------------------------------------------------

	SkinnedMesh::SkinnedMesh(const String& name)
		: Mesh(name)
	{
	}

	SkinnedMesh::~SkinnedMesh()
	{
	}

	void SkinnedMesh::MergeSubMesh(const map<SubMesh*, Vector4>::type& submeshes, const map<SubMesh*, std::pair<Vector4, int>>::type& stsubmeshes)
	{
		SubMesh* pTargetSub = nullptr;
		if (m_subMeshs.size() == 0)
		{
			pTargetSub = LordNew SubMesh;
			LordAssert(pTargetSub);
			m_subMeshs.push_back(pTargetSub);
			pTargetSub->m_VertexElements.clear();

			RenderInput::VertexElement verElements;
			verElements.semantic = RenderInput::VS_POSITION;
			verElements.pixFmt = PF_RGB32_FLOAT;
			pTargetSub->m_VertexElements.push_back(verElements);

			verElements.semantic = RenderInput::VS_NORMAL;
			verElements.pixFmt = PF_RGB32_FLOAT;
			pTargetSub->m_VertexElements.push_back(verElements);

			verElements.semantic = RenderInput::VS_TEXCOORD;
			verElements.pixFmt = PF_RG32_FLOAT;
			pTargetSub->m_VertexElements.push_back(verElements);

			verElements.semantic = RenderInput::VS_BLENDINDICES;
			verElements.pixFmt = PF_RGBA8_UINT;
			pTargetSub->m_VertexElements.push_back(verElements);
			//boneWeight
			verElements.semantic = RenderInput::VS_BLENDWEIGHTS;
			verElements.pixFmt = PF_RGB32_FLOAT;
			pTargetSub->m_VertexElements.push_back(verElements);
		}

		m_version = 3;
		m_isSkinned = true;
		Box box; box.reset();
		pTargetSub->m_vertStride = sizeof(SkinnedMeshVertex);
		pTargetSub->m_idxStride = sizeof(Word);
		int vertexCount = 0;
		int indexCount = 0;
		int numBoneInfluenced = 0;
		for (auto it = submeshes.begin(); it != submeshes.end(); ++it)
		{
			SubMesh* pSubmesh = it->first;
			numBoneInfluenced += pSubmesh->getBoneNum();
			vertexCount += pSubmesh->getVertexCount();
			indexCount += pSubmesh->getIndexCount();
			box.unionBox(pSubmesh->m_box);
		}
		for (auto it = stsubmeshes.begin(); it != stsubmeshes.end(); ++it)
		{
			SubMesh* pSubmesh = it->first;
			vertexCount += pSubmesh->getVertexCount();
			indexCount += pSubmesh->getIndexCount();
			box.unionBox(pSubmesh->m_box);
		}

		pTargetSub->m_vertCount = vertexCount;
		uint vertBuffSize = vertexCount * pTargetSub->m_vertStride;
		pTargetSub->m_vertices = LordAlloc(Byte, vertBuffSize);

		pTargetSub->m_idxCount = indexCount;
		uint idxBuffSize = indexCount * sizeof(Word);
		pTargetSub->m_indices = LordAlloc(Byte, idxBuffSize);

		pTargetSub->m_box.vMin = box.vMin;
		pTargetSub->m_box.vMax = box.vMax;
		pTargetSub->m_box.mExtent = Box::EXTENT_FINITE;

		struct staticmeshVertex
		{
			Vector3	position;
			Vector3	normal;
			Vector2	texCoord;
		};
		float* pTargetVertex = (float*)pTargetSub->m_vertices;
		ui16* pTargetIndex = (ui16*)pTargetSub->m_indices;
		ui16 indexbias = 0;
		int bonebias = 0;

		vector<uint>::type boneIdxs;
		vector<uint>::type boneIdxresult;
		//动态模型
		for (auto it = submeshes.begin(); it != submeshes.end(); ++it)
		{
			SubMesh* pSubmesh = it->first;
			int curBoneIdx = pTargetSub->m_boneIdxs.size();
			for (int i = 0; i < pSubmesh->getBoneNum(); ++i)
			{
				boneIdxs.push_back(pSubmesh->m_boneIdxs[i]);
				auto itfind = std::find(boneIdxresult.begin(), boneIdxresult.end(), pSubmesh->m_boneIdxs[i]);
				if (itfind == boneIdxresult.end())
				{
					boneIdxresult.push_back(pSubmesh->m_boneIdxs[i]);
				}
			}

			Vector4 uvBias = it->second;
			float* pVertex = (float*)pSubmesh->m_vertices;
			for (int veri = 0; veri < pSubmesh->getVertexCount(); ++veri)
			{
				SkinnedMeshVertex& pDst = *(SkinnedMeshVertex*)pTargetVertex;
				SkinnedMeshVertex& pSrc = *(SkinnedMeshVertex*)pVertex;
				pDst.position = pSrc.position;
				pDst.normal = pSrc.normal;
				pDst.texCoord = pSrc.texCoord * Vector2(uvBias.z, uvBias.w) + Vector2(uvBias.x, uvBias.y);
				for (int i=0;i <4; ++i)
				{
					int srcIndices = pSrc.blendIndices[i] + bonebias;
					uint value = boneIdxs[srcIndices];
					auto itindex = std::find(boneIdxresult.begin(), boneIdxresult.end(), value);
					uint newVal = std::distance(boneIdxresult.begin(), itindex) ;

					pDst.blendIndices[i] = newVal;
				}
				for (int i=0; i<MAX_BONE_WEIGHT_NUM; ++i)
				{
					pDst.blendWeights[i] = pSrc.blendWeights[i];
				}
				pTargetVertex += pTargetSub->m_vertStride / sizeof(float);
				pVertex += pSubmesh->m_vertStride / sizeof(float);
			}

			ui16* pIndex = (ui16*)pSubmesh->m_indices;
			for (int idx=0; idx<pSubmesh->getIndexCount(); ++idx)
			{
				*pTargetIndex++ = indexbias + *pIndex++;
			}
			indexbias += pSubmesh->getVertexCount();

			bonebias += pSubmesh->getBoneNum();
		}

		//静态模型
		for (auto it = stsubmeshes.begin(); it != stsubmeshes.end(); ++it)
		{
			boneIdxs.push_back(it->second.second);
			auto itfind = std::find(boneIdxresult.begin(), boneIdxresult.end(), it->second.second);
			if (itfind == boneIdxresult.end())
			{
				boneIdxresult.push_back(it->second.second);
			}

			SubMesh* pSubmesh = it->first;
			Vector4 uvBias = it->second.first;
			float* pVertex = (float*)pSubmesh->m_vertices;
			for (int veri = 0; veri < pSubmesh->getVertexCount(); ++veri)
			{
				SkinnedMeshVertex& pDst = *(SkinnedMeshVertex*)pTargetVertex;
				staticmeshVertex& pSrc = *(staticmeshVertex*)pVertex;
				pDst.position = pSrc.position;
				pDst.normal = pSrc.normal;
				pDst.texCoord = pSrc.texCoord * Vector2(uvBias.z, uvBias.w) + Vector2(uvBias.x, uvBias.y);

				int srcIndices = boneIdxs.size() - 1;
				uint value = boneIdxs[srcIndices];
				auto itindex = std::find(boneIdxresult.begin(), boneIdxresult.end(), value);
				uint newVal = std::distance(boneIdxresult.begin(), itindex);

				pDst.blendIndices[0] = newVal;
				pDst.blendIndices[1] = 0;
				pDst.blendIndices[2] = 0;
				pDst.blendIndices[3] = 0;
				pDst.blendWeights[0] = 1.0f;
				pDst.blendWeights[1] = 0.0f;
				pDst.blendWeights[2] = 0.0f;

				pTargetVertex += pTargetSub->m_vertStride / sizeof(float);
				pVertex += pSubmesh->m_vertStride / sizeof(float);
			}

			ui16* pIndex = (ui16*)pSubmesh->m_indices;
			for (int idx = 0; idx < pSubmesh->getIndexCount(); ++idx)
			{
				*pTargetIndex++ = indexbias + *pIndex++;
			}
			indexbias += pSubmesh->getVertexCount();
		}
		for (auto& i : boneIdxresult)
		{
			pTargetSub->m_boneIdxs.push_back(i);
		}
		m_box.unionBox(pTargetSub->m_box);

		mLoadingState = LOADSTATE_PREPARE;
	}

	bool SkinnedMesh::prepareImpl()
	{
		return prepareImplInteral( true);
	}

	//bool SkinnedMesh::loadImpl()
	//{
	//	m_pDiffTex->load();
	//			
	//	Buffer vertBuff(m_vertStride*m_vertCount, m_vertices);
	//	m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, vertBuff);
	//	Buffer indexBuff(m_idxCount*m_idxStride, m_indices);
	//	m_IndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, indexBuff);

	//	return true;
	//}
}
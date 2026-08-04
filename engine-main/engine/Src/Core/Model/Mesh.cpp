#include "Core.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "Util/StringUtil.h"
#include "Util/PathUtil.h"
#include "Object/Root.h"
#include "Render/TextureManager.h"
#include "Render/Renderer.h"
#include "Resource/ResourceGroupManager.h"
#include "Image/PixelFormat.h"
#include "Render/GPUBuffer.h"
#include "Util/StringUtil.h"

namespace LORD
{
	//---------------------------------------------------------------------

	Mesh::Mesh(const String& name)
		: Resource(name, RESOURCE_TYPE_NORMAL)
		, m_pMeshStream(NULL)
		, m_bHasMaskTexture(false)
	{
	}

	Mesh::~Mesh()
	{
		unloadImpl();
	}

	const Box& Mesh::getBox() const
	{
		return m_box;
	}

	size_t Mesh::calculateSize() const
	{
		size_t result = 0;
		for ( size_t i=0; i<m_subMeshs.size(); i++)
			result += m_subMeshs[i]->calculateSize();

		return result;
	}

	bool Mesh::prepareImplInteral( bool isSkinned)
	{
		m_pMeshStream = ResourceGroupManager::Instance()->openResource(mName);
		if(!m_pMeshStream)
			return false;

		// bool bRenderHighLevel = SceneManager::Instance()->isRenderActorHigh();
		m_pMeshStream->read(&m_version, sizeof(int));

		m_isSkinned = isSkinned;

		// Version 1 :
		if( m_version==1)
		{
			SubMesh* pSubMesh = LordNew SubMesh; 
			LordAssert(pSubMesh);

			// material
			uint mtrlLen;
			char mtrlPath[260];
			m_pMeshStream->read(&mtrlLen, sizeof(uint));
			m_pMeshStream->read(&mtrlPath, mtrlLen);
			mtrlPath[mtrlLen] = 0;

			m_DiffuseVector.push_back(mtrlPath);
			// skip 80 bytes
			m_pMeshStream->read(&pSubMesh->m_isUseNormal, sizeof(bool));
			m_pMeshStream->read(&pSubMesh->m_isUseVertexColor, sizeof(bool));
			m_pMeshStream->skip(78);

			// setup vertex element
			uint vertUnitOffset = 0;
			pSubMesh->m_VertexElements.clear();
			RenderInput::VertexElement verElements;

			verElements.semantic = RenderInput::VS_POSITION;
			verElements.pixFmt = PF_RGB32_FLOAT;
			pSubMesh->m_VertexElements.push_back(verElements);
			vertUnitOffset += PixelUtil::GetPixelSize(PF_RGB32_FLOAT);

			if( pSubMesh->m_isUseNormal)
			{
				verElements.semantic = RenderInput::VS_NORMAL;
				verElements.pixFmt = PF_RGB32_FLOAT;
				pSubMesh->m_VertexElements.push_back(verElements);
				vertUnitOffset += PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
			}

			if( pSubMesh->m_isUseVertexColor)
			{
				verElements.semantic = RenderInput::VS_COLOR;
				verElements.pixFmt = PF_RGBA8_UNORM;
				pSubMesh->m_VertexElements.push_back(verElements);
				vertUnitOffset += PixelUtil::GetPixelSize(PF_RGBA8_UNORM);
			}

			verElements.semantic = RenderInput::VS_TEXCOORD;
			verElements.pixFmt = PF_RG32_FLOAT;
			pSubMesh->m_VertexElements.push_back(verElements);
			vertUnitOffset += PixelUtil::GetPixelSize(PF_RG32_FLOAT);

			// 动画数据(骨骼权重与索引)
			if( isSkinned)
			{
				//boneIndex
				verElements.semantic = RenderInput::VS_BLENDINDICES;
				verElements.pixFmt = PF_RGBA8_UINT;
				pSubMesh->m_VertexElements.push_back(verElements);
				vertUnitOffset += PixelUtil::GetPixelSize(PF_RGBA8_UINT);
				//boneWeight
				verElements.semantic = RenderInput::VS_BLENDWEIGHTS;
				verElements.pixFmt = PF_RGB32_FLOAT;
				pSubMesh->m_VertexElements.push_back(verElements);
				vertUnitOffset += PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
			}

			// load vertices
			uint vertCount;
			m_pMeshStream->read(&vertCount, sizeof(uint));
			pSubMesh->m_vertCount = vertCount;
			pSubMesh->m_vertStride = vertUnitOffset;

			uint vertBuffSize = vertCount * pSubMesh->m_vertStride;
			pSubMesh->m_vertices = LordAlloc(Byte, vertBuffSize);
			m_pMeshStream->read(pSubMesh->m_vertices, vertBuffSize);

			// load indices
			uint faceCount;
			m_pMeshStream->read(&faceCount, sizeof(uint));
			pSubMesh->m_idxCount = faceCount * 3;
			pSubMesh->m_idxStride = sizeof(Word);

			uint idxBuffSize = faceCount * 3 * sizeof(Word);
			pSubMesh->m_indices = LordAlloc(Byte, idxBuffSize);
			m_pMeshStream->read(pSubMesh->m_indices, idxBuffSize);

			// load aabb
			m_pMeshStream->read(&pSubMesh->m_box.vMin, sizeof(Vector3));
			m_pMeshStream->read(&pSubMesh->m_box.vMax, sizeof(Vector3));
			pSubMesh->m_box.mExtent = Box::EXTENT_FINITE;

			m_box = pSubMesh->m_box;

			m_subMeshs.push_back( pSubMesh);

			LordSafeDelete(m_pMeshStream);

			return true;
		}
		// Version 2 :
		else if( m_version>=2)
		{
			m_box.reset();

			// 子模型数量
			int subMeshNum=0;
			m_pMeshStream->read(&subMeshNum, sizeof(int));

			// 预留空间
			m_pMeshStream->skip(72);

			for( int i=0; i<subMeshNum; i++)
			{
				SubMesh* pSubMesh = LordNew SubMesh;
				LordAssert(pSubMesh);

				// name
				uint smNameLen;
				char smName[260];
				m_pMeshStream->read(&smNameLen, sizeof(uint));
				m_pMeshStream->read(&smName, smNameLen);
				smName[smNameLen] = 0;
				pSubMesh->m_name = smName;

				// material
				uint mtrlLen;
				char mtrlPath[260];
				m_pMeshStream->read(&mtrlLen, sizeof(uint));
				if (mtrlLen > 0)
					m_pMeshStream->read(&mtrlPath, mtrlLen);
				mtrlPath[mtrlLen] = 0;

				if (mtrlPath[0] != 0)
					m_DiffuseVector.push_back(mtrlPath);
				
				// skip 80 bytes
				int	 lightmapResolution;
				m_pMeshStream->read(&pSubMesh->m_isUseNormal, sizeof(bool));
				m_pMeshStream->read(&pSubMesh->m_isUseVertexColor, sizeof(bool));
				m_pMeshStream->read(&pSubMesh->m_isUseLightmapUV, sizeof(bool));
				m_pMeshStream->read(&pSubMesh->m_isUseTangentBinormal, sizeof(bool));
				m_pMeshStream->read(&lightmapResolution, sizeof(int));
				m_pMeshStream->skip(72);

				pSubMesh->m_lightmapResolution = LORD::Math::Clamp( lightmapResolution, 8, 512);

				// setup vertex element
				uint vertUnitOffset = 0;
				pSubMesh->m_VertexElements.clear();
				RenderInput::VertexElement verElements;

				verElements.semantic = RenderInput::VS_POSITION;
				verElements.pixFmt = PF_RGB32_FLOAT;
				pSubMesh->m_VertexElements.push_back(verElements);
				vertUnitOffset += PixelUtil::GetPixelSize(PF_RGB32_FLOAT);

				if(pSubMesh->m_isUseNormal)
				{
					verElements.semantic = RenderInput::VS_NORMAL;
					verElements.pixFmt = PF_RGB32_FLOAT;
					pSubMesh->m_VertexElements.push_back(verElements);
					vertUnitOffset += PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
				}

				if(pSubMesh->m_isUseVertexColor)
				{
					verElements.semantic = RenderInput::VS_COLOR;
					verElements.pixFmt = PF_RGBA8_UNORM;
					pSubMesh->m_VertexElements.push_back(verElements);
					vertUnitOffset += PixelUtil::GetPixelSize(PF_RGBA8_UNORM);
				}

				if (m_DiffuseVector.size() > 0)
				{
					verElements.semantic = RenderInput::VS_TEXCOORD;
					verElements.pixFmt = PF_RG32_FLOAT;
					pSubMesh->m_VertexElements.push_back(verElements);
					vertUnitOffset += PixelUtil::GetPixelSize(PF_RG32_FLOAT);
				}

				if( pSubMesh->m_isUseLightmapUV)
				{
					verElements.semantic = RenderInput::VS_TEXCOORD1;
					verElements.pixFmt	 = PF_RG32_FLOAT;
					pSubMesh->m_VertexElements.push_back( verElements);
					vertUnitOffset += PixelUtil::GetPixelSize( PF_RG32_FLOAT);
				}

				// 动画数据(骨骼权重与索引)
				if( isSkinned)
				{
					//boneIndex
					verElements.semantic = RenderInput::VS_BLENDINDICES;
					verElements.pixFmt = PF_RGBA8_UINT;
					pSubMesh->m_VertexElements.push_back(verElements);
					vertUnitOffset += PixelUtil::GetPixelSize(PF_RGBA8_UINT);
					//boneWeight
					verElements.semantic = RenderInput::VS_BLENDWEIGHTS;
					verElements.pixFmt = PF_RGB32_FLOAT;
					pSubMesh->m_VertexElements.push_back(verElements);
					vertUnitOffset += PixelUtil::GetPixelSize(PF_RGB32_FLOAT);
				}

				// load 骨骼索引数据
				if( isSkinned && m_version>=3)
				{
					uint numBoneInfluenced=0;
					m_pMeshStream->read( &numBoneInfluenced, sizeof(uint));
					pSubMesh->m_boneIdxs.resize( numBoneInfluenced);

					m_pMeshStream->read( pSubMesh->m_boneIdxs.data(), sizeof(uint)*numBoneInfluenced);
				}

				// load vertices
				uint vertCount;
				m_pMeshStream->read(&vertCount, sizeof(uint));
				pSubMesh->m_vertCount = vertCount;
				pSubMesh->m_vertStride = vertUnitOffset;

				uint vertBuffSize = vertCount * pSubMesh->m_vertStride;
				pSubMesh->m_vertices = LordAlloc(Byte, vertBuffSize);
				m_pMeshStream->read(pSubMesh->m_vertices, vertBuffSize);

				// load indices
				uint faceCount;
				m_pMeshStream->read(&faceCount, sizeof(uint));
				pSubMesh->m_idxCount = faceCount * 3;
				pSubMesh->m_idxStride = sizeof(Word);

				uint idxBuffSize = faceCount * 3 * sizeof(Word);
				pSubMesh->m_indices = LordAlloc(Byte, idxBuffSize);
				m_pMeshStream->read(pSubMesh->m_indices, idxBuffSize);

				// load aabb
				m_pMeshStream->read(&pSubMesh->m_box.vMin, sizeof(Vector3));
				m_pMeshStream->read(&pSubMesh->m_box.vMax, sizeof(Vector3));
				pSubMesh->m_box.mExtent = Box::EXTENT_FINITE;

				m_box.unionBox( pSubMesh->m_box);

				m_subMeshs.push_back( pSubMesh);
			}

			LordSafeDelete(m_pMeshStream);

			return true;
		}

		return false;
	}

	bool Mesh::prepareImpl()
	{
		return prepareImplInteral( false);
	}

	void Mesh::unprepareImpl()
	{
		if(!m_bNeedBackUp)
		{
			for ( size_t i=0; i<m_subMeshs.size(); i++)
				m_subMeshs[i]->unprepareImpl();
		}
	}

	bool Mesh::loadImpl()
	{
		bool bRenderHighLevel = SceneManager::Instance()->isRenderActorHigh();

		bool result = true;
		SubMesh* pSubMesh = NULL;
		for (size_t i = 0; i < m_subMeshs.size(); i++)
		{
			pSubMesh = m_subMeshs[i];
			result = pSubMesh->load();
			if (!result)
				return false;

			pSubMesh->m_pDiffTex = Root::getBlankTexture(); // Mesh的贴图暂用默认Blank贴图取代，由上层去选择多线程或单线程加载diffuse和mask贴图
			pSubMesh->m_pDiffTex->addRefrenceCount();
			if (bRenderHighLevel&&m_bHasMaskTexture)
			{
				pSubMesh->m_pMaskTex = pSubMesh->m_pDiffTex;
				pSubMesh->m_pMaskTex->addRefrenceCount();
			}
		}
		return result;
	}

	void Mesh::rebuildVertexFomat_Remove(RenderInput::VertexSemantic semantic)
	{
		if(!m_bNeedBackUp)
			return;

		for ( size_t i=0; i<m_subMeshs.size(); i++)
			m_subMeshs[i]->rebuildVertexFomat_Remove( semantic);
	}

	void Mesh::rebuildVertexFomat_Insert(const RenderInput::VertexElement& element, void* templateData)
	{
		if(!m_bNeedBackUp)
			return;

		for ( size_t i=0; i<m_subMeshs.size(); i++)
			m_subMeshs[i]->rebuildVertexFomat_Insert( element, templateData);
	}

	void Mesh::unloadImpl()
	{	
		for ( size_t i=0; i<m_subMeshs.size(); i++)
		{
			m_subMeshs[i]->unloadImpl();
			LordSafeDelete(m_subMeshs[i]);
		}
		m_subMeshs.clear();
		m_DiffuseVector.clear();
	}

	// 获取子模型
	SubMesh* Mesh::getSubMesh( int idx)
	{
		LordAssert(idx>=0 && idx<static_cast<int>(m_subMeshs.size()));

		return m_subMeshs[idx];
	}

	void writeString(FILE* fp, const char* str)
	{
		unsigned int len = (unsigned int)strlen(str);
		fwrite(&len, sizeof(unsigned int), 1, fp);
		fwrite(str, sizeof(char), len, fp);
	}

	// 保存
	bool Mesh::save( const char* filePath)
	{
		if( !m_subMeshs.size()) return false;

		FILE *fp = fopen( filePath, "wb");
		if(!fp)
			return false;

		try
		{
			// 版本号
			int ver = 3;
			fwrite(&ver, sizeof(int), 1, fp);

			int subMeshNum = static_cast<int>( m_subMeshs.size());
			fwrite( &subMeshNum, sizeof(int), 1, fp);

			unsigned char reserve[72];
			memset(reserve, 0, 72);
			fwrite(reserve, 72, 1, fp);

			for( size_t i=0; i<m_subMeshs.size(); i++)
			{
				SubMesh* pSubMesh = m_subMeshs[i];

				// 贴图
				if( !pSubMesh->m_pDiffTex)
					throw;

				// 保存Mesh名称
				String meshName = pSubMesh->m_name;
				//GBKToUTF8( meshName, pSubMesh->m_name);
				writeString( fp, meshName.c_str());

				String mtrlName = pSubMesh->m_pDiffTex->getName();
				//GBKToUTF8(mtrlName, pSubMesh->m_pDiffTex->getName());
				writeString(fp, mtrlName.c_str());

				// 材质预留80字节
				bool bExpNormal = pSubMesh->m_isUseNormal;
				fwrite(&bExpNormal, sizeof(bool), 1, fp);

				bool bExpVertexColor = pSubMesh->m_isUseVertexColor;
				fwrite(&bExpVertexColor, sizeof(bool), 1, fp);

				bool bExpUniqueUV= pSubMesh->m_isUseLightmapUV;
				fwrite( &bExpUniqueUV, sizeof(bool), 1, fp);

				bool bExpTangentBinormal = pSubMesh->m_isUseTangentBinormal;
				fwrite( &bExpTangentBinormal, sizeof(bool), 1, fp);

				int	 lightMapResolution = pSubMesh->m_lightmapResolution;
				fwrite( &lightMapResolution, sizeof(int), 1, fp);

				unsigned char reserve[72];
				memset(reserve, 0, 72);
				fwrite(reserve, 72, 1, fp);

				// 引用骨骼数据
				if( m_isSkinned)
				{
					ui32 numBoneInfluenced = (ui32)pSubMesh->m_boneIdxs.size();
					fwrite( &numBoneInfluenced, sizeof(ui32), 1, fp);

					fwrite( &pSubMesh->m_boneIdxs[0], sizeof(ui32)*pSubMesh->m_boneIdxs.size(), 1, fp);
				}

				// load vertices
				uint vertCount = pSubMesh->m_vertCount;
				fwrite( &vertCount, sizeof(uint), 1, fp);

				uint vertBuffSize = vertCount * pSubMesh->m_vertStride;
				fwrite( pSubMesh->m_vertices, vertBuffSize, 1, fp);

				// load indices
				uint faceCount = pSubMesh->m_idxCount / 3;
				fwrite( &faceCount, sizeof(uint), 1, fp);

				uint idxBuffSize = faceCount * 3 * sizeof(Word);
				fwrite( pSubMesh->m_indices, idxBuffSize, 1, fp);

				// load aabb
				fwrite( &pSubMesh->m_box.vMin, sizeof(Vector3), 1, fp);
				fwrite( &pSubMesh->m_box.vMax, sizeof(Vector3), 1, fp);
			}
		}
		catch(...)
		{
			return false;
		}

		fclose(fp);

		return true;
	}

	void Mesh::setTextureName(int submeshid, const String& name)
	{
		if (m_DiffuseVector.empty())
		{
			m_DiffuseVector.push_back(name);
		}
		else
		{
			m_DiffuseVector[submeshid] = name;
		}
	}

	String Mesh::getTextureName(int submeshid, ui32 uint)
	{
		LordAssert(uint < 2);//目前每个SubMesh只支持两张纹理， 一张diffuse 一张mask
		if (uint == 0)
		{
			return m_DiffuseVector[submeshid];
		}
		else
		{
			const String& diffuseName = m_DiffuseVector[submeshid];
			size_t nPosition = diffuseName.find_last_of('.');
			String strMaskFile = diffuseName.substr(0, nPosition);
			String strSuffix = diffuseName.substr(nPosition, diffuseName.length() - nPosition);
			strMaskFile += "_s" + strSuffix;

			if (!ResourceGroupManager::Instance()->resourceExistsInAnyGroup(strMaskFile))
			{
				strMaskFile = Root::Instance()->getBlankTexture()->getName();
			}

			return strMaskFile;
		}
	}

}

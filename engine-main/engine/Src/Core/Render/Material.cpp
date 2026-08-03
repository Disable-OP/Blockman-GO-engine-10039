#include "Core.h"
#include "Material.h"
#include "Util/StringUtil.h"
#include "Util/PathUtil.h"
#include "Object/Exception.h"
#include "Object/Root.h"

#include "Renderer.h"
#include "ShaderProgramManager.h"
#include "Image/PixelFormat.h"
#include "Resource/ResourceGroupManager.h"
#include "MaterialDesc.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

namespace LORD
{
	int TextureModify::keyGen = 0;

	TextureModify::TextureModify(const Vector2i& pos, const Vector2i& size, int pixcelBytes)
		: m_pos(pos)
		, m_size(size)
		, m_pixelBytes(pixcelBytes)
	{
		m_data = (ui8*)LordMalloc(pixcelBytes * size.x * size.y);
		m_key = keyGen++;
	}

	TextureModify::~TextureModify()
	{
		LordSafeFree(m_data);
	}

	Material::Material()
		: m_name(StringUtil::BLANK)
		, m_MaterialKey(0)
		, m_ShaderParams(NULL)
		, m_pShaderProgram(NULL)
		, m_SParamNum(0)
		, m_SParamWriteIndex(0)
		, m_bEnableGlow(false)
		, m_pTextureModify(NULL)
	{
		for (size_t i = 0; i < MAX_TEXTURE_SAMPLER; ++i)
		{
			m_Textures[i] = 0;
			createSamplerState(i, SamplerState::SamplerDesc());
		}

		createBlendState(BlendState::BlendDesc());
		createDepthState(DepthStencilState::DepthStencilDesc());
		createRasterizerState(RasterizerState::RasterizerDesc());
	}

	Material::~Material()
	{
		free();
	}

	void Material::free()
	{
		// LordSafeDelete(m_pTextureModify); // outside call, outside manage texModify lift time.
		LordSafeFree(m_ShaderParams);
		LordSafeDelete(m_pBlendState);
		LordSafeDelete(m_pDepthState);
		LordSafeDelete(m_pRasterizerState);
		for (size_t i = 0; i < MAX_TEXTURE_SAMPLER; ++i)
		{
			LordSafeDelete(m_pSamplerState[i]);
			m_Textures[i] = NULL;
		}
	}

	void Material::createBlendState(const BlendState::BlendDesc& desc)
	{
		m_pBlendState = Renderer::Instance()->createBlendState(desc);
	}

	void Material::createDepthState(const DepthStencilState::DepthStencilDesc& desc)
	{
		m_pDepthState = Renderer::Instance()->createDepthStencilState(desc);
	}

	void Material::createRasterizerState(const RasterizerState::RasterizerDesc& desc)
	{
		m_pRasterizerState = Renderer::Instance()->createRasterizerState(desc);
	}

	void Material::createSamplerState(int stage, const SamplerState::SamplerDesc& desc)
	{
		m_pSamplerState[stage] = Renderer::Instance()->createSamplerState(desc);
	}

	void Material::setBlendState(const BlendState::BlendDesc& desc)
	{
		m_pBlendState->setDesc(desc);
	}

	void Material::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
	{
		m_pDepthState->setDesc(desc);
	}

	void Material::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
	{
		m_pRasterizerState->setDesc(desc);
	}

	void Material::setSamplerState(int stage, const SamplerState::SamplerDesc& desc)
	{
		m_pSamplerState[stage]->setDesc(desc);
	}

	void Material::active()
	{
		LordAssert(m_pShaderProgram);
		LordAssert(m_pBlendState);
		LordAssert(m_pDepthState);
		LordAssert(m_pRasterizerState);

		bindShaderParams();

		Renderer* pRenderer = Renderer::Instance();
		pRenderer->setDepthStencilState(m_pDepthState);
		pRenderer->setRasterizerState(m_pRasterizerState);
		pRenderer->setBlendState(m_pBlendState);
		for (uint i = 0; i < MAX_TEXTURE_SAMPLER; ++i)
		{
			if (m_Textures[i])
			{
				if (m_pTextureModify && i == 0 && m_Textures[0]->getUserData() != m_pTextureModify->getKey())
				{
					Vector2i pos = m_pTextureModify->getPosition();
					Vector2i size = m_pTextureModify->getSize();
					ui8* data = m_pTextureModify->getData();
					ERect rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
					m_Textures[0]->updateSubTex2D(0, rect, data);
					m_Textures[0]->setUserData(m_pTextureModify->getKey());
					pRenderer->setTexture(i, nullptr);
				}
				pRenderer->setTexture(i, m_Textures[i]);
			}
		}
	}

	void Material::activeTexture(int stage, Texture* tex)
	{
		if (tex)
		{
			SamplerState* pState = m_pSamplerState[stage];
			LordAssert(pState);
			tex->setSamplerState(pState);
		}

		m_Textures[stage] = tex;

		calcMaterialKey();
	}

	void Material::beginShaderParams(ui32 paramNum)
	{
		LordSafeFree(m_ShaderParams);

		LordAssert(paramNum > 0);
		m_ShaderParams = LordAlloc(ShaderParam, paramNum);
		m_SParamNum = paramNum;
		m_SParamWriteIndex = 0;
	}

	void Material::modifyShaderParam(const String& paramName, ShaderParamType type, void* param, ui32 num/* =1 */)
	{
		LordAssert(m_SParamWriteIndex == m_SParamNum);
		ui32 modifyIndex = 0;
		ui32 physics = m_pShaderProgram->getParamPhysicsIndex(paramName);
		for (modifyIndex; modifyIndex < m_SParamNum; modifyIndex++)
		{
			if (m_ShaderParams[modifyIndex].physicsIndex == physics)
				break;
		}
		LordAssert(modifyIndex < m_SParamNum);
		LordAssert(m_ShaderParams[modifyIndex].physicsIndex == physics);

		m_ShaderParams[modifyIndex].stype = type;
		m_ShaderParams[modifyIndex].pData = param;
		m_ShaderParams[modifyIndex].ParamsLength = num;
	}

	ui32 Material::insertShaderParam(const String& paramName, ShaderParamType type, void* param, ui32 num/*=1*/)
	{
		LordAssert(m_SParamNum > 0);
		ShaderParam* params = LordAlloc(ShaderParam, ++m_SParamNum);
		memcpy(params, m_ShaderParams, (m_SParamNum - 1) * sizeof(ShaderParam));
		LordSafeFree(m_ShaderParams);
		m_ShaderParams = params;
		setShaderParam(paramName, type, param, num);

		return m_SParamWriteIndex - 1;
	}

	void Material::removeShaderParam(ui32 index)
	{
		LordAssert(m_SParamNum > index);
		ShaderParam* params = LordAlloc(ShaderParam, --m_SParamNum);
		memcpy(params, m_ShaderParams, index * sizeof(ShaderParam));
		if (m_SParamNum != index)
		{
			ShaderParam* p = params + index;
			ShaderParam* p2 = m_ShaderParams + index + 1;
			memcpy(p, p2, (m_SParamNum - index) * sizeof(ShaderParam));
		}
		LordSafeFree(m_ShaderParams);
		m_ShaderParams = params;
		--m_SParamWriteIndex;
	}

	void Material::bindShaderParams()
	{
		m_pShaderProgram->bind();
		for (size_t i = 0; i < m_SParamNum; ++i)
		{
			ShaderParam& param = m_ShaderParams[i];
			m_pShaderProgram->setUniform(param.physicsIndex, param.pData, param.stype, param.ParamsLength);
		}
		m_pShaderProgram->bindUniforms();
	}
}

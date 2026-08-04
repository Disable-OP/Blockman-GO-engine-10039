/********************************************************************
filename: 	Material.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_MATERIAL_H__
#define __LORD_MATERIAL_H__

#include "RenderState.h"
#include "ShaderProgram.h"
#include "RenderInput.h"
#include "Renderable.h"

namespace LORD
{
	static const size_t MAX_TEXTURE_SAMPLER = 4;

	class LORD_CORE_API TextureModify : public ObjectAlloc
	{
	public:
		TextureModify(const Vector2i& pos, const Vector2i& size, int pixelBytes);
		~TextureModify();
		const Vector2i& getPosition() const { return m_pos; }
		const Vector2i& getSize() const { return m_size; }
		const int getPixelBytes() const { return m_pixelBytes; }
		ui8* getData() { return m_data; }

		void updateKey() { m_key = keyGen++; }
		int getKey() const { return m_key; }

	private:
		Vector2i m_pos;
		Vector2i m_size;
		ui8* m_data;
		int m_pixelBytes;
		int m_key;
		static int keyGen;
	};

	class Renderer;
	class LORD_CORE_API Material : public ObjectAlloc
	{
	private:
		struct ShaderParam
		{
			ShaderParamType stype;
			ui32			physicsIndex;
			ui32			ParamsLength;  // shader constance register num.
			void*			pData;
		};
	public:
		Material();
		~Material();
	protected:
		// create manual
		void createBlendState(const BlendState::BlendDesc& desc);
		void createDepthState(const DepthStencilState::DepthStencilDesc& desc);
		void createRasterizerState(const RasterizerState::RasterizerDesc& desc);
		void createSamplerState(int stage, const SamplerState::SamplerDesc& desc);

	public:
		// Free all state
		void free();
		void setBlendState(const BlendState::BlendDesc& desc);
		void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
		void setRasterizerState(const RasterizerState::RasterizerDesc& desc);
		void setSamplerState(int stage, const SamplerState::SamplerDesc& desc);

		inline BlendState* getBlendState() const
		{
			return m_pBlendState;
		}

		inline DepthStencilState* getDepthState() const
		{
			return m_pDepthState;
		}

		inline RasterizerState* getRasterizerState() const
		{
			return m_pRasterizerState;
		}

		inline SamplerState* getSamplerState(int stage) const
		{
			LordAssert(stage >= 0 && stage < MAX_TEXTURE_SAMPLER);
			return m_pSamplerState[stage];
		}

		inline void setShaderProgram(ShaderProgram* programe)
		{
			m_pShaderProgram = programe;
			calcMaterialKey();
		}

		inline ShaderProgram* getShaderProgram() const
		{
			return m_pShaderProgram;
		}

		inline void setGlowEnable(bool data)
		{
			m_bEnableGlow = data;
		}

		inline bool getGlowEnable() const
		{
			return m_bEnableGlow;
		}

		void setTextureModify(TextureModify* modify) { m_pTextureModify = modify; }

		void active();
		void activeTexture(int stage, Texture* tex);
		Texture* getTexture(int stage) { LordAssert(stage < 8); return m_Textures[stage]; }
		SamplerState* getSamplerStateByTexStage(int stage) { LordAssert(stage < 8); return m_pSamplerState[stage]; }

		void beginShaderParams(ui32 paramNum);

		void endShaderParams()
		{
			LordAssert(m_SParamWriteIndex == m_SParamNum);
		}

		void setShaderParam(const String& paramName, ShaderParamType type, void* param, ui32 num = 1)
		{
			m_ShaderParams[m_SParamWriteIndex].physicsIndex = m_pShaderProgram->getParamPhysicsIndex(paramName);
			m_ShaderParams[m_SParamWriteIndex].stype = type;
			m_ShaderParams[m_SParamWriteIndex].pData = param;
			m_ShaderParams[m_SParamWriteIndex].ParamsLength = num;
			m_SParamWriteIndex++;
		}
		// you can modify the shader param binding
		void modifyShaderParam(const String& paramName, ShaderParamType type, void* param, ui32 num = 1);
		ui32 insertShaderParam(const String& paramName, ShaderParamType type, void* param, ui32 num = 1);
		void removeShaderParam(ui32 physics);

		// for render.
		void bindShaderParams();
		void bindTextures(Renderer* pRenderer);

		void calcMaterialKey()
		{
			m_MaterialKey = 0x00000000;
			m_MaterialKey |= (ui32)(m_Textures[1] ? (m_Textures[1]->getHandle() & 0x000000FF) : 0);
			m_MaterialKey |= (ui32)(m_Textures[0] ? ((m_Textures[0]->getHandle() & 0x000000FF) << 8) : 0); // default Tex first
			m_MaterialKey |= (ui32)((m_pShaderProgram->getUUID() & 0x000000FF) << 16);
			m_MaterialKey |= (ui32)(m_pBlendState->getDesc().bBlendEnable ? 0x01000000 : 0x00000000);
		}

		ui32 getMaterialKey() const { return m_MaterialKey; }
	private:
		ui32				m_MaterialKey;
		ui32				m_SParamWriteIndex;
		ui32				m_SParamNum;

		Texture*			m_Textures[MAX_TEXTURE_SAMPLER];     // now only one texture sampler.
		ShaderParam*		m_ShaderParams;/// shader params

		// render state
		BlendState*			m_pBlendState;
		DepthStencilState*	m_pDepthState;
		RasterizerState*	m_pRasterizerState;
		SamplerState*		m_pSamplerState[MAX_TEXTURE_SAMPLER];

		// shader
		ShaderProgram*		m_pShaderProgram;
		TextureModify*		m_pTextureModify;  // only override the texture stage 0!
		String				m_name;

		//glow flag
		bool				m_bEnableGlow;
	};
}

#endif

#ifndef __LORD_MODELENTITY_H__
#define __LORD_MODELENTITY_H__

#include "Core.h"
#include "Render/Renderable.h"
#include "Math/Matrix4.h"
#include "Image/Color.h"
#include "Resource/LordThread.h"

namespace LORD
{
	/**
	Renderalbe.
	add this ModelEntity to RenderQueue[xxxx_mesh_for_xxxx];
	**/
	class TextureLoadEvent;
	class LORD_CORE_API ModelEntity : public ObjectAlloc
	{
	public:
		// 渲染类型
		enum RenderType
		{
			RT_FOR_ACTOR,
			RT_FOR_ACTOR_WRITEDEPTH,		// 半透明角色
			RT_FOR_ACTOR_TRANSPARENT,		// 半透明角色
			RT_FOR_SCENE_L,
			RT_FOR_SCENE_L_NOFOG,
			RT_FOR_SCENE,
			RT_FOR_HAZE
		};

		enum EFFECT_ID
		{
			STONE = 0,
			FROZEN,
			POISON
		};

		typedef list<TextureLoadEvent*>::type TextureLoadEventList;

		ModelEntity();
		virtual ~ModelEntity();

	public:
		void			setMesh(Mesh* pMesh) { m_pMesh = pMesh; }
		Mesh*			getMesh() const { return m_pMesh; }

		Material*       getMaterial() { return m_Material; }

		void			attachTo(SceneNode* pNode) { m_pSceneNode = pNode; }
		SceneNode*		getSceneNode() const { return m_pSceneNode; }

		void			setMaterial(Material* material);

		void			setRenderType(RenderType type) { m_renderType = type; }
		RenderType		getRenderType() const { return m_renderType; }

		virtual void	createRenderable(bool isUIActor = false);
		virtual void	update(const Matrix4* viewproj = NULL);
		virtual void	update2(); // for mc hold item mesh render.

		void			setBrightness(const Color& color) { m_brightness = color; }
		void			setNaturalColor(const Color& color) { m_naturalColor = color; }
		const Color&	getNaturalColor() const { return m_naturalColor; }

		void			setCustomColor(const Color& color) { m_customColor = color; }
		const Color&	getCustomColor() const { return m_customColor; }
		
		void			setAlphaColor(const Color& color) { m_alphaColor = color; }
		const Color&	getAlphaColor() const { return m_alphaColor; }

		Renderable*	    getRenderables(int id) { if(id >=0 && id < m_renderables.size()) return m_renderables[id]; }

		void			setEditorColor(i32 subId);
		void			resetEditorColor();

		Real			getViewDepth(Camera* pCamera) const;

		Box		getLocalAABB() const;
		Box		getWorldAABB() const;

		void	setVisible(const bool& visible) { isVisible = visible; }
		const bool&		getVisible() const { return isVisible; }

		void	setFadeOut(bool fadeOut) { isFadeOut = fadeOut; }
		bool	getFadeOut() { return isFadeOut; }

		// 提交到渲染队列
		virtual void updateRenderQueue(RenderQueue* pQueue);

		inline void updateUVanimation(const Vector2& deltaUV)
		{
			m_UVAnimationPos += deltaUV;

			if (m_UVAnimationPos.x > 1.0f)			m_UVAnimationPos.x -= 1.0f;
			else if (m_UVAnimationPos.x < -1.0f)	m_UVAnimationPos.x += 1.0f;

			if (m_UVAnimationPos.y > 1.0f)			m_UVAnimationPos.y -= 1.0f;
			else if (m_UVAnimationPos.y < -1.0f)	m_UVAnimationPos.y += 1.0f;
		}
		inline void resetUVanimation()
		{
			m_UVAnimationPos = Vector2::ZERO;
		}

		inline void		setColorParams(const Vector4& params) { m_colorParams = params; }

		inline void		setRimColor(const Color& color) { m_edgeColor = color; }
		inline void		setAmbientColor(const Color& color) { m_ambientColor = color; }

		virtual void	setEffectTexture(Texture* pTexture, int effect);
		virtual void	revertEffectTexture();
		void			setLMTexture(int subId, Texture* pTexture);
		void			unloadLMTexture();

		void			setLightMapUVBias(int subId, const Vector4& lmUVparam, const Vector3& scale)
		{
			m_lightmapUV[subId] = lmUVparam;
			m_lightmapScale[subId] = scale;
		}

		void	setDarkRadio(float factor)
		{
			m_RenderParam.y = factor;
		}

		// 设置是否使用XRay
		virtual void    setUseXRay(bool isUseXRay, const Vector4* color);

		virtual void	EnableAlphaTest(bool bEnable);

		virtual void replaceTexture(Texture* pTexture, size_t subid, ui32 uint);

		static const uint TextureUint0;
		static const uint TextureUint1;
		static const uint TextureUint2;

		virtual void addTextureLoadEvent(TextureLoadEvent* event)
		{
			m_TextureEventList.push_back(event);
		}

		virtual void deleteTextureLoadEvent(TextureLoadEvent* event)
		{
			TextureLoadEventList::iterator it = std::find(m_TextureEventList.begin(), m_TextureEventList.end(), event);
			if (it != m_TextureEventList.end())
			{
				m_TextureEventList.erase(it);
			}
		}

		virtual void prepareTexture(bool bSingleThread, const String& func = StringUtil::BLANK);

		void EnableGrayRender(float gray) { m_RenderParam.x = gray; }
		float IsEnableGrayRender() const { return m_RenderParam.x; }
	protected:
		static	Vector4 m_UIFogParam;
		bool						isVisible;			// 可见性
		bool                        isFadeOut;
		bool						m_isUseXRay;		// 是否使用xray
		bool						m_isAlphaTest;		// 是否开启alpha discard
		bool						m_isUIActor;

		RenderType					m_renderType;		// 渲染类型
		i32							editor_subId;

		Mesh*						m_pMesh;			// 关联模型
		SceneNode*					m_pSceneNode;		// 场景结点
		Material*					m_Material;			// 材质

		Vector2						m_RenderParam;		// 灰度渲染 暗化
		Vector2						m_UVAnimationPos;	// UV动画
		Vector4						m_colorParams;
		Vector4						m_xrayColor;		// xray 颜色
		Color						m_naturalColor;		// 自发光颜色
		Color						m_customColor;		// 用户自定义肤色
		Color						m_alphaColor;		// 半透明颜色 a分量有效
		Color						m_edgeColor;
		Color						editor_Color;
		Color						m_ambientColor;		// 自定义环境色
		Matrix4						m_matWVP;			// 世界观察投影矩阵
		Color						m_brightness;		// 亮度

		vector<RenderInput*>::type	m_renderInputs;		// 几何体数据
		vector<Renderable*>::type	m_renderables;		// 可渲染对象
		vector<Renderable*>::type	m_xrayRenderables;	// 可渲染对象供Xray用
		vector<Vector3>::type       m_lightmapScale;
		vector<Vector4>::type	    m_lightmapUV;
		TextureLoadEventList		m_TextureEventList;
	};

	class TextureLoadEvent : public ThreadEvent
	{
	public:
		TextureLoadEvent(int subid, ui32 uint, ModelEntity* entity);
		virtual ~TextureLoadEvent();
		virtual bool IsAutoDestroy() { return true; }

		// 返回该事件的优先级别
		virtual EVENT_LEVEL GetLevel() { return E_HIGHT; }
	public:
		virtual bool ProcessEvent();
		virtual bool RespondEvent();
	public:
		int mSubMeshId;
		ui32 mTextureUint;
		ModelEntity* mEntity;
		Texture* m_Texture;
		String mName;
	};
}

#endif
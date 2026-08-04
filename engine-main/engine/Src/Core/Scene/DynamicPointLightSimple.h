#ifndef _SIMPLE_DYNAMICPOINTLIGHT_
#define _SIMPLE_DYNAMICPOINTLIGHT_

#include "GameObject.h"

namespace LORD
{
	/**
	 * SimpleDyamicPointLight 2014-01-02
	 * 简化版动态点光源 适合于锁定视角2.5D游戏
	 */
	class LORD_CORE_API DynamicPointLightSimple : public GameObject
	{
	public:
		DynamicPointLightSimple();
		~DynamicPointLightSimple();

		// 设置半径
		void setRadius( float radius);

		// 设置衰减
		void setFalloff( float falloff) { m_falloff = falloff; }

		// 设置颜色值
		void setColor( const Color& color) { m_color=color; }

		// 设置是否影响主角
		void setInfluenceActor( bool isInfluence);

		// 开关
		void setEnabled( bool enabled) { m_isEnabled = enabled; }

		// 渲染
		void _render();
		void frameMove();

		// 更新世界包围盒
		virtual void updateWorldAABB();

	private:
		// 准备资源
		virtual void _prepare_io();

		// io线程完成后的工作,创建d3d对象,挂接到场景节点的过程
		virtual void _prepare_obj();

		// 准备编辑器所需资源
		virtual void _prepare_editor();

		// 处理无用资源
		virtual void _unprepare();
		
		// 获取世界包围盒
		virtual const Box& getWorldAABB() const;

		///// 导出数据 用于游戏
		virtual bool exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc);

	private:
		bool				m_isEnabled;			// 是否可用
		Matrix4				m_matW;					// 世界矩阵
		Matrix4				m_matWVP;				// 世界观察投影矩阵
		Vector4				m_positionAndInvRadius;	// 位置与半径的逆
		Vector4				m_colorAddFalloff;		// 颜色与衰减值数
		Color				m_color;				// 光源颜色
		float				m_radius;				// 半径
		float				m_falloff;				// 衰减
		Box					m_localAABB;			// 本地包围盒
		Renderable*			m_renderable;			// 渲染对象
		RenderInput*		m_renderInput;			// 输入
		RenderInput::VertexElementList m_VertexElement;
	
		static int			m_dynamicPLNum;			// 点光源数量
		static GPUBuffer*	m_vertexBuffer;			// 顶点流(表示点光源几何体)
		static GPUBuffer*	m_indexBuffer;			// 索引流
	};

	class DynamicLightSubset : public ObjectSubset
	{
	public:
		DynamicLightSubset(){m_Type = GOT_SIMDPOINTLIGHT;}
		virtual ~DynamicLightSubset(){}

		virtual void frameMove(i32 deltaTime)
		{
			for (set<DynamicPointLightSimple*>::type::iterator it = mObjects.begin(); it != mObjects.end(); ++it)
			{
				DynamicPointLightSimple* object = *it;
				object->frameMove();
				object->_render();
			}
		}
		virtual void addObject(GameObject* object)
		{
			object->setFrustumCull(false);
			mObjects.insert((DynamicPointLightSimple*)object);
		}
		virtual void deleteObject(GameObject* object)
		{
			mObjects.erase((DynamicPointLightSimple*)object);
		}
		virtual void clearObjects()
		{
			set<DynamicPointLightSimple*>::type::iterator it = mObjects.begin();
			for (; it != mObjects.end(); ++it)
			{
				DynamicPointLightSimple* pObject = *it;
				pObject->setFrustumCull(true);
			}
			mObjects.clear(); 
		}
	protected:
		set<DynamicPointLightSimple*>::type mObjects;
	};
}

#endif
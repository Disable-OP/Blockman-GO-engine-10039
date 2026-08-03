#include "DynamicPointLightSimple.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"
#include "Scene/Scene.h"

namespace LORD
{
	int		   DynamicPointLightSimple::m_dynamicPLNum = 0;
	GPUBuffer* DynamicPointLightSimple::m_vertexBuffer = NULL;	// 顶点数据
	GPUBuffer* DynamicPointLightSimple::m_indexBuffer  = NULL;	// 顶点数据

	// 构造函数
	DynamicPointLightSimple::DynamicPointLightSimple()
		: GameObject()
		, m_renderable( NULL)
		, m_renderInput( NULL)
		, m_radius( 1.f)
		, m_color( Color::WHITE)
		, m_falloff( 1.f)
		, m_isEnabled( true)
	{
		m_Type = GOT_SIMDPOINTLIGHT;
		m_isNeedSave = false;
		m_dynamicPLNum++;
	}

	// 析构函数
	DynamicPointLightSimple::~DynamicPointLightSimple()
	{
		_unprepare();

		LordSafeDelete( m_renderable);
		LordSafeDelete( m_renderInput);

		if( (--m_dynamicPLNum)<=0)
		{
			LordSafeDelete( m_vertexBuffer);
			LordSafeDelete( m_indexBuffer);
		}
	}

	// 设置是否影响主角
	void DynamicPointLightSimple::setInfluenceActor( bool isInfluence)
	{
		//if( isInfluence)
		//	m_renderable->setRenderQueueType( "DynamicPointLightSimpleInfluenceActor");
		//else
		//	m_renderable->setRenderQueueType( RENDER_QUEUE_dynamic_point_light_simple);
	}

	// 渲染
	void DynamicPointLightSimple::_render()
	{
		if( m_isEnabled)
		{
			m_RenderQueue->addRenderable(m_renderable);
		}
	}

	// 设置半径
	void DynamicPointLightSimple::setRadius( float radius) 
	{ 
		m_radius = radius; 

		m_SceneNode->setLocalScaling( Vector3( m_radius, m_radius, m_radius));
		m_SceneNode->update();
	}

	// 每帧更新
	void DynamicPointLightSimple::frameMove()
	{
		if (m_isEnabled)
		{
			Matrix4 matVP = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();

			m_matW = m_SceneNode->getWorldMatrix();
			m_matWVP = m_matW * matVP;
			m_positionAndInvRadius = Vector4(m_Position.x, m_Position.y, m_Position.z, 1.f / m_radius);
			m_colorAddFalloff = Vector4(m_color.r, m_color.g, m_color.b, m_falloff);
		}
	}

	// 准备资源
	void DynamicPointLightSimple::_prepare_io()
	{
		
	}

	// io线程完成后的工作,创建d3d对象,挂接到场景节点的过程
	void DynamicPointLightSimple::_prepare_obj()
	{
		//创建GPUBuffer
		if( !m_vertexBuffer)
		{
			Vector3 vertexBufferData[] =
			{
				Vector3( -1.f, 0.f, -1.f),
				Vector3(  1.f, 0.f, -1.f),
				Vector3(  1.f, 0.f,  1.f),
				Vector3( -1.f, 0.f, 1.f)
			};

			ui16 indexBufferData[] = { 0, 3, 2, 0, 2, 1};

			Buffer vertexBuffer( sizeof( vertexBufferData), vertexBufferData);
			Buffer indexBuffer( sizeof( indexBufferData), indexBufferData);
			m_vertexBuffer = Renderer::Instance()->createVertexBuffer( GPUBuffer::GBU_GPU_READ, vertexBuffer);
			m_indexBuffer  = Renderer::Instance()->createIndexBuffer( GPUBuffer::GBU_GPU_READ, indexBuffer);
		}
		m_localAABB = Box( -1.f, -1.f, -1.f, 1.f, 1.f, 1.f);

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode  = m_ParentNode->createChild();

		LordSafeDelete( m_renderable);
		LordSafeDelete( m_renderInput);

		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("DynamicPointLightSimple");
		m_renderable = LordNew Renderable();
		Material* material = LordNew Material();
		material->setShaderProgram(shaderProgram);
		m_renderable->setMaterial(material);

		RenderInput::VertexElement element;
		element.semantic = RenderInput::VS_POSITION;
		element.pixFmt = PF_RGB32_FLOAT;
		m_VertexElement.push_back(element);

		m_renderInput = Renderer::Instance()->createRenderInput(m_VertexElement, shaderProgram);
		m_renderInput->bindGPUBuffers(m_vertexBuffer, sizeof(Vector3), m_indexBuffer, sizeof(ui16));
		m_renderable->setRenderInput(m_renderInput);

		material->beginShaderParams(4);
		material->setShaderParam("matW", SPT_MAT4, (void*)&m_matW);
		material->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
		material->setShaderParam("LightPositionAndInvRadius", SPT_VEC4, (void*)&m_positionAndInvRadius);
		material->setShaderParam("LightColorAndFalloffExponent", SPT_VEC4, (void*)&m_colorAddFalloff);
		material->endShaderParams();

		frameMove();

		Scene* scene = SceneManager::Instance()->getCurrentScene(); LordAssert(scene);
		scene->registerGameObjectToZoom(this);

		m_IOprepared = true;
	}

	// 准备编辑器所需资源
	void DynamicPointLightSimple::_prepare_editor()
	{

	}

	// 处理无用资源
	void DynamicPointLightSimple::_unprepare()
	{
		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		pScene->unregisterGameObjectFromZoom(this);
		pScene->registerGameObjectToZoom_unprepare(this);

		m_ParentNode->destroyChild(m_SceneNode);
		m_SceneNode = NULL;
	}

	// 获取世界包围盒
	const Box& DynamicPointLightSimple::getWorldAABB() const
	{
		return m_worldAABB;
	}

	// 更新世界包围盒
	void DynamicPointLightSimple::updateWorldAABB()
	{
		m_worldAABB = m_localAABB.transform( m_SceneNode->getWorldMatrix());
	}

	///// 导出数据 用于游戏
	bool DynamicPointLightSimple::exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc)
	{
		return m_isNeedSave ? GameObject::exportData( parentNode, doc) : false;
	}
}
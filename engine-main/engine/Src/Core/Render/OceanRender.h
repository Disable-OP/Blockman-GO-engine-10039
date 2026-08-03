/********************************************************************
filename: 	OceanRender.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_OCEANRENDER_H__
#define __LORD_OCEANRENDER_H__

#include "Renderable.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "rapidjson/document.h"

namespace LORD
{
	class LORD_CORE_API OceanRender : public Renderable
	{
	public:
		struct OceanBrushInfo
		{
			float radius;
			float strength;
			bool depth;
		};
	public:
		OceanRender();
		virtual ~OceanRender();

		void prepare_sys()
		{
			_prepare_res();
			_prepare_io();
			_prepare_obj();
		}
		/// 从磁盘读入纹理 1张反射cubemap 1张normalmap
		void _prepare_io();
		/// 创建资源对象
		void _prepare_res();
		/// 创建设备所需资源 
		void _prepare_obj();
		/// 销毁纹理和GpuBuffer.
		void _unprepare();
		/// 创建几何
		void _createGeometry(ui32 size);
		void _destroyGeometry();
	
		void setGridNum(ui16 _num);
		void setGridSize(float _size);
		ui16 getGridNum() const { return m_NumGird; }
		float getGridSize() const { return m_GridSize; }

		virtual void _updateRenderQueue(RenderQueue* pRenderQueue);		

		/// 导出数据 二进制
		void exportData(DataStream& dataStream);
		/// 导入数据 二进制
		void importData(DataStream& dataStream);
		/// 导出海洋深度数据 二进制
		void exportDepthData(DataStream& dataStream);
		/// 导入海洋深度数据 二进制
		void importDepthData(DataStream& dataStream);
		/// xml
		void exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc);
		void importData(rapidxml::xml_node<>* parentNode);
		/// json
		void exportData(rapidjson::Value& node, rapidjson::Document* doc);
		void importData(rapidjson::Value& node);

		void setHeight( float wl);
		float getHeight() const { return m_WaterHeight; }

		void setdtSpace(float val) { m_WaveParam.x = m_WaveParam.y = val; }
		float getdtSpace() const { return m_WaveParam.x; }

		void setWaveParam(const Vector4& f);
		Vector4 getWaveParam() const { return Vector4(m_WaterHeight, m_WaveSpeed, m_WaveParam.z, m_WaveParam.w); }

		void setShoreDark(const Vector3& dark) { m_ShoreDark = dark; }
		const Vector3& getShoreDark(){ return  m_ShoreDark; }

		void setShoreLight(const Vector3& light) { m_ShoreLight = light; }
		const Vector3& getShoreLight(){ return m_ShoreLight; }

		void setSeaDark(const Vector3& dark) { m_SeaDark = dark; }
		const Vector3& getSeaDark() { return m_SeaDark; }

		void setSeaLight(const Vector3& light){ m_SeaLight = light; }
		const Vector3& getSeaLight(){ return m_SeaLight; }

		void setLightPos(const Vector3& pos){ m_LightPos = pos; }
		const Vector3& getLightPos(){ return m_LightPos; }

		const String& getNormalMapName() const { return m_NormalmapName; }
		const String& getFoamName() const { return m_FoamName; }

		void setFadeOutDistance(float dist) { m_PixelParam.x = dist; }
		float getFadeOutDistance() const { return m_PixelParam.x; }

		void changeDark(float radio) { m_PixelParam.y = radio; }

		ui32 getVertexStride() const { return m_VertexSize; }
		ui32 getIndexCount() const { return m_IndexCount; }
		const float* getVertexBuffer() const { return m_VertexBuffer; }
		const ui16* getIndexBuffer() const { return m_IndexBuffer; }

		Vector3 editor_modify(const Vector2& pos, const OceanBrushInfo& brushInfo, bool bBrush);
	protected:
		bool		m_isPrepared;
		float		m_GridSize;
		float		m_WaveSpeed;
		float       m_WaterHeight;
		float		m_CurTime;
		Vector2		m_PixelParam;
		Vector3		m_ShoreDark;
		Vector3		m_ShoreLight;
		Vector3		m_SeaDark;
		Vector3		m_SeaLight;
		Vector3		m_LightPos;
		Vector4		m_WaveParam;// u_1DivLevelWidth, u_1DivLevelHeight, WAVE_HEIGHT, WAVE_MOVEMENT
		Matrix4		m_matVP;

		int		m_NumGird;
		ui32	m_VertexCount;
		ui32    m_IndexCount;
		ui32	m_VertexSize;
		float*	m_VertexBuffer;	// 水面顶点
		ui16*	m_IndexBuffer;	// 水面索引
		GPUBuffer* m_GpuVertexBuffer;
		GPUBuffer* m_GpuIndexBuffer;
		Texture*	m_pNormalmap;		// 法线图
		Texture*	m_pFoamMap;
		String	m_NormalmapName;		// 法线图名称
		String	m_FoamName;
	};
}

#endif // __UBI_OCEANRENDER_H__

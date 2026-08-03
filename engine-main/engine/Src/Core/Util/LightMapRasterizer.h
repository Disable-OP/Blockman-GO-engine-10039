#ifndef _LIGHTMAPRASTERIZER_H_ 
#define _LIGHTMAPRASTERIZER_H_

#include "Core.h"
#include "Scene/Scene.h"

namespace LORD
{ 
	/**
	 * 定制光栅器,光照图烘焙使用
	 * 参考：http://joshbeam.com/articles/triangle_rasterization 
	 */
	class  LORD_CORE_API LightmapRasterizer : public ObjectAlloc
	{
	public:
		// 顶点格式(顶点计算)
		struct VertexInput
		{
			Vector3		m_position;		// 位置
			Vector3		m_normal;		// 顶点法线
			Vector2		m_uv;			// 漫反射UV
			Vector2		m_uniqueUV;		// 光照图UV
		};

		// 顶点格式(象素计算)
		struct PixelInput
		{
			Vector2	 m_uv;				// 纹理坐标
			Vector3	 m_position;		// 位置
			Vector3	 m_normal;			// 法线

			// 重载运算符 "-"
			PixelInput operator + ( const PixelInput& pi) const;

			// 重载运算符 "-"
			PixelInput operator - ( const PixelInput& pi) const;

			// 重载运算符 "*"
			PixelInput operator * ( float scale) const;
		};

		// 边
		struct Edge
		{
			PixelInput m_v1;
			PixelInput m_v2;	// 位置法线数据

			Edge( const PixelInput& v1, const PixelInput& v2);

			// 辅助函数
			float X1() const { return m_v1.m_uv.x; }
			float X2() const { return m_v2.m_uv.x; }
			float Y1() const { return m_v1.m_uv.y; }
			float Y2() const { return m_v2.m_uv.y; }
		};

		// 一行象素
		struct Span
		{
			PixelInput m_v1, m_v2;

			Span(const PixelInput &v1, const PixelInput &v2);

			// 辅助函数
			int X1() const { return static_cast<int>( m_v1.m_uv.x); }
			int X2() const { return static_cast<int>( m_v2.m_uv.x); }
		};

	public:
		LightmapRasterizer();
		~LightmapRasterizer();

		// 设置帧缓冲宽高
		void setFramebuffer( uint width, uint height);

		// 绘制
		void draw( vector<VertexInput>::type& vertices, vector<Word>::type& indices, const Scene::GameObjectList& ltList);

		// 获取颜色值
		const vector<Color>::type& getFrameBuffer() const { return m_frameBuffer; }

	protected:
		// 顶点着色器
		virtual void execVertexShader( PixelInput& out, const VertexInput& input);

		// 象素着色器
		virtual Color execPixelShader( const PixelInput& input, const Scene::GameObjectList& ltList);

	private:
		// 清空帧缓冲
		void clear( const Color& color);

		// 绘制三角形
		void drawTriangle( const VertexInput& v0, const VertexInput& v1, const VertexInput& v2, const Scene::GameObjectList& ltList);

		// 绘制两边之间的象素
		void drawSpansBetweenEdges( const Edge& e1, const Edge& e2, const Scene::GameObjectList& ltList);

		// 绘制象素
		void drawSpan(const Span &span, int y, const Scene::GameObjectList& ltList);

		// 设置象素值
		void setPixel( uint x, uint y, const Color &color);

		// 边缘扩展 
		void edgePadding( int extendPixel=4);

		// 模糊处理
		void gaussianBlur( float radius=4.f);

	private:
		uint				m_width;		// 宽
		uint				m_height;		// 高
		vector<Color>::type	m_frameBuffer;	// 帧缓冲

		Matrix4				m_world;		// 世界矩阵
	};
}

#endif
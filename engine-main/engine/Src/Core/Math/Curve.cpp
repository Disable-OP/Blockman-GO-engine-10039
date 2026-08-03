#include "Core.h"
#include "Math/Curve.h"

namespace LORD
{
	//---------------------------------------------------------------
	// 主要参考：<<DirectX 实时渲染技术详解>> 第21章节 贝赛尔面片
	// 
	// 说明：
	//	由一组控制点来表述的曲线(此处我暂只给出四个控制点的实现)
	// out 返回点
	// v1,v2,v3,v4 控制点
	// amount      控制量
	Vector3 Bezier3( Vector3& out, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float amount)
	{
		// amount
		float  s  = amount;
		float _s = 1.f - amount;

		out = v1 *   _s * _s * _s +
			  v2 *  3.f *  s * _s * _s +
			  v3 *  3.f *  s *  s * _s +
			  v4 *    s *  s *  s;

		return out;
	}

	// 说明：
	//     Catmull-Rom样条可以使空间中的点序列变得平滑,它产生了一条
	// 能够经过原始路径中所有控制点的曲线(不像Bezier曲线,虽然比较平
	// 滑但是不能经过所有的控制点)。
	Vector3 CatmullRom3( Vector3& out, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float amount)
	{
		// amount
		float u3 = amount * amount * amount;
		float u2 = amount * amount;
		float u1 = amount;

		out = v1 * ( -0.5f * u3 +         u2 - 0.5f * u1) +
			v2 * (  1.5f * u3 + -2.5f * u2 + 1.f)		  +
			v3 * ( -1.5f * u3 +   2.f * u2 + 0.5f * u1) +
			v4 * (  0.5f * u3 -  0.5f * u2);

		return out;
	}
}
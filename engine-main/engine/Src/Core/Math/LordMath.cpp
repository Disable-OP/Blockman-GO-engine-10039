#include "Core.h"
#include "Math/LordMath.h"

namespace LORD
{

	namespace Math
	{
		// predefined specific
		const Real PI						= (Real)(3.14159265358979323846);
		const Real PI_2						= Math::PI * 2.0f;
		const Real PI_DIV2					= Math::PI * 0.5f;
		const Real PI_DIV3					= Math::PI / 3.0f;
		const Real PI_DIV4					= Math::PI / 4.0f;
		const Real PI_DIV5					= Math::PI / 5.0f;
		const Real PI_DIV6					= Math::PI / 6.0f;
		const Real PI_DIV8					= Math::PI / 8.0f;
		const Real PI_DIV180				= Math::PI / 180.0f;
		const Real PI_SQR					= (Real)(9.86960440108935861883449099987615113531369940724079);
		const Real PI_INV					= (Real)(0.31830988618379067153776752674502872406891929148091);
		//const Real EPSILON				= (Real)(1e-10);
		const Real EPSILON					= std::numeric_limits<Real>::epsilon();
		const Real LOWEPSILON				= (Real)(1e-04);
		const Real POS_INFINITY				= std::numeric_limits<Real>::infinity();
		const Real NEG_INFINITY				= -std::numeric_limits<Real>::infinity();
		const Real LN2						= std::log(2.0f);
		const Real LN10						= std::log(10.0f);
		const Real INV_LN2					= 1.0f / LN2;
		const Real INV_LN10					= 1.0f / LN10;
		const Real DEG2RAD					= (Real)0.01745329;
		const Real RAD2DEG					= (Real)57.29577;
		const Real SMALL_NUMBER				= (1.e-8f);
		const Real KINDA_SMALL_NUMBER		= (1.e-4f);

#ifdef LORD_PRECISION_DOUBLE
		const Real MIN_REAL					= 2.2250738585072014e-308;
		const Real MAX_REAL					= 1.7976931348623158e+308;
#else
		const Real MIN_REAL					= 1.175494351e-38F;
		const Real MAX_REAL					= 3.402823466e+38F;
		const Real SQUARER3					= (Real)1.7320508075689;
		const Real SQUARER2					= (Real)1.4142135623731;

#endif

		const float MIN_FLOAT				= 1.175494351e-38F;
		const float MAX_FLOAT				= 3.402823466e+38F;
		const double MIN_DOUBLE				= 2.2250738585072014e-308;
		const double MAX_DOUBLE				= 1.7976931348623158e+308;

		const Byte MAX_BYTE					= 0xff;
		const short MIN_SHORT				= -32768;
		const short MAX_SHORT				= 32767;
		const int MIN_INT					= -2147483647-1;
		const int MAX_INT					= 2147483647;
		const Word MAX_WORD					= 0xff;
		const Dword MAX_DWORD				= 0xffff;
		const i8 MIN_I8						= -128;
		const i8 MAX_I8						= 127;
		const ui8 MAX_UI8					= 0xff;
		const i16 MIN_I16					= -32768;
		const i16 MAX_I16					= 32767;
		const ui16 MAX_UI16					= 0xffff;
		const i32 MIN_I32					= -2147483647-1;
		const i32 MAX_I32					= 2147483647;
		const ui32 MAX_UI32					= 0xffffffff;
		const i64 MIN_I64					= -9223372036854775807-1;
		const i64 MAX_I64					= 9223372036854775807;
		const ui64 MAX_UI64					= 0xffffffffffffffff;

		const int SINCOSTABSZ				= 65536;
		const int SINCOSTABSZ_LOG2			= 10;
		float* g_sintab						= NULL;


		void InitMathLib()
		{
			LordAssertX(g_sintab == NULL, "Math lib have inited");

			g_sintab = (float*)LordMalloc(sizeof(float) * (SINCOSTABSZ));
		
			for (int i = 0; i < SINCOSTABSZ; i++)
			{
				g_sintab[i] = (float)sin((double)i * PI_2 / 65536.0);
			}
		}

		void ReleaseMathLib()
		{
			LordSafeFree(g_sintab);
		}

// 		ui32 FAST_TABLE_SIN_SIZE			= 360;
// 		ui32 FAST_TABLE_TAN_SIZE			= 360;
// 		float INV_SIN_TABLE_SIZE			= 1.0f/FAST_TABLE_SIN_SIZE;
// 		float INV_TAN_TABLE_SIZE			= 1.0f/FAST_TABLE_TAN_SIZE;
// 		float INV_SIN_TABLE_SIZE_RAD_TO_DEGREE = INV_SIN_TABLE_SIZE * RAD2DEG;
// 		float INV_TAN_TABLE_SIZE_RAD_TO_DEGREE = INV_TAN_TABLE_SIZE * RAD2DEG;
// 		float* ARR_SIN_TABLE				= NULL;
// 		float* ARR_TAN_TABLE				= NULL;

		ui32 FAST_TABLE_COS_SIZE			= 360;
		float COS_FACTOR					= FAST_TABLE_COS_SIZE / PI;
		float* ARR_COS_TABLE				= NULL;

		bool FAST_MATH_TABLE_INIT			= false;

		Real UnitRandom(unsigned int uiSeed)
		{
			if (uiSeed > 0)
			{
				srand(uiSeed);
			}

			Real dRatio = ((Real)rand())/((Real)(RAND_MAX));
			return dRatio;
		}

		Real SymmetricRandom(unsigned int uiSeed)
		{
			if (uiSeed > 0.0)
				srand(uiSeed);

			Real dRatio = ((Real)rand())/((Real)(RAND_MAX));
			return (2.0f * dRatio - 1.0f);
		}

		Real IntervalRandom (int fMin, int fMax, unsigned int uiSeed)
		{
			return IntervalRandom(static_cast<Real>(fMin), static_cast<Real>(fMax), uiSeed);
		}

		Real IntervalRandom(Real fMin, Real fMax, unsigned int uiSeed)
		{
			if (uiSeed > 0)
				srand(uiSeed);

			Real dRatio = ((Real)rand())/((Real)(RAND_MAX));
			return (fMin + (fMax-fMin)*dRatio);
		}

		// 计算法线(仅适用于Int型索引, trianglelist拓扑类型)
		void TBNComputeNormal( Vector3* normals, const Vector3* positions, int numVertex, const Word* tris, int numTris)
		{
			memset( normals, 0, sizeof(Vector3)*numVertex);
			for ( int i=0; i<numTris; i++)
			{
				int  idxB = i*3;
				Word idx0 = tris[idxB+0];
				Word idx1 = tris[idxB+1];
				Word idx2 = tris[idxB+2];

				LordAssert( idx0<numVertex);
				LordAssert( idx1<numVertex);
				LordAssert( idx2<numVertex);

				Vector3 edge0 = positions[idx1] - positions[idx0];
				Vector3 edge1 = positions[idx2] - positions[idx0];

				// calc triangle normal
				Vector3 faceNormal;
				Vector3::Cross( faceNormal, edge0, edge1);
				faceNormal.normalize();

				normals[idx0] += faceNormal;
				normals[idx1] += faceNormal;
				normals[idx2] += faceNormal;
			}

			// 单位化
			for ( int i=0; i<numVertex; i++)
				normals[i].normalize();
		}

		//  Calculate tangent binormal
		void TBNComputeTangentBinormal( Vector3* tangents, Vector3* binormals, const Vector3* positions, const Vector2* uvs, const Vector3* normals, int numVertex, const Word* tris, int numTris)
		{
			memset( tangents, 0, sizeof(Vector3)*numVertex);
			memset( binormals,0, sizeof(Vector3)*numVertex);

			for ( int i=0; i<numTris; i++)
			{
				int idxB = i*3;
				int idx0 = tris[idxB+0];
				int idx1 = tris[idxB+1];
				int idx2 = tris[idxB+2];

				const Vector3& v0 = positions[idx0];
				const Vector3& v1 = positions[idx1];
				const Vector3& v2 = positions[idx2];

				Vector3 v1v0	  = v1 - v0;
				Vector3 v2v0	  = v2 - v0;

				const Vector2& uv0= uvs[idx0];
				const Vector2& uv1= uvs[idx1];
				const Vector2& uv2= uvs[idx2];

				float s1 = uv1.x - uv0.x;
				float t1 = uv1.y - uv0.y;

				float s2 = uv2.x - uv0.x;
				float t2 = uv2.y - uv0.y;

				float denominator = s1 * t2 - s2 * t1;

				Vector3 tangent, binormal;
				if( std::abs(denominator) < std::numeric_limits<float>::epsilon())
				{
					tangent = Vector3( 1, 0, 0);
					binormal= Vector3( 0, 1, 0);
				}
				else
				{
					tangent  = ( t2 * v1v0 - t1 * v2v0) / denominator;
					binormal = ( s1 * v2v0 - s2 * v1v0) / denominator;
				}

				// 副线
				binormals[idx0] += binormal;
				binormals[idx1] += binormal;
				binormals[idx2] += binormal;

				// 切线
				tangents[idx0]  += tangent;
				tangents[idx1]  += tangent;
				tangents[idx2]  += tangent;
			}

			for( int i=0; i<numVertex; i++)
			{
				Vector3 tT = tangents[i];
				Vector3 tB = binormals[i];
				Vector3 tN = normals[i];

				// Gram-Schmidt orthogonalize
				tT = ( tT - tN * Vector3::Dot( tT, tN));
				tT.normalize();

				// calculate handedness
				float k=1;
				Vector3 binormal;
				Vector3::Cross( binormal, tN, tT);
				if( Vector3::Dot( binormal, tB) <0.f)
					k = -1;			

				tangents[i]  = k * tT;
				Vector3::Cross( binormals[i], tN, tT);
			}
		}

// 		void BuildFastMathTable(ui32 fastTableCosSize/*, ui32 fastTableTanSize*/)
// 		{
// 			FAST_TABLE_SIN_SIZE = fastTableSinSize;
// 			FAST_TABLE_TAN_SIZE = fastTableTanSize;
// 
// 			INV_SIN_TABLE_SIZE = 1.0f / fastTableSinSize;
// 			INV_TAN_TABLE_SIZE = 1.0f / fastTableTanSize;
// 			INV_SIN_TABLE_SIZE_RAD_TO_DEGREE = FAST_TABLE_SIN_SIZE / PI;
// 			INV_TAN_TABLE_SIZE_RAD_TO_DEGREE = FAST_TABLE_TAN_SIZE / (2 * PI);
// 
// 			ARR_SIN_TABLE = (float*)LordMalloc(sizeof(float)*FAST_TABLE_SIN_SIZE);
// 			ARR_TAN_TABLE = (float*)LordMalloc(sizeof(float)*FAST_TABLE_TAN_SIZE);
// 
// 			float ratioSin = INV_SIN_TABLE_SIZE * PI;
// 			for (ui32 i = 0 ; i < FAST_TABLE_SIN_SIZE; ++i)
// 			{
// 				ARR_SIN_TABLE[i] = std::sin(i*ratioSin);
// 			}
// 
// 			float ratioTan = INV_TAN_TABLE_SIZE * PI * 2;
// 			for (ui32 i = 0; i < FAST_TABLE_TAN_SIZE; ++i)
// 			{
// 				ARR_TAN_TABLE[i] = std::tan(i*ratioTan);//std::tan(floorf((i+0.5*ratioTan)/ratioTan) *ratioTan);
// 			}
// 			FAST_MATH_TABLE_INIT = true;
// 		}

		void BuildFastMathTable(ui32 fastTableCosSize)
		{
			FAST_TABLE_COS_SIZE = fastTableCosSize;

			COS_FACTOR = FAST_TABLE_COS_SIZE / PI;

			ARR_COS_TABLE = (float*)LordMalloc(sizeof(float)*(FAST_TABLE_COS_SIZE + 1));

			float ratioCos = PI / FAST_TABLE_COS_SIZE;
			for (ui32 i = 0 ; i < FAST_TABLE_COS_SIZE; ++i)
			{
				ARR_COS_TABLE[i] = std::cos(i*ratioCos);
			}

			ARR_COS_TABLE[FAST_TABLE_COS_SIZE] = -ARR_COS_TABLE[0];

			FAST_MATH_TABLE_INIT = true;
		}

		void DeleteFastMathTable()
		{
// 			LordFree(ARR_SIN_TABLE);
// 			LordFree(ARR_TAN_TABLE);
			LordFree(ARR_COS_TABLE);
			FAST_MATH_TABLE_INIT = false;
		}

	} // - End namespace Math
}
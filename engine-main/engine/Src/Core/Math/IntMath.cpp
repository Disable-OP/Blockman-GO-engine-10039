#include "Core.h"
#include "Math/IntMath.h"

namespace LORD
{
	const Vector2i Vector2i::INVALID(Math::MAX_I32, Math::MAX_I32);
	const Vector2i Vector2i::ZERO(0, 0);
	const Vector2i Vector2i::ONE(1, 1);
	const Vector2i Vector2i::NEG_ONE(-1, -1);
	const Vector2i Vector2i::UNIT_X(1, 0);
	const Vector2i Vector2i::UNIT_Y(0, 1);
	const Vector2i Vector2i::NEG_UNIT_X(-1, 0);
	const Vector2i Vector2i::NEG_UNIT_Y(0, -1);

	const Vector2i Vector2i::MXI_ONE(MXI_UNIT, MXI_UNIT);
	const Vector2i Vector2i::MXI_NEG_ONE(-MXI_UNIT, -MXI_UNIT);
	const Vector2i Vector2i::MXI_UNIT_X(MXI_UNIT, 0);
	const Vector2i Vector2i::MXI_UNIT_Y(0, MXI_UNIT);
	const Vector2i Vector2i::MXI_NEG_UNIT_X(-MXI_UNIT, 0);
	const Vector2i Vector2i::MXI_NEG_UNIT_Y(0, -MXI_UNIT);

	int Vector2i::dot(const Vector2i& rhs) const
	{
		i64 ix = (i64(x) * rhs.x >> MXI_FBIT) + (i64(y) * rhs.y >> MXI_FBIT);
		assert(ix < Math::MAX_INT);
		return static_cast<int>(ix);
	}

	int Vector2i::cross(const Vector2i& rhs) const
	{
		i64 ix = (i64(x) * rhs.y >> MXI_FBIT) - (i64(y) * rhs.x >> MXI_FBIT);
		assert(ix < Math::MAX_INT);
		return static_cast<int>(ix);
	}


	const Vector3i Vector3i::ZERO(0, 0, 0);
	const Vector3i Vector3i::ONE(1, 1, 1);
	const Vector3i Vector3i::INVALID(Math::MAX_INT, Math::MAX_I16, Math::MAX_INT);
	const Vector3i Vector3i::UNIT_X(1, 0, 0);
	const Vector3i Vector3i::UNIT_Y(0, 1, 0);
	const Vector3i Vector3i::UNIT_Z(0, 0, 1);
	const Vector3i Vector3i::NEG_UNIT_X(-1, 0, 0);
	const Vector3i Vector3i::NEG_UNIT_Y(0, -1, 0);
	const Vector3i Vector3i::NEG_UNIT_Z(0, 0, -1);

	const Vector3i Vector3i::MXI_ONE(MXI_UNIT, MXI_UNIT, MXI_UNIT);
	const Vector3i Vector3i::MXI_UNIT_X(MXI_UNIT, 0, 0);
	const Vector3i Vector3i::MXI_UNIT_Y(0, MXI_UNIT, 0);
	const Vector3i Vector3i::MXI_UNIT_Z(0, 0, MXI_UNIT);
	const Vector3i Vector3i::MXI_NEG_UNIT_X(-MXI_UNIT, 0, 0);
	const Vector3i Vector3i::MXI_NEG_UNIT_Y(0, -MXI_UNIT, 0);
	const Vector3i Vector3i::MXI_NEG_UNIT_Z(0, 0, -MXI_UNIT);

	int Vector3i::dot(const Vector3i& rhs) const
	{
		i64 ix = (i64(x) * rhs.x >> MXI_FBIT) + (i64(y) * rhs.y >> MXI_FBIT) + (i64(z) * rhs.z >> MXI_FBIT);
		assert(ix < Math::MAX_INT);
		return static_cast<int>(ix);
	}

	Vector3i Vector3i::cross(const Vector3i& rhs) const
	{
		Vector3i vec;
		i64 ix = (i64(y) * rhs.z >> MXI_FBIT) - (i64(z) * rhs.y >> MXI_FBIT);
		i64 iy = (i64(z) * rhs.x >> MXI_FBIT) - (i64(x) * rhs.z >> MXI_FBIT);
		i64 iz = (i64(x) * rhs.y >> MXI_FBIT) - (i64(y) * rhs.x >> MXI_FBIT);

		assert(ix < Math::MAX_INT); vec.x = static_cast<int>(ix);
		assert(iy < Math::MAX_INT); vec.y = static_cast<int>(iy);
		assert(iz < Math::MAX_INT); vec.z = static_cast<int>(iz);
		return vec;
	}

	const Vector4i Vector4i::ZERO(0, 0, 0, 0);
	const Vector4i Vector4i::ONE(1, 1, 1, 1);
	const Vector4i Vector4i::UNIT_X(1, 0, 0, 0);
	const Vector4i Vector4i::UNIT_Y(0, 1, 0, 0);
	const Vector4i Vector4i::UNIT_Z(0, 0, 1, 0);
	const Vector4i Vector4i::UNIT_W(0, 0, 0, 1);
	const Vector4i Vector4i::NEG_UNIT_X(-1, 0, 0, 0);
	const Vector4i Vector4i::NEG_UNIT_Y(0, -1, 0, 0);
	const Vector4i Vector4i::NEG_UNIT_Z(0, 0, -1, 0);
	const Vector4i Vector4i::NEG_UNIT_W(0, 0, 0, -1);

	const Vector4i Vector4i::MXI_ONE(MXI_UNIT, MXI_UNIT, MXI_UNIT, MXI_UNIT);
	const Vector4i Vector4i::MXI_UNIT_X(MXI_UNIT, 0, 0, 0);
	const Vector4i Vector4i::MXI_UNIT_Y(0, MXI_UNIT, 0, 0);
	const Vector4i Vector4i::MXI_UNIT_Z(0, 0, MXI_UNIT, 0);
	const Vector4i Vector4i::MXI_UNIT_W(0, 0, 0, MXI_UNIT);
	const Vector4i Vector4i::MXI_NEG_UNIT_X(-MXI_UNIT, 0, 0, 0);
	const Vector4i Vector4i::MXI_NEG_UNIT_Y(0, -MXI_UNIT, 0, 0);
	const Vector4i Vector4i::MXI_NEG_UNIT_Z(0, 0, -MXI_UNIT, 0);
	const Vector4i Vector4i::MXI_NEG_UNIT_W(0, 0, 0, -MXI_UNIT);

	int Vector4i::dot(const Vector4i& rhs) const
	{
		i64 ix =
			(i64(x) * rhs.x >> MXI_FBIT) +
			(i64(y) * rhs.y >> MXI_FBIT) +
			(i64(z) * rhs.z >> MXI_FBIT) +
			(i64(w) * rhs.w >> MXI_FBIT);
		assert(ix < Math::MAX_INT);
		return static_cast<int>(ix);
	}

	Vector3i& Vector3i::normalize()
	{
		int length = len();
		if (length > 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return *this;
	}

	int Vector3i::len() const
	{
		int vecLen;

		i64 sum = (i64(x) * x + i64(y) * y + i64(z) * z) >> MXI_FBIT;
		assert(sum > Math::MAX_INT);
		vecLen = int(Math::Sqrt(float(sum)));

		return vecLen;
	}

	int Vector3i::lenSqr() const
	{
		return x*x + y*y + z*z;
	}

	/*const Quaternioni Quaternioni::IDENTITY(MXI_UNIT, 0, 0, 0);
	const Quaternioni Quaternioni::INVALID(Math::MAX_INT, Math::MAX_INT, Math::MAX_INT, Math::MAX_INT);

	Quaternioni operator * (int i, const Quaternioni& q)
	{
		Quaternioni result;

		i64 tempx = i64(i) * q.x >> MXI_FBIT; assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		i64 tempy = i64(i) * q.y >> MXI_FBIT; assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		i64 tempz = i64(i) * q.z >> MXI_FBIT; assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);
		i64 tempw = i64(i) * q.w >> MXI_FBIT; assert(tempw < Math::MAX_INT); result.w = static_cast<int>(tempw);

		return result;
	}

	Quaternioni Quaternioni::operator* (int i) const
	{
		Quaternioni result;

		i64 tempx = i64(i) * x >> MXI_FBIT; assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		i64 tempy = i64(i) * y >> MXI_FBIT; assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		i64 tempz = i64(i) * z >> MXI_FBIT; assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);
		i64 tempw = i64(i) * w >> MXI_FBIT; assert(tempw < Math::MAX_INT); result.w = static_cast<int>(tempw);

		return result;
	}

	Quaternioni Quaternioni::operator * (const Quaternioni& b) const
	{
		Quaternioni result;

		i64 tempx = (i64(w) * b.x + i64(x) * b.w + i64(y) * b.z - i64(z) *b.y) >> MXI_FBIT;
		i64 tempy = (i64(w) * b.y + i64(y) * b.w + i64(z) * b.x - i64(x) *b.z) >> MXI_FBIT;
		i64 tempz = (i64(w) * b.z + i64(z) * b.w + i64(x) * b.y - i64(y) *b.x) >> MXI_FBIT;
		i64 tempw = (i64(w) * b.w - i64(x) * b.x - i64(y) * b.y - i64(z) *b.z) >> MXI_FBIT;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);
		assert(tempw < Math::MAX_INT); result.w = static_cast<int>(tempw);

		return result;
	}

	Vector3i Quaternioni::operator* (const Vector3i& v) const
	{
		// nVidia SDK implementation
		Vector3i uv, uuv;
		Vector3i qvec(x, y, z);
		Vector3i fpv = v << MXI_FBIT;
		uv = qvec.cross(fpv);
		uuv = qvec.cross(uv);
		uv >>= MXI_FBIT;
		uuv >>= MXI_FBIT;
		uv *= w * 2;
		uv >>= MXI_FBIT;
		uuv *= 2;

		return v + uv + uuv;
	}

	Vector4i Quaternioni::operator* (const Vector4i& v) const
	{
		// nVidia SDK implementation
		Vector3i vec(v);
		Vector3i uv, uuv;
		Vector3i qvec(x, y, z);
		uv = qvec.cross(vec);
		uuv = qvec.cross(uv);
		uv *= 2 * w;
		uuv *= 2;

		return Vector4i(vec + uv + uuv, 1);
	}

	int Quaternioni::dot(const Quaternioni& rhs) const
	{
		i64 result = (i64(w) * rhs.w + i64(x) * rhs.x + i64(y) * rhs.y + i64(z) * rhs.z) >> MXI_FBIT;
		assert(result < Math::MAX_INT);
		return static_cast<int>(result);
	}

	Quaternioni Quaternioni::cross(const Quaternioni& rhs)
	{
		Quaternioni result;

		i64 tempx = (i64(w) * rhs.x + i64(x) * rhs.w + i64(y) * rhs.z - i64(z) * rhs.y) >> MXI_FBIT;
		i64 tempy = (i64(w) * rhs.y + i64(y) * rhs.w + i64(z) * rhs.x - i64(x) * rhs.z) >> MXI_FBIT;
		i64 tempz = (i64(w) * rhs.z + i64(z) * rhs.w + i64(x) * rhs.y - i64(y) * rhs.x) >> MXI_FBIT;
		i64 tempw = (i64(w) * rhs.w - i64(x) * rhs.x - i64(y) * rhs.y - i64(z) * rhs.z) >> MXI_FBIT;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);
		assert(tempw < Math::MAX_INT); result.w = static_cast<int>(tempw);

		return result;
	}

	void Quaternioni::inverse()
	{
		//conjugate();
		const int norm = lenSqr();

		if (norm == 0)
		{
			invalid();
			return;
		}

		x /= norm;
		y /= norm;
		z /= norm;
		w /= -norm;
	}

	int	Quaternioni::len() const
	{
		int qLen;

		i64 sum = (i64(w) * w + i64(x) * x + i64(y) * y + i64(z) * z) >> MXI_FBIT;
		assert(sum < Math::MAX_INT);
		qLen = int(Math::Sqrt(float(sum)));

		return qLen;
	}

	int	Quaternioni::lenSqr() const
	{
		i64 sum = (i64(w) * w + i64(x) * x + i64(y) * y + i64(z) * z) >> MXI_FBIT;
		assert(sum < Math::MAX_INT);
		return static_cast<int>(sum);
	}

	void Quaternioni::rotateVec3(Vector3i& outVec, const Vector3i& vec) const
	{
		//Mat4 matRotate = ToMat4();
		//return matRotate.RotateVec3(vec);

		// nVidia SDK implementation
		Vector3i uv, uuv;
		Vector3i qvec(x, y, z);
		uv = qvec.cross(vec);
		uuv = qvec.cross(uv);
		uv *= 2 * w;
		uuv *= 2;

		outVec = vec + uv + uuv;
	}

	Vector3i Quaternioni::xAxis() const
	{
		//float fTx  = 2.0*x;
		i64 fTy = i64(y) * 2;
		i64 fTz = i64(z) * 2;
		i64 fTwy = fTy * w;
		i64 fTwz = fTz * w;
		i64 fTxy = fTy * x;
		i64 fTxz = fTz * x;
		i64 fTyy = fTy * y;
		i64 fTzz = fTz * z;

		return Vector3i(int(MXI_UNIT - (fTyy + fTzz)), int(fTxy + fTwz), int(fTxz - fTwy));
	}

	Vector3i Quaternioni::yAxis() const
	{
		i64 fTx = i64(x) * 2;
		i64 fTy = i64(y) * 2;
		i64 fTz = i64(z) * 2;
		i64 fTwx = fTx * w;
		i64 fTwz = fTz * w;
		i64 fTxx = fTx * x;
		i64 fTxy = fTy * x;
		i64 fTyz = fTz * y;
		i64 fTzz = fTz * z;

		return Vector3i(int(fTxy - fTwz), int(MXI_UNIT - (fTxx + fTzz)), int(fTyz + fTwx));
	}

	Vector3i Quaternioni::zAxis() const
	{
		i64 fTx = i64(x) * 2;
		i64 fTy = i64(y) * 2;
		i64 fTz = i64(z) * 2;
		i64 fTwx = fTx * w;
		i64 fTwy = fTy * w;
		i64 fTxx = fTx * x;
		i64 fTxz = fTz * x;
		i64 fTyy = fTy * y;
		i64 fTyz = fTz * y;

		return Vector3i(int(fTxz + fTwy), int(fTyz - fTwx), int(MXI_UNIT - (fTxx + fTyy)));
	}

	int Quaternioni::getRadian() const
	{
		assert(w >= -MXI_UNIT && w <= MXI_UNIT);
		float radian = Math::ACos(float(w) / MXI_UNIT) * 2.0f;
		return int(radian / Math::PI_2 * Math::SINCOSTABSZ);
	}

	void Quaternioni::rotateAxis(const Vector3i& axis, int iRadian)
	{
		int alpha = iRadian / 2;
		int sina = Math::Sin_tabi(alpha);
		i64 tempx = i64(sina) * axis.x >> MXI_FBIT;
		i64 tempy = i64(sina) * axis.y >> MXI_FBIT;
		i64 tempz = i64(sina) * axis.z >> MXI_FBIT;

		w = Math::Cos_tabi(alpha);
		assert(tempx < Math::MAX_INT); x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); z = static_cast<int>(tempz);
	}

	void Quaternioni::fromEulerAngle(int iPitch, int iYaw, int iRoll)
	{
		int iCosHRoll = Math::Cos_tabi(iRoll / 2);
		int iSinHRoll = Math::Sin_tabi(iRoll / 2);
		int iCosHPitch = Math::Cos_tabi(iPitch / 2);
		int iSinHPitch = Math::Sin_tabi(iPitch / 2);
		int iCosHYaw = Math::Cos_tabi(iYaw / 2);
		int iSinHYaw = Math::Sin_tabi(iYaw / 2);

		i64 tempx = ((i64(iCosHRoll) * iSinHPitch >> MXI_FBIT) * iCosHYaw >> MXI_FBIT) +
			((i64(iSinHRoll) * iCosHPitch >> MXI_FBIT) * iSinHYaw >> MXI_FBIT);
		i64 tempy = ((i64(iCosHRoll) * iCosHPitch >> MXI_FBIT) * iSinHYaw >> MXI_FBIT) -
			((i64(iSinHRoll) * iSinHPitch >> MXI_FBIT) * iCosHYaw >> MXI_FBIT);
		i64 tempz = ((i64(iSinHRoll) * iCosHPitch >> MXI_FBIT) * iCosHYaw >> MXI_FBIT) -
			((i64(iCosHRoll) * iSinHPitch >> MXI_FBIT) * iSinHYaw >> MXI_FBIT);
		i64 tempw = ((i64(iCosHRoll) * iCosHPitch >> MXI_FBIT) * iCosHYaw >> MXI_FBIT) +
			((i64(iSinHRoll) * iSinHPitch >> MXI_FBIT) * iSinHYaw >> MXI_FBIT);

		assert(tempx < Math::MAX_INT); x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); z = static_cast<int>(tempz);
		assert(tempw < Math::MAX_INT); w = static_cast<int>(tempw);
	}

	void Quaternioni::toEulerAngle(int& iPitch, int& iYaw, int& iRoll) const
	{
		i64 rolly = ((i64(w) * z + i64(x) * y) >> MXI_FBIT) * 2;
		i64 rollx = MXI_UNIT - (((i64(z) * z + i64(x) * x) >> MXI_FBIT) * 2);
		i64 pitchi = ((i64(w) * x + i64(y) * z) >> MXI_FBIT) * 2;
		i64 yawy = ((i64(w) * y + i64(z) * x) >> MXI_FBIT) * 2;
		i64 yawx = MXI_UNIT - (((i64(x) * x + i64(y) * y) >> MXI_FBIT) * 2);

		float roll = Math::ATan2(float(rolly) / MXI_UNIT, float(rollx) / MXI_UNIT);
		float pitch = Math::ASin(float(pitchi) / MXI_UNIT);
		float yaw = Math::ATan2(float(yawy) / MXI_UNIT, float(yawx) / MXI_UNIT);

		iPitch = int(pitch / Math::PI_2 * Math::SINCOSTABSZ);
		iYaw = int(yaw / Math::PI_2 * Math::SINCOSTABSZ);
		iRoll = int(roll / Math::PI_2 * Math::SINCOSTABSZ);
	}


	void Quaternioni::toMat4(Matrix4i& mat) const
	{
		i64 xs = x * 2;
		i64 ys = y * 2;
		i64 zs = z * 2;
		i64 wx = w * xs >> MXI_FBIT;
		i64 wy = w * ys >> MXI_FBIT;
		i64 wz = w * zs >> MXI_FBIT;
		i64 xx = x * xs >> MXI_FBIT;
		i64 xy = x * ys >> MXI_FBIT;
		i64 xz = x * zs >> MXI_FBIT;
		i64 yy = y * ys >> MXI_FBIT;
		i64 yz = y * zs >> MXI_FBIT;
		i64 zz = z * zs >> MXI_FBIT;

		i64 temp00 = MXI_UNIT - (yy + zz);	assert(temp00 < Math::MAX_INT); mat.m00 = static_cast<int>(temp00);
		i64 temp01 = xy + wz;					assert(temp01 < Math::MAX_INT); mat.m01 = static_cast<int>(temp01);
		i64 temp02 = xz - wy;					assert(temp02 < Math::MAX_INT); mat.m02 = static_cast<int>(temp02);

		i64 temp10 = xy - wz;					assert(temp10 < Math::MAX_INT); mat.m10 = static_cast<int>(temp10);
		i64 temp11 = MXI_UNIT - (xx + zz);	assert(temp11 < Math::MAX_INT); mat.m11 = static_cast<int>(temp11);
		i64 temp12 = yz + wx;					assert(temp12 < Math::MAX_INT); mat.m12 = static_cast<int>(temp12);

		i64 temp20 = xz + wy;					assert(temp20 < Math::MAX_INT); mat.m20 = static_cast<int>(temp20);
		i64 temp21 = yz - wx;					assert(temp21 < Math::MAX_INT); mat.m21 = static_cast<int>(temp21);
		i64 temp22 = MXI_UNIT - (xx + yy);	assert(temp22 < Math::MAX_INT); mat.m22 = static_cast<int>(temp22);

		mat.m03 = 0;
		mat.m13 = 0;
		mat.m23 = 0;

		mat.m30 = 0;
		mat.m31 = 0;
		mat.m32 = 0;
		mat.m33 = MXI_UNIT;
	}

	void Quaternioni::toMat3(Matrix3i& mat) const
	{
		i64 xs = i64(x) * 2;
		i64 ys = i64(y) * 2;
		i64 zs = i64(z) * 2;
		i64 wx = i64(w) * xs >> MXI_FBIT;
		i64 wy = i64(w) * ys >> MXI_FBIT;
		i64 wz = i64(w) * zs >> MXI_FBIT;
		i64 xx = i64(x) * xs >> MXI_FBIT;
		i64 xy = i64(x) * ys >> MXI_FBIT;
		i64 xz = i64(x) * zs >> MXI_FBIT;
		i64 yy = i64(y) * ys >> MXI_FBIT;
		i64 yz = i64(y) * zs >> MXI_FBIT;
		i64 zz = i64(z) * zs >> MXI_FBIT;

		i64 temp00 = MXI_UNIT - (yy + zz);	assert(temp00 < Math::MAX_INT); mat.m00 = static_cast<int>(temp00);
		i64 temp01 = xy + wz;					assert(temp01 < Math::MAX_INT); mat.m01 = static_cast<int>(temp01);
		i64 temp02 = xz - wy;					assert(temp02 < Math::MAX_INT); mat.m02 = static_cast<int>(temp02);

		i64 temp10 = xy - wz;					assert(temp10 < Math::MAX_INT); mat.m10 = static_cast<int>(temp10);
		i64 temp11 = MXI_UNIT - (xx + zz);	assert(temp11 < Math::MAX_INT); mat.m11 = static_cast<int>(temp11);
		i64 temp12 = yz + wx;					assert(temp12 < Math::MAX_INT); mat.m12 = static_cast<int>(temp12);

		i64 temp20 = xz + wy;					assert(temp20 < Math::MAX_INT); mat.m20 = static_cast<int>(temp20);
		i64 temp21 = yz - wx;					assert(temp21 < Math::MAX_INT); mat.m21 = static_cast<int>(temp21);
		i64 temp22 = MXI_UNIT - (xx + yy);	assert(temp22 < Math::MAX_INT); mat.m22 = static_cast<int>(temp22);
	}

	// predefined specific matrices
	const Matrix3i Matrix3i::ZERO(
		0, 0, 0,
		0, 0, 0,
		0, 0, 0);

	const Matrix3i Matrix3i::INVALID(
		Math::MAX_INT, Math::MAX_INT, Math::MAX_INT,
		Math::MAX_INT, Math::MAX_INT, Math::MAX_INT,
		Math::MAX_INT, Math::MAX_INT, Math::MAX_INT);

	const Matrix3i Matrix3i::IDENTITY(
		MXI_UNIT, 0, 0,
		0, MXI_UNIT, 0,
		0, 0, MXI_UNIT);

	Matrix3i& Matrix3i::operator*= (const int i)
	{
		i64 temp00 = i64(m00) * i >> MXI_FBIT;
		i64 temp01 = i64(m01) * i >> MXI_FBIT;
		i64 temp02 = i64(m02) * i >> MXI_FBIT;

		i64 temp10 = i64(m10) * i >> MXI_FBIT;
		i64 temp11 = i64(m11) * i >> MXI_FBIT;
		i64 temp12 = i64(m12) * i >> MXI_FBIT;

		i64 temp20 = i64(m20) * i >> MXI_FBIT;
		i64 temp21 = i64(m21) * i >> MXI_FBIT;
		i64 temp22 = i64(m22) * i >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);

		return *this;
	}

	Matrix3i& Matrix3i::operator*= (const Matrix3i& rhs)
	{
		Matrix3i result;

		i64 temp00 = (i64(m00) * rhs.m00 + i64(m01) * rhs.m10 + i64(m02) * rhs.m20) >> MXI_FBIT;
		i64 temp01 = (i64(m00) * rhs.m01 + i64(m01) * rhs.m11 + i64(m02) * rhs.m21) >> MXI_FBIT;
		i64 temp02 = (i64(m00) * rhs.m02 + i64(m01) * rhs.m12 + i64(m02) * rhs.m22) >> MXI_FBIT;

		i64 temp10 = (i64(m10) * rhs.m00 + i64(m11) * rhs.m10 + i64(m12) * rhs.m20) >> MXI_FBIT;
		i64 temp11 = (i64(m10) * rhs.m01 + i64(m11) * rhs.m11 + i64(m12) * rhs.m21) >> MXI_FBIT;
		i64 temp12 = (i64(m10) * rhs.m02 + i64(m11) * rhs.m12 + i64(m12) * rhs.m22) >> MXI_FBIT;

		i64 temp20 = (i64(m20) * rhs.m00 + i64(m21) * rhs.m10 + i64(m22) * rhs.m20) >> MXI_FBIT;
		i64 temp21 = (i64(m20) * rhs.m01 + i64(m21) * rhs.m11 + i64(m22) * rhs.m21) >> MXI_FBIT;
		i64 temp22 = (i64(m20) * rhs.m02 + i64(m21) * rhs.m12 + i64(m22) * rhs.m22) >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); result.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); result.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); result.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); result.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); result.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); result.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); result.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); result.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); result.m22 = static_cast<int>(temp22);

		*this = result;

		return *this;
	}

	Matrix3i Matrix3i::operator + (const Matrix3i& b) const
	{
		Matrix3i result;

		result.m00 = m00 + b.m00;
		result.m01 = m01 + b.m01;
		result.m02 = m02 + b.m02;

		result.m10 = m10 + b.m10;
		result.m11 = m11 + b.m11;
		result.m12 = m12 + b.m12;

		result.m20 = m20 + b.m20;
		result.m21 = m21 + b.m21;
		result.m22 = m22 + b.m22;

		return result;
	}

	Matrix3i Matrix3i::operator - (const Matrix3i& b) const
	{
		Matrix3i result;

		result.m00 = m00 - b.m00;
		result.m01 = m01 - b.m01;
		result.m02 = m02 - b.m02;

		result.m10 = m10 - b.m10;
		result.m11 = m11 - b.m11;
		result.m12 = m12 - b.m12;

		result.m20 = m20 - b.m20;
		result.m21 = m21 - b.m21;
		result.m22 = m22 - b.m22;

		return result;
	}

	const Vector3i operator* (const Vector3i& v, const Matrix3i& m)
	{
		Vector3i result;

		i64 ix = (i64(v.x) * m.m00 + i64(v.y) * m.m10 + i64(v.z) * m.m20) >> MXI_FBIT;
		i64 iy = (i64(v.x) * m.m01 + i64(v.y) * m.m11 + i64(v.z) * m.m21) >> MXI_FBIT;
		i64 iz = (i64(v.x) * m.m02 + i64(v.y) * m.m12 + i64(v.z) * m.m22) >> MXI_FBIT;

		assert(ix < Math::MAX_INT); result.x = static_cast<int>(ix);
		assert(iy < Math::MAX_INT); result.y = static_cast<int>(iy);
		assert(iz < Math::MAX_INT); result.z = static_cast<int>(iz);

		return result;
	}

	Matrix3i Matrix3i::operator* (const Matrix3i& b) const
	{
		Matrix3i result;

		i64 temp00 = (i64(m00) * b.m00 + i64(m01) * b.m10 + i64(m02) * b.m20) >> MXI_FBIT;
		i64 temp01 = (i64(m00) * b.m01 + i64(m01) * b.m11 + i64(m02) * b.m21) >> MXI_FBIT;
		i64 temp02 = (i64(m00) * b.m02 + i64(m01) * b.m12 + i64(m02) * b.m22) >> MXI_FBIT;

		i64 temp10 = (i64(m10) * b.m00 + i64(m11) * b.m10 + i64(m12) * b.m20) >> MXI_FBIT;
		i64 temp11 = (i64(m10) * b.m01 + i64(m11) * b.m11 + i64(m12) * b.m21) >> MXI_FBIT;
		i64 temp12 = (i64(m10) * b.m02 + i64(m11) * b.m12 + i64(m12) * b.m22) >> MXI_FBIT;

		i64 temp20 = (i64(m20) * b.m00 + i64(m21) * b.m10 + i64(m22) * b.m20) >> MXI_FBIT;
		i64 temp21 = (i64(m20) * b.m01 + i64(m21) * b.m11 + i64(m22) * b.m21) >> MXI_FBIT;
		i64 temp22 = (i64(m20) * b.m02 + i64(m21) * b.m12 + i64(m22) * b.m22) >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); result.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); result.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); result.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); result.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); result.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); result.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); result.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); result.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); result.m22 = static_cast<int>(temp22);

		return result;
	}

	Matrix3i Matrix3i::operator * (const int i) const
	{
		Matrix3i result;

		i64 temp00 = i64(m00) * i >> MXI_FBIT;
		i64 temp01 = i64(m01) * i >> MXI_FBIT;
		i64 temp02 = i64(m02) * i >> MXI_FBIT;

		i64 temp10 = i64(m10) * i >> MXI_FBIT;
		i64 temp11 = i64(m11) * i >> MXI_FBIT;
		i64 temp12 = i64(m12) * i >> MXI_FBIT;

		i64 temp20 = i64(m20) * i >> MXI_FBIT;
		i64 temp21 = i64(m21) * i >> MXI_FBIT;
		i64 temp22 = i64(m22) * i >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); result.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); result.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); result.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); result.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); result.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); result.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); result.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); result.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); result.m22 = static_cast<int>(temp22);

		return result;
	}

	const Matrix3i operator* (const int i, const Matrix3i& a)
	{
		Matrix3i result;

		i64 temp00 = i64(a.m00) * i >> MXI_FBIT;
		i64 temp01 = i64(a.m01) * i >> MXI_FBIT;
		i64 temp02 = i64(a.m02) * i >> MXI_FBIT;

		i64 temp10 = i64(a.m10) * i >> MXI_FBIT;
		i64 temp11 = i64(a.m11) * i >> MXI_FBIT;
		i64 temp12 = i64(a.m12) * i >> MXI_FBIT;

		i64 temp20 = i64(a.m20) * i >> MXI_FBIT;
		i64 temp21 = i64(a.m21) * i >> MXI_FBIT;
		i64 temp22 = i64(a.m22) * i >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); result.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); result.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); result.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); result.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); result.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); result.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); result.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); result.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); result.m22 = static_cast<int>(temp22);

		return result;
	}

	Matrix3i Matrix3i::operator / (const int i) const
	{
		Matrix3i result;

		i64 temp00 = (i64(m00) << MXI_FBIT) / i;
		i64 temp01 = (i64(m01) << MXI_FBIT) / i;
		i64 temp02 = (i64(m02) << MXI_FBIT) / i;

		i64 temp10 = (i64(m10) << MXI_FBIT) / i;
		i64 temp11 = (i64(m11) << MXI_FBIT) / i;
		i64 temp12 = (i64(m12) << MXI_FBIT) / i;

		i64 temp20 = (i64(m20) << MXI_FBIT) / i;
		i64 temp21 = (i64(m21) << MXI_FBIT) / i;
		i64 temp22 = (i64(m22) << MXI_FBIT) / i;

		assert(temp00 < Math::MAX_INT); result.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); result.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); result.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); result.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); result.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); result.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); result.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); result.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); result.m22 = static_cast<int>(temp22);

		return result;
	}

	Vector3i Matrix3i::transform(const Vector3i& vec) const
	{
		Vector3i result;

		i64 tempx = (i64(vec.x) * m00 + i64(vec.y) * m10 + i64(vec.z) * m20) >> MXI_FBIT;
		i64 tempy = (i64(vec.x) * m01 + i64(vec.y) * m11 + i64(vec.z) * m21) >> MXI_FBIT;
		i64 tempz = (i64(vec.x) * m02 + i64(vec.y) * m12 + i64(vec.z) * m22) >> MXI_FBIT;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);

		return result;
	}

	void Matrix3i::scale(int x, int y, int z)
	{
		i64 temp00 = i64(m00) * x >> MXI_FBIT;
		i64 temp01 = i64(m01) * x >> MXI_FBIT;
		i64 temp02 = i64(m02) * x >> MXI_FBIT;

		i64 temp10 = i64(m10) * y >> MXI_FBIT;
		i64 temp11 = i64(m11) * y >> MXI_FBIT;
		i64 temp12 = i64(m12) * y >> MXI_FBIT;

		i64 temp20 = i64(m20) * z >> MXI_FBIT;
		i64 temp21 = i64(m21) * z >> MXI_FBIT;
		i64 temp22 = i64(m22) * z >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);
	}

	void Matrix3i::rotateAxis(const Vector3i& v, int iRadian)
	{
		int x = v.x;
		int y = v.y;
		int z = v.z;

		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		Matrix3i  local, final;

		i64 temp00 = ((i64(x) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp01 = ((i64(x) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + (i64(z) * iSin >> MXI_FBIT);
		i64 temp02 = ((i64(x) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - (i64(y) * iSin >> MXI_FBIT);

		i64 temp10 = ((i64(y) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - (i64(z) * iSin >> MXI_FBIT);
		i64 temp11 = ((i64(y) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp12 = ((i64(y) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + (i64(x) * iSin >> MXI_FBIT);

		i64 temp20 = ((i64(z) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + (i64(y) * iSin >> MXI_FBIT);
		i64 temp21 = ((i64(z) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - (i64(x) * iSin >> MXI_FBIT);
		i64 temp22 = ((i64(z) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;

		assert(temp00 < Math::MAX_INT); local.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); local.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); local.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); local.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); local.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); local.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); local.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); local.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); local.m22 = static_cast<int>(temp22);

		final = local * (*this);
		*this = final;
	}

	void Matrix3i::rotateAxisReplace(const Vector3i& axis, int iRadian)
	{
		int x = axis.x;
		int y = axis.y;
		int z = axis.z;

		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp00 = ((i64(x) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp01 = ((i64(x) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + (i64(z) * iSin >> MXI_FBIT);
		i64 temp02 = ((i64(x) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - (i64(y) * iSin >> MXI_FBIT);

		i64 temp10 = ((i64(y) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - (i64(z) * iSin >> MXI_FBIT);
		i64 temp11 = ((i64(y) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp12 = ((i64(y) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + (i64(x) * iSin >> MXI_FBIT);

		i64 temp20 = ((i64(z) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + (i64(y) * iSin >> MXI_FBIT);
		i64 temp21 = ((i64(z) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - (i64(x) * iSin >> MXI_FBIT);
		i64 temp22 = ((i64(z) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;

		assert(temp00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);
	}

	// this = Rx * this
	void Matrix3i::rotateX(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp10 = (i64(m10) * iCos + i64(m20) * iSin) >> MXI_FBIT;
		i64 temp11 = (i64(m11) * iCos + i64(m21) * iSin) >> MXI_FBIT;
		i64 temp12 = (i64(m12) * iCos + i64(m22) * iSin) >> MXI_FBIT;

		i64 temp20 = (i64(m10) *-iSin + i64(m20) * iCos) >> MXI_FBIT;
		i64 temp21 = (i64(m11) *-iSin + i64(m21) * iCos) >> MXI_FBIT;
		i64 temp22 = (i64(m12) *-iSin + i64(m22) * iCos) >> MXI_FBIT;

		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);
	}

	void Matrix3i::rotateXReplace(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		m00 = MXI_UNIT; m01 = 0;  m02 = 0;
		m10 = 0; m11 = iCos;  m12 = iSin;
		m20 = 0; m21 = -iSin; m22 = iCos;
	}

	void Matrix3i::rotateY(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp00 = (i64(m00) * iCos - i64(m20) * iSin) >> MXI_FBIT;
		i64 temp01 = (i64(m01) * iCos - i64(m21) * iSin) >> MXI_FBIT;
		i64 temp02 = (i64(m02) * iCos - i64(m22) * iSin) >> MXI_FBIT;

		i64 temp20 = (i64(m00) * iSin + i64(m20) * iCos) >> MXI_FBIT;
		i64 temp21 = (i64(m01) * iSin + i64(m21) * iCos) >> MXI_FBIT;
		i64 temp22 = (i64(m02) * iSin + i64(m22) * iCos) >> MXI_FBIT;

		assert(m00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(m01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(m02 < Math::MAX_INT); m02 = static_cast<int>(temp02);

		assert(m20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(m21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(m22 < Math::MAX_INT); m22 = static_cast<int>(temp22);
	}

	void Matrix3i::rotateYReplace(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		m00 = iCos; m01 = 0;		m02 = -iSin;
		m10 = 0;    m11 = MXI_UNIT; m12 = 0;
		m20 = iSin; m21 = 0;		m22 = iCos;
	}

	void Matrix3i::rotateZ(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp00 = (i64(m00) * iCos + i64(m10) * iSin) >> MXI_FBIT;
		i64 temp01 = (i64(m01) * iCos + i64(m11) * iSin) >> MXI_FBIT;
		i64 temp02 = (i64(m02) * iCos + i64(m12) * iSin) >> MXI_FBIT;

		i64 temp10 = (i64(m00) *-iSin + i64(m10) * iCos) >> MXI_FBIT;
		i64 temp11 = (i64(m01) *-iSin + i64(m11) * iCos) >> MXI_FBIT;
		i64 temp12 = (i64(m02) *-iSin + i64(m12) * iCos) >> MXI_FBIT;

		assert(m00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(m01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(m02 < Math::MAX_INT); m02 = static_cast<int>(temp02);

		assert(m10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(m11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(m12 < Math::MAX_INT); m12 = static_cast<int>(temp12);
	}

	void Matrix3i::rotateZReplace(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		m00 = iCos; m01 = iSin; m02 = 0;
		m10 = -iSin; m11 = iCos; m12 = 0;
		m20 = 0;    m21 = 0;    m22 = MXI_UNIT;
	}

	Vector3i Matrix3i::rotateVec3(const Vector3i& vec)
	{
		Vector3i result;

		i64 ix = (i64(vec.x) * m00 + i64(vec.y) * m10 + i64(vec.z) * m20) >> MXI_FBIT;
		i64 iy = (i64(vec.x) * m01 + i64(vec.y) * m11 + i64(vec.z) * m21) >> MXI_FBIT;
		i64 iz = (i64(vec.x) * m02 + i64(vec.y) * m12 + i64(vec.z) * m22) >> MXI_FBIT;

		assert(ix < Math::MAX_INT); result.x = static_cast<int>(ix);
		assert(iy < Math::MAX_INT); result.y = static_cast<int>(iy);
		assert(iz < Math::MAX_INT); result.z = static_cast<int>(iz);

		return result;
	}

	void Matrix3i::Transpose(Matrix3i& outMat, const Matrix3i& matrix)
	{
		outMat = matrix;
		outMat.transpose();
	}

	void Matrix3i::TransformVec3(Vector3i& outVec, const Vector3i& v, const Matrix3i& matrix)
	{
		i64 ix = (i64(v.x) * matrix.m00 + i64(v.y) * matrix.m10 + i64(v.z) * matrix.m20) >> MXI_FBIT;
		i64 iy = (i64(v.x) * matrix.m01 + i64(v.y) * matrix.m11 + i64(v.z) * matrix.m21) >> MXI_FBIT;
		i64 iz = (i64(v.x) * matrix.m02 + i64(v.y) * matrix.m12 + i64(v.z) * matrix.m22) >> MXI_FBIT;

		assert(ix < Math::MAX_INT); outVec.x = static_cast<int>(ix);
		assert(iy < Math::MAX_INT); outVec.y = static_cast<int>(iy);
		assert(iz < Math::MAX_INT); outVec.z = static_cast<int>(iz);
	}

	void Matrix3i::Inverse(Matrix3i& outMat, const Matrix3i& matrix)
	{
		outMat = matrix;
		outMat.inverse();
	}

	const Matrix4i Matrix4i::ZERO(
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0);

	const Matrix4i Matrix4i::INVALID(
		Math::MAX_INT, Math::MAX_INT, Math::MAX_INT, Math::MAX_INT,
		Math::MAX_INT, Math::MAX_INT, Math::MAX_INT, Math::MAX_INT,
		Math::MAX_INT, Math::MAX_INT, Math::MAX_INT, Math::MAX_INT,
		Math::MAX_INT, Math::MAX_INT, Math::MAX_INT, Math::MAX_INT);

	const Matrix4i Matrix4i::IDENTITY(
		MXI_UNIT, 0, 0, 0,
		0, MXI_UNIT, 0, 0,
		0, 0, MXI_UNIT, 0,
		0, 0, 0, MXI_UNIT);

	Matrix4i& Matrix4i::operator*= (const Matrix4i& rhs)
	{
		Matrix4i result;

		i64 temp00 = (i64(m00) * rhs.m00 + i64(m01) * rhs.m10 + i64(m02) * rhs.m20 + i64(m03) * rhs.m30) >> MXI_FBIT;
		i64 temp01 = (i64(m00) * rhs.m01 + i64(m01) * rhs.m11 + i64(m02) * rhs.m21 + i64(m03) * rhs.m31) >> MXI_FBIT;
		i64 temp02 = (i64(m00) * rhs.m02 + i64(m01) * rhs.m12 + i64(m02) * rhs.m22 + i64(m03) * rhs.m32) >> MXI_FBIT;
		i64 temp03 = (i64(m00) * rhs.m03 + i64(m01) * rhs.m13 + i64(m02) * rhs.m23 + i64(m03) * rhs.m33) >> MXI_FBIT;

		i64 temp10 = (i64(m10) * rhs.m00 + i64(m11) * rhs.m10 + i64(m12) * rhs.m20 + i64(m13) * rhs.m30) >> MXI_FBIT;
		i64 temp11 = (i64(m10) * rhs.m01 + i64(m11) * rhs.m11 + i64(m12) * rhs.m21 + i64(m13) * rhs.m31) >> MXI_FBIT;
		i64 temp12 = (i64(m10) * rhs.m02 + i64(m11) * rhs.m12 + i64(m12) * rhs.m22 + i64(m13) * rhs.m32) >> MXI_FBIT;
		i64 temp13 = (i64(m10) * rhs.m03 + i64(m11) * rhs.m13 + i64(m12) * rhs.m23 + i64(m13) * rhs.m33) >> MXI_FBIT;

		i64 temp20 = (i64(m20) * rhs.m00 + i64(m21) * rhs.m10 + i64(m22) * rhs.m20 + i64(m23) * rhs.m30) >> MXI_FBIT;
		i64 temp21 = (i64(m20) * rhs.m01 + i64(m21) * rhs.m11 + i64(m22) * rhs.m21 + i64(m23) * rhs.m31) >> MXI_FBIT;
		i64 temp22 = (i64(m20) * rhs.m02 + i64(m21) * rhs.m12 + i64(m22) * rhs.m22 + i64(m23) * rhs.m32) >> MXI_FBIT;
		i64 temp23 = (i64(m20) * rhs.m03 + i64(m21) * rhs.m13 + i64(m22) * rhs.m23 + i64(m23) * rhs.m33) >> MXI_FBIT;

		i64 temp30 = (i64(m30) * rhs.m00 + i64(m31) * rhs.m10 + i64(m32) * rhs.m20 + i64(m33) * rhs.m30) >> MXI_FBIT;
		i64 temp31 = (i64(m30) * rhs.m01 + i64(m31) * rhs.m11 + i64(m32) * rhs.m21 + i64(m33) * rhs.m31) >> MXI_FBIT;
		i64 temp32 = (i64(m30) * rhs.m02 + i64(m31) * rhs.m12 + i64(m32) * rhs.m22 + i64(m33) * rhs.m32) >> MXI_FBIT;
		i64 temp33 = (i64(m30) * rhs.m03 + i64(m31) * rhs.m13 + i64(m32) * rhs.m23 + i64(m33) * rhs.m33) >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); result.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); result.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); result.m02 = static_cast<int>(temp02);
		assert(temp03 < Math::MAX_INT); result.m03 = static_cast<int>(temp03);

		assert(temp10 < Math::MAX_INT); result.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); result.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); result.m12 = static_cast<int>(temp12);
		assert(temp13 < Math::MAX_INT); result.m13 = static_cast<int>(temp13);

		assert(temp20 < Math::MAX_INT); result.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); result.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); result.m22 = static_cast<int>(temp22);
		assert(temp23 < Math::MAX_INT); result.m23 = static_cast<int>(temp23);

		assert(temp30 < Math::MAX_INT); result.m30 = static_cast<int>(temp30);
		assert(temp31 < Math::MAX_INT); result.m31 = static_cast<int>(temp31);
		assert(temp32 < Math::MAX_INT); result.m32 = static_cast<int>(temp32);
		assert(temp33 < Math::MAX_INT); result.m33 = static_cast<int>(temp33);

		*this = result;

		return *this;
	}

	const Vector4i operator * (const Vector4i& v, const Matrix4i& m)
	{
		Vector4i result;

		i64 tempx = (i64(v.x) * m.m00 + i64(v.y) * m.m10 + i64(v.z) * m.m20 + i64(v.w) * m.m30) >> MXI_FBIT;
		i64 tempy = (i64(v.x) * m.m01 + i64(v.y) * m.m11 + i64(v.z) * m.m21 + i64(v.w) * m.m31) >> MXI_FBIT;
		i64 tempz = (i64(v.x) * m.m02 + i64(v.y) * m.m12 + i64(v.z) * m.m22 + i64(v.w) * m.m32) >> MXI_FBIT;
		i64 tempw = (i64(v.x) * m.m03 + i64(v.y) * m.m13 + i64(v.z) * m.m23 + i64(v.w) * m.m33) >> MXI_FBIT;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);
		assert(tempw < Math::MAX_INT); result.w = static_cast<int>(tempw);

		return result;
	}

	const Vector3i operator* (const Vector3i& v, const Matrix4i& m)
	{
		Vector3i result;

		i64 tempx = ((i64(v.x) * m.m00 + i64(v.y) * m.m10 + i64(v.z) * m.m20) >> MXI_FBIT) + m.m30;
		i64 tempy = ((i64(v.x) * m.m01 + i64(v.y) * m.m11 + i64(v.z) * m.m21) >> MXI_FBIT) + m.m31;
		i64 tempz = ((i64(v.x) * m.m02 + i64(v.y) * m.m12 + i64(v.z) * m.m22) >> MXI_FBIT) + m.m32;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);

		return result;
	}

	Matrix4i Matrix4i::operator* (const Matrix4i& b) const
	{
		Matrix4i result;

		i64 temp00 = (i64(m00) * b.m00 + i64(m01) * b.m10 + i64(m02) * b.m20 + i64(m03) * b.m30) >> MXI_FBIT;
		i64 temp01 = (i64(m00) * b.m01 + i64(m01) * b.m11 + i64(m02) * b.m21 + i64(m03) * b.m31) >> MXI_FBIT;
		i64 temp02 = (i64(m00) * b.m02 + i64(m01) * b.m12 + i64(m02) * b.m22 + i64(m03) * b.m32) >> MXI_FBIT;
		i64 temp03 = (i64(m00) * b.m03 + i64(m01) * b.m13 + i64(m02) * b.m23 + i64(m03) * b.m33) >> MXI_FBIT;

		i64 temp10 = (i64(m10) * b.m00 + i64(m11) * b.m10 + i64(m12) * b.m20 + i64(m13) * b.m30) >> MXI_FBIT;
		i64 temp11 = (i64(m10) * b.m01 + i64(m11) * b.m11 + i64(m12) * b.m21 + i64(m13) * b.m31) >> MXI_FBIT;
		i64 temp12 = (i64(m10) * b.m02 + i64(m11) * b.m12 + i64(m12) * b.m22 + i64(m13) * b.m32) >> MXI_FBIT;
		i64 temp13 = (i64(m10) * b.m03 + i64(m11) * b.m13 + i64(m12) * b.m23 + i64(m13) * b.m33) >> MXI_FBIT;

		i64 temp20 = (i64(m20) * b.m00 + i64(m21) * b.m10 + i64(m22) * b.m20 + i64(m23) * b.m30) >> MXI_FBIT;
		i64 temp21 = (i64(m20) * b.m01 + i64(m21) * b.m11 + i64(m22) * b.m21 + i64(m23) * b.m31) >> MXI_FBIT;
		i64 temp22 = (i64(m20) * b.m02 + i64(m21) * b.m12 + i64(m22) * b.m22 + i64(m23) * b.m32) >> MXI_FBIT;
		i64 temp23 = (i64(m20) * b.m03 + i64(m21) * b.m13 + i64(m22) * b.m23 + i64(m23) * b.m33) >> MXI_FBIT;

		i64 temp30 = (i64(m30) * b.m00 + i64(m31) * b.m10 + i64(m32) * b.m20 + i64(m33) * b.m30) >> MXI_FBIT;
		i64 temp31 = (i64(m30) * b.m01 + i64(m31) * b.m11 + i64(m32) * b.m21 + i64(m33) * b.m31) >> MXI_FBIT;
		i64 temp32 = (i64(m30) * b.m02 + i64(m31) * b.m12 + i64(m32) * b.m22 + i64(m33) * b.m32) >> MXI_FBIT;
		i64 temp33 = (i64(m30) * b.m03 + i64(m31) * b.m13 + i64(m32) * b.m23 + i64(m33) * b.m33) >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); result.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); result.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); result.m02 = static_cast<int>(temp02);
		assert(temp03 < Math::MAX_INT); result.m03 = static_cast<int>(temp03);

		assert(temp10 < Math::MAX_INT); result.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); result.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); result.m12 = static_cast<int>(temp12);
		assert(temp13 < Math::MAX_INT); result.m13 = static_cast<int>(temp13);

		assert(temp20 < Math::MAX_INT); result.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); result.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); result.m22 = static_cast<int>(temp22);
		assert(temp23 < Math::MAX_INT); result.m23 = static_cast<int>(temp23);

		assert(temp30 < Math::MAX_INT); result.m30 = static_cast<int>(temp30);
		assert(temp31 < Math::MAX_INT); result.m31 = static_cast<int>(temp31);
		assert(temp32 < Math::MAX_INT); result.m32 = static_cast<int>(temp32);
		assert(temp33 < Math::MAX_INT); result.m33 = static_cast<int>(temp33);

		return result;
	}

	void Matrix4i::scale(int x, int y, int z)
	{
		i64 temp00 = i64(m00) * x >> MXI_FBIT;
		i64 temp01 = i64(m01) * x >> MXI_FBIT;
		i64 temp02 = i64(m02) * x >> MXI_FBIT;
		i64 temp03 = i64(m03) * x >> MXI_FBIT;

		i64 temp10 = i64(m10) * y >> MXI_FBIT;
		i64 temp11 = i64(m11) * y >> MXI_FBIT;
		i64 temp12 = i64(m12) * y >> MXI_FBIT;
		i64 temp13 = i64(m13) * y >> MXI_FBIT;

		i64 temp20 = i64(m20) * z >> MXI_FBIT;
		i64 temp21 = i64(m21) * z >> MXI_FBIT;
		i64 temp22 = i64(m22) * z >> MXI_FBIT;
		i64 temp23 = i64(m23) * z >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); m02 = static_cast<int>(temp02);
		assert(temp03 < Math::MAX_INT); m03 = static_cast<int>(temp03);

		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);
		assert(temp13 < Math::MAX_INT); m13 = static_cast<int>(temp13);

		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);
		assert(temp23 < Math::MAX_INT); m23 = static_cast<int>(temp23);
	}

	Vector3i Matrix4i::transform(const Vector3i& vec) const
	{
		Vector3i result;

		i64 w = ((i64(vec.x) * m03 + i64(vec.y) * m13 + i64(vec.z) * m23) >> MXI_FBIT) + m33;
		assert(w < Math::MAX_INT);

		i64 tempx = (i64(vec.x) * m00 + i64(vec.y) * m10 + i64(vec.z) * m20 + (m30 << MXI_FBIT)) / w;
		i64 tempy = (i64(vec.x) * m01 + i64(vec.y) * m11 + i64(vec.z) * m21 + (m31 << MXI_FBIT)) / w;
		i64 tempz = (i64(vec.x) * m02 + i64(vec.y) * m12 + i64(vec.z) * m22 + (m32 << MXI_FBIT)) / w;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);

		return result;
	}

	Vector3i Matrix4i::transformAffine(const Vector3i& vec) const
	{
		assert(isAffine());
		Vector3i result;

		i64 tempx = ((i64(vec.x) * m00 + i64(vec.y) * m10 + i64(vec.z) * m20) >> MXI_FBIT) + m30;
		i64 tempy = ((i64(vec.x) * m01 + i64(vec.y) * m11 + i64(vec.z) * m21) >> MXI_FBIT) + m31;
		i64 tempz = ((i64(vec.x) * m02 + i64(vec.y) * m12 + i64(vec.z) * m22) >> MXI_FBIT) + m32;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);

		return result;
	}

	Matrix4i& Matrix4i::inverse()
	{
		Math::Swap(m01, m10);
		Math::Swap(m02, m20);
		Math::Swap(m12, m21);

		m03 = 0;
		m13 = 0;
		m23 = 0;

		m30 = -m30;
		m31 = -m31;
		m32 = -m32;
		m33 = MXI_UNIT;

		return *this;
	}

	void Matrix4i::fromQuan(const Quaternioni& quan)
	{
		quan.toMat4(*this);
	}

	void Matrix4i::rotateAxis(const Vector3i& axis, int iRadian)
	{
		int x = axis.x;
		int y = axis.y;
		int z = axis.z;

		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		Matrix4i  local, final;

		i64 temp00 = ((i64(x) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp01 = ((i64(x) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(z) * iSin) >> MXI_FBIT);
		i64 temp02 = ((i64(x) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(y) * iSin) >> MXI_FBIT);

		i64 temp10 = ((i64(y) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(z) * iSin) >> MXI_FBIT);
		i64 temp11 = ((i64(y) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp12 = ((i64(y) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(x) * iSin) >> MXI_FBIT);

		i64 temp20 = ((i64(z) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(y) * iSin) >> MXI_FBIT);
		i64 temp21 = ((i64(z) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(x) * iSin) >> MXI_FBIT);
		i64 temp22 = ((i64(z) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;

		assert(temp00 < Math::MAX_INT); local.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); local.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); local.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); local.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); local.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); local.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); local.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); local.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); local.m22 = static_cast<int>(temp22);

		local.m03 = 0;
		local.m13 = 0;
		local.m23 = 0;

		local.m30 = 0;
		local.m31 = 0;
		local.m32 = 0;
		local.m33 = MXI_UNIT;

		final = local * (*this);
		*this = final;
	}

	void Matrix4i::rotateAxisReplace(const Vector3i& axis, int iRadian)
	{
		int x = axis.x;
		int y = axis.y;
		int z = axis.z;

		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp00 = ((i64(x) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp01 = ((i64(x) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(z) * iSin) >> MXI_FBIT);
		i64 temp02 = ((i64(x) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(y) * iSin) >> MXI_FBIT);

		i64 temp10 = ((i64(y) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(z) * iSin) >> MXI_FBIT);
		i64 temp11 = ((i64(y) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp12 = ((i64(y) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(x) * iSin) >> MXI_FBIT);

		i64 temp20 = ((i64(z) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(y) * iSin) >> MXI_FBIT);
		i64 temp21 = ((i64(z) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(x) * iSin) >> MXI_FBIT);
		i64 temp22 = ((i64(z) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;

		assert(temp00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);

		m03 = 0;
		m13 = 0;
		m23 = 0;

		m30 = 0;
		m31 = 0;
		m32 = 0;
		m33 = MXI_UNIT;
	}

	void Matrix4i::rotateX(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp10 = (i64(m10) * iCos + i64(m20) * iSin) >> MXI_FBIT;
		i64 temp11 = (i64(m11) * iCos + i64(m21) * iSin) >> MXI_FBIT;
		i64 temp12 = (i64(m12) * iCos + i64(m22) * iSin) >> MXI_FBIT;
		i64 temp13 = (i64(m13) * iCos + i64(m23) * iSin) >> MXI_FBIT;

		i64 temp20 = (i64(m10) *-iSin + i64(m20) * iCos) >> MXI_FBIT;
		i64 temp21 = (i64(m11) *-iSin + i64(m21) * iCos) >> MXI_FBIT;
		i64 temp22 = (i64(m12) *-iSin + i64(m22) * iCos) >> MXI_FBIT;
		i64 temp23 = (i64(m13) *-iSin + i64(m23) * iCos) >> MXI_FBIT;

		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);
		assert(temp13 < Math::MAX_INT); m13 = static_cast<int>(temp13);
		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);
		assert(temp23 < Math::MAX_INT); m23 = static_cast<int>(temp23);
	}

	void Matrix4i::rotateXReplace(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		m00 = MXI_FBIT; m01 = 0;		m02 = 0;	m03 = 0;
		m10 = 0;	    m11 = iCos;		m12 = iSin; m13 = 0;
		m20 = 0;		m21 = -iSin;	m22 = iCos; m23 = 0;
		m30 = 0;		m31 = 0;		m32 = 0;	m33 = MXI_FBIT;
	}

	void Matrix4i::rotateY(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp00 = (i64(m00) * iCos - i64(m20) * iSin) >> MXI_FBIT;
		i64 temp01 = (i64(m01) * iCos - i64(m21) * iSin) >> MXI_FBIT;
		i64 temp02 = (i64(m02) * iCos - i64(m22) * iSin) >> MXI_FBIT;
		i64 temp03 = (i64(m03) * iCos - i64(m23) * iSin) >> MXI_FBIT;

		i64 temp20 = (i64(m00) * iSin + i64(m20) * iCos) >> MXI_FBIT;
		i64 temp21 = (i64(m01) * iSin + i64(m21) * iCos) >> MXI_FBIT;
		i64 temp22 = (i64(m02) * iSin + i64(m22) * iCos) >> MXI_FBIT;
		i64 temp23 = (i64(m03) * iSin + i64(m23) * iCos) >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); m02 = static_cast<int>(temp02);
		assert(temp03 < Math::MAX_INT); m03 = static_cast<int>(temp03);
		assert(temp20 < Math::MAX_INT); m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); m22 = static_cast<int>(temp22);
		assert(temp23 < Math::MAX_INT); m23 = static_cast<int>(temp23);
	}

	void Matrix4i::rotateYReplace(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		m00 = iCos;  m01 = 0;		 m02 = -iSin; m03 = 0;
		m10 = 0;     m11 = MXI_UNIT; m12 = 0;    m13 = 0;
		m20 = iSin;  m21 = 0;		 m22 = iCos; m23 = 0;
		m30 = 0;     m31 = 0;		 m32 = 0;    m33 = MXI_UNIT;
	}

	void Matrix4i::rotateZ(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp00 = (i64(m00) * iCos + i64(m10) * iSin) >> MXI_FBIT;
		i64 temp01 = (i64(m01) * iCos + i64(m11) * iSin) >> MXI_FBIT;
		i64 temp02 = (i64(m02) * iCos + i64(m12) * iSin) >> MXI_FBIT;
		i64 temp03 = (i64(m03) * iCos + i64(m13) * iSin) >> MXI_FBIT;

		i64 temp10 = (i64(m00) *-iSin + i64(m10) * iCos) >> MXI_FBIT;
		i64 temp11 = (i64(m01) *-iSin + i64(m11) * iCos) >> MXI_FBIT;
		i64 temp12 = (i64(m02) *-iSin + i64(m12) * iCos) >> MXI_FBIT;
		i64 temp13 = (i64(m03) *-iSin + i64(m13) * iCos) >> MXI_FBIT;

		assert(temp00 < Math::MAX_INT); m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); m02 = static_cast<int>(temp02);
		assert(temp03 < Math::MAX_INT); m03 = static_cast<int>(temp03);
		assert(temp10 < Math::MAX_INT); m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); m12 = static_cast<int>(temp12);
		assert(temp13 < Math::MAX_INT); m13 = static_cast<int>(temp13);
	}

	void Matrix4i::rotateZReplace(int iRadian)
	{
		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		m00 = iCos; m01 = iSin; m02 = 0;		m03 = 0;
		m10 = -iSin; m11 = iCos; m12 = 0;		m13 = 0;
		m20 = 0;	m21 = 0;	m22 = MXI_UNIT; m23 = 0;
		m30 = 0;	m31 = 0;	m32 = 0;		m33 = MXI_UNIT;
	}

	Vector3i Matrix4i::rotateVec3(const Vector3i& vec)
	{
		Vector3i result;

		i64 tempx = (i64(vec.x) * m00 + i64(vec.y) * m10 + i64(vec.z) * m20) >> MXI_FBIT;
		i64 tempy = (i64(vec.x) * m01 + i64(vec.y) * m11 + i64(vec.z) * m21) >> MXI_FBIT;
		i64 tempz = (i64(vec.x) * m02 + i64(vec.y) * m12 + i64(vec.z) * m22) >> MXI_FBIT;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);

		return result;
	}

	Vector4i Matrix4i::rotateVec4(const Vector4i& vec)
	{
		Vector4i result;

		i64 tempx = (i64(vec.x) * m00 + i64(vec.y) * m10 + i64(vec.z) * m20) >> MXI_FBIT;
		i64 tempy = (i64(vec.x) * m01 + i64(vec.y) * m11 + i64(vec.z) * m21) >> MXI_FBIT;
		i64 tempz = (i64(vec.x) * m02 + i64(vec.y) * m12 + i64(vec.z) * m22) >> MXI_FBIT;

		assert(tempx < Math::MAX_INT); result.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); result.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); result.z = static_cast<int>(tempz);
		result.w = MXI_UNIT;

		return result;
	}

	void Matrix4i::Transpose(Matrix4i& outMat, const Matrix4i& matrix)
	{
		outMat = matrix;
		outMat.transpose();
	}

	void Matrix4i::TransformVec3(Vector3i& outVec, const Vector3i& v, const Matrix4i& m)
	{
		i64 tempx = ((i64(v.x) * m.m00 + i64(v.y) * m.m10 + i64(v.z) * m.m20) >> MXI_FBIT) + m.m30;
		i64 tempy = ((i64(v.x) * m.m01 + i64(v.y) * m.m11 + i64(v.z) * m.m21) >> MXI_FBIT) + m.m31;
		i64 tempz = ((i64(v.x) * m.m02 + i64(v.y) * m.m12 + i64(v.z) * m.m22) >> MXI_FBIT) + m.m32;

		assert(tempx < Math::MAX_INT); outVec.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); outVec.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); outVec.z = static_cast<int>(tempz);
	}

	void Matrix4i::TransformVec4(Vector4i& outVec, const Vector4i& v, const Matrix4i& matrix)
	{
		i64 tempx = (i64(v.x) * matrix.m00 + i64(v.y) * matrix.m10 + i64(v.z) * matrix.m20 + i64(v.w) * matrix.m30) >> MXI_FBIT;
		i64 tempy = (i64(v.x) * matrix.m01 + i64(v.y) * matrix.m11 + i64(v.z) * matrix.m21 + i64(v.w) * matrix.m31) >> MXI_FBIT;
		i64 tempz = (i64(v.x) * matrix.m02 + i64(v.y) * matrix.m12 + i64(v.z) * matrix.m22 + i64(v.w) * matrix.m32) >> MXI_FBIT;
		i64 tempw = (i64(v.x) * matrix.m03 + i64(v.y) * matrix.m13 + i64(v.z) * matrix.m23 + i64(v.w) * matrix.m33) >> MXI_FBIT;

		assert(tempx < Math::MAX_INT); outVec.x = static_cast<int>(tempx);
		assert(tempy < Math::MAX_INT); outVec.y = static_cast<int>(tempy);
		assert(tempz < Math::MAX_INT); outVec.z = static_cast<int>(tempz);
		assert(tempw < Math::MAX_INT); outVec.w = static_cast<int>(tempw);
	}

	void Matrix4i::Inverse(Matrix4i& outMat, const Matrix4i& matrix)
	{
		outMat = matrix;
		outMat.inverse();
	}

	void Matrix4i::RotateAxis(Matrix4i& outMat, const Vector3i& axis, int iRadian)
	{
		int x = axis.x;
		int y = axis.y;
		int z = axis.z;

		int iSin, iCos;
		iSin = Math::Sin_tabi(iRadian);
		iCos = Math::Cos_tabi(iRadian);

		i64 temp00 = ((i64(x) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp01 = ((i64(x) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(z) * iSin) >> MXI_FBIT);
		i64 temp02 = ((i64(x) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(y) * iSin) >> MXI_FBIT);

		i64 temp10 = ((i64(y) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(z) * iSin) >> MXI_FBIT);
		i64 temp11 = ((i64(y) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;
		i64 temp12 = ((i64(y) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(x) * iSin) >> MXI_FBIT);

		i64 temp20 = ((i64(z) * x >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + ((i64(y) * iSin) >> MXI_FBIT);
		i64 temp21 = ((i64(z) * y >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) - ((i64(x) * iSin) >> MXI_FBIT);
		i64 temp22 = ((i64(z) * z >> MXI_FBIT) * (MXI_UNIT - iCos) >> MXI_FBIT) + iCos;

		assert(temp00 < Math::MAX_INT); outMat.m00 = static_cast<int>(temp00);
		assert(temp01 < Math::MAX_INT); outMat.m01 = static_cast<int>(temp01);
		assert(temp02 < Math::MAX_INT); outMat.m02 = static_cast<int>(temp02);

		assert(temp10 < Math::MAX_INT); outMat.m10 = static_cast<int>(temp10);
		assert(temp11 < Math::MAX_INT); outMat.m11 = static_cast<int>(temp11);
		assert(temp12 < Math::MAX_INT); outMat.m12 = static_cast<int>(temp12);

		assert(temp20 < Math::MAX_INT); outMat.m20 = static_cast<int>(temp20);
		assert(temp21 < Math::MAX_INT); outMat.m21 = static_cast<int>(temp21);
		assert(temp22 < Math::MAX_INT); outMat.m22 = static_cast<int>(temp22);

		outMat.m03 = 0;
		outMat.m13 = 0;
		outMat.m23 = 0;

		outMat.m30 = 0;
		outMat.m31 = 0;
		outMat.m32 = 0;
		outMat.m33 = MXI_UNIT;
	}

	void Matrix4i::RotateYawPitchRoll(Matrix4i& outMat, int iYaw, int iPitch, int iRoll)
	{
		Quaternioni q;
		q.fromEulerAngle(iPitch, iYaw, iRoll);
		outMat.fromQuan(q);
	}
	*/
}

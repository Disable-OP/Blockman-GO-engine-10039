#include "UnitTest.h"

#include "Object/Root.h"

namespace BLOCKMAN
{

void testMathLib()
{
	// test for render 3d box in gui.
	Vector3 boxface[12];
	boxface[0] = Vector3(-0.5f, 0.5f, -0.5f);
	boxface[1] = Vector3(0.5f, 0.5f, -0.5f);
	boxface[2] = Vector3(0.5f, 0.5f, 0.5f);
	boxface[3] = Vector3(-0.5f, 0.5f, 0.5f);

	boxface[4] = Vector3(-0.5f, 0.5f, 0.5f);
	boxface[5] = Vector3(-0.5f, -0.5f, 0.5f);
	boxface[6] = Vector3(0.5f, -0.5f, 0.5f);
	boxface[7] = Vector3(0.5f, 0.5f, 0.5f);

	boxface[8] = Vector3(-0.5f, 0.5f, -0.5f);
	boxface[9] = Vector3(-0.5f, -0.5f, -0.5f);
	boxface[10] = Vector3(-0.5f, 0.5f, 0.5f);
	boxface[11] = Vector3(-0.5f, -0.5f, 0.5f);

	Matrix4 boxRot;
	Matrix4::RotateYawPitchRoll(boxRot, Math::PI_DIV4, Math::PI_DIV6, 0);

	for (int i = 0; i < 12; ++i)
		boxface[i] = (boxface[i] * boxRot) * 1.25f * 0.5f;
}


}
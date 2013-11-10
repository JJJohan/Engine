#include "stdafx.h"

namespace Engine
{
	Vector3::Vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3::Vector3(float a_x, float a_y, float a_z)
	{
		x = a_x;
		y = a_y;
		z = a_z;
	}
}

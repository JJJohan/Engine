#include "stdafx.h"

namespace Engine
{
	Quaternion::Quaternion()
	{
		w = 0.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Quaternion::Quaternion(float a_w, float a_x, float a_y, float a_z)
	{
		w = a_w;
		x = a_x;
		y = a_y;
		z = a_z;
	}
}

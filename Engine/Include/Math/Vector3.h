#ifndef VECTOR3_H
#define VECTOR3_H

namespace Engine
{
	class Vector3
	{
	public:
		Vector3();
		Vector3(float a_x, float a_y, float a_z);

		float x;
		float y;
		float z;
	};
}

#endif // VECTOR3_H
#ifndef QUATERNION_H
#define QUATERNION_H

namespace Engine
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float a_w, float a_x, float a_y, float a_z);

		float w;
		float x;
		float y;
		float z;
	};
}

#endif // QUATERNION_H
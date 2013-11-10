#ifndef QUATERNION_H
#define QUATERNION_H

namespace Engine
{
	class Quaternion
	{
	public:
		__declspec(dllexport) Quaternion();
		__declspec(dllexport) Quaternion(float a_w, float a_x, float a_y, float a_z);

		float w;
		float x;
		float y;
		float z;
	};
}

#endif // QUATERNION_H
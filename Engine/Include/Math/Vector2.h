#ifndef VECTOR2_H
#define VECTOR2_H

namespace Engine
{
	class Vector2
	{
	public:
		__declspec(dllexport) Vector2();
		__declspec(dllexport) Vector2(float a_x, float a_y);

		float x;
		float y;
	};
}

#endif // VECTOR2_H
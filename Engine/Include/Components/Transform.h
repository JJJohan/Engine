#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "stdafx.h"

namespace Engine
{
	class Transform : public Component
	{
	public:
		Transform(Object* a_pParent);
		~Transform();

		void Position(Vector3 a_pos);
		Vector3& Position();

	private:
		Vector3 m_pos;
	};
}

#endif // TRANSFORM_H
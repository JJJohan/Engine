#include "stdafx.h"

namespace Engine
{
	Transform::Transform(Object* a_pParent)
		: Component(a_pParent)
	{
	}

	Transform::~Transform()
	{
	}

	void Transform::Position(Vector3 a_pos)
	{
		m_pos = a_pos;
	}

	Vector3& Transform::Position()
	{
		return m_pos;
	}
}
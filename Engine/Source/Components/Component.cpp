#include "stdafx.h"

namespace Engine
{
	Component::Component(Object* a_pParent)
	{
		m_pParent = a_pParent;
	}

	Component::~Component()
	{

	}
}
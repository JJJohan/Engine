#ifndef COMPONENT_H
#define COMPONENT_H

#include "stdafx.h"

namespace Engine
{
	class Object;

	class Component
	{
	public:
		Component(Object* a_pParent);
		virtual ~Component() = 0;

	private:
		Object* m_pParent;
	};
}

#endif // COMPONENT_H
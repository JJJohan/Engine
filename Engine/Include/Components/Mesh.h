#ifndef MESH_H
#define MESH_H

#include "stdafx.h"

namespace Engine
{
	class Mesh : public Component
	{
	public:
		Mesh(Object* a_pParent);
		~Mesh();

		void Test() { LOG("I'm a mesh!"); }

	private:

	};
}

#endif // MESH_H
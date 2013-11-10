#ifndef MESH_H
#define MESH_H

#include "stdafx.h"

namespace Engine
{
	class Mesh : public Component
	{
	public:
		__declspec(dllexport) Mesh(Object* a_pParent);
		__declspec(dllexport) ~Mesh();

		void Test() { LOG("I'm a mesh!"); }

	private:

	};
}

#endif // MESH_H
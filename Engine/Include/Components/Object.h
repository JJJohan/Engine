#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <typeinfo>
#include "stdafx.h"

namespace Engine
{
	class Object
	{
	public:
		__declspec(dllexport) Object(Object* a_pParent = NULL);
		__declspec(dllexport) ~Object();

		__declspec(dllexport) Object* AddChild();
		__declspec(dllexport) Object* GetParent();

		void RemoveChild(Object* a_pChild);
		void SetParent(Object* a_pParent);

		template <class T>
		T* AddComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "T must be of type Component.");

			T* component = new T(this);
			m_components.push_back(component);

			return component;
		}

		template <class T>
		T* GetComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "T must be of type Component.");

			for (unsigned int i = 0; i < m_components.size(); ++i)
			{
				if (typeid(m_components[i]) == typeid(T))
					return static_cast<T*>(m_components[i]);
			}

			return NULL;
		}

	private:
		Transform* m_pTransform;
		Object* m_pParent;
		std::vector<Component*> m_components;
		std::vector<Object*> m_children;
	};
}

#endif // OBJECT_H
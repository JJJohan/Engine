#include "stdafx.h"

namespace Engine
{
	Object::Object(Object* a_pParent)
	{
		m_pParent = a_pParent;
		m_pTransform = AddComponent<Transform>();
	}

	Object::Object()
	{
		m_pParent = NULL;
		m_pTransform = AddComponent<Transform>();
	}

	Object::~Object()
	{
		// Remove parent connection
		if (m_pParent)
			m_pParent->RemoveChild(this);

		// Delete components
		for (auto iter = m_components.begin(); iter != m_components.end();)
		{
			SAFE_DELETE(*iter);
			iter = m_components.erase(iter);
		}

		// Delete children
		for (auto iter = m_children.begin(); iter != m_children.end();)
		{
			Object* obj = *iter;
			iter = m_children.erase(iter);
			SAFE_DELETE(obj);
		}
	}

	Object* Object::AddChild()
	{
		// Add a child object
		Object* child = new Object(this);
		m_children.push_back(child);
		return child;
	}

	void Object::RemoveChild(Object* a_pChild)
	{
		// Remove a particular child
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			if (a_pChild == *iter)
			{
				m_children.erase(iter);
				return;
			}
		}
	}

	Object* Object::GetParent()
	{
		return m_pParent;
	}

	void Object::SetParent(Object* a_pParent)
	{
		// Check if a parent exists
		if (a_pParent && m_pParent)
			LOG_ERROR("Tried to set parent while parent already exists.");

		// Set a new parent
		m_pParent = a_pParent;
	}
}
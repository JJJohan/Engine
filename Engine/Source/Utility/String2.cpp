#include "stdafx.h"
#include <sstream>

namespace Engine
{
	String::String(std::string a_text)
	{
		m_text = a_text;
	}

	String::String()
	{
		m_text = "";
	}

	String String::operator+(String rhs)
	{
		return String(m_text + rhs.StdString());
	}

	void String::operator=(std::string rhs)
	{
		m_text = rhs;
	}

	bool String::operator==(std::string rhs)
	{
		return (m_text == rhs);
	}

	std::string& String::StdString()
	{
		return m_text;
	}

	const char* String::CString()
	{
		return m_text.c_str();
	}

	std::vector<std::string> String::Split(const char a_char)
	{
		std::vector<std::string> vec;
		std::stringstream ss(m_text);
		std::string item;
		
		while (std::getline(ss, item, a_char)) 
		{
			vec.push_back(item);
		}

		return vec;
	}

	String String::StripWhitespace()
	{
		bool whitespace = false;
		std::stringstream ss;
		for (unsigned int i = 0; i < m_text.size(); ++i)
		{
			if (m_text[i] == ' ' || m_text[i] == 32)
			{
				if (!whitespace)
				{
					whitespace = true;
					ss << m_text[i];
				}
			}
			else
			{
				whitespace = false;
				ss << m_text[i];
			}
		}

		return String(ss.str());
	}

	String String::RemoveChar(const char a_char)
	{
		std::string string;
		std::vector<std::string> vec = String::Split(a_char);
		for (unsigned int i = 0; i < vec.size(); ++i)
			string += vec[i];	

		return String(string);
	}

	int String::ToInt()
	{
		return atoi(m_text.c_str());
	}
}
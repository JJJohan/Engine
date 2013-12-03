#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>

namespace Engine
{
	class String
	{
	public:
		__declspec(dllexport) String(std::string a_text);
		__declspec(dllexport) String();
		__declspec(dllexport) String operator+(String rhs);
		__declspec(dllexport) void operator=(std::string rhs);
		__declspec(dllexport) bool operator==(std::string rhs);
		__declspec(dllexport) std::string& StdString();
		__declspec(dllexport) const char* CString();

		__declspec(dllexport) std::vector<std::string> Split(const char a_char);
		__declspec(dllexport) String RemoveChar(const char a_char);
		__declspec(dllexport) String StripWhitespace();
		__declspec(dllexport) int ToInt();

	private:
		std::string m_text;
	};
}

#endif // STRING_H
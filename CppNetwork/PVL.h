#ifndef _PVL_H
#define _PVL_H

#include <string>
#include <sstream>

enum commands {
	Passive = -1,
	Back_To_Chat,
	Exit,
	Clear_UI,
	Connect,
	Disconnect,
	SizeOfChatHistory,
	Modify_IP,
	Modify_Port
};

enum trassmision {
	join = 1,
	left,
	normal,
	server,
	ok,
	invalid
};

namespace pvl
{
	template<typename T>
	std::string convertToString(T value)
	{
		std::ostringstream strs;
		strs << value;
		return strs.str();
	}
	std::string padString(const std::string& data, size_t width, char filler = ' ');
}
#endif // !
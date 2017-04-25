#include "PVL.h"

namespace pvl
{
	std::string padString(const std::string& data, size_t width, char filler)
	{
		if (data.length() < width)
		{
			std::string output = data;
			return output.append(width - output.size(), filler);
		}

		//to seperate long names with a space
		//crucial for the integretiy of the database
		if (filler == ' ' && data != "" && data[data.length() - 1] != ' ')
			return data + ' ';

		return data;
	}

}
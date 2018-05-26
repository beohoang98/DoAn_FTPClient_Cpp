#include "stdafx.h"

#include "OutputClass.h"

#include <string>
#include <iostream>

namespace std
{
	Output::Output(ostream& os)
	{
		this->out = &os;
		data = "";
	}

	Output::~Output()
	{
		data.clear();
	}

	Output& Output::operator<<(const int num)
	{
		data += to_string(num);
		return *this;
	}
	Output& Output::operator<<(const string s)
	{
		data += s;
		return *this;
	}
	Output& Output::operator<<(const char * c_str)
	{
		data += string(c_str);
		return *this;
	}
	Output& Output::operator<<(const char c)
	{
		data.push_back(c);
		return *this;
	}

	void Output::flush()
	{
		this->xulyStream();
		*out << data;
		data.clear();
		this->setToDefault();
	}
}
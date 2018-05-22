#include "stdafx.h"
#include "ConsoleOutput.h"

namespace std
{
	FtpStatus& FtpStatus::operator << (const int n)
	{
		printTab();
		*out << n;
		return *this;
	}

	void FtpStatus::printTab()
	{
		*out << "\t";
	}

	FtpStatus& FtpStatus::operator << (const string s)
	{
		printTab();
		*out << s.c_str();
		return *this;
	}
	FtpStatus& FtpStatus::operator << (const char * s)
	{
		printTab();
		*out << s;
		return *this;
	}

	FtpStatus& FtpStatus::operator << (const char c)
	{
		printTab();
		*out << c;
		return *this;
	}
}
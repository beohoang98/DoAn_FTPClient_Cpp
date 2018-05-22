#pragma once
#include <iostream>

namespace std {
	struct FtpStatus {
		FtpStatus(ostream& os) : out(&os) {}
		ostream * out;

		void printTab();

		FtpStatus& operator<<(const int);
		FtpStatus& operator<<(const string);
		FtpStatus& operator<<(const char *);
		FtpStatus& operator<<(const char);
	};

	static FtpStatus ftpStt(cout);
}
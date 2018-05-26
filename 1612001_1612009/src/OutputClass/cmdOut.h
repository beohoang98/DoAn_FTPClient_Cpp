#include "OutputClass.h"
#include <iostream>
#include <Windows.h>

namespace std
{
	class CmdOut : public Output
	{
	private:
		void setColorWhite()
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY
				| FOREGROUND_RED
				| FOREGROUND_GREEN
				| FOREGROUND_BLUE);
		}
		void setToDefault() override
		{
			setColorWhite();
		}

		void xulyStream() override
		{
			setColorWhite();
		}

	public:
		CmdOut(ostream& os) : Output(os)
		{

		}
	};

	static CmdOut cmdOut(cout);
};
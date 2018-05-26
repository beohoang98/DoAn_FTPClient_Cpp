#include "OutputClass.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>

namespace std
{
	class FtpStatus : public Output
	{
	private:
		void setColorBlue()
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		}

		void setToDefault() override
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY
				| FOREGROUND_RED
				| FOREGROUND_GREEN
				| FOREGROUND_BLUE);
		}

		void xulyStream() override
		{
			setColorBlue();
			data.insert(data.begin(), '\t');
			
			string::iterator it = data.begin();
			while(it != data.end())
			{
				it = find(it, data.end(), '\n');
				if (it >= data.end()-1)
					break;

				data.insert(it, '\t');
				it+=2;
			}
			
		}

	public:
		FtpStatus(ostream& os) : Output(os)
		{

		}		
	};

	static FtpStatus ftpStt(cout);
};
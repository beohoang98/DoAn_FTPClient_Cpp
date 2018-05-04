// eptipi.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "eptipi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			//==========================================================================
			
			// TODO: code your application's behavior here.
			if (AfxSocketInit() == FALSE)
			{
				cout << "Khong the khoi tao Socket";
				return FALSE;
			}
			wchar_t default_servername[] = L"127.0.0.1";

			//Chuong trinh bat dau
			try {
				Eptipi Client;
				string user_input;
				stringstream to_split_input;
				string user_cmd, user_path;

				if (argc > 2) 
					Client.connectServer(argv[1]);
				else
					Client.connectServer(default_servername);

				if (!Client.login()) {
					return 1;
				}

				while (true) {
					cout << "ftp> ";

					cin.sync();
					getline(cin, user_input);

					to_split_input = stringstream(user_input);
					getline(to_split_input, user_cmd, ' ');
					getline(to_split_input, user_path);

					Client.handleCmd(user_cmd, user_path);

					if (user_cmd == "quit")
						break;
				}
			}
			catch (l_exception& e) {
				cout << "ERROR: " << e.what() << endl;
			}

			//===========================================================================
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

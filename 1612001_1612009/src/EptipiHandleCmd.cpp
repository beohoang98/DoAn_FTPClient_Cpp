#pragma once

#include "stdafx.h"
#include "eptipi.h"
#include <fstream>

using namespace std;


/*------------------------------------------
	dir
	@liet ke chi tiet thu muc tren server
*/
void Eptipi::lietKeChiTiet()
{
	struct deKhaiBaoHam {
		static bool before(EptipiCallbackParam cb) {
			string cmd = "LIST\r\n";
			cb.cmdCon->Send(cmd.c_str(), cmd.length());

			char buffer[BUFFER_LENGTH] = { 0 };
			cb.cmdCon->Receive(buffer, BUFFER_LENGTH - 1);
			cout << "\t" << buffer << endl;

			int code;
			stringstream ss(buffer);
			ss >> code;

			if (code != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};
		static void after(EptipiCallbackParam cb) {
			if (cb.dataCon == NULL) return;

			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);
			while (cb.dataCon->Receive(buffer, BUFFER_LENGTH - 1) > 0) {
				cout << buffer;
				memset(buffer, 0, BUFFER_LENGTH);
			}
		}
	};
	EptipiCallbackParam cb;

	openDataPort(deKhaiBaoHam::before, deKhaiBaoHam::after, cb);
}

/*------------------------------------------
	ls
	@liet ke ten cac thu muc va file tren server
*/
void Eptipi::lietKeDonGian()
{
	struct ASD {
		static bool before(EptipiCallbackParam cb) {
			string cmd = "NLST\r\n";
			cb.cmdCon->Send(cmd.c_str(), cmd.length());

			char buffer[BUFFER_LENGTH] = { 0 };
			cb.cmdCon->Receive(buffer, BUFFER_LENGTH - 1);
			cout << "\t" << buffer << endl;

			int code;
			stringstream ss(buffer);
			ss >> code;

			if (code != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};
		static void after(EptipiCallbackParam cb) {
			if (cb.dataCon == NULL) return;

			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);
			while (cb.dataCon->Receive(buffer, BUFFER_LENGTH - 1) > 0) {
				cout << buffer;
				memset(buffer, 0, BUFFER_LENGTH);
			}
		}
	};
	EptipiCallbackParam cb;

	openDataPort(ASD::before, ASD::after, cb);
}
 

/*------------------------------------------

*/
void Eptipi::changeServerDir(string path)
{

}
 
/*------------------------------------------

*/
void Eptipi::changeClientDir(string path)
{

}
 

/*------------------------------------------

*/
void Eptipi::printServerPath()
{

}
 
/*------------------------------------------

*/
void Eptipi::printClientPath()
{

}
 

/*------------------------------------------

*/
void Eptipi::upFile(string fileName)
{

}
 
/*------------------------------------------

*/
void Eptipi::downFile(string fileName)
{
	struct ASD {
		string filename;
		static bool before(EptipiCallbackParam cb)
		{
			string cmd = "RETR " + cb.path + "\r\n";
			cb.cmdCon->Send(cmd.c_str(), cmd.length());

			char buffer[BUFFER_LENGTH] = { 0 };
			cb.cmdCon->Receive(buffer, BUFFER_LENGTH - 1);
			cout << "\t" << buffer << endl;

			int code;
			stringstream ss(buffer);
			ss >> code;

			if (code != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};
		static void after(EptipiCallbackParam cb)
		{
			if (cb.dataCon == NULL)
				return;

			ofstream fileout(cb.path);
			if (!fileout.is_open()) {
				cout << "\tError: cannot save downloaded file\n\n";
				return;
			}

			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);
			while (cb.dataCon->Receive(buffer, BUFFER_LENGTH - 1) > 0) {
				fileout << buffer;
				memset(buffer, 0, BUFFER_LENGTH);
			}

			cout << "\tDownload " << cb.path << " successfully\n\n";
			fileout.close();
		}
	};

	EptipiCallbackParam callbackparam;
	callbackparam.path = fileName;

	openDataPort(ASD::before, ASD::after, callbackparam);
}
 
/*------------------------------------------

*/
void Eptipi::upNhieuFile(string fileNames)
{

}
 
/*------------------------------------------

*/
void Eptipi::downNhieuFile(string fileNames)
{

}
 
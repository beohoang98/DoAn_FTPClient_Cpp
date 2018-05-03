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

		/*
			thuc hien cau lenh de su dung data port
			@param (cb) du lieu luu lai de callback
		*/
		static bool before(FTP::CallbackInfo cb) {
			string cmd = "LIST\r\n";
			cb.cmdCon->Send(cmd.c_str(), cmd.length());

			char buffer[BUFFER_LENGTH] = { 0 };
			Eptipi::receiveOneLine(cb.cmdCon, buffer, BUFFER_LENGTH);
			cout << "\t" << buffer << endl;

			int code;
			stringstream ss(buffer);
			ss >> code;

			if (code != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};

		/*
			thuc hien sau khi port duoc connect thanh cong
			@param (cb) du lieu callback luu lai bao gom dataCon(data port da connect)
		*/
		static void after(FTP::CallbackInfo cb) {
			if (cb.dataCon == NULL) return;

			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);
			while (cb.dataCon->Receive(buffer, BUFFER_LENGTH - 1) > 0) {
				cout << buffer;
				memset(buffer, 0, BUFFER_LENGTH);
			}
		}
	};

	this->sendCmd("TYPE A\r\n"); //ascii mode
	this->receiveOneLine();

	FTP::CallbackInfo cb;
	openDataPort(deKhaiBaoHam::before, deKhaiBaoHam::after, cb);
}

/*------------------------------------------
	ls
	@liet ke ten cac thu muc va file tren server
*/
void Eptipi::lietKeDonGian()
{
	struct ASD {
		/*
		thuc hien cau lenh de su dung data port
		@param (cb) du lieu luu lai de callback
		*/
		static bool before(FTP::CallbackInfo cb) {
			string cmd = "NLST\r\n";
			cb.cmdCon->Send(cmd.c_str(), cmd.length());

			char buffer[BUFFER_LENGTH] = { 0 };
			Eptipi::receiveOneLine(cb.cmdCon, buffer, BUFFER_LENGTH);
			cout << "\t" << buffer << endl;

			int code;
			stringstream ss(buffer);
			ss >> code;

			if (code != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};
		/*
		thuc hien sau khi port duoc connect thanh cong
		@param (cb) du lieu callback luu lai bao gom dataCon(data port da connect)
		*/
		static void after(FTP::CallbackInfo cb) {
			if (cb.dataCon == NULL) return;

			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);
			while (cb.dataCon->Receive(buffer, BUFFER_LENGTH - 1) > 0) {
				cout << buffer;
				memset(buffer, 0, BUFFER_LENGTH);
			}
		}
	};

	this->sendCmd("TYPE A\r\n"); //ascii mode
	this->receiveOneLine();

	FTP::CallbackInfo cb;
	openDataPort(ASD::before, ASD::after, cb);
}
 
/*
ldir
*/
void Eptipi::lietKeClientChiTiet()
{
	cout << "\tClient dir:\n\n";
	system("dir");
	cout << endl;
}

/*
lls
*/
void Eptipi::lietKeClientDonGian()
{
	cout << "\tClient dir:\n\n";
	system("dir /b");
	cout << endl;
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

		/*
		thuc hien cau lenh de su dung data port
		@param (cb) du lieu luu lai de callback
		*/
		static bool before(FTP::CallbackInfo cb)
		{
			string cmd = "RETR " + cb.path + "\r\n";
			cb.cmdCon->Send(cmd.c_str(), cmd.length());

			char buffer[BUFFER_LENGTH] = { 0 };
			Eptipi::receiveOneLine(cb.cmdCon, buffer, BUFFER_LENGTH);
			cout << "\t" << buffer << endl;

			int code;
			stringstream ss(buffer);
			ss >> code;

			if (code != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};

		/*
		thuc hien sau khi port duoc connect thanh cong
		@param (cb) du lieu callback luu lai bao gom dataCon(data port da connect)
		*/
		static void after(FTP::CallbackInfo cb)
		{
			if (cb.dataCon == NULL)
				return;

			//create new file
			ofstream fileout(cb.path);
			if (!fileout.is_open()) {
				cout << "\tError: cannot save downloaded file\n\n";
				return;
			}

			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);
			while (cb.dataCon->Receive(buffer, BUFFER_LENGTH - 1) > 0) {
				//write data to file
				fileout << buffer;
				memset(buffer, 0, BUFFER_LENGTH);
			}

			cout << "\tDownload " << cb.path << " successfully\n\n";
			fileout.close();
		}
	};

	this->sendCmd("TYPE I\r\n"); //binary mode
	this->receiveOneLine();

	FTP::CallbackInfo callbackparam;
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
 
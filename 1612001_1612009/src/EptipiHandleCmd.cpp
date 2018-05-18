#pragma once

#include "stdafx.h"
#include "eptipi.h"
#include <fstream>
#include <iomanip>

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
		static bool before(CallbackInfo &cb) {
			cb.mainFTP->sendCmd("LIST\r\n");
			cb.mainFTP->receiveOneLine();
			cout << '\t' << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};

		/*
			thuc hien sau khi port duoc connect thanh cong
			@param (cb) du lieu callback luu lai bao gom dataCon(data port da connect)
		*/
		static void after(CallbackInfo &cb) {
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

	CallbackInfo cb;
	cb.mainFTP = this;
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
		static bool before(CallbackInfo &cb) {
			cb.mainFTP->sendCmd("NLST\r\n");
			cb.mainFTP->receiveOneLine();
			cout << '\t' << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};
		/*
		thuc hien sau khi port duoc connect thanh cong
		@param (cb) du lieu callback luu lai bao gom dataCon(data port da connect)
		*/
		static void after(CallbackInfo &cb) {
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

	CallbackInfo cb;
	cb.mainFTP = this;
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
	SetCurrentDirectoryA(path.c_str());
	system("cd");
	cout << endl;
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
		static void showPercent(ostream &os, INT64 cur, INT64 size, clock_t time) {
			INT64 showLength = cur * 20 / size;
			int speed;
			if (time > 0) speed = 1024 * CLOCKS_PER_SEC / time;
			else speed = 0;

			os << "\r\t[";
			for (int i = 0; i < showLength; ++i)
				os << char(219);
			for (int i = 0; i < 20 - showLength; ++i)
				os << '-';
			os << "] " << cur/1024 << "/" << size/1024 << "KB ";
			os << setw(4) << speed << "KB/s ";
		}
		/*
		thuc hien cau lenh de su dung data port
		@param (cb) du lieu luu lai de callback
		*/
		static bool before(CallbackInfo&cb)
		{
			string cmd;
			char buffer[BUFFER_LENGTH] = { 0 };
			INT64 filesize = 0;

			//get file size
			cb.mainFTP->sendCmd("SIZE " + cb.path + "\r\n");
			cb.mainFTP->receiveOneLine();
			if (cb.mainFTP->getCode() == FTPCode::FILE_STATUS) {
				stringstream forReadStr(cb.mainFTP->getReturnStr());
				forReadStr >> filesize	// first is code (213)
						>> filesize;	// second is actually size
				cb.filesize = filesize;
			}
			else {
				cout << "\t" << cb.mainFTP->getReturnStr() << endl;
			}
			
			//switch to binary mode
			cb.mainFTP->sendCmd("TYPE I\r\n");
			cb.mainFTP->receiveOneLine();
			
			// get file
			cb.mainFTP->sendCmd("RETR " + cb.path + "\r\n");
			cb.mainFTP->receiveOneLine();
			cout << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};

		/*
		thuc hien sau khi port duoc connect thanh cong
		@param (cb) du lieu callback luu lai bao gom dataCon(data port da connect)
		*/
		static void after(CallbackInfo&cb)
		{
			if (cb.dataCon == NULL)
				return;
			int filesize = 0;
			int bytes = 0;
			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);

			//create new file
			ofstream fileout(cb.path, ios::binary);
			if (!fileout.is_open()) {
				cout << "\tError: cannot save downloaded file\n\n";
				return;
			}

			clock_t countTm = clock();
			int oldKB = 1024 * 1024; // 1MB
			while ((bytes = cb.dataCon->Receive(buffer, BUFFER_LENGTH)) && bytes > 0) {
				//write data to file
				fileout.write(buffer, bytes);
				memset(buffer, 0, BUFFER_LENGTH);
				filesize += bytes;

				if (cb.filesize > 0 && filesize > oldKB) {
					countTm = clock() - countTm;
					ASD::showPercent(cout, filesize, cb.filesize, countTm);
					oldKB += 1024 * 1024;
				}
			}

			if (cb.filesize > 0) ASD::showPercent(cout, filesize, cb.filesize, 0);
			cout << endl;
			
			cout << "\tDownload " << cb.path << " successfully\n";
			cout << "\tLength: " << filesize << "\n\n";
			fileout.close();			
		}
	};

	CallbackInfo callbackparam;
	callbackparam.path = fileName;
	callbackparam.mainFTP = this;

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
	struct GetFileList {
		static bool before(CallbackInfo& cb) {
			cb.mainFTP->sendCmd("NLST " + cb.path + "\r\n");
			cb.mainFTP->receiveOneLine();
			cout << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER)
				return false;
			return true;
		};
		static void after(CallbackInfo& cb) {
			if (cb.dataCon == NULL)
				return;

			cb.path = "";
			char buffer[BUFFER_LENGTH] = { 0 };
			while (cb.dataCon->Receive(buffer, BUFFER_LENGTH - 1)) {
				cb.path += buffer;
			}
		}
	};

	CallbackInfo cb;
	cb.mainFTP = this;
	cb.path = fileNames;
	openDataPort(GetFileList::before, GetFileList::after, cb);

	stringstream forSplitFile(cb.path);
	string filename;
	string cmd;

	while (!forSplitFile.eof()) {
		getline(forSplitFile, filename, '\r');
		if (filename[0] == '\n') filename.erase(0, 1);
		if (filename == "") break;

		cout << "Get " << filename << "?(y-yes/else-no): ";

		cin.sync(); //flush \n
		getline(cin, cmd);
		if (cmd == "y" || cmd == "yes") {
			this->downFile(filename);
		}
		else {
			//do nothing
		}
	}

	cout << endl;
}
 

void Eptipi::showAllCmd() {
	for (auto cmd : listCmd) {
		cout << '\t' << setw(20) << left 
			<< cmd.first
			<< " : " 
			<< cmd.second.title << endl;
	}
	cout << endl;
}

void Eptipi::showHelpFor(string cmd) {
	auto info = listCmd.find(cmd);
	
	if (info == listCmd.end()) {
		cout << "\tkhong tim thay " << cmd << "\n\n";
		return;
	}
	
	cout << "\t" << info->second.title << endl;
	cout << info->second.description << endl;
	cout << endl;
}


void Eptipi::switchToPassive()
{
	if (this->dataMode == FTPDataMode::PASSIVE)
	{
		cout << "\tVan dang tren passive mode\n\n";
	}
	else {
		this->dataMode = FTPDataMode::PASSIVE;
		cout << "\tDa chuyen sang passive mode\n\n";
	}
}
void Eptipi::switchToActive()
{
	if (this->dataMode == FTPDataMode::ACTIVE)
	{
		cout << "\tVan dang tren active mode\n\n";
	}
	else {
		this->dataMode = FTPDataMode::ACTIVE;
		cout << "\tDa chuyen sang active mode\n\n";
	}
}
void Eptipi::switchToBinary()
{
	this->fileMode = FTPFileMode::BINARY;
	cout << "\tDa chuyen sang BINARY mode\n\n";
}
void Eptipi::switchToAscii()
{
	this->fileMode = FTPFileMode::ASCII;
	cout << "\tDa chuyen sang ASCII mode\n\n";
}

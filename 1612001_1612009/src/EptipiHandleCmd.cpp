/**
 * Source nay de define cac ham xu ly lenh ftp
 * 
 * Created by BeoHoang
*/

#pragma once

#include "stdafx.h"
#include "eptipi.h"
#include <fstream>
#include <sstream>
#include <iomanip>

#include "ProgressBar.h"

using namespace std;


/*------------------------------------------
	dir
	@liet ke chi tiet thu muc tren server
*/
void Eptipi::lietKeChiTiet(string path)
{
	struct deKhaiBaoHam {
		
		static bool before(CallbackInfo &cb) {
			cb.mainFTP->sendCmd("LIST "+cb.path+"\r\n");
			cb.mainFTP->receiveStatus();
			cout << '\t' << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER 
				&& cb.mainFTP->getCode() != FTPCode::DATA_ALREADY_OPEN)
				return false;
			return true;
		};

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
	this->receiveStatus();

	CallbackInfo cb;
	cb.path = path;
	cb.mainFTP = this;
	openDataPort(deKhaiBaoHam::before, deKhaiBaoHam::after, cb);
}

/*------------------------------------------
	ls
	@liet ke ten cac thu muc va file tren server
*/
void Eptipi::lietKeDonGian(string path)
{
	struct ASD {
		/*
		thuc hien cau lenh de su dung data port
		@param (cb) du lieu luu lai de callback
		*/
		static bool before(CallbackInfo &cb) {
			cb.mainFTP->sendCmd("NLST " + cb.path +"\r\n");
			cb.mainFTP->receiveStatus();
			cout << '\t' << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER
				&& cb.mainFTP->getCode() != FTPCode::DATA_ALREADY_OPEN)
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
	this->receiveStatus();

	CallbackInfo cb;
	cb.mainFTP = this;
	cb.path = path;
	openDataPort(ASD::before, ASD::after, cb);
}
 
/*
ldir
*/
void Eptipi::lietKeClientChiTiet(string path)
{
	system(("dir "+path).c_str());
	cout << endl;
}

/*
lls
*/
void Eptipi::lietKeClientDonGian(string path)
{
	system(("dir /b "+path).c_str());
	cout << endl;
}

/*------------------------------------------
cd
*/
void Eptipi::changeServerDir(string path)
{
	this->sendCmd("CWD "+path+"\r\n"); //ascii mode
	this->receiveStatus();
	cout << this->getReturnStr() << endl;
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
	this->sendCmd("PWD\r\n"); //ascii mode
	this->receiveStatus();
	cout << this->getReturnStr() << endl;
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
		/*
		thuc hien cau lenh de su dung data port
		@param (cb) du lieu luu lai de callback
		*/
		static bool before(CallbackInfo&cb)
		{
			string cmd;
			char buffer[BUFFER_LENGTH] = { 0 };
			UINT64 filesize = 0;

			//get file size
			cb.mainFTP->sendCmd("SIZE " + cb.path + "\r\n");
			cb.mainFTP->receiveStatus();
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
			cb.mainFTP->receiveStatus();
			
			// get file
			cb.mainFTP->sendCmd("RETR " + cb.path + "\r\n");
			cb.mainFTP->receiveStatus();
			cout << '\t' << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER
				&& cb.mainFTP->getCode() != FTPCode::DATA_ALREADY_OPEN)
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
			UINT64 filesize = 0;

			int bytes = 0;
			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);

			//create new file
			ofstream fileout(cb.path, ios::binary);
			if (!fileout.is_open()) {
				cout << "\tError: cannot save downloaded file\n\n";
				return;
			}

			ProgressBar display;
			display.setBarSize(20);
			display.setTotalSize(cb.filesize);

			while ((bytes = cb.dataCon->Receive(buffer, BUFFER_LENGTH)) && bytes > 0) {
				//write data to file
				fileout.write(buffer, bytes);
				memset(buffer, 0, BUFFER_LENGTH);
				filesize += bytes;

				display.updateAndDraw(cout, filesize);
			}

			//finally
			display.update(cb.filesize);
			if (cb.filesize > 0) display.draw(cout);

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
			cb.mainFTP->receiveStatus();
			cout << '\t' << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER
				&& cb.mainFTP->getCode() != FTPCode::DATA_ALREADY_OPEN)
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

	stringstream split_path(fileNames);
	string path_each;
	string filename;
	string cmd;
	UCHAR isPrompt = true;

	//ask user prompt or not
	cout << "Do you want open prompt for each file?(y-yes, else-no) ";
	cin >> cmd;
	cin.sync();
	if (cmd != "y" && cmd != "yes")
		isPrompt = false;

	while (!split_path.eof()) {
		getline(split_path, path_each, ' ');

		CallbackInfo cb;
		cb.mainFTP = this;
		cb.path = path_each;
		openDataPort(GetFileList::before, GetFileList::after, cb);

		stringstream forSplitFile(cb.path);

		while (!forSplitFile.eof()) {
			getline(forSplitFile, filename, '\r');
			if (filename[0] == '\n') filename.erase(0, 1);
			if (filename == "") break;

			if (!isPrompt) {
				this->downFile(filename);
				continue;
			}

			cout << "Get " << filename << "?(y-yes/else-no): ";
			cin.sync(); //flush \n
			getline(cin, cmd);
			if (cmd == "y" || cmd == "yes") {
				this->downFile(filename);
			}
		}
	}

	cout << endl;
}
 

void Eptipi::showAllCmd() {
	for (auto cmd : FTPCommand::listCmd) {
		cout << '\t' << setw(20) << left 
			<< cmd.first
			<< " : " 
			<< cmd.second.title << endl;
	}
	cout << endl;
}


void Eptipi::showHelpFor(string cmd) {
	auto info = FTPCommand::listCmd.find(cmd);
	
	if (info == FTPCommand::listCmd.end()) {
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

void Eptipi::xoaFile(string filename)
{
	this->sendCmd("DELE " + filename + "\r\n"); //ascii mode
	this->receiveStatus();
	cout << this->getReturnStr() << endl;
}

void Eptipi::xoaFolder(string tenfolder)
{
	this->sendCmd("XRMD " + tenfolder + "\r\n"); //ascii mode
	this->receiveStatus();
	cout << this->getReturnStr() << endl;
}

void Eptipi::taoFolder(string tenfolder)
{
	this->sendCmd("XMKD " + tenfolder + "\r\n"); //ascii mode
	this->receiveStatus();
	cout << this->getReturnStr() << endl;
}
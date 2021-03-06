/**
 * Source nay de define cac ham xu ly lenh ftp
 * 
 * Created by BeoHoang
*/

#pragma once
#include<Windows.h>
#include "stdafx.h"
#include "eptipi.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

#include "ProgressBar.h"
#include<stdio.h>
#include<string>
#include<iostream>
using namespace std;


/*------------------------------------------
	dir
	@liet ke chi tiet thu muc tren server
*/
void Eptipi::lietKeChiTiet(string path)
{
	vector<string> dirList = this->getLIST(path);
	for (string &name : dirList)
	{
		cout << name << endl;
	}

	cout << endl;
}

/*------------------------------------------
	ls
	@liet ke ten cac thu muc va file tren server
*/
void Eptipi::lietKeDonGian(string path)
{
	vector<string> nameList = this->getNLST(path);
	for (string &name : nameList)
	{
		cout << name << endl;
	}

	cout << endl;
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
	pwd
*/
void Eptipi::printServerPath()
{
	this->sendCmd("PWD\r\n"); //ascii mode
	this->receiveStatus();
	cout << this->getReturnStr() << endl;
}
 
/*------------------------------------------
	lpwd
*/
void Eptipi::printClientPath()
{

}
 

/*------------------------------------------
	put
*/
void Eptipi::upFile(string fileName)
{
	struct ASD {
		
		static bool before(CallbackInfo&cb)
		{
			cb.mainFTP->sendCmd("STOR "+cb.path+"\r\n");
			cb.mainFTP->receiveStatus();
			cout << '\t' << cb.mainFTP->getReturnStr() << endl;

			if (cb.mainFTP->getCode() != FTPCode::READY_TRANSFER
				&& cb.mainFTP->getCode() != FTPCode::DATA_ALREADY_OPEN)
				return false;
			return true;
		};

		static void after(CallbackInfo&cb)
		{
			if (cb.dataCon == NULL)
				return;
			int bytes = 0;
			char buffer[BUFFER_LENGTH];
			memset(buffer, 0, BUFFER_LENGTH);
			UINT64 filesize = 0;

			//create new file
			ifstream fileinp(cb.path, ios::binary);
			if (!fileinp.is_open()) 
			{
				cout << "\tError: cannot open file\n\n";
				return;
			}
			fileinp.seekg(0, ios::end);
			cb.filesize = fileinp.tellg();//total size
			fileinp.seekg(0, ios::beg);


			ProgressBar display;
			display.setBarSize(20);
			display.setTotalSize(cb.filesize);

			while (!fileinp.eof()) 
			{
				//read data to file
				fileinp.read(buffer, BUFFER_LENGTH);
				bytes = cb.dataCon->Send(buffer, fileinp.gcount());
				memset(buffer, 0, BUFFER_LENGTH);
				filesize += fileinp.gcount();
				display.updateAndDraw(cout, filesize);
			}
			//finally
			display.update(cb.filesize);
			if (cb.filesize > 0) display.draw(cout);

			cout << endl;
			cout << "\tUpload " << cb.path << " successfully\n";
			cout << "\tLength: " << filesize << "\n\n";
			fileinp.close();
		}
	};
	 
	CallbackInfo callbackparam;
	callbackparam.path = fileName;
	callbackparam.mainFTP = this;
	ifstream fileinp(fileName, ios::binary);
	if (!fileinp.is_open())
	{
		cout <<"\t"+fileName+ ": File not found\n\n";
		return;
	}
	else
		fileinp.close();

	openDataPort(ASD::before, ASD::after, callbackparam);
}
 
/*------------------------------------------
	get
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

			//switch to binary mode
			cb.mainFTP->sendCmd("TYPE I\r\n");
			cb.mainFTP->receiveStatus();


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
	mput
*/
void Eptipi::upNhieuFile(string fileNames)
{
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

	while (!split_path.eof()) 
	{
		getline(split_path, path_each, ' ');

		char buf[100];
		GetCurrentDirectoryA(100,buf);
		string sDir = buf;
		WIN32_FIND_DATAA fdFile;
		HANDLE hFind = NULL;
		string sPath;
		sPath = sDir + "\\" + path_each;
		if ((hFind = FindFirstFileA(sPath.c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
		{
			cout <<"\t"<<path_each <<": File not found" << endl;
			continue;
		}

		do
		{
			if (strcmp(fdFile.cFileName, ".") != 0
				&& strcmp(fdFile.cFileName, "..") != 0)
			{
				sPath = sDir + "\\" + fdFile.cFileName;
				if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
				{
				}
				else{
					filename = sPath.substr(sDir.length() + 1, sPath.length());
					if (!isPrompt)
					{
						this->upFile(filename);
						continue;
					}
					cout << "Put " << filename << "?(y-yes/else-no): ";
					cin.sync(); //flush \n
					getline(cin, cmd);
					if (cmd == "y" || cmd == "yes")
					{
						this->upFile(filename);
					}
				}
			}
		} while (FindNextFileA(hFind, &fdFile)); //Find the next file.

		FindClose(hFind); //Always, Always, clean things up!
	}

	cout << endl;
}
 
/*------------------------------------------
	mget
*/


void Eptipi::downNhieuFile(string fileNames)
{
	vector<string> nameList;
	vector<string> dirList;
	string cmd;
	UCHAR isPrompt = true;

	stringstream split_path(fileNames);
	string path_each;

	while (!split_path.eof())
	{
		getline(split_path, path_each, ' ');

		vector<string> nameListEach = this->getNLST(path_each);
		vector<string> dirListEach = this->getLIST(path_each);

		nameList.insert(nameList.end(), nameListEach.begin(), nameListEach.end());
		dirList.insert(dirList.end(), dirListEach.begin(), dirListEach.end());
	}	
	// nameList & dirList must be same size
	if (nameList.size() != dirList.size())
	{
		cout << "Something went wrong" << endl;
		return;
	}
	if (nameList.size() == 0) return;
	
	//ask user prompt or not
	cout << "Do you want open prompt for each file?(y-yes, else-no) ";
	cin >> cmd;
	cin.sync();
	if (cmd != "y" && cmd != "yes")
		isPrompt = false;

	for (size_t i = 0, length = nameList.size(); i < length; ++i)
	{
		if (dirList[i][0] != 'd')
		{
			//is not directory
			//maybe is file...
			if (isPrompt)
			{
				cout << "Get " << nameList[i] << " (y-yes,else-no)? ";
				getline(cin, cmd);
				cin.sync();

				trimPath(cmd);

				if (cmd != "yes" && cmd != "y")
					continue;
			}
			
			this->downFile(nameList[i]);
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

void Eptipi::xoaNhieuFile(string fileNames)
{
	struct DelFileList {
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
	cout << "Do you want to ask for delete each file(y-yes, else-no)?";
	cin >> cmd;
	cin.sync();
	if (cmd != "y" && cmd != "yes")
		isPrompt = false;

	while (!split_path.eof()) {
		getline(split_path, path_each, ' ');

		CallbackInfo cb;
		cb.mainFTP = this;
		cb.path = path_each;
		openDataPort(DelFileList::before, DelFileList::after, cb);

		stringstream forSplitFile(cb.path);

		while (!forSplitFile.eof()) {
			getline(forSplitFile, filename, '\r');
			if (filename[0] == '\n') filename.erase(0, 1);
			if (filename == "") break;

			if (!isPrompt) {
				this->xoaFile(filename);
				continue;
			}

			cout << "Del " << filename << "?(y-yes/else-no): ";
			cin.sync(); //flush \n
			getline(cin, cmd);
			if (cmd == "y" || cmd == "yes") {
				this->xoaFile(filename);
			}
		}
	}

	cout << endl;
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
/**
 * 
 * File Header de khai bao class Epitpi va cac constant lien quan
 * 
 * Created by BeoHoang
*/

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "resource.h"
#include <afxsock.h>
#include <iostream>
#include <vector>

#include "EptipiCommand.h"
#include "EptipiConstants.h"

#define BUFFER_LENGTH 512

using namespace std;

class Eptipi {
private:
	wstring server_addr;
	string client_addr;

	CSocket cmdConn; // command connection
	int returnCode;
	string returnStr;
	int returnPort;
	UCHAR isConnect;

	UCHAR dataMode;
	UCHAR fileMode;

protected:
	struct CallbackInfo {
		std::string path = "";
		Eptipi * mainFTP = NULL;
		CSocket * dataCon = NULL;
		UINT64 filesize = 0;
	};

	CSocket * openActivePortAndConnect();
	CSocket * openPassivePortAndConnect();
	void openDataPort(bool(*)(CallbackInfo&), void(*)(CallbackInfo&), CallbackInfo&);
	bool receiveAll();
	bool receiveOneLine();

public:
	// ham co ban
	void sendCmd(string cmd);
	void sendCmd(wstring cmd);
	void receiveStatus();

	//get method
	int getCode();
	int getReturnPort();
	string getReturnStr();

	Eptipi();

	void connectServer(const wchar_t * serverAddr);
	bool login();
	void handleCmd(string cmd, string path);

	//main command
	void lietKeChiTiet(string path); // dir
	void lietKeDonGian(string path); // ls
	void lietKeClientChiTiet(string path); //ldir
	void lietKeClientDonGian(string path); //lls
	
	void changeServerDir(string path); // cd
	void changeClientDir(string path); // lcd
	
	void printServerPath(); // pwd
	void printClientPath(); // lpwd

	void upFile(string fileName); // put
	void downFile(string fileName); // get
	void upNhieuFile(string fileNames); // mput
	void downNhieuFile(string fileNames); // mget
	void xoaFile(string filename);
	void xoaNhieuFile(string fileNames);//mdel
	void taoFolder(string tenfolder);
	void xoaFolder(string tenfolder);

	void showAllCmd();
	void showHelpFor(string cmd);

	void switchToPassive();
	void switchToActive();
	void switchToBinary();
	void switchToAscii();


	// HAM HO TRO
	vector<string> getNLST(string path);
	vector<string> getLIST(string path);

	~Eptipi();
};


class l_exception : exception {
private:
	wstring msg;
public:
	l_exception() : exception() {};
	l_exception(const wchar_t* s) {
		msg = s;
	}
	l_exception(const wstring s) {
		msg = s;
	}

	virtual const wchar_t * what() {
		return msg.c_str();
	}
};

// khai bao cac mo ta cua cau lenh tai day
// define description of cmd here

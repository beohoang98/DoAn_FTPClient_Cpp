#pragma once

#include "resource.h"
#include <afxsock.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <conio.h>

enum FTPCode {
	CONNECT_SUCCESS = 220,

	LOGIN_SUCCESS = 230,
	LOGIN_FAILED = 530,

	OPEN_PASV_PORT = 227,
	OPEN_LPSV_PORT = 228,
	OPEN_ESPV_PORT = 229,

	OPEN_ACTIVE_PORT_SUCCESS = 200,
	CANNOT_OPEN_DATA_CONNECT = 425,

	READY_TRANSFER = 150,

	TRANSFER_SUCCESS = 226,

	CONNECT_FAILED = 421
};

struct EptipiCallbackParam {
	std::string path;
	CSocket * cmdCon;
	CSocket * dataCon;
};

#define BUFFER_LENGTH 512

using namespace std;

class Eptipi {
// class nay de thuc hien cac cac giao thuc ftp den server 
private:
	const wchar_t * servername;

	CSocket cmdConn; // command connection
	int returnCode;
	string returnStr;
	int returnPort;

protected:
	void sendCmd(string cmd);
	void sendCmd(wstring cmd);
	bool receive();

	int getCode();
	int getReturnPort();
	string getReturnStr();

	CSocket * openActivePortAndConnect();
	CSocket * openPassivePortAndConnect();
	void openDataPort(bool(*)(EptipiCallbackParam), void(*)(EptipiCallbackParam), EptipiCallbackParam);

public:
	Eptipi();

	void connectServer(const wchar_t * serverAddr);

	bool login();

	void handleCmd(string cmd, string path);

	//main command
	void lietKeChiTiet(); // dir
	void lietKeDonGian(); // ls
	
	void changeServerDir(string path); // cd
	void changeClientDir(string path); // lcd
	
	void printServerPath(); // pwd
	void printClientPath(); // lpwd

	void upFile(string fileName); // put
	void downFile(string fileName); // get
	void upNhieuFile(string fileNames); // mput
	void downNhieuFile(string fileNames); // mget
	void xoaFile(string filename);
	
	void taoFolder(string tenfolder);
	void xoaFolder(string tenfolder);

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
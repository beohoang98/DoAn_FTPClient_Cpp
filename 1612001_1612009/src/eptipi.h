#pragma once

#include "resource.h"
#include <afxsock.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <map>

enum FTPCode {
	CONNECT_SUCCESS = 220,

	LOGIN_SUCCESS = 230,
	LOGIN_FAILED = 530,

	OPEN_PASV_PORT = 227,
	OPEN_LPSV_PORT = 228,
	OPEN_ESPV_PORT = 229,

	COMMAND_SUCCESS = 200,
	CANNOT_OPEN_DATA_CONNECT = 425,

	READY_TRANSFER = 150,

	TRANSFER_SUCCESS = 226,

	FILE_STATUS = 213,

	CONNECT_FAILED = 421
};


#define BUFFER_LENGTH 512

using namespace std;

class Eptipi {
private:
	const wchar_t * servername;

	CSocket cmdConn; // command connection
	int returnCode;
	string returnStr;
	int returnPort;

protected:
	struct CallbackInfo {
		std::string path = "";
		Eptipi * mainFTP = NULL;
		CSocket * dataCon = NULL;
		int filesize = 0;
	};

	CSocket * openActivePortAndConnect();
	CSocket * openPassivePortAndConnect();
	void openDataPort(bool(*)(CallbackInfo&), void(*)(CallbackInfo&), CallbackInfo&);

public:
	// ham co ban
	void sendCmd(string cmd);
	void sendCmd(wstring cmd);
	bool receiveAll();
	bool receiveOneLine();

	int getCode();
	int getReturnPort();
	string getReturnStr();

	Eptipi();

	void connectServer(const wchar_t * serverAddr);
	bool login();
	void handleCmd(string cmd, string path);

	//main command
	void lietKeChiTiet(); // dir
	void lietKeDonGian(); // ls
	void lietKeClientChiTiet(); //ldir
	void lietKeClientDonGian(); //lls
	
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

	void showAllCmd();

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

const map<string, string> listCmd = {
	{ "dir", "liet ke chi tiet thu muc tren server" },
	{ "ls", "liet ke ten cac thu muc tren server" },
	{ "ldir", "liet ke chi tiet thu muc client" },
	{ "lls", "liet ke ten thu muc client" },
	{ "cd", "thay doi duong dan tren server" },
	{ "lcd", "thay doi duong dan o client" },
	{ "pwd", "in ra duong dan hien tai tren server" },
	{ "get [ten file]", "download file [ten file] ve client" },
	{ "mget [expr]", "download nhieu file thoa man [expr]" },
	{ "put [asd]", "upload file [asd] len server" },
	{ "mput", "upload nhieu file" },
	{ "quit", "thoat ftp" }
};
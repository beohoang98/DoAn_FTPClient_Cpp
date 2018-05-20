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

	CWD_SUCCESS = 250,
	CWD_FAILED = 550,

	FILE_STATUS = 213,

	DISCONNECT = 421
};

namespace FTPDataMode {
	const UCHAR PASSIVE = 0;
	const UCHAR ACTIVE = 1;
	const UCHAR DEFAULT = PASSIVE;
}
namespace FTPFileMode {
	const UCHAR BINARY = 0;
	const UCHAR ASCII = 1;
	const UCHAR DEFAULT = BINARY;
}

#define BUFFER_LENGTH 512

using namespace std;

class Eptipi {
private:
	wstring servername;

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
		INT64 filesize = 0;
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
	
	void taoFolder(string tenfolder);
	void xoaFolder(string tenfolder);

	void showAllCmd();
	void showHelpFor(string cmd);

	void switchToPassive();
	void switchToActive();
	void switchToBinary();
	void switchToAscii();

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
struct cmdDescription {
	string title, description;
};
const map<string, cmdDescription> listCmd = {
	{
		"open",
		cmdDescription{
			"Ket noi server",
			
			"open [server name] - ket noi den dia chi [server name]"
		}
	},
	{
		"help",
		cmdDescription{
			"Hien thi thong tin cua ham",

			"help - thong tin tat ca ca ham\n"
			"help [cmd] - thong tin cho ham [cmd]"
		}
	},
	{
		"dir",
		cmdDescription{
			"liet ke chi tiet thu muc tren server",
			""
		}
	},
	{
		"ls",
		cmdDescription{
			"liet ke ten cac thu muc tren server",
			""
		}
	},
	{
		"ldir",
		cmdDescription{
			"liet ke chi tiet thu muc client",
			""
		}
	},
	{
		"lls",
		cmdDescription{
			"liet ke ten thu muc client",
			""
		}
	},
	{
		"cd",
		cmdDescription{
			"thay doi duong dan tren server",
			"\tcd [duong dan] de thay doi working dir den [duong dan]"
		}
	},
	{
		"lcd",
		cmdDescription{
			"thay doi duong dan o client",
			"\tlcd [duong dan] de thay doi working dir den [duong dan]"
		}
	},
	{
		"pwd",
		cmdDescription{
			"in ra duong dan hien tai tren server",
			""
		}
	},
	{
		"get",
		cmdDescription{
			"download file ve client",

			"\tget [ten file] - de download file [ten file] tren )\n"
			"\t\tserver ve path hien tai cua client"
		}
	},
	{
		"mget",
		cmdDescription{
			"mget [expr] - download nhieu file thoa man [expr]",
			
			"\t[expr] co the la:\n"
			"\t- *.txt\n"
			"\t- folder/*.*\n"
			"\t..."
		}
	},
	{
		"put",
		cmdDescription{
			"put [asd] - upload file [asd] len server",
			""
		}
	},
	{
		"mput",
		cmdDescription{
			"upload nhieu file",
			""
		}
	},
	{
		"mode",
		cmdDescription{
			"set mode to BINARY or ASCII",
			
			"\tA - ASCII mode\n"
			"\tI - BINARY mode"
		}
	},
	{ 
		"quit", 
		cmdDescription{
			"thoat ftp",
			""
		}
	},
	{
		"del",
		cmdDescription{
			"Xoa file tren server",
			
			"\tmdel [ten-file] - xoa file [ten-file]"
		}
	},
	{
		"mdel",
		cmdDescription{
			"Xoa nhieu file tren server",

			"\tmdel [expr...] - xoa file thoa man [expr...]\n"
			"\t- mdel *.txt *.asd - xoa het file .txt va .asd\n"
			"\t- ..."
		}
	},
	{
		"rmdir",
		cmdDescription{
			"Xoa thu muc trong",

			"\trmdir [tenfolder] - Xoa thu muc [tenfolder]\n"
			"\t- Neu thu muc khong rong, khong xoa dc"
		}
	},
	{
		"mkdir",
		cmdDescription{
			"Tao thu muc moi",

			"\tmkdir [tenfolder] - Tao thu muc moi co ten [tenfolder]"
		}
	}
};
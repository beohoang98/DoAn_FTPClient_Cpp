#pragma once

#include "resource.h"
#include <afxsock.h>
#include <iostream>

#define BUFFER_LENGTH 512

const enum FTPCODE {
	SUCCESS = 220,
	EXIT = 400
};

using namespace std;

class Eptipi {
// class nay de thuc hien cac cac giao thuc ftp den server 
private:
	CSocket * client;
	char buffer[BUFFER_LENGTH];

public:
	Eptipi();

	void connectServer(const wchar_t * serverAddr);

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
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
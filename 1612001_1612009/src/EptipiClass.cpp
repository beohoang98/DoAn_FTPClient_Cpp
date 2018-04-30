#include "stdafx.h"
#include "eptipi.h"

using namespace std;

Eptipi::Eptipi() {
	client = NULL;
}

void Eptipi::connectServer(const wchar_t * serverAddr) {
	client = new CSocket();
	client->Create();

	bool isConnect = client->Connect(serverAddr, 21);

	if (!isConnect) {
		wstring s = wstring(L"Khong the ket noi den server ") + serverAddr;
		throw l_exception(s.c_str());
	}
		

	client->Receive(buffer, BUFFER_LENGTH, 0);
	cout << buffer << endl;
}

Eptipi::~Eptipi() {
	free(client);
}
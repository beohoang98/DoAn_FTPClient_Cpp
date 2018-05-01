#pragma once

#include "stdafx.h"
#include "eptipi.h"


/*------------------------------------------
	dir
	@liet ke chi tiet thu muc tren server
*/
void Eptipi::lietKeChiTiet()
{
	CSocket * transferPort = openTransferPort();
	if (transferPort != NULL) {
		this->sendCmd("LIST\r\n");
		this->receive();
		cout << '\t' << getReturnStr() << endl;

		char buffer[BUFFER_LENGTH];
		memset(buffer, 0, BUFFER_LENGTH);
		while (transferPort->Receive(buffer, BUFFER_LENGTH - 1) > 0) {
			cout << buffer;
			memset(buffer, 0, BUFFER_LENGTH);
		}

		transferPort->Close();
		free(transferPort);
	}

	this->receive();
	cout << '\t' << getReturnStr() << endl;
	//425 Cannot open data connect
}

/*------------------------------------------
	ls
	@liet ke ten cac thu muc va file tren server
*/
void Eptipi::lietKeDonGian()
{
	CSocket * transferPort = openActivePortAndConnect();
	CSocket server;
	CSocket * tmp;

	if (transferPort != NULL) {
		this->sendCmd("NLST\r\n");
		this->receive();
		cout << '\t' << getReturnStr() << endl;

		transferPort->Listen(1);
		transferPort->Accept(server);

		char buffer[BUFFER_LENGTH];
		memset(buffer, 0, BUFFER_LENGTH);
		while (server.Receive(buffer, BUFFER_LENGTH - 1) > 0) {
			cout << buffer;
			memset(buffer, 0, BUFFER_LENGTH);
		}

		transferPort->Close();
		free(transferPort);
	}

	this->receive();
	cout << '\t' << getReturnStr() << endl;
	//425 Cannot open data connect
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

}
 
#include "stdafx.h"
#include "eptipi.h"
#include <algorithm>

using namespace std;

/* ======================================================= */

Eptipi::Eptipi() {
}

/*----------------------------------------------------
	connect to server through port 21
*/
void Eptipi::connectServer(const wchar_t * serverAddr) 
{
	cmdConn.Create();

	bool isConnect = cmdConn.Connect(serverAddr, 21);

	if (!isConnect) {
		wstring s = wstring(L"Khong the ket noi den server ") + serverAddr;
		throw l_exception(s.c_str());
	}
	this->servername = serverAddr;
		
	char buffer[BUFFER_LENGTH];
	memset(buffer, 0, BUFFER_LENGTH);
	cmdConn.Receive(buffer, BUFFER_LENGTH-1, 0);
	cout << buffer << endl;
}

/*--------------------------------------------------
	read password
*/
void readPassword(string& pass) {
	bool isEnter = false;
	pass = "";

	while (!isEnter) {
		if (_kbhit()) {
			char c = _getch();

			if (c == '\n' || c == '\r') {
				isEnter = true;
				break;
			}
			else if (c == '\b') {
				cout << "\b \b";
				if (pass.length() > 0)
					pass.pop_back();
			}
			else {
				cout << "*";
				pass.push_back(c);
			}
		}
	}
}

/*--------------------------------------------------
	login to server
*/
bool Eptipi::login() 
{
	string name, pass;

	//username
	cout << "username: " << endl;
	cin.sync();
	getline(cin, name);

	this->sendCmd("USER " + name + "\r\n");
	this->receive();
	cout << "\t" << getReturnStr() << endl;

	//password
	cout << "password: " << endl;
	readPassword(pass);

	this->sendCmd("PASS " + pass + "\r\n");
	this->receive();
	cout << "\t" << getReturnStr() << endl;

	if (getCode() == FTPCode::LOGIN_SUCCESS) {
		cout << "\tLogin success\n\n";
		return true;
	}
	else {
		cout << "\tLogin failed\n\n";
		return false;
	}
}


/* PROTECTED FUNC ========================================= */

/*------------------------------------------------------------
	send raw command to ftp server
*/
void Eptipi::sendCmd(string cmd) {
	this->cmdConn.Send(cmd.c_str(), cmd.length());
}

/*------------------------------------------------------------
	send raw command to ftp server
*/
void Eptipi::sendCmd(wstring cmd) {
	this->cmdConn.Send(cmd.c_str(), cmd.size());
}

/*--------------------------------------------------------------
	receive string return by server and fetch code and port
	(can than) neu dung khong dung luc server tra ve, chuong trinh se bi lap vo tan
*/
bool Eptipi::receive() {
	stringstream ss; // for split return string
	string unusedStr;

	char buffer[BUFFER_LENGTH];
	memset(buffer, 0, BUFFER_LENGTH);

	int bytes = this->cmdConn.Receive(buffer, BUFFER_LENGTH - 1);
	if (bytes <= 0) {
		//disconnect to server
		return false;
	}

	this->returnStr = buffer;

	//split code
	ss = stringstream(buffer);
	ss >> this->returnCode;
	//split string
	getline(ss, unusedStr, '(');
	//split return port
	if (this->returnCode == FTPCode::OPEN_PASV_PORT) {
		//227 Entering Passive Mode (h1,h2,h3,h4,p1,p0)
		int unusedInt, p1, p0;
		char comma;
		ss >> unusedInt >> comma >> unusedInt >> comma
			>> unusedInt >> comma >> unusedInt >> comma
			>> p1 >> comma >> p0;

		if (ss.fail())
			this->returnPort = -1;
		else
			this->returnPort = p1 * 256 + p0;
	}
	else if (this->returnCode == FTPCode::OPEN_ESPV_PORT) {
		// 227 Entering Extented Passive Mode (address, port)
		string tmp;
		ss >> tmp;
		ss >> this->returnPort;
	}
	else if (this->returnCode == FTPCode::OPEN_LPSV_PORT) {
		// 227 Entering Long Passive Mode (|||port|)
		ss >> this->returnPort;
	}
	else if (this->returnCode == FTPCode::OPEN_ACTIVE_PORT_SUCCESS) {
		// Chu dong mo port nen da co san port
	}
	else {
		//khong co port tra ve
	}

	return true;
}

/*---------------------------------------------------
	get return code after call Eptipi::receive
*/
unsigned short Eptipi::getCode()
{
	return this->returnCode;
}

/*-------------------------------------------------------
	get return port after call Eptipi::receive
	@return -1 if do not any port return
*/
unsigned short Eptipi::getReturnPort()
{
	return this->returnPort;
}

/*-----------------------------------------------------
	get return string after call Eptipi::receive
*/
string Eptipi::getReturnStr() {
	return this->returnStr;
}


/*----------------------------------------------------------
	open active port and send connect request to server
	@return: pointer of CSocket that connected
	@return: NULL when cannot open port
*/
CSocket * Eptipi::openActivePortAndConnect() {
	CSocket * newSocket = new CSocket();
	CString hostname_buffer;
	UINT port;
	short p1, p0; // split port
	stringstream request;

	if (!newSocket->Create()) {
		free(newSocket);
		return NULL;
	}

	newSocket->GetSockName(hostname_buffer, port);
	p1 = port/256;
	p0 = port%256;

	if (!newSocket->Listen(1)) {
		free(newSocket);
		return NULL;
	}

	request << "PORT 127,0,0,1," << p1 << "," << p0 << "\r\n";
	this->sendCmd(request.str());
	this->receive();

	if (this->getCode() != FTPCode::OPEN_ACTIVE_PORT_SUCCESS) {
		//server khong ket noi duoc port cua minh
		free(newSocket);
		return NULL;
	}

	return newSocket;
}

/*-------------------------------------------------------------
	request server open port and connect to it
	@return: CSocket* of the connected socket
	@return: NULL if cannot connect
*/
CSocket * Eptipi::openPassivePortAndConnect() {
	CSocket * newSocket = NULL;
	
	//try normal PASV
	this->sendCmd("PASV\r\n");
	this->receive();
	if (this->getReturnPort() == -1) {
		//try EPSV
		this->sendCmd("ESPV\r\n");
		this->receive();

		if (this->getReturnPort() == -1) {
			//try LPSV
			this->sendCmd("LSPV\r\n");
			this->receive();
		}
	}

	if (this->getReturnPort() == -1) {
		//neu thu 3 cach khong dc thi tra ve NULL
		return NULL;
	}

	newSocket = new CSocket();
	if (!newSocket->Create()) {
		return NULL;
	}
	if (!newSocket->Connect(this->servername, (UINT)this->getReturnPort())) {
		return NULL;
	}

	return newSocket;
}

/*------------------------------------------------------------
	open transfer port
	@return (NULL) if cannot open any port in active and pasv mode
	@return (CSocket *) if have one mode connected
*/
CSocket * Eptipi::openTransferPort() {
	// try passive
	CSocket * transferPort = openPassivePortAndConnect();

	if (transferPort == NULL) {
		// try active
		transferPort = openActivePortAndConnect();
		if (transferPort == NULL) {
			// cannot connect
			return NULL;
		}
	}
	
	//this->receive();
	//cout << "\t" << getReturnStr() << endl;
	//150 Open ....

	return transferPort;
}

/* MAIN FUNC ============================================== */

/*------------------------------------------------------
	handle user command
	@param (cmd) user command
	@param (path) path or filename afer user command
*/
void Eptipi::handleCmd(string cmd, string path) 
{
	if (cmd == "dir")
	{ // liet ke chi tiet folder va file
		this->lietKeChiTiet();
	}
	else if (cmd == "ls")
	{// liet ke ten folder va file
		this->lietKeDonGian();
	}
	else if (cmd == "cd")
	{// thay doi duong dan tren server
		this->changeServerDir(path);
	}
	else if (cmd == "lcd")
	{// thay doi duong dan tai client
		this->changeClientDir(path);
	}
	else if (cmd == "pwd")
	{// in ra duong dan tren server
		this->printServerPath();
	}
	else if (cmd == "lpwd")
	{// in ra duong dan tai client
		this->printClientPath();
	}
	else if (cmd == "put")
	{// up file len server
		this->upFile(path);
	}
	else if (cmd == "mput")
	{// up nhieu file len server
		this->upNhieuFile(path);
	}
	else if (cmd == "get")
	{// down file ve client
		this->downFile(path);
	}
	else if (cmd == "mget")
	{// up nhieu file len server
		this->downNhieuFile(path);
	}
	else if (cmd == "quit") 
	{
		this->sendCmd("QUIT\r\n");
		this->receive();
		cout << this->getReturnStr() << endl;
	}
	else {
		cout << "\tunknown command\n\n";
	}
}


/*-----------------------------------------------
*/
Eptipi::~Eptipi() {
	sendCmd("QUIT\r\n");
	cmdConn.Close();
}
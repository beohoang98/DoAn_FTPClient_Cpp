#include "stdafx.h"
#include "eptipi.h"
#include <algorithm>

using namespace std;

/* ======================================================= */

Eptipi::Eptipi() {
	this->returnCode = -1;
	this->returnPort = -1;
	this->returnStr = "";

	this->dataMode = FTPDataMode::DEFAULT;//passive
	this->fileMode = FTPFileMode::DEFAULT;//binary
}

/*----------------------------------------------------
	connect to server through port 21
*/
void Eptipi::connectServer(const wchar_t * serverAddr) 
{
	cmdConn.Create();

	int isConnect = cmdConn.Connect(serverAddr, 21);

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
	this->receiveOneLine();
	cout << "\t" << getReturnStr() << endl;

	//password
	cout << "password: " << endl;
	readPassword(pass);

	this->sendCmd("PASS " + pass + "\r\n");
	this->receiveOneLine();
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
bool Eptipi::receiveAll() {
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
	this->returnCode = -1;
	this->returnPort = -1;

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

	return true;
}

bool Eptipi::receiveOneLine() {
	stringstream ss; // for split return string
	string unusedStr;

	char buffer[BUFFER_LENGTH];
	memset(buffer, 0, BUFFER_LENGTH);
	char * pos = buffer;

	while (cmdConn.Receive(pos, 1)) {
		char c = pos[0];
		pos++;

		if (c == '\n') {
			break;
		}
	}
	if (pos == buffer) {
		//disconnect to server
		return false;
	}

	this->returnStr = buffer;
	this->returnCode = -1;
	this->returnPort = -1;

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

	return true;
}


/*---------------------------------------------------
	get return code after call Eptipi::receive
*/
int Eptipi::getCode()
{
	return this->returnCode;
}

/*-------------------------------------------------------
	get return port after call Eptipi::receive
	@return -1 if do not any port return
*/
int Eptipi::getReturnPort()
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
	this->receiveOneLine();

	if (this->getCode() != FTPCode::COMMAND_SUCCESS) {
		//server khong chap nhan port cua minh
		free(newSocket);
		return NULL;
	}
	if (!newSocket->Listen(1)) {
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
	this->receiveOneLine();
	if (this->getReturnPort() == -1) {
		//try EPSV
		this->sendCmd("ESPV\r\n");
		this->receiveOneLine();

		if (this->getReturnPort() == -1) {
			//try LPSV
			this->sendCmd("LSPV\r\n");
			this->receiveOneLine();
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
	@param (beforeConnect) ham callback thuc hien truoc khi server va client ket noi data port
	@param (afterConnect) ham callback thuc hien sau khi server va client ket noi data port
*/
void Eptipi::openDataPort(bool (*beforeConnect)(CallbackInfo&), void (*afterConnect)(CallbackInfo&), CallbackInfo & cb) {
	CSocket * transferPort = NULL;
	bool isResponseOK = true;

	if (this->dataMode == FTPDataMode::ACTIVE) {
		// try active
		transferPort = openActivePortAndConnect();
		CSocket server;

		if (transferPort != NULL) {
			isResponseOK = beforeConnect(cb);
			
			if (!transferPort->Accept(server, (SOCKADDR*)this->servername))
			{
				afterConnect(cb);
			}
			else 
			{
				cb.dataCon = &server;
				
				if (isResponseOK) afterConnect(cb);
				
				server.Close();
			}
		}
	}
	else {
		//passive method
		transferPort = openPassivePortAndConnect();
		cb.dataCon = transferPort;

		isResponseOK = beforeConnect(cb);
		if(isResponseOK) 
			afterConnect(cb);
	}

	transferPort->Close();
	delete transferPort;

	if (isResponseOK) {
		this->receiveOneLine();
		cout << "\t" << getReturnStr() << endl;
	}
}

/* MAIN FUNC ============================================== */

/*------------------------------------------------------
	handle user command
	@param (cmd) user command
	@param (path) path or filename afer user command
*/
void Eptipi::handleCmd(string cmd, string path) 
{
	if (path == "--help") {
		this->showHelpFor(cmd);
		return;
	}
	
	if (cmd == "dir")
	{ // liet ke chi tiet folder va file tren server
		this->lietKeChiTiet(path);
	}
	else if (cmd == "ls")
	{// liet ke ten folder va file tren server
		this->lietKeDonGian(path);
	}
	else if (cmd == "ldir")
	{ // liet ke chi tiet folder va file o Client
		this->lietKeClientChiTiet(path);
	}
	else if (cmd == "lls")
	{// liet ke ten folder va file o Client
		this->lietKeClientDonGian(path);
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
	else if (cmd == "type")
	{
		if (path == "I" || path == "i")
			this->switchToBinary();
		else if (path == "A" || path == "a")
			this->switchToAscii();
		else if (path == "--help") {
			this->showHelpFor(cmd);
		}
		else {
			cout << "\tType khong hop le" << endl
				<< "\tType hop le bao gom: A - ASCII, I - BINARY"
				<< endl << endl;
		}
	}
	else if (cmd == "mode")
	{
		if (path == "A" || path == "a") {
			this->switchToActive();
		}
		else if (path == "P" || path == "p") {
			this->switchToPassive();
		}
		else {
			cout << "\tMode khong hop le" << endl;
			this->showHelpFor("mode");
		}
	}
	else if (cmd == "quit" || cmd == "bye" || cmd == "disconnect")
	{
		this->sendCmd("QUIT\r\n");
		this->receiveOneLine();
		cout << this->getReturnStr() << endl;
	}
	else if (cmd == "help") {
		if (path == "")
			this->showAllCmd();
		else
			this->showHelpFor(path);
	}
	else if (cmd == "") {
		//do nothing
	}
	else {
		cout << "\tunknown command, type help to show all command\n\n";
	}
}


/*-----------------------------------------------
*/
Eptipi::~Eptipi() {
	sendCmd("QUIT\r\n");
	cmdConn.Close();
}
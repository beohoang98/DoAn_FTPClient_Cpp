/**
 * Source de define cac ham co ban cua class Eptipi
 * 
 * Created by BeoHoang
*/


#include "stdafx.h"
#include "eptipi.h"
#include <algorithm>
#include <conio.h>
#include <sstream>

using namespace std;

/* ======================================================= */

Eptipi::Eptipi() {
	this->returnCode 	= -1;
	this->returnPort 	= -1;
	this->returnStr 	= "";
	this->isConnect 	= false;

	this->server_addr = L"";
	this->client_addr = "";

	this->dataMode 	= FTPDataMode::DEFAULT;//passive
	this->fileMode 	= FTPFileMode::DEFAULT;//binary
}

/*----------------------------------------------------
	connect to server through port 21
*/
void Eptipi::connectServer(const wchar_t * serverAddr) 
{
	cmdConn.Create();
	
	cout << "Wait..." << endl;
	this->isConnect = cmdConn.Connect(serverAddr, 21);
	
	if (!this->isConnect) {
		cout << "Khong the ket noi den ";
		wcout << serverAddr << endl;
		cmdConn.Close();
		return;
	}

	cout << "Connect OK..." << endl;
	this->server_addr.assign(&serverAddr[0], &serverAddr[wcslen(serverAddr)]);
		
	this->receiveStatus();
	cout << this->getReturnStr() << endl;

	this->sendCmd("OPTS UTF8 ON\r\n");
	this->receiveStatus();
	cout << this->getReturnStr() << endl;

	CString hostname_buf;
	UINT port; // unused
	cmdConn.GetSockName(hostname_buf, port);

	wstring whostname = hostname_buf;
	client_addr.assign(whostname.begin(), whostname.end());
	replace(client_addr.begin(), client_addr.end(), '.', ',');
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

/**
 * login to server
 */
bool Eptipi::login() 
{
	string name, pass;

	//username
	cout << "username: " << endl;
	cin.sync();
	getline(cin, name);

	this->sendCmd("USER " + name + "\r\n");
	this->receiveStatus();
	cout << "\t" << getReturnStr() << endl;

	//password
	cout << "password: " << endl;
	readPassword(pass);

	this->sendCmd("PASS " + pass + "\r\n");
	this->receiveStatus();
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

/**-------------------------------------------------------------
 * send raw command to ftp server
 * 
 * @param {wstring} cmd : Raw protocal command 
*/
void Eptipi::sendCmd(string cmd) {
	this->cmdConn.Send(cmd.c_str(), cmd.length());
}

/**-------------------------------------------------------------
 * send raw command to ftp server
 * 
 * @param {wstring} cmd : Raw protocal command 
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
	return true;
}

/** ----------------------------------------------------------------
 * Ham nhan 1 dong status tu server tra ve
 * Sau do loc cac thong tin nhu code, port, status
 * roi luu lai vao cac member
 * @return {true} neu van con status dang cho
 * @return {false} neu da nhan het
*/
bool Eptipi::receiveOneLine() {
	stringstream ss; // for split return string
	string unusedStr;
	int isContinueStatus = false;
	char charNextToCode = '\0';
	char buffer[BUFFER_LENGTH];
	char * pos = NULL;

	memset(buffer, 0, BUFFER_LENGTH);
	pos = &buffer[0];

	while (cmdConn.Receive(pos, 1)) {
		char c = *pos;

		if (c == '\n') {
			break;
		}

		pos++;
	}
	if (pos == buffer) {
		//disconnect to server
		return false;
	}

	this->returnStr += buffer;

	//split code
	ss = stringstream(buffer);
	ss >> this->returnCode;

	if (ss.fail()) {
		//cannot read code
		//co nghia la van con buffer chua doc het
		return true;
	}

	ss >> charNextToCode;
	isContinueStatus = (charNextToCode == '-');

	//split string
	getline(ss, unusedStr, '(');
	//split return port
	if (this->returnCode == FTPCode::OPEN_PASV_PORT) {
		//227 Entering Passive Mode (h1,h2,h3,h4,p1,p0)
		int h1, h2, h3, h4, p1, p0;
		char comma;
		ss >> h1 >> comma >> h2 >> comma
			>> h3 >> comma >> h4 >> comma
			>> p1 >> comma >> p0;

		if (ss.fail())
			this->returnPort = -1;
		else {
			this->returnPort = p1 * 256 + p0;
			this->server_addr = to_wstring(h1) + L"." 
				+ to_wstring(h2) + L"."
				+ to_wstring(h3) + L"."
				+ to_wstring(h4);
		}	
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
	else if (this->returnCode == FTPCode::DISCONNECT) {
		//disconnect
		cmdConn.Close();
		this->isConnect = false;
	}

	return isContinueStatus;
}

/**----------------------------------------------------------
 * Ham nhan 1 status, bao gom nhieu dong (neu co)
 * Update tu receiveOneLine()
*/
void Eptipi::receiveStatus()
{
	this->returnStr = "";
	this->returnPort = -1;
	this->returnCode = -1;

	//repeat until server return [code][space][...]
	while (receiveOneLine());
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


/**----------------------------------------------------------
 * open active port and send connect request to server
 * @return: pointer of CSocket that connected
 * @return: NULL when cannot open port
*/
CSocket * Eptipi::openActivePortAndConnect() {
	CSocket * newSocket = new CSocket();
	CString hostname_buffer;
	UINT port;
	short p1, p0; // split port
	stringstream request;

	if (!newSocket->Create()) {
		delete newSocket;
		return NULL;
	}

	newSocket->GetSockName(hostname_buffer, port);
	p1 = port/256;
	p0 = port%256;

	if (!newSocket->Listen(1)) {
		delete newSocket;
		return NULL;
	}

	request << "PORT " << client_addr << "," << p1 << "," << p0 << "\r\n";
	this->sendCmd(request.str());
	this->receiveStatus();

	if (this->getCode() != FTPCode::COMMAND_SUCCESS) {
		//server khong chap nhan port cua minh
		delete newSocket;
		return NULL;
	}
	if (!newSocket->Listen(1)) {
		delete newSocket;
		return NULL;
	}

	return newSocket;
}

/**--------------------------------------------------------
* request server open port and connect to it
* @return: CSocket* of the connected socket
* @return: NULL if cannot connect
*/
CSocket * Eptipi::openPassivePortAndConnect() {
	CSocket * newSocket = NULL;
	
	//try normal PASV
	this->sendCmd("PASV\r\n");
	this->receiveStatus();
	cout << '\t' << getReturnStr() << endl;

	if (this->getReturnPort() == -1) {
		//try EPSV
		this->sendCmd("EPSV\r\n");
		this->receiveStatus();
		cout << '\t' << getReturnStr() << endl;

		if (this->getReturnPort() == -1) {
			//try LPSV
			this->sendCmd("LPSV\r\n");
			this->receiveStatus();
			cout << '\t' << getReturnStr() << endl;
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
	if (!newSocket->Connect(this->server_addr.c_str(), (UINT)this->getReturnPort())) {
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
			
			if (!transferPort->Accept(server, (SOCKADDR*)this->server_addr.c_str()))
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

	if (transferPort != NULL) {
		transferPort->Close();
		delete transferPort;
	}
	
	if (isResponseOK) {
		this->receiveStatus();
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
	//check cmd exist
	if (FTPCommand::listCmd.find(cmd) == FTPCommand::listCmd.end()) {
		cout << "\tunknown command, type help to show all command\n\n";
		return;
	}

	//do cmd dont need connected
	if (cmd == "open") {
		if (path == "") {
			showHelpFor(cmd);
			return;
		}

		wstring wPath(path.begin(), path.end());
		this->connectServer(wPath.c_str());

		if (this->isConnect) {
			while (!this->login()) {
				cout << "\tDang nhap lai:\n" << endl;
			}
		}
	}
	else if (cmd == "quit" || cmd == "bye" || cmd == "disconnect")
	{
		if (isConnect) {
			this->sendCmd("QUIT\r\n");
			this->receiveStatus();
			cout << this->getReturnStr() << endl;
			
			this->cmdConn.Close();
			this->isConnect = false;
			this->server_addr.clear();
		}
		return;
	}
	else if (cmd == "help") {
		if (path == "")
			this->showAllCmd();
		else
			this->showHelpFor(path);
		return;
	}
	else if (cmd == "") {
		//do nothing
		return;
	}


	//check connect for commands need connection
	if (!this->isConnect) {
		cout << "\tBan chua ket noi den server\n"
			<< "\tGoi lenh open de ket noi den server\n"
			<< endl;
		return;
	}
	else if (cmd == "dir")
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
		if (path == "")
			showHelpFor(cmd);
		else
			this->upFile(path);
	}
	else if (cmd == "mput")
	{// up nhieu file len server
		if (path == "")
			showHelpFor(cmd);
		else
			this->upNhieuFile(path);
	}
	else if (cmd == "get")
	{// down file ve client
		if (path == "")
			showHelpFor(cmd);
		else
			this->downFile(path);
	}
	else if (cmd == "mget")
	{// up nhieu file len server
		if (path == "")
			showHelpFor(cmd);
		else
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
			showHelpFor(cmd);
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
			showHelpFor("mode");
		}
	}
	else if (cmd == "del")
	{
		this->xoaFile(path);
	}
	else if (cmd == "rmdir")
	{
		this->xoaFolder(path);
	}
	else if (cmd == "mkdir")
	{
		this->taoFolder(path);
	}
	else if (cmd == "pwd")
	{
		this->printServerPath();
	}
}


/*-----------------------------------------------
*/
Eptipi::~Eptipi() {
	sendCmd("QUIT\r\n");
	cmdConn.Close();
}
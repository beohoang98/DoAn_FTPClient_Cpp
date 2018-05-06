# CLASS EPTIPI
- [CLASS MEMBER](##CLASS_MEMBER)
- [BASIC FUNCTION USED FOR ALL](##BASIC_FUNC) 
- [MAIN FUNCTION](##MAIN_FUNCTION)
- [OPEN DATA PORT](##OPEN_DATA_PORT)
---
## CÁC HẰNG SỐ VÀ SUBCLASS
- **enum** FTPCode
	- Các hằng số mã trả về cơ bản của **ftp**
	```
	CONNECT_SUCCESS 			= 220,
	LOGIN_SUCCESS 				= 230,
	LOGIN_FAILED 				= 530,
	OPEN_PASV_PORT 				= 227,
	OPEN_LPSV_PORT 				= 228,
	OPEN_ESPV_PORT 				= 229,
	COMMAND_SUCCESS 			= 200,
	CANNOT_OPEN_DATA_CONNECT 		= 425,
	READY_TRANSFER 				= 150,
	TRANSFER_SUCCESS 			= 226,
	FILE_STATUS 				= 213,
	CONNECT_FAILED 				= 421
	```
- **BUFFER_LENGTH** = 512
	- Độ lớn mặc định của buffer dùng để đọc thông tin server gửi về
- **struct** CallbackInfo
	- Dùng để truyền dữ liệu cần thiết khi gọi [Eptipi::openDataPort](###openPort)
	```
	struct CallbackInfo {
		std::string path = "";
		Eptipi * mainFTP = NULL;
		CSocket * dataCon = NULL;
		int filesize = 0;
	};
	```
---
## CLASS MEMBER
- **CSocket** cmdConn
	- Sử dụng để kết nối đến control port của server (port 21)
	- Gửi lệnh và nhận phản hồi
- **const wchar_t** servername
	- Lưu lại tên server dùng sử dụng lại.
- **int** returnCode
	- Lưu lại mã phản hồi từ server (VD: 220, 530, ...)
- **std::string** returnStr
	- Lưu lại chuỗi phản hồi từ server (VD: 200 OK)
- **int** returnPort
	- Lưu lại thông tin port trả về từ server (nếu có)
	- -1 nếu không có thông tin port trả về.
---
## BASIC FUNC
- ### *public void* sendCmd(*std::string* cmd)
	Gửi chuỗi lệnh raw lên server thông qua **cmdConn**
	```
	cmdConn.Send(cmd.c_str(), cmd.length());
	```
- ### *public void* sendCmd(*std::wstring* cmd);
	Tương tự như trên
- ### *public void* receiveAll();
	Nhận các phản hồi trên server xuống rồi sau đó truyền vào **returnStr**
- ### *public void* receiveOneLine();
	Nhận 1 phản hồi (1 dòng có kí tự cuối là *\n*) từ server xuống và xử lý phản hồi đó thành **returnCode**, **returnStr** và **returnPort**(nếu có). **VD**:
	- **buffer** = 200 Command OK
		- returnCode = 220
		- returnStr = "200 Command OK"
		- returnPort = -1
	- **buffer** = 227 Entering Passive Port (127,0,0,1,25,5)
		- returnCode = 227
		- returnStr = *"227 Entering Passive Port (127,0,0,1,25,5)"*
		- returnPort = 25*256 + 5 = 6405
- ### *public int* getCode()
	Trả về **returnCode**
- ### *public std::string* getReturnStr()
	Trả về **returnStr**
- ### *public int* getReturnPort()
	Trả về **returnPort**
---
## MAIN FUNCTION

---
## OPEN DATA PORT
>(Để viết mấy cái kia cho nhanh)
- ### *protected CSocket* * openPassivePortAndConnect();
- ### *protected CSocket* * openActivePortAndConnect();
- ### *protected void* openDataPort(bool (*before)(*CallbackInfo*&), void (*after)(*CallbackInfo*&), *CallbackInfo*&);
---
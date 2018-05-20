# <center>BÁO CÁO ĐỒ ÁN</center>
<center>[Github](https://github.com/beohoang98/DoAn_MMT_FTPSocket/)</center>

## THÀNH VIÊN

Tên 			| MSSV		| Github
---				|---		|---
Hoàng Dân An 	| 1612001 	|[beohoang98](https://github.com/beohoang98)
Nguyễn Phước An | 1612009 	|[phuocantd](https://github.com/phuocantd)

## PHÂN CHIA CÔNG VIỆC

STT		|	CÔNG VIỆC						| Người đảm nhiệm
---:	|---								|---
1		|Thiết kế flow chương trình chính 	|Dân An
2		|Thiết kế OOP cho chương trình		|Dân An
3		|Tạo các hàm xử lý giao thức cơ bản	|Dân An
4		|Phân chia thư mục và tạo project	|Phước An
5		|Viết các hàm cd, lcd, pwd, mdelete, mkdir, rmdir, quit, exit | Phước An
6		|Viết hàm xử lý mở port, active, passive |Dân An
7		|Viết các hàm put, mput				|Phước An
8		|Viết các hàm get, mget				|Dân An
9		|Viết câu lệnh help và description cho các câu lệnh ftp | Cả hai
10		|Quản lý git cho mã nguồn			|Dân An
11		|Phụ trách document					|Cả hai
12		|Phụ trách ngoại giao (*copy ý tưởng team khác*) |Phước An
13		|Test lỗi và tìm bug				|Phước An

## ĐÁNH GIÁ MỨC ĐỘ HOÀN THÀNH

STT		|	Chức năng					| Hoàn thành
--:		|---							|---:
1		|Kết nối và login đến server	|1
2		|Xử lý đầy đủ các câu lệnh		|1
3		|Câu lệnh ls, dir				|1
4		|Câu lệnh cd, lcd, pwd, lpwd	|1
5		|put							|0
6		|mput							|0
7		|get							|1
8		|mget							|1
9		|delete							|0
10		|mdelete						|0
11		|mkdir							|0
12		|rmdir							|0
13		|Cơ chế passive và active		|1
|**Tổng**|**54%**

## BÁO CÁO CÁC HÀM VÀ CẤU TRÚC CHƯƠNG TRÌNH

### 1. CẤU TRÚC CHƯƠNG TRÌNH
	
1. Khởi tạo kết nối server với địa chỉ từ arguments, hoặc từ lệnh open
2. Gọi hàm login để người dùng login vào, lặp lại đến khi login thành công
3. Đọc lệnh từ bàn phím của người dùng
4. Xử lý lệnh người dùng và gọi hàm đã viết
5. Lặp về Bước 3

### 2. CẤU TRÚC CLASS EPTIPI

Dùng để xử lý các câu lệnh và các giao thức ftp nhanh hơn

Chứa thông tin socket kết nối đến server

Xử lý các hàm nhận vào từ người dùng (```'ls', 'dir', 'get'```, ...)

**CÁC HÀM CHÍNH**
- **bool** ```Eptipi::handleCmd(string cmd)```
	
	Xử lý chuỗi lệnh ```cmd``` truyền vào

- **void** `Eptipi::connect(const wchar_t *)`

	Kết nối đến chuỗi địa chỉ server truyền vào, nếu không throw exception ra

- **bool** `Eptipi::login()`

	Tạo prompt đăng nhặp trên màn hình console

	Nếu đăng nhập không thành công, hàm trả về **false** 


## TÀI LIỆU THAM KHẢO

- [FTP Command](https://en.wikipedia.org/wiki/List_of_FTP_commands)

- [FTP Return codes](https://en.wikipedia.org/wiki/List_of_FTP_server_return_codes)

- [How to get file by raw ftp command]()

- [How to put file by raw ftp command]()

- [Passive and Active in ftp]()
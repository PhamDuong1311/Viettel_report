# I. Socket
## 1. Tổng quan về Socket
### 1.1 Giới thiệu
Socket là phương tiện cho phép giao tiếp giữa hai tiến trình khác nhau trên cùng một máy/mạng hoặc các máy/mạng khác nhau. Nói chính xác hơn, đó là cách để giao tiếp với các máy tính khác và trao đổi dữ liệu. Trong Unix, mọi hành động I/O đều được thực hiện bằng cách ghi hoặc đọc một descriptor file. Socket là end-point của giao tiếp, trong đó mỗi tiến trình có socket và một descriptor socket cho tất cả các hoạt động.
### 1.2 Socket address structure
Hầu hết các hàm socket đều yêu cầu một con trỏ đến cấu trúc địa chỉ socket làm đối số. Mỗi bộ giao thức được hỗ trợ đều định nghĩa cấu trúc địa chỉ socket riêng. Tên của các cấu trúc này bắt đầu bằng sockaddr_ và kết thúc bằng hậu tố duy nhất cho mỗi bộ giao thức.
#### a. IPv4 socket address structure
Cấu trúc địa chỉ socket IPv4, thường được gọi là "cấu trúc địa chỉ socket Internet", được đặt tên là sockaddr_in và được định nghĩa bằng cách include <netinet/in.h>

![image](https://github.com/user-attachments/assets/542f083d-5f27-4262-b119-82ef6f40d301)
#### b. General socket address structure
Cấu trúc địa chỉ socket luôn được truyền theo tham chiếu khi được truyền dưới dạng đối số cho bất kỳ hàm socket nào. Nhưng bất kỳ hàm socket nào lấy một trong những con trỏ này làm đối số phải xử lý các cấu trúc địa chỉ socket từ bất kỳ họ giao thức nào được hỗ trợ.
Một vấn đề phát sinh trong cách khai báo loại con trỏ được truyền. Với ANSI C, giải pháp rất đơn giản: void * là loại con trỏ chung. Nhưng các hàm socket có trước ANSI C và giải pháp được chọn vào năm 1982 là định nghĩa một cấu trúc địa chỉ socket chung trong tiêu đề <sys/socket.h>.

![image](https://github.com/user-attachments/assets/adabe647-3cc9-4aa9-964a-207d6d44eab9)
#### c. IPv6 socket address struture
Địa chỉ socket IPv6 được xác định bằng cách include <netinet/in.h>

![image](https://github.com/user-attachments/assets/9a44f9f4-0b35-45c5-ae98-e12fa30df250)

### 1.3 Value-result arguments
https://www.informit.com/articles/article.aspx?p=169505&seqNum=3
### 1.4 Byte ordering functions
Không phải tất cả máy tính (Host) đều lưu trữ các byte tạo nên giá trị multiple-byte theo cùng một thứ tự. Xét một mạng internet 16 bit được tạo thành từ 2 byte. Có hai cách để lưu trữ giá trị này:

- **Little Endian** − Byte bậc thấp được lưu trữ ở địa chỉ bắt đầu (A) và byte bậc cao được lưu trữ ở địa chỉ tiếp theo (A + 1).

- **Big Endian** − Byte bậc cao được lưu trữ ở địa chỉ bắt đầu (A) và byte bậc thấp được lưu trữ ở địa chỉ tiếp theo (A + 1).

Để cho phép các máy có quy ước thứ tự byte khác nhau giao tiếp với nhau, các giao thức Internet chỉ định một quy ước thứ tự byte chuẩn cho dữ liệu được truyền qua mạng. Quy ước này được gọi là **Network Byte Order**.

Khi thiết lập kết nối socket Internet, bạn phải đảm bảo rằng dữ liệu trong các thành viên sin_port và sin_addr của cấu trúc sockaddr_in (hoặc cấu trúc khác) được biểu diễn trong Network Byte Order.
#### Byte ordering functions

![image](https://github.com/user-attachments/assets/8ead5bdf-6fac-48eb-aaa6-960485cc1846)

 - unsigned short htons(unsigned short hostshort) − Hàm chuyển đổi 16-bit (2-byte) từ host byte order sang network byte order.
  
 - unsigned long htonl(unsigned long hostlong) − Hàm chuyển đổi 32-bit (4-byte) từ host byte order sang network byte order.
  
 - unsigned short ntohs(unsigned short netshort) − Hàm chuyển đổi 16-bit (2-byte) từ network byte order sang host byte order.
  
 - unsigned long ntohl(unsigned long netlong) − Hàm chuyển đổi 32-bit (4-byte) từ network byte order sang host byte order.
### 1.5 inet_aton, inet_addr, inet_ntoa functions
Các hàm inet_aton, inet_addr, và inet_ntoa là các hàm liên quan đến việc chuyển đổi địa chỉ IP giữa các dạng khác nhau trong lập trình mạng. Các hàm này được định nghĩa trong thư viện <arpa/inet.h>.
#### a. inet_aton
Hàm inet_aton chuyển đổi một địa chỉ IP dưới dạng ASCII string (dạng "xxx.xxx.xxx.xxx") thành dạng Network Byte Order (đã định nghĩa ở phần trên) binary (dạng in_addr).
```c
int inet_aton(const char *cp, struct in_addr *inp);
```
**Argument:**
- cp: Một chuỗi ký tự đại diện cho địa chỉ IP dưới dạng ASCII (ví dụ: "192.168.1.1").
- inp: Con trỏ đến cấu trúc in_addr (đại diện cho địa chỉ IP dưới dạng nhị phân).
**Return:**
- Nếu chuyển đổi thành công, hàm trả về 1, và địa chỉ IP nhị phân được lưu trữ trong inp.
- Nếu có lỗi (địa chỉ IP không hợp lệ), hàm trả về 0.
#### b. inet_addr
Hàm inet_addr chuyển đổi một địa chỉ IP dưới dạng ASCII string (dạng "xxx.xxx.xxx.xxx") thành địa chỉ IP nhị phân dưới dạng kiểu in_addr_t (dạng Network Byte Order).
```c
in_addr_t inet_addr(const char *cp);
```
**Argument:**

- cp: Chuỗi ký tự đại diện cho địa chỉ IP (ví dụ: "192.168.1.1").
  
Giải thích:

Hàm trả về địa chỉ IP dưới dạng kiểu in_addr_t (thường là kiểu unsigned long).
Nếu địa chỉ IP không hợp lệ, hàm trả về giá trị đặc biệt INADDR_NONE, thường có giá trị là 0xFFFFFFFF.
#### c. inet_ntoa
### 1.6 inet_pton, inet_ntop functions
### 1.7 sock_ntop and related functions
### 1.8 readn, writen and readline functions

## 2. Socket TCP
## 3. Socket UDP

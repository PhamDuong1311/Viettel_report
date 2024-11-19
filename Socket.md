# I. Socket
## 1. Tổng quan về Socket
### 1.1 Giới thiệu
Socket là phương tiện cho phép giao tiếp giữa hai tiến trình khác nhau trên cùng một máy/mạng hoặc các máy/mạng khác nhau. Nói chính xác hơn, đó là cách để giao tiếp với các máy tính khác và trao đổi dữ liệu. Trong Unix, mọi hành động I/O đều được thực hiện bằng cách ghi hoặc đọc một descriptor file. Socket là end-point của giao tiếp, trong đó mỗi tiến trình có socket và một descriptor socket cho tất cả các hoạt động.
### 1.2 Socket address structure
Hầu hết các hàm socket đều yêu cầu một con trỏ đến cấu trúc địa chỉ socket làm đối số. Mỗi bộ giao thức được hỗ trợ đều định nghĩa cấu trúc địa chỉ socket riêng. Tên của các cấu trúc này bắt đầu bằng sockaddr_ và kết thúc bằng hậu tố duy nhất cho mỗi bộ giao thức.
#### a. IPv4 socket address structure
Cấu trúc địa chỉ socket IPv4, thường được gọi là "cấu trúc địa chỉ socket Internet", được đặt tên là sockaddr_in và được định nghĩa bằng cách include <netinet/in.h>
```c
struct in_addr {
  in_addr_t   s_addr;           /* 32-bit IPv4 address */
                                /* network byte ordered */
};

struct sockaddr_in {
  uint8_t         sin_len;      /* length of structure (16) */
  sa_family_t     sin_family;   /* AF_INET */
  in_port_t       sin_port;     /* 16-bit TCP or UDP port number */
                                /* network byte ordered */
  struct in_addr  sin_addr;     /* 32-bit IPv4 address */
                                /* network byte ordered */
  char            sin_zero[8];  /* unused */
};
```
#### b. General socket address structure
Cấu trúc địa chỉ socket luôn được truyền theo tham chiếu khi được truyền dưới dạng đối số cho bất kỳ hàm socket nào. Nhưng bất kỳ hàm socket nào lấy một trong những con trỏ này làm đối số phải xử lý các cấu trúc địa chỉ socket từ bất kỳ họ giao thức nào được hỗ trợ.
Một vấn đề phát sinh trong cách khai báo loại con trỏ được truyền. Với ANSI C, giải pháp rất đơn giản: void * là loại con trỏ chung. Nhưng các hàm socket có trước ANSI C và giải pháp được chọn vào năm 1982 là định nghĩa một cấu trúc địa chỉ socket chung trong tiêu đề <sys/socket.h>.
```c
struct sockaddr {
  uint8_t      sa_len;
  sa_family_t  sa_family;    /* address family: AF_xxx value */
  char         sa_data[14];  /* protocol-specific address */
};
```
#### c. IPv6 socket address struture
Địa chỉ socket IPv6 được xác định bằng cách include <netinet/in.h>
```c
struct in6_addr {
  uint8_t  s6_addr[16];          /* 128-bit IPv6 address */
                                 /* network byte ordered */
};

#define SIN6_LEN      /* required for compile-time tests */

struct sockaddr_in6 {
  uint8_t         sin6_len;      /* length of this struct (28) */
  sa_family_t     sin6_family;   /* AF_INET6 */
  in_port_t       sin6_port;     /* transport layer port# */
                                 /* network byte ordered */
  uint32_t        sin6_flowinfo; /* flow information, undefined */
  struct in6_addr sin6_addr;     /* IPv6 address */
                                 /* network byte ordered */
  uint32_t        sin6_scope_id; /* set of interfaces for a scope */
};
```
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

**Syntax**:
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

**Syntax**:
```c
in_addr_t inet_addr(const char *cp);
```
**Argument:**
- cp: Chuỗi ký tự đại diện cho địa chỉ IP (ví dụ: "192.168.1.1").
  
**Return:**
- Hàm trả về địa chỉ IP dưới dạng kiểu in_addr_t (thường là kiểu unsigned long).
- Nếu địa chỉ IP không hợp lệ, hàm trả về giá trị đặc biệt INADDR_NONE, thường có giá trị là 0xFFFFFFFF.
#### c. inet_ntoa
Hàm inet_ntoa chuyển đổi một địa chỉ IP nhị phân (dạng in_addr) thành ASCII string (dạng "xxx.xxx.xxx.xxx").

**Syntax**:
```c
char *inet_ntoa(struct in_addr in);
```
**Argument**:
- in: Địa chỉ IP nhị phân (kiểu struct in_addr).

**Return**:
- Hàm trả về một chuỗi ký tự chứa địa chỉ IP dưới dạng ASCII.
- Lưu ý rằng chuỗi trả về là một con trỏ tĩnh và có thể bị thay đổi nếu gọi lại hàm inet_ntoa trong lần sau.
### 1.6 inet_pton, inet_ntop functions
Chức năng tương tự như các hàm chuyển đổi trên nhưng ở đây mở rộng dùng cho cả IPv6
#### a. inet_pton
Hàm inet_pton (viết tắt của "presentation to network") chuyển đổi một địa chỉ IP từ dạng ASCII sang dạng nhị phân (dạng Network Byte Order).

**Syntax**:
```c
int inet_pton(int af, const char *src, void *dst);
```
**Argument**:
- af: Loại giao thức mạng (có thể là AF_INET cho IPv4 hoặc AF_INET6 cho IPv6).
- src: Chuỗi địa chỉ IP cần chuyển đổi (ví dụ: "192.168.1.1" cho IPv4 hoặc "2001:db8::1" cho IPv6).
- dst: Con trỏ đến bộ nhớ nơi sẽ lưu trữ địa chỉ IP dạng nhị phân (ví dụ: struct in_addr cho IPv4 hoặc struct in6_addr cho IPv6).
  
**Return**:
- Nếu chuyển đổi thành công, hàm trả về 1.
- Nếu có lỗi (ví dụ, chuỗi IP không hợp lệ), hàm trả về 0.
- Nếu địa chỉ không hợp lệ cho giao thức được chỉ định, hàm trả về -1 và thiết lập errno.
#### b. inet_ntop
Hàm inet_ntop (viết tắt của "network to presentation") chuyển đổi một địa chỉ IP từ dạng nhị phân (dạng Network Byte Order) sang dạng ASCII.

**Syntax**:
```c
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```
**Argument**:
- af: Loại giao thức mạng (có thể là AF_INET cho IPv4 hoặc AF_INET6 cho IPv6).
- src: Con trỏ đến dữ liệu địa chỉ IP dạng nhị phân (ví dụ: struct in_addr cho IPv4 hoặc struct in6_addr cho IPv6).
- dst: Chuỗi ký tự nơi địa chỉ IP dưới dạng chuỗi sẽ được lưu trữ.
- size: Kích thước của bộ đệm dst (số byte tối đa cho phép để chứa địa chỉ IP sau khi chuyển đổi).
  
**Return**:
- Nếu chuyển đổi thành công, hàm trả về con trỏ đến chuỗi ký tự đại diện cho địa chỉ IP.
- Nếu có lỗi, hàm trả về NULL và thiết lập errno.
### 1.7 sock_ntop and related functions
Hàm inet_ntop có 1 vấn đề là nó yêu cầu truyền pointer vào địa chỉ binary. Địa chỉ này yêu cầu người gọi hàm biết cấu trúc của hắn (`struct sockaddr_in` cho IPv4 hay 'struct sockaddr_in6' cho IPv6) và địa chỉ family (IPv4: AF_INET, IPv6: AF_INET6) => Khi làm việc với một struct sockaddr, phải tự tay kiểm tra sa_family (là AF_INET hay AF_INET6) để quyết định cách xử lý => Điều này dẫn đến mã nguồn dài dòng và dễ nhầm lẫn, đặc biệt khi cần hỗ trợ cả IPv4 và IPv6.

=> Hàm sock_ntop khắc phục nhược điểm này bằng cách:
- Nhận trực tiếp một con trỏ tới struct sockaddr.
- Tự động kiểm tra sa_family để quyết định xử lý IPv4 hay IPv6.
- Kết hợp cả địa chỉ IP và port vào chuỗi kết quả.

Hàm sock_ntop không phải hàm chuẩn (sử dụng trong include "unp.h" để dễ tái sử dụng), sử dụng static memory để lưu kết quả => giá trị trả về luôn cùng 1 bộ nhớ (dễ lỗi).

**Syntax**:
```c
const char *sock_ntop(const struct sockaddr *sa, socklen_t salen);
```

**Argument**:
- sa: Con trỏ đến một cấu trúc sockaddr (có thể là sockaddr_in cho IPv4, sockaddr_in6 cho IPv6, hoặc các loại sockaddr khác).
- salen: Kích thước của cấu trúc sockaddr (ví dụ: sizeof(struct sockaddr_in) cho IPv4 và sizeof(struct sockaddr_in6) cho IPv6).
- 
**Return**:
- Hàm này sẽ trả về một chuỗi chứa địa chỉ mạng ở dạng chuỗi (địa chỉ IP với cổng).
- Nếu có lỗi trong quá trình chuyển đổi, nó sẽ trả về NULL.
### 1.8 readn, writen and readline functions
Là những hàm trong bối cảnh UNP để xử lý input, output cho stream socket. Không như file I/O, socket I/O có hàm đọc và viết chỉ có thể xử lý 1 phần data do giới hạn của kernel buffer (để viết/đọc hết data thì cần thử lại hàm nhiều lần).
#### a. readn
Hàm này được thiết kế để đảm bảo đọc chính xác n bytes từ một descriptor (như file descriptor hoặc socket), bất kể dữ liệu thực sự có sẵn ngay lập tức hay không.

**Syntax**: ```c ssize_t readn(int fd, void *buf, size_t n); ```
#### b. writen
Hàm này đảm bảo ghi chính xác n bytes từ bộ nhớ đến descriptor, khắc phục vấn đề write có thể không ghi hết dữ liệu trong một lần gọi.

**Syntax**: ```c ssize_t writen(int fd, const void *buf, size_t n); ```
#### c. readline
Hàm này đọc dữ liệu từ fd từng byte một cho đến khi gặp:
- Ký tự xuống dòng (\n).
- Hoặc hết bộ đệm/dữ liệu.

**Syntax**: ```c ssize_t readline(int fd, void *buf, size_t maxlen); ```
## 2. Socket TCP

## 3. Socket UDP

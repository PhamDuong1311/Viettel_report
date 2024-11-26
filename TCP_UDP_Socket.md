# I. Socket
## 1. Tổng quan về Socket
### 1.1 Giới thiệu
Socket là phương tiện cho phép giao tiếp giữa hai tiến trình khác nhau trên cùng một máy/mạng hoặc các máy/mạng khác nhau. Nói chính xác hơn, đó là cách để giao tiếp với các máy tính khác và trao đổi dữ liệu. Trong Unix, mọi hành động I/O đều được thực hiện bằng cách ghi hoặc đọc một descriptor file. Socket là end-point của giao tiếp, trong đó mỗi tiến trình có socket và một descriptor socket cho tất cả các hoạt động.
### 1.2 Socket address structure
Hầu hết các hàm socket đều yêu cầu một con trỏ đến cấu trúc địa chỉ socket làm đối số. Mỗi bộ giao thức được hỗ trợ đều định nghĩa cấu trúc địa chỉ socket riêng. Tên của các cấu trúc này bắt đầu bằng `sockaddr_` và kết thúc bằng hậu tố duy nhất cho mỗi bộ giao thức.
#### a. IPv4 socket address structure
Cấu trúc địa chỉ socket IPv4, thường được gọi là "cấu trúc địa chỉ socket Internet", được đặt tên là `sockaddr_in` và được định nghĩa bằng cách `#include <netinet/in.h>`
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
Một vấn đề phát sinh trong cách khai báo loại con trỏ được truyền. Với ANSI C, giải pháp rất đơn giản: void * là loại con trỏ chung. Nhưng các hàm socket có trước ANSI C và giải pháp được là định nghĩa một cấu trúc địa chỉ socket chung trong `#include <sys/socket.h>`.
```c
struct sockaddr {
  uint8_t      sa_len;
  sa_family_t  sa_family;    /* address family: AF_xxx value */
  char         sa_data[14];  /* protocol-specific address */
};
```
#### c. IPv6 socket address struture
Địa chỉ socket IPv6 được xác định bằng cách `#include <netinet/in.h>`.
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
Em chưa hiểu phần này lắm 
### 1.4 Byte ordering functions
Không phải tất cả máy tính (Host) đều lưu trữ các byte tạo nên giá trị multiple-byte theo cùng một thứ tự. Xét một mạng internet 16 bit được tạo thành từ 2 byte. Có hai cách để lưu trữ giá trị này:
- **Little Endian** − Byte bậc thấp được lưu trữ ở địa chỉ bắt đầu (A) và byte bậc cao được lưu trữ ở địa chỉ tiếp theo (A + 1).
- **Big Endian** − Byte bậc cao được lưu trữ ở địa chỉ bắt đầu (A) và byte bậc thấp được lưu trữ ở địa chỉ tiếp theo (A + 1).
  
Để cho phép các máy có quy ước thứ tự byte khác nhau giao tiếp với nhau, các giao thức Internet chỉ định một quy ước thứ tự byte chuẩn cho dữ liệu được truyền qua mạng. Quy ước này được gọi là **Network Byte Order**.

Khi thiết lập kết nối socket Internet, bạn phải đảm bảo rằng dữ liệu trong các thành viên `sin_port` và `sin_addr` của cấu trúc `sockaddr_in` (hoặc cấu trúc khác) được biểu diễn trong Network Byte Order.
#### Byte ordering functions

![image](https://github.com/user-attachments/assets/8ead5bdf-6fac-48eb-aaa6-960485cc1846)
 - unsigned short htons(unsigned short hostshort) − Hàm chuyển đổi 16-bit (2-byte) từ host byte order sang network byte order.
 - unsigned long htonl(unsigned long hostlong) − Hàm chuyển đổi 32-bit (4-byte) từ host byte order sang network byte order.
 - unsigned short ntohs(unsigned short netshort) − Hàm chuyển đổi 16-bit (2-byte) từ network byte order sang host byte order.
 - unsigned long ntohl(unsigned long netlong) − Hàm chuyển đổi 32-bit (4-byte) từ network byte order sang host byte order.
### 1.5 `inet_aton`, `inet_addr`, `inet_ntoa` functions
Các hàm `inet_aton`, `inet_addr`, và `inet_ntoa` là các hàm liên quan đến việc chuyển đổi địa chỉ IP giữa các dạng khác nhau trong lập trình mạng. Các hàm này được định nghĩa trong thư viện <arpa/inet.h>.
#### a. `inet_aton`
Hàm `inet_aton` chuyển đổi một địa chỉ IP dưới dạng ASCII string (dạng "xxx.xxx.xxx.xxx") thành dạng Network Byte Order (đã định nghĩa ở phần trên) binary (dạng `in_addr`).

**Syntax**:
```c
int inet_aton(const char *cp, struct in_addr *inp);
```
**Argument:**
- cp: Một chuỗi ký tự đại diện cho địa chỉ IP dưới dạng ASCII (ví dụ: "192.168.1.1").
- inp: Con trỏ đến cấu trúc `in_addr` (đại diện cho địa chỉ IP dưới dạng nhị phân).
  
**Return:**
- Nếu chuyển đổi thành công, hàm trả về 1, và địa chỉ IP nhị phân được lưu trữ trong inp.
- Nếu có lỗi (địa chỉ IP không hợp lệ), hàm trả về 0.
#### b. `inet_addr`
Hàm `inet_addr` chuyển đổi một địa chỉ IP dưới dạng ASCII string (dạng "xxx.xxx.xxx.xxx") thành địa chỉ IP nhị phân dưới dạng kiểu `in_addr_t` (dạng Network Byte Order).

**Syntax**:
```c
in_addr_t inet_addr(const char *cp);
```
**Argument:**
- cp: Chuỗi ký tự đại diện cho địa chỉ IP (ví dụ: "192.168.1.1").
  
**Return:**
- Hàm trả về địa chỉ IP dưới dạng kiểu `in_addr_t` (thường là kiểu unsigned long).
- Nếu địa chỉ IP không hợp lệ, hàm trả về giá trị đặc biệt `INADDR_NONE`, thường có giá trị là `0xFFFFFFFF`.
#### c. `inet_ntoa`
Hàm `inet_ntoa` chuyển đổi một địa chỉ IP nhị phân (dạng `in_addr`) thành ASCII string (dạng "xxx.xxx.xxx.xxx").

**Syntax**:
```c
char *inet_ntoa(struct in_addr in);
```
**Argument**:
- in: Địa chỉ IP nhị phân (kiểu `struct in_addr`).

**Return**:
- Hàm trả về một chuỗi ký tự chứa địa chỉ IP dưới dạng ASCII.
- Lưu ý rằng chuỗi trả về là một con trỏ tĩnh và có thể bị thay đổi nếu gọi lại hàm `inet_ntoa` trong lần sau.
### 1.6 `inet_pton`, `inet_ntop` functions
Chức năng tương tự như các hàm chuyển đổi trên nhưng ở đây mở rộng dùng cho cả IPv6
#### a. `inet_pton`
Hàm `inet_pton` (viết tắt của "presentation to network") chuyển đổi một địa chỉ IP từ dạng ASCII sang dạng nhị phân (dạng Network Byte Order).

**Syntax**:
```c
int inet_pton(int af, const char *src, void *dst);
```
**Argument**:
- af: Loại giao thức mạng (có thể là `AF_INET` cho IPv4 hoặc `AF_INET6` cho IPv6).
- src: Chuỗi địa chỉ IP cần chuyển đổi (ví dụ: "192.168.1.1" cho IPv4 hoặc "2001:db8::1" cho IPv6).
- dst: Con trỏ đến bộ nhớ nơi sẽ lưu trữ địa chỉ IP dạng nhị phân (ví dụ: `struct in_addr` cho IPv4 hoặc `struct in6_addr` cho IPv6).
  
**Return**:
- Nếu chuyển đổi thành công, hàm trả về 1.
- Nếu có lỗi (ví dụ, chuỗi IP không hợp lệ), hàm trả về 0.
- Nếu địa chỉ không hợp lệ cho giao thức được chỉ định, hàm trả về -1 và thiết lập errno.
#### b. `inet_ntop`
Hàm `inet_ntop` (viết tắt của "network to presentation") chuyển đổi một địa chỉ IP từ dạng nhị phân (dạng Network Byte Order) sang dạng ASCII.

**Syntax**:
```c
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```
**Argument**:
- af: Loại giao thức mạng (có thể là `AF_INET` cho IPv4 hoặc `AF_INET6` cho IPv6).
- src: Con trỏ đến dữ liệu địa chỉ IP dạng nhị phân (ví dụ: `struct in_addr` cho IPv4 hoặc `struct in6_addr` cho IPv6).
- dst: Chuỗi ký tự nơi địa chỉ IP dưới dạng chuỗi sẽ được lưu trữ.
- size: Kích thước của bộ đệm dst (số byte tối đa cho phép để chứa địa chỉ IP sau khi chuyển đổi).
  
**Return**:
- Nếu chuyển đổi thành công, hàm trả về con trỏ đến chuỗi ký tự đại diện cho địa chỉ IP.
- Nếu có lỗi, hàm trả về `NULL` và thiết lập errno.
### 1.7 `sock_ntop` and related functions
Hàm `inet_ntop` có 1 vấn đề là nó yêu cầu truyền pointer vào địa chỉ binary. Địa chỉ này yêu cầu người gọi hàm biết cấu trúc của hắn (`struct sockaddr_in` cho IPv4 hay `struct sockaddr_in6` cho IPv6) và địa chỉ family (IPv4: `AF_INET`, IPv6: `AF_INET6`) => Khi làm việc với một `struct sockaddr`, phải tự tay kiểm tra `sa_family` (là `AF_INET` hay `AF_INET6`) để quyết định cách xử lý => Điều này dẫn đến mã nguồn dài dòng và dễ nhầm lẫn, đặc biệt khi cần hỗ trợ cả IPv4 và IPv6.

=> Hàm `sock_ntop` khắc phục nhược điểm này bằng cách:
- Nhận trực tiếp một con trỏ tới `struct sockaddr`.
- Tự động kiểm tra `sa_family` để quyết định xử lý IPv4 hay IPv6.
- Kết hợp cả địa chỉ IP và port vào chuỗi kết quả.

Hàm `sock_ntop` không phải hàm chuẩn (sử dụng trong `#include "unp.h"` để dễ tái sử dụng), sử dụng static memory để lưu kết quả => giá trị trả về luôn cùng 1 bộ nhớ (dễ lỗi).

**Syntax**:
```c
const char *sock_ntop(const struct sockaddr *sa, socklen_t salen);
```

**Argument**:
- sa: Con trỏ đến một cấu trúc `sockaddr` (có thể là `sockaddr_in` cho IPv4, `sockaddr_in6` cho IPv6, hoặc các loại `sockaddr` khác).
- salen: Kích thước của cấu trúc `sockaddr` (ví dụ: `sizeof(struct sockaddr_in)` cho IPv4 và `sizeof(struct sockaddr_in6)` cho IPv6).
- 
**Return**:
- Hàm này sẽ trả về một chuỗi chứa địa chỉ mạng ở dạng chuỗi (địa chỉ IP với cổng).
- Nếu có lỗi trong quá trình chuyển đổi, nó sẽ trả về `NULL`.
### 1.8 `readn`, `writen` and `readline` functions
Là những hàm trong bối cảnh UNP để xử lý input, output cho stream socket. Không như file I/O, socket I/O có hàm đọc và viết chỉ có thể xử lý 1 phần data do giới hạn của kernel buffer (để viết/đọc hết data thì cần thử lại hàm nhiều lần).
#### a. `readn`
Hàm này được thiết kế để đảm bảo đọc chính xác n bytes từ một descriptor (như file descriptor hoặc socket), bất kể dữ liệu thực sự có sẵn ngay lập tức hay không.

**Syntax**: 
```c 
ssize_t readn(int fd, void *buf, size_t n);
```
#### b. `writen`
Hàm này đảm bảo ghi chính xác n bytes từ bộ nhớ đến descriptor, khắc phục vấn đề write có thể không ghi hết dữ liệu trong một lần gọi.

**Syntax**: 
```c 
ssize_t writen(int fd, const void *buf, size_t n);
```
#### c. `readline`
Hàm này đọc dữ liệu từ fd từng byte một cho đến khi gặp:
- Ký tự xuống dòng (\n).
- Hoặc hết bộ đệm/dữ liệu.

**Syntax**:
```c 
ssize_t readline(int fd, void *buf, size_t maxlen);
```
## 2. Socket TCP

![image](https://github.com/user-attachments/assets/37440647-d979-4f57-bd74-64b224959f34)
### 2.1 `socket` function
**Syntax**:
```c
int socket(int domain, int type, int protocol);
```
**Argument**:
- domain: Giao thức mạng (e.g., `AF_INET` cho IPv4, `AF_INET6` cho IPv6).
- type: Loại socket (`SOCK_STREAM` cho TCP, `SOCK_DGRAM` cho UDP).
- protocol: Giao thức cụ thể, thường là 0 (mặc định).
  
**Return**:
- File descriptor của socket (>= 0) nếu thành công.
- -1 nếu lỗi.

**Note**:
- Tạo một socket để giao tiếp qua mạng (gọi bởi server và client).
- `socket()` không chỉ ra data đến từ đâu (cũng như gửi từ đâu), nó chỉ tạo ra 1 interface.

### 2.2 `connect` function
 **Syntax**:
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
**Argument**:
- sockfd: File descriptor của socket từ `socket()`.
- addr: Con trỏ tới địa chỉ của server.
- addrlen: Kích thước của cấu trúc địa chỉ.
  
**Return**:
- 0 nếu thành công.
- -1 nếu lỗi.

**Note**:
- Yêu cầu thiết lập kết nối đến server.
- Nếu `connect()` = 1 => `socket()` không sử dụng được nữa và phải `close()`.
- Không thể gọi `connect()` lại trên `socket()` (mà phải gọi lại socket nếu trong vòng lặp).
- `connect()` khởi tạo TCP (three-way handshake).

### 2.3 `bind` function
**Syntax**:
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
**Argument**:
- sockfd: File descriptor của socket từ hàm `socket()`.
- addr: Con trỏ tới cấu trúc chứa địa chỉ (e.g., `struct sockaddr_in`).
- addrlen: Kích thước của cấu trúc địa chỉ.

**Return**:
- 0 nếu thành công.
- -1 nếu lỗi.

**Note**:
- Gắn socket với một địa chỉ IP và port cụ thể.
- Định nghĩa 1 số term quan trọng:
  + **Ephemeral port** (49152 - 65535): là 1 cổng tạm thời (tự động gán cho client bởi kernel chọn khi muốn kết nối với server)
  + **Reserved port** (0 - 1023): cổng chuẩn của từng service
- Nếu client muốn kết nối tới 1 service (giả sử port 80) của server, client nhờ thằng ku kernel chọn 1 ephemeral port cho nó làm source port (giả sử 52000). Sau đó gửi từ 52000 -> 80 để yêu cầu kết nối, server sẽ phản hồi từ 80 -> 52000 (server sẽ `bind()` reserved port của nó khi bắt đầu)
- Tương tự port, ở client, thằng kernel cũng tự động gán cho thằng client 1 cái source IP address dựa trên outgoing interface. Còn ở server, server sẽ `bind()` và gán specific IP address cho socket nó đẻ ra. Nếu server không `bind()`, kernel sẽ sử dụng destination IP address của client gửi đến qua gói `SYN` (trong TCP's 3-way) (tức là server sẽ xử lý mọi Ip mà không ràng buộc như specific IP address => khó quản lý => nên hơi lỏ :v)

![image](https://github.com/user-attachments/assets/ff42558c-a2cf-4c4c-a1de-16be7202c63e)


### 2.4 `listen` function
**Syntax**:
```c
int listen(int sockfd, int backlog);
```
**Argument**:
- sockfd: File descriptor của socket từ `bind()`.
- backlog: Số lượng kết nối tối đa có thể xếp hàng chờ xử lý.

**Return**:
- 0 nếu thành công.
- -1 nếu lỗi.

**Note**: 
- Đánh dấu socket để bắt đầu chờ kết nối đến từ client.
- Hàm `listen()` thực hiện 2 hành động:
  + Chuyển socket thành passive: chức năng này ở trên socket thông báo cho kernel rằng socket sẵn sàng nhận yêu cầu kết nối từ client.
  + Xác định số lượng kết nối tối đa: sử dụng đối số backlog lưu các kết nối vào `queue`.
- Kernel duy trì 2 queue:
  + Incomplete connection queue: chứa các entry cho mỗi `SYN` từ client đến rồi và server đang chờ hoàn tất TCP's 3-way.
  + Complete connection queue: chứa các đàu mục cho mỗi client hoàn thành TCP's 3-way.

![image](https://github.com/user-attachments/assets/10bdb507-e323-40b1-a6ea-bdbb97d5a049)

### 2.5 `accept` function
**Syntax**:
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
**Argument**:
- sockfd: File descriptor của socket từ listen().
- addr: Con trỏ để lưu địa chỉ client (e.g., struct sockaddr_in).
- addrlen: Con trỏ tới kích thước của cấu trúc địa chỉ.

**Return**:
- File descriptor của socket mới cho kết nối đã chấp nhận.
- -1 nếu lỗi.

**Note**: 
- Chấp nhận một kết nối từ client.
- Hàm `accept()` cho phép chấp nhận các kết nối từ client và tạo 1 descriptor mới cho client này kernel (descriptor mới đó là connected socket)

### 2.6 `fork` and `exec` functions
#### a. `fork` function
**Syntax**:
```c
pid_t fork(void);
```

**Return**:
- Trả về `pid_t`:
  + Trong tiến trình cha: trả về ID của tiến trình con (PID của tiến trình con).
  + Trong tiến trình con: trả về 0.
  + Nếu gặp lỗi: trả về -1.
  
**Note**:
- Khi `fork()` được gọi, nó sẽ trả về 2 proccess:
  + Parent proccess: trẻ về proccess IP của child proccess mới tạo ra.
  + Child proccess: trả về 0 (mà không trả về PID của parent vì child chỉ có 1 parent, còn parent thì nhiều child - điều hiển nhiên :V)
- Tính năng chính của `fork()`: Shared file descriptor: tất cả các descriptor mở trong parent trước khi gọi `fork()` được share với child sau khi trả về của `fork()`. Parent sau khi `accept()` kết nối và `fork()` cho 1 child xử lý từng kết nối trong khi parent sẽ `listen()` 1 cái mới.
- Các ứng dụng:
  + Concurrent operation: Sử dụng các copy (child) của nó để xử lý các task (sẽ trình bày ở phần sau).
  + Executing new program: Sử dụng các copy của nó gọi `exec()` để thay memory space của nó với chương trình mới (eg. shell).
#### b. `exec` function
Gọi 1 trong 6 hàm `exec()` là **cách dùng duy nhất** để 1 chương trình có thể thực thi trên đĩa bởi Unix. `exec()` thay thế proccess image hiện tại bằng program file mới (PID không cần thay đổi) và bắt đầu thực thi tại `main()` (`exec()` không tạo ra proccess mới, nó thay đổi chương trình trong proccess hiện tại)

**Return**:
- Nếu thành công: không trả về giá trị vì tiến trình hiện tại bị thay thế bởi tiến trình mới.
- Nếu thất bại: trả về -1 và thiết lập errno để chỉ rõ lỗi.

![image](https://github.com/user-attachments/assets/53ad1517-58b7-454d-b372-fbb2d29dadcd)

### 2.7 Concurrent servers
Là việc sử dụng `fork()` để tạo ra các bản sao của parent proccess để xử lý mỗi yêu cầu của client:

![image](https://github.com/user-attachments/assets/18b64263-75f4-4bd0-8841-c37b65c7cb35)

Hình 4.14: Đây là trạng thái client/server trước khi gọi `accept()` return.

![image](https://github.com/user-attachments/assets/2e33a99e-2d68-4b00-93c6-14668affce85)

Hình 4.15: Đây là sự kết nối được `accept()` bởi kernel và 1 socket mớ được tạo ra (connfd).

![image](https://github.com/user-attachments/assets/e718b29f-f4b0-4774-8e98-49b4a392d2c8)

Hình 4.16: Đây là trạng thái sau khi họi `fork()` trong concurrent server. Thông báo rằng cả 2 socket descriptor (listenfd và connfd) được share (duplicate) giữa parent và child.

![image](https://github.com/user-attachments/assets/fb3a271e-e488-4ad8-a170-d8f95c306265)

Hình 4.17: Parent `close()` socket đã kết nốt và child `close()` socket đang lắng nghe.

### 2.8 `close` function
Đóng socket và giải phóng tài nguyên.

**Syntax**:
```c
int close(int sockfd);
```
**Argument**:
- sockfd: File descriptor của socket cần đóng.

**Return**:
- 0 nếu thành công.
- -1 nếu lỗi.

### 2.9 `getsockname` and `getpeername` functions
Hai hàm này trả về hoặc là địa chỉ local protocol được kết nối với một socket (`getsockname()`) hoặc địa chỉ remote protocol được kết nối với 1 socket (`getpeername()`).

**Syntax**:
```c
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Argument**:
- sockfd: Là socket descriptor của socket mà bạn muốn lấy thông tin.
- addr: Là con trỏ tới cấu trúc sockaddr để lưu trữ địa chỉ của socket.
- addrlen: Là kích thước của cấu trúc sockaddr. Sau khi hàm thực thi, giá trị của addrlen sẽ được thay đổi thành kích thước thực tế của địa chỉ

**Return**:
- Nếu thành công trả về 0.
- Xảy ra lỗi trả về -1.

**Note**:
  + Khi 1 TCP client `connect()` thành công tới server (client không gọi `bind()`), kernel sẽ tự động chọn 1 local IP address và port để kết nối,`getsockname()` sử dụng để lấy lại local IP address và port gán bởi kernel.
  + Nếu TCP cient gọi `bind()` với port = 0 (kernel chọn local port tự động), `getsockname()` vẫn dùng lấy port gán bởi kernel.
  + `getsockname()` quyết định address family của 1 socket.
  + Ở TCP server, sau khi gọi `accept()`, server gọi`getsockname()` cũng chứa local IP addrress và port mà kernel gán.
  + Khi TCP server `fork()` và `exec()` => tạo ra child nhưng memory image sẽ mất hết (kể SAS). Chỉ có 1 cách để child lấy địa chỉ client là gọi `getpeername()`.
## 3. I/O multiplexing: `select` and `poll` functions
### 3.1 Introduction
Khi client chờ nhập từ phím (stdin) nhưng bị chặn khi đọc dữ liệu từ đó, trong khi server gửi kết thúc kết nối (`FIN`) => client không đọc được tín hiệu EOF cho đến khi đọc từ socket => cần khả năng theo dõi nhiều nguồn I/O (như stdin hay socket) đồng thời => I/O multiplexing. I/O multiplexing cho phép ứng dụng theo dõi từ nhiều nguồn I/O (socket, file, stdin...) và thực hiện tất cả khi sẵn sàng.
### 3.2 I/O multiplexing
#### a. `select()` function
Là 1 system call phép proccess hướng dẫn kernel thực hiện 1 trong 2 hành động:
- Chờ bất kỳ sự kiện nào trong số nhiều sự kiện xảy ra và chỉ đánh thức proccess khi 1 hoặc nhiều sự kiện xảy ra.
- Hoặc khi chờ 1 khoảng thời gian nhất định trôi qua.

**Syntax**:
```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

**Argument**:
- nfds: chỉ định số lượng descriptor cần kiểm tra (max descriptor + 1).
- readfds, writefds, exceptfds: chỉ định các descriptor mà muốn kiểm tra với các điều kiện đọc/ ghi/except. `select()` sử dụng các descriptor set (thường là mảng số nguyên) với mỗi bit trong mỗi số nguyên tương ứng với 1 descriptor:
```c
void FD_ZERO(fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
void FD_CLEAR(int fd, fd_set *fdset);
int FD_ISSET(int fd, fd_set *fdset);
```
- timeout: cho biết kernel phải đợi bao lâu để 1 trong các descriptor được chỉ định sẵn sàng. Cấu trúc `timeval`:
```c
struct timeval {
  long tv_sec;
  long tv_usec;
};
```

**Return**:
- Tổng bit đã sẵn sàng descriptor.
- 0 nếu timeout.
- -1 nếu lỗi.
#### b. `poll()` function
Hàm này tương tự `select()` và cung cấp thông tin bổ sung khi xử lý các thiết bị STREAMS

**Syntax**:
```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

**Argument**:
- fds: con trỏ đến phần tử đầu tiên của mảng các cấu trúc, mỗi phần tử của mảng là 1 cấu trúc `pollfd` chỉ định các điều kiện cần kiểm tra cho 1 descriptor nhất định:
```c
struct pollfd {
    int fd;          // The file descriptor to monitor
    short events;    // The events to wait for (e.g., POLLIN for read, POLLOUT for write)
    short revents;   // The events that actually occurred
};
```
- nfds: số lượng phần tử trong mảng cấu trúc.
- timeout: thời gian phải đợi trước khi return.

**Return**:
- Số lượng descriptor sẵn sàng
- O nếu timeout
- -1 nếu lỗi

#### c. Comparision `select()`and `poll()`
- Giới hạn số lượng file descriptor:
  + `select()`: giới hạn tối đa 1024
  + `poll()`: không giới hạn
- Hiệu năng:
  + `select()`: phải kiểm tra từng bit trong `fd_set` để biết file descriptor (bit) nào cần theo dõi (phải mất công duyệt hết cmnl)
  + `poll()`: chỉ cần truyền cấu trúc chính xác file descriptor kèm theo sự kiện quan tâm (`pollfd.fd`, `pollfd.event`) thì kernelsex kiểm tra duy nhất các fd đó
- Tương thích:
  + `select()`: tương thích với nhiều hệ thống
  + `poll()`: có nhiều biến thể ở các hệ thống khác nhau

## 4. Socket UDP
### 4.1 Introduction
UDP là 1 connectionless,unreliable,datagram protocol (đã trình bày ở report trước). Application phổ biến sử dụng UDP: DNS, NFS, SNMP ... Dưới đây là quy trình giao tiếp giữa UDP server và UDP client:

![image](https://github.com/user-attachments/assets/856c95ff-7d94-46cb-9454-540ea619c50c)

### 4.2 `recvfrom` and `sendto` functions
Tương tự 2 hàm tiêu chuẩn read/write nhưng có thêm 3 argument

**Syntax**:
```c
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, 
                 struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, 
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

**Argument**:
- sockfd, buf, len: giống hàm read/write (descriptor, con trỏ tới buffer để đọc/ghi, số byte đọc/ghi)
- flags: đặt bằng 0
- dest_addr của `sendto()`: là SAS chứa (IP, port) nơi mà data được gửi, addrlen là kích thước SAS (dạng interger value)
- src_addr của `recvfrom()`: là SAS chứa (IP, port) nơi data nhận, addrlen là kích thước SAS (pointer tới integer value hay gọi là value-result argument ???)

**Return**:
- Số lượng byte đọc/viết nếu thành công
- -1 nếu lỗi

### 4.3 TCP and UDP client/server with 2 clients

![image](https://github.com/user-attachments/assets/0583dfb2-a903-4d82-afeb-94e614d36043)

**TCP server là concurrent**

![image](https://github.com/user-attachments/assets/ecdce2db-cbd3-4004-9163-d1a777171b71)

**UDP server là iterative**



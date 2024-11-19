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
Không phải tất cả máy tính đều lưu trữ các byte tạo nên giá trị đa byte theo cùng một thứ tự. Hãy xem xét một mạng internet 16 bit được tạo thành từ 2 byte. Có hai cách để lưu trữ giá trị này:
Little Endian − Byte bậc thấp được lưu trữ ở địa chỉ bắt đầu (A) và byte bậc cao được lưu trữ ở địa chỉ tiếp theo (A + 1).

Big Endian − Byte bậc cao được lưu trữ ở địa chỉ bắt đầu (A) và byte bậc thấp được lưu trữ ở địa chỉ tiếp theo (A + 1).
### 1.5 inet_aton, inet_addr, inet_ntoa functions
### 1.6 inet_pton, inet_ntop functions
### 1.7 sock_ntop and related functions
### 1.8 readn, writen and readline functions

## 2. Socket TCP
## 3. Socket UDP

# I. Socket
## 1. Tổng quan về Socket
### 1.1 Giới thiệu
Socket là phương tiện cho phép giao tiếp giữa hai tiến trình khác nhau trên cùng một máy/mạng hoặc các máy/mạng khác nhau. Nói chính xác hơn, đó là cách để giao tiếp với các máy tính khác và trao đổi dữ liệu. Trong Unix, mọi hành động I/O đều được thực hiện bằng cách ghi hoặc đọc một descriptor file. Socket là end-point của giao tiếp, trong đó mỗi tiến trình có socket và một descriptor socket cho tất cả các hoạt động.
### 1.2 Socket address structure
Hầu hết các hàm socket đều yêu cầu một con trỏ đến cấu trúc địa chỉ socket làm đối số. Mỗi bộ giao thức được hỗ trợ đều định nghĩa cấu trúc địa chỉ socket riêng. Tên của các cấu trúc này bắt đầu bằng sockaddr_ và kết thúc bằng hậu tố duy nhất cho mỗi bộ giao thức.
#### a. IPv4 socket address structure
Cấu trúc địa chỉ socket IPv4, thường được gọi là "cấu trúc địa chỉ socket Internet", được đặt tên là sockaddr_in và được định nghĩa bằng cách bao gồm tiêu đề <netinet/in.h>
![image](https://github.com/user-attachments/assets/542f083d-5f27-4262-b119-82ef6f40d301)
#### b. General socket address structure

### 1.3 Value-result arguments
### 1.4 Byte ordering functions
### 1.5 inet_aton, inet_addr, inet_ntoa functions
### 1.6 inet_pton, inet_ntop functions
### 1.7 sock_ntop and related functions
### 1.8 readn, writen and readline functions

## 2. Socket TCP
## 3. Socket UDP

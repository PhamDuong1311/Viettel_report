# 1. Process
## 1.1 Cách code để tạo ra một Daemon process?
Daemon là một loại process chạy nền, không phụ thuộc vào terminal và thường thực hiện các dịch vụ hệ thống như quản lý log, xử lý tác vụ định kỳ, hoặc cung cấp dịch vụ mạng.
### Quá trình tạo Daemon
Để tạo một daemon, ta cần thực hiện các bước sau:
- Tách khỏi terminal bằng cách `fork()` và kill parent process.
- Tạo một session mới bằng cách sử dụng `setsid()`.
- Đặt lại `umask` về `0` để daemon có thể tạo file với bất kỳ quyền nào.
- Thay đổi thư mục làm việc về root (`/`) để tránh giữ các thư mục cũ không thể `unmount`.
- Đóng các file descriptor: Đóng `stdin`, `stdout`, và `stderr`.
- Vòng lặp thực hiện nhiệm vụ.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void createDaemon() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid > 0) {
        exit(0);
    }

    if (setsid() < 0) {
        perror("Setsid failed");
        exit(1);
    }

    umask(0);

    if (chdir("/") < 0) {
        perror("Chdir failed");
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        sleep(1);
    }
}

int main() {
    createDaemon();
    return 0;
}
```
## 1.2 Ví dụ cụ thể về một use case mà process chuyển đổi giữa kernel mode và user mode
Ví dụ về việc truyền, nhận dữ liệu giữa 2 thiết bị qua mạng:
### a. Tạo dữ liệu trong User Mode
- Ứng dụng cần gửi dữ liệu một chuỗi văn bản "Hello, Server!".
- Dữ liệu này được lưu trữ trong vùng nhớ của process trong user mode, có thể là trong một buffer hoặc mảng.

### b. Tạo socket và kết nối (User Mode & Kernel Mode)
Ứng dụng cần tạo một socket để bắt đầu giao tiếp với một server qua mạng. Để thực hiện điều này, ứng dụng sử dụng các system call.
#### Tạo socket (socket())
- User mode: Ứng dụng gọi hàm socket() để yêu cầu kernel tạo một socket.
- Kernel mode: Kernel thực hiện các bước sau:
    - Cấp phát bộ nhớ và tài nguyên cần thiết cho socket.
    - Xác định loại giao thức (TCP, UDP, v.v.) và loại kết nối (stream, datagram).
    - Trả về một file descriptor (FD) cho ứng dụng, để ứng dụng có thể sử dụng socket này.

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
```

#### Kết nối tới server (connect())
- User mode: Ứng dụng gọi hàm connect() để thiết lập kết nối với server.
- Kernel mode: Kernel thực hiện: Đối với TCP: Khởi tạo quá trình TCP 3-way handshake để thiết lập kết nối.
    - Gửi gói tin SYN từ client tới server.
    - Server trả lời với gói tin SYN-ACK.
    - Client phản hồi với gói tin ACK để hoàn tất kết nối.

```c
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "192.168.1.1", &server_addr.sin_addr);

connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

### c. Gửi dữ liệu (send() - User Mode)
Khi kết nối đã được thiết lập, ứng dụng gửi dữ liệu qua socket.
- User mode: Ứng dụng gọi hàm send() để truyền dữ liệu qua socket. Dữ liệu trong bộ nhớ của ứng dụng được sao chép vào kernel buffer.
- Kernel sẽ tiếp nhận dữ liệu và chuẩn bị đóng gói dữ liệu trước khi truyền qua mạng.

```c
const char *message = "Hello, Server!";
send(sockfd, message, strlen(message), 0);
```

### d. Đóng gói dữ liệu trong kernel (Kernel Mode)
Sau khi dữ liệu đã được sao chép vào kernel buffer, kernel tiến hành đóng gói dữ liệu để gửi qua mạng bằng việc gửi qua networkstack subsystem của kernel mode (qua mỗi layer của networkstack sẽ đóng gói thêm header tương ứng)

### e. Gửi dữ liệu tới phần cứng (NIC - Kernel Mode)
Kernel:
- Gói tin đã được đóng gói sẽ được gửi xuống driver của NIC (Network Interface Controller).
- NIC chuyển đổi gói tin thành các tín hiệu điện (Ethernet) hoặc tín hiệu vô tuyến (Wi-Fi) để truyền qua mạng.

### f. Truyền dữ liệu qua mạng (Physical Medium)
Dữ liệu sẽ được truyền qua mạng, bao gồm các AP, switch và router.
- Switch: Dựa trên địa chỉ MAC trong header Ethernet, chuyển tiếp gói tin đến đúng thiết bị trong mạng LAN (truyền có dây).
- AP: Dựa trên địa chỉ MAC trong header Wi-Fi, chuyển tiếp gói tin đến đúng thiết bị trong mạng LAN (truyền không dây).
- Router:
    - Đọc thông tin từ header IP để xác định đường đi của gói tin.
    - Chuyển tiếp gói tin đến địa chỉ IP đích trong mạng WAN.

### g. Thiết bị nhận xử lý dữ liệu
Khi gói tin đến thiết bị nhận, quá trình ngược lại diễn ra. Gói tin sẽ được gửi tới NIC của thiết bị đích:
- Kernel: Sẽ lần lượt đi tới NIC -> networkstack -> ...(các thành phần của kernel).
- User: Dữ liệu được gửi tới ứng dụng tương ứng qua một buffer.
Nếu thiết bị không muốn nhận toàn bộ các gói dữ liệu, sau khi đi qua networkstack của kernel, gói dữ liệu có thể được pass hoặc drop thông quan user space
=> tốn rất nhiều thời gian nếu có qua nhiều gói dữ liệu được nhận và phải đi qua networkstack và xử lý chúng 
=> có thể xử lý gói dữ liệu (pass/drop gói dữ liệu) ngay tại NIC bằng công nghệ eBPF.
## 1.3 IPC: khi gửi signal đến một process có nhiều threads thì thread nào handle signal? Có kiểm soát được việc này không

# 2. Thread
## 1.4 Tìm hiểu về deadlock khi dùng mutex: khái niệm, nguyên nhân, hệ quả
## 1.5 Giữa các thread có thể gửi signal cho nhau được không?

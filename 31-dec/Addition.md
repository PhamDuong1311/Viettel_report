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
Quy trình xử lý tín hiệu trong môi trường đa thread:
- Gửi tín hiệu đến process: Khi một tín hiệu được gửi đến một process, không phải tất cả các thread trong process đều nhận tín hiệu đó. Hệ điều hành sẽ chỉ gửi tín hiệu đến một thread trong process.
- Cách chọn thread để xử lý tín hiệu:
    - Theo mặc định, tín hiệu sẽ được gửi đến một thread bất kỳ trong process, và thread đó sẽ xử lý tín hiệu.
    - Nếu một thread đang chặn tín hiệu (blocked signal), hệ điều hành có thể gửi tín hiệu đến thread khác.
    - Trong một số trường hợp, thread chính (main thread) có thể được ưu tiên trong việc nhận tín hiệu.
- Cấu hình xử lý tín hiệu: Trong một số tình huống, bạn có thể có một chút kiểm soát đối với việc xử lý tín hiệu:
    - Sử dụng `pthread_sigmask()`: Đây là một hàm trong POSIX cho phép bạn chặn hoặc chấp nhận tín hiệu trong một thread cụ thể.
    - Sử dụng `sigaction()`: Tín hiệu có thể được xử lý theo cách được chỉ định trong sigaction() với các cờ như `SA_SIGINFO`, `SA_NODEFER`, và `SA_RESETHAND` để điều chỉnh cách thức tín hiệu được xử lý.
- Kiểm soát thread nào xử lý tín hiệu:
    - Thread chính sẽ là thread nhận tín hiệu nếu không có sự can thiệp nào.
    - Nếu muốn kiểm soát rõ ràng thread nào sẽ xử lý tín hiệu, bạn có thể chặn tín hiệu đối với tất cả các thread ngoại trừ một thread cụ thể và chỉ cho phép thread đó nhận tín hiệu.
# 2. Thread
## 2.1 Tìm hiểu về deadlock khi dùng mutex: khái niệm, nguyên nhân, hệ quả
### a. Khái niệm Deadlock:
Deadlock là một tình trạng trong lập trình đa luồng (multithreading) hoặc đa tiến trình (multiprocessing) khi các tiến trình (hoặc luồng) chờ đợi nhau một cách vô hạn, không thể tiếp tục thực thi do các tài nguyên đang bị khóa mà không ai có thể giải phóng chúng. Khi deadlock xảy ra, các tiến trình hoặc luồng sẽ không thể hoàn thành công việc của mình, dẫn đến tình trạng treo hệ thống hoặc chương trình.
### b. Nguyên nhân gây ra Deadlock:
Deadlock có thể xảy ra khi các tiến trình hoặc luồng phải yêu cầu một số tài nguyên để hoàn thành công việc của mình. Nếu các tài nguyên này được yêu cầu theo một thứ tự nhất định và không được giải phóng kịp thời, deadlock có thể xảy ra. Nguyên nhân gây ra deadlock thường được xác định qua 4 điều kiện:
- Mutual Exclusion (Loại trừ lẫn nhau):
    - Một tài nguyên không thể được chia sẻ giữa các tiến trình, chỉ có thể được một tiến trình sử dụng tại một thời điểm.
    - Ví dụ: Nếu một mutex được sử dụng để bảo vệ tài nguyên, chỉ một tiến trình có thể chiếm được mutex và truy cập tài nguyên, những tiến trình khác phải chờ đợi.
- Hold and Wait (Giữ và chờ):
    - Một tiến trình đã giữ ít nhất một tài nguyên và đang chờ đợi một hoặc nhiều tài nguyên khác mà các tiến trình khác đang giữ.
    - Ví dụ: Một tiến trình giữ mutex A và chờ đợi mutex B, trong khi một tiến trình khác giữ mutex B và chờ đợi mutex A.
- No Preemption (Không thể tước đoạt):
    - Tài nguyên không thể bị tước đoạt khỏi các tiến trình đang sử dụng chúng. Tài nguyên chỉ có thể được giải phóng khi tiến trình đang giữ tài nguyên đó tự nguyện trả lại.
    - Ví dụ: Khi một tiến trình đang giữ mutex, tiến trình đó không thể bị "tước đoạt" mutex đó bởi một tiến trình khác.
- Circular Wait (Chờ vòng tròn):
    - Tồn tại một chuỗi tiến trình trong đó mỗi tiến trình đang chờ đợi tài nguyên mà tiến trình tiếp theo trong chuỗi giữ.
    - Ví dụ: Tiến trình A chờ mutex B, tiến trình B chờ mutex C, tiến trình C chờ mutex A. Tạo thành một chu trình vòng tròn.
### c. Hệ quả của Deadlock:
- Tình trạng treo hệ thống: Deadlock có thể làm treo toàn bộ hoặc một phần của hệ thống, khi các tiến trình không thể tiếp tục thực hiện công việc.
- Tiến trình không thể tiến hành: Các tiến trình bị deadlock không thể hoàn thành công việc của chúng. Chúng cứ chờ đợi tài nguyên mà không bao giờ được cấp phát.
- Tài nguyên bị lãng phí: Deadlock có thể dẫn đến tình trạng lãng phí tài nguyên vì tài nguyên không thể được chia sẻ hoặc giải phóng.
- Khó phát hiện và xử lý: Deadlock là một vấn đề khó phát hiện và xử lý, đặc biệt là trong hệ thống phức tạp với nhiều tài nguyên và tiến trình.
### Ví dụ về Deadlock khi sử dụng Mutex:
Giả sử có hai mutex mutexA và mutexB, và hai tiến trình P1 và P2 như sau:
- P1 chiếm mutexA và yêu cầu mutexB.
- P2 chiếm mutexB và yêu cầu mutexA.
=> Kết quả là cả hai tiến trình P1 và P2 đều đang chờ nhau giải phóng mutex còn lại mà không ai giải phóng được tài nguyên của mình. Đây là một deadlock vì cả hai tiến trình đều không thể tiếp tục, dẫn đến tình trạng hệ thống treo.

```c
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;

void* thread1_func(void* arg) {
    pthread_mutex_lock(&mutexA);
    printf("Thread 1 locked mutexA\n");

    sleep(1); // Chờ để thread 2 có thể lock mutexB

    pthread_mutex_lock(&mutexB);
    printf("Thread 1 locked mutexB\n");

    pthread_mutex_unlock(&mutexB);
    pthread_mutex_unlock(&mutexA);
    return NULL;
}

void* thread2_func(void* arg) {
    pthread_mutex_lock(&mutexB);
    printf("Thread 2 locked mutexB\n");

    sleep(1); // Chờ để thread 1 có thể lock mutexA

    pthread_mutex_lock(&mutexA);
    printf("Thread 2 locked mutexA\n");

    pthread_mutex_unlock(&mutexA);
    pthread_mutex_unlock(&mutexB);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
```

Trong ví dụ trên, P1 và P2 có thể gây ra deadlock khi P1 giữ mutexA và chờ mutexB, trong khi P2 giữ mutexB và chờ mutexA.

### e. Giải pháp để tránh Deadlock:
- Xác định thứ tự tài nguyên: Một cách để tránh deadlock là quy định một thứ tự cố định trong việc yêu cầu tài nguyên. Nếu mọi tiến trình yêu cầu tài nguyên theo thứ tự cố định, thì sẽ không có chu trình chờ vòng tròn.
- Sử dụng timeout: Đặt một thời gian tối đa cho việc chờ đợi tài nguyên. Nếu một tiến trình không thể lấy tài nguyên trong khoảng thời gian đó, nó sẽ từ bỏ và thử lại sau.
- Deadlock detection và recovery: Một số hệ thống phát hiện deadlock và phục hồi bằng cách tước đoạt tài nguyên từ các tiến trình để phá vỡ chu trình.
- Khóa tài nguyên theo nhóm: Một phương pháp khác là yêu cầu tất cả các tài nguyên phải được khóa một cách nguyên tử (atomic), nghĩa là phải khóa tất cả tài nguyên cần thiết trong một thao tác duy nhất để tránh tình trạng bị chặn ở giữa quá trình.
## 2.2 Giữa các thread có thể gửi signal cho nhau được không?

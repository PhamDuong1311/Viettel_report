# What are Raw Sockets?
## 1. The Sockets API
When opening a socket, we should determine the address family, the socket type, and, optionally, the communication protocol. If we don’t specify the protocol (set to zero), the kernel will choose the default one for the given address family and socket type.

```c
socket(int family, int type, int protocol);
```

The first argument specifies the address family.

![image](https://github.com/user-attachments/assets/ff09bc82-2432-45be-b6ef-17a9aa76e3b4)

The second argument specifies the socket type.

![image](https://github.com/user-attachments/assets/b29bfbba-2326-4932-a7fc-5642b5e7c746)

The last argument shows the protocol.

![image](https://github.com/user-attachments/assets/22790fcb-6944-416a-8164-6459505a81ca)

## 2.Raw Socket
Commonly, the socket API is used for inter-process communication at the transport layer (OSI layer 4). However, some special socket types can be employed to access the network layer (OSI layer 3) and the data link layer (OSI layer 2). These socket types are called raw sockets.

**Raw sockets are used when we want to access the network layer and the data link layer directly.** For instance, we can use raw sockets to implement a network protocol below the transport layer, like `ICMP`, `ARP`, or `OSPF`. Furthermore, we can use them for monitoring a network, sniffing packets, or hacking, by sending forged packets.

### 2.1 Raw Sockets at the Network Layer
If we want to open a raw socket at the network layer (OSI layer 3), we must specify the AF_INET address family and the SOCK_RAW type. In the last argument, we need to define the Internet protocol number, which is included in the IP header field “Protocol”:

```c
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
```

Moreover, at the network layer (Internet Protocol), we have two options regarding how to handle the IP packet header construction:
- We can let the kernel construct the IP header for us (the IP_HDRINCL option is not set using setsockopt())
- We can build the IP header ourselves (the IP_HDRINCL option is set using setsockopt())

If we opt for the second option, we have total control over the IP header fields like Time-to-Live (employed, for instance, by traceroute) and source IP address (used for IP address spoofing), but we have to calculate the checksum ourselves. This way works the well-known ping command, by sending an ICMP echo request message:

![image](https://github.com/user-attachments/assets/57765508-f30a-438c-9f0b-4a5d231c1576)

### 2.2 Raw Sockets at the Link Layer
If we want to open a raw socket at the data link layer (OSI layer 2), we need to specify the AF_PACKET address family and the SOCK_RAW socket type:

```c
socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
```

At the data link layer, we need to build the frame by ourselves. Moreover, on a computer having more than one network interface controller (NIC), we should also specify the interface at which we want the frame to send or receive.
## 3. Summary
To summarize the differences between raw sockets at OSI layers 2 and 3 and traditional sockets at OSI layer 4, we provide a table comparing the essential features side by side:

![image](https://github.com/user-attachments/assets/ecc84dcb-38b0-4e01-86c4-e2b6febd8c80)


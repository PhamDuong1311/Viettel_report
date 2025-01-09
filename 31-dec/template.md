# Software Design Description
## For <project name>

Version 0.1
Prepared by Pham Hong Duong  
 
Table of Contents
=================
* 1 [Introduction](#1-introduction)
  * 1.1 [Document Purpose](#11-document-purpose)
  * 1.2 [Product Scope](#12-product-scope)
  * 1.3 [Definitions, Acronyms and Abbreviations](#13-definitions-acronyms-and-abbreviations)
  * 1.4 [References](#14-references)
  * 1.5 [Document Overview](#15-document-overview)
* 2 [Design](#2-overall-description)
  * 2.1 [Stakeholder concerns](#21-stakeholder-concerns)
  * 2.2 [Selected Viewpoints](#22-selected-viewpoints)
    * 2.2.1 [Context](#221-context)
    * 2.2.2 [Composition](#222-composition)
    * 2.2.3 [Logical](#223-logical)
    * 2.2.4 [Dependency](#224-dependency)
    * 2.2.5 [Information](#225-information)
    * 2.2.6 [Patterns](#226-patterns)
    * 2.2.7 [Interface](#227-interface)
    * 2.2.8 [Structure](#228-structure)
    * 2.2.9 [Interaction](#229-interaction)
    * 2.2.10 [State dynamics](#2210-state-dynamics)
    * 2.2.11 [Algorithm](#2211-algorithm)
    * 2.2.12 [Resources](#2212-resources)
    * 2.2.13 [Physical](#2213-physical)
    * 2.2.14 [Deployment](#2214-deployment)
    * 2.2.15 [Concurrency](#2215-concurrency)
    * 2.2.16 [Behavioral](#2216-behavioral)
    * 2.2.17 [Test cases](#2217-test-cases)
* 3 [Appendixes](#3-appendixes)

## 1. Introduction
Document này là cung cấp mô tả thiết kế phần mềm cho việc implement giao thức **ARP** trong **userspace** bằng **raw socket**. Document sẽ mô tả các yêu cầu, thiết kế, và các chi tiết implement phần mềm **ARP**, giúp lập trình viên và người phát triển hiểu rõ về cách hệ thống hoạt động và tương tác với mạng.
### 1.1 Document Purpose
Sản phẩm này sẽ triển khai giao thức **ARP** trong **userspace**, sử dụng **raw socket** để nhận và gửi các gói **ARP Request** và **ARP Reply**. Các chức năng chính bao gồm:
- Quản lý **ARP cache** với **timeout** là `15` giây.
- Gửi và nhận các gói **ARP Request** và **ARP Reply**.
- Tương tác với hệ thống mạng mà không thông qua **kernel network stack**.
### 1.2 Subject Scope

### 1.3 Definitions, Acronyms and Abbreviations
- **ARP**: Address Resolution Protocol, giao thức mapping địa chỉ IP -> địa chỉ MAC thuộc L2 trong cùng 1 network.
- **Raw Socket**: Loại socket nhận data từ L2 trong kernel network stack và bypass lên thẳng userspace.
- **Cache Timeout**: Thời gian sống của một entry trong ARP table để lookup sự mapping giữa địa chỉ IP và địa chỉ MAC, sau thời gian này entry sẽ bị xóa.
### 1.4 References
- RFC 826 - "Ethernet Address Resolution Protocol"
- "Unix Network Programming" - Example of Raw Sockets
- "Linux Interface Programming"
- etc... 
### 1.5 Document Overview
Tài liệu này được chia thành các phần sau:
- Phần 1: Mục đích tài liệu và mô tả hệ thống.
- Phần 2: Thiết kế phần mềm chi tiết.
- Phần 3: Các phụ lục và tài liệu tham khảo.
## 2. Design
### 2.1 Stakeholder Concerns

### 2.2 Selected Viewpoints
#### 2.2.1 Context
Thiết kế này định nghĩa một ứng dụng trong không gian người dùng, tương tác trực tiếp với mạng thông qua raw socket. Hệ thống sẽ nhận các gói ARP từ mạng, xử lý chúng, và gửi lại phản hồi nếu cần thiết.
#### 2.2.2 Composition
Ứng dụng sẽ có các thành phần sau:
Raw Socket - Dùng để nhận và gửi gói ARP.
ARP Cache - Lưu trữ các mục ARP, bao gồm địa chỉ MAC và IP.
Quản lý thời gian sống - Cơ chế kiểm tra và xóa mục ARP cache khi hết hạn
#### 2.2.3 Logical
Các thành phần logic chính:
Socket Layer: Tạo và quản lý raw socket.
ARP Handler: Xử lý ARP request và reply.
Cache Manager: Quản lý ARP cache, bao gồm thêm, xóa và kiểm tra thời gian sống của cache.
#### 2.2.4 Dependency
Raw socket: Sử dụng raw socket trong không gian người dùng.
ARP Cache: Quản lý bộ nhớ đệm ARP để lưu trữ các mục và thời gian sống của chúng.
Time Management: Cơ chế để kiểm tra và xử lý cache timeout
#### 2.2.5 Information
Thông tin cần lưu trữ trong ARP cache bao gồm:
Địa chỉ IP.
Địa chỉ MAC.
Thời gian thêm mục vào cache.
#### 2.2.6 Patterns

#### 2.2.7 Interface
Giao diện chương trình sẽ sử dụng các API raw socket chuẩn như socket(), recvfrom(), sendto(). Ngoài ra, sẽ có giao diện dòng lệnh để người dùng kiểm tra ARP cache.
#### 2.2.8 Structure
3.2.7 Structure
ARP Cache: Cấu trúc lưu trữ dữ liệu ARP.
Raw Socket Management: Quản lý các raw socket để nhận và gửi gói ARP.
Timeout Handler: Kiểm tra và xóa các mục trong cache khi hết thời gian.
#### 2.2.9 Interaction
Ứng dụng nhận các gói ARP từ raw socket, sau đó phân tích và kiểm tra các yêu cầu ARP.
Nếu là ARP request, chương trình trả lời bằng ARP reply và thêm thông tin vào cache.
#### 2.2.10 State dynamics
Ứng dụng có các trạng thái:
Idle: Chương trình chờ nhận gói ARP.
Processing: Chương trình xử lý gói ARP (Request hoặc Reply).
Cache Management: Quản lý ARP cache, kiểm tra và loại bỏ các mục hết hạn.
#### 2.2.11 Algorithm
Ứng dụng có các trạng thái:
Idle: Chương trình chờ nhận gói ARP.
Processing: Chương trình xử lý gói ARP (Request hoặc Reply).
Cache Management: Quản lý ARP cache, kiểm tra và loại bỏ các mục hết hạn.
#### 2.2.12 Resources
Bộ nhớ: Quản lý bộ nhớ ARP cache.
Raw socket: Để nhận và gửi gói ARP.
Thời gian sống (Timeout): Để quản lý cache.
#### 2.2.13 Physical
Ứng dụng sẽ chạy trên hệ điều hành Linux với raw socket hỗ trợ
#### 2.2.14 Deployment
Ứng dụng sẽ được triển khai trên máy chủ Linux, yêu cầu quyền root để tạo và sử dụng raw socket.
#### 2.2.15 Concurrency
Ứng dụng sẽ sử dụng một luồng đơn để nhận và xử lý các gói ARP. Quản lý ARP cache và xử lý timeout sẽ được thực hiện trong cùng một luồng.
#### 2.2.16 Behavioral
Chương trình sẽ hoạt động liên tục, nhận và xử lý các gói ARP, đồng thời quản lý ARP cache và xử lý timeout.
#### 2.2.17 Test cases

## 3. Appendixes
Phụ lục có thể bao gồm các tài liệu tham khảo, mã nguồn chi tiết, và các ví dụ về cách sử dụng hệ thống.

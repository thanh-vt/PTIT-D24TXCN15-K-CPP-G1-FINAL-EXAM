# Hệ thống đăng nhập và đăng ký tài khoản và quản lý ví điểm thưởng

Dự án Hệ thống đăng nhập và đăng ký chủ yếu liên quan đến quá trình đăng ký người dùng. Thông tin xác thực người dùng như tên người dùng và mật khẩu được yêu cầu từ người dùng. Nếu người dùng đăng ký thành công thì với thông tin xác thực được cung cấp, một bản ghi (có thể là tập tin) của người dùng cụ thể sẽ được tạo trong cơ sở dữ liệu (CSDL).

Ví điểm thưởng là cơ chế quản lý các lượng (đơn vị : điểm) được dùng để quy đổi ra hàng hóa (còn gọi là hoạt động mua - bán). Giữa các ví có giao dịch chuyển điểm.

## Thành viên nhóm và phân công công việc

1. **Vũ Tất Thành** - __K24DTCN638__
   - Phụ trách: Thiết kế hệ thống User và Authentication
   - Công việc: Phát triển class User, hệ thống đăng nhập, sinh mật khẩu tự động, thay đổi mật khẩu

2. **Nguyễn Quang Đạo** - __K24DTCN639__
   - Phụ trách: Phát triển hệ thống Wallet và Transaction
   - Công việc: Class Wallet, giao dịch atomic, chuyển điểm, transaction log

3. **Nguyễn Việt** - __K24DTCN640__
   - Phụ trách: Hệ thống Database và lưu trữ dữ liệu
   - Công việc: Class Database, file I/O, mã hóa mật khẩu (hash), backup/restore

4. **Nguyễn Viết Tùng** - __K24DTCN633__
   - Phụ trách: Giao diện người dùng và chức năng quản lý
   - Công việc: Class UI, chức năng người dùng quản lý, OTP/2FA, menu system

## Đặc tả chức năng

### A. Các chức năng chính

**1. Tạo mới tài khoản:**
- Người dùng nhập dữ liệu để tạo tài khoản
- Nhân viên quản lý có thể tạo tài khoản hộ người dùng
- Cấu trúc dữ liệu User class quản lý thông tin tài khoản

**2. Lưu trữ dữ liệu:**
- Lưu dữ liệu tài khoản người dùng xuống tập tin
- Mã hóa mật khẩu bằng hàm băm (SHA-256)
- Hệ thống backup và restore dữ liệu

**3. Quản lý đăng nhập:**
- **3.1** Sinh mật khẩu tự động khi nhân viên quản lý tạo tài khoản
- **3.2** Thay đổi mật khẩu (yêu cầu thay đổi ngay lần đăng nhập đầu với mật khẩu tự sinh)
- **3.3** OTP - One Time Password: bảo mật hai lớp để xác thực giao dịch
- **3.4** Đăng nhập: xác thực thông tin và cho phép vào hệ thống

### B. Phân chia người dùng và chức năng

**a/ Người dùng thông thường:**
- Chỉ được phép truy xuất thông tin cá nhân của mình
- Được phép điều chỉnh tên, mật khẩu
- Khi điều chỉnh xong, hệ thống gửi mã OTP để xác nhận thao tác cập nhật

**b/ Người dùng quản lý:**
- **b.1** Theo dõi danh sách nhóm
- **b.2** Tạo thêm tài khoản mới
- **b.3** Điều chỉnh thông tin của tài khoản khi có yêu cầu từ chủ tài khoản (làm hộ)
- Khi điều chỉnh xong, hệ thống gửi mã OTP đến chủ tài khoản để xác nhận

### C. Quản lý hoạt động ví

**Cấu trúc ví:**
- Mỗi người dùng có một ví với mã số định danh duy nhất
- Ví tổng: nguồn duy nhất sinh ra tổng số điểm trong hệ thống

**Giao dịch chuyển điểm (Atomic Transaction):**
1. Tìm và mở ví A (ví chủ giao dịch)
2. Tìm và mở ví B (ví đích)
3. Giao dịch atomic:
   - Kiểm tra `A.balance >= amount`
   - Nếu đủ: `A.balance -= amount` và `B.balance += amount`
   - Nếu không đủ: "low balance. Cannot proceed."
   - Bất kỳ exception nào đều dẫn đến rollback toàn bộ giao dịch

**Tính năng bổ sung:**
- Sử dụng OTP để xác nhận giao dịch chuyển điểm
- Ghi nhận giao dịch trong transaction log
- Báo cáo số dư và lịch sử giao dịch với trạng thái thực hiện

## Yêu cầu hệ thống

- C++17 compatible compiler (Visual Studio 2019+, GCC 8+, Clang 7+)
- CMake 3.10 hoặc cao hơn
- Windows/Linux/macOS

## Hướng dẫn build chương trình

1. Clone repository:
```bash
git clone https://github.com/thanh-vt/PTIT-D24TXCN15-K-CPP-G1-FINAL-EXAM.git
cd PTIT-D24TXCN15-K-CPP-G1-FINAL-EXAM
```

2. Tạo thư mục build và build project:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

3. Chạy chương trình:
```bash
# Windows
Release\ewallet-manager.exe

# Linux/macOS
./ewallet-manager
```

## Hướng dẫn sử dụng

### Đăng nhập Admin:
- Username: `admin`
- Password: `admin123`

### Menu chính:
1. **Login** - Đăng nhập vào hệ thống
2. **Register** - Đăng ký tài khoản mới
3. **Exit** - Thoát chương trình

### Chức năng User thường:
- Xem thông tin cá nhân
- Quản lý ví (xem số dư, chuyển điểm)
- Xem lịch sử giao dịch
- Thay đổi mật khẩu
- Bật/tắt 2FA

### Chức năng Admin
- **b.1** Xem tất cả người dùng (theo dõi danh sách nhóm)
- **b.2** Tạo tài khoản mới (tạo thêm tài khoản mới)
- **b.3** Cập nhật thông tin người dùng (điều chỉnh thông tin tài khoản)
- **C** Xem tất cả ví trong hệ thống (báo cáo số dư)
- **C** Xem tất cả giao dịch (lịch sử giao dịch)

## Lưu trữ dữ liệu

- Dữ liệu người dùng: `users.dat`
- Dữ liệu ví: `wallets.dat`
- Dữ liệu giao dịch: `transactions.dat`
- Tự động lưu khi thoát chương trình
- Tự động load khi khởi động

## Bảo mật

- Mã hóa mật khẩu SHA-256
- Xác thực 2FA (tạm thời sử dụng mã test "123456")
- Kiểm soát quyền truy cập bằng flag isAdmin
- Bảo vệ admin user khỏi bị xóa
- Ghi log tất cả thao tác admin

## Nguồn tham khảo

- [1] CPP_OTP, https://github.com/patzol768/cpp-otp, truy cập lần cuối ngày 04/10/2024
- [2] COTP, https://github.com/tilkinsc/COTP, truy cập lần cuối ngày 04/10/2024
- [3] ACID, https://200lab.io/blog/acid-la-gi/, truy cập lần cuối ngày 04/10/2024
- [C++ Reference](https://cppreference.com/)
- [CMake Documentation](https://cmake.org/documentation/)

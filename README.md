# E-Wallet Management System

Hệ thống quản lý ví điện tử với giao diện văn bản, hỗ trợ xác thực người dùng, 2FA và chuyển điểm.

## Thành viên nhóm và phân công công việc

1. **Vũ Tất Thành** - __K24DTCN638__
   - Phụ trách: Thiết kế hệ thống User và Authentication
   - Công việc: Phát triển class User, hệ thống đăng nhập, 2FA, mã hóa mật khẩu

2. **Nguyễn Quang Đạo** - __K24DTCN639__
   - Phụ trách: Phát triển hệ thống Wallet và Transaction
   - Công việc: Class Wallet, xử lý giao dịch, chuyển điểm, lịch sử giao dịch

3. **Nguyễn Việt** - __K24DTCN640__
   - Phụ trách: Hệ thống Database và lưu trữ dữ liệu
   - Công việc: Class Database, file I/O, serialization, backup/restore

4. **Nguyễn Viết Tùng** - __K24DTCN633__
   - Phụ trách: Giao diện người dùng và chức năng Admin
   - Công việc: Class UI, Admin functions, menu system, user experience

## Đặc tả chức năng

### Chức năng chính:
1. **Quản lý người dùng:**
   - Đăng ký tài khoản mới với mật khẩu tự động tạo
   - Đăng nhập với username/password
   - Bật/tắt xác thực 2FA
   - Thay đổi mật khẩu
   - Xem và cập nhật thông tin cá nhân

2. **Quản lý ví điện tử:**
   - Tạo và quản lý ví
   - Chuyển điểm giữa các ví
   - Xem số dư ví
   - Xem lịch sử giao dịch

3. **Chức năng Admin (chỉ dành cho user có flag isAdmin):**
   - Xem tất cả người dùng trong hệ thống
   - Tạo tài khoản người dùng mới
   - Cập nhật thông tin người dùng
   - Xóa tài khoản người dùng
   - Reset mật khẩu người dùng
   - Xem tất cả ví trong hệ thống
   - Quản lý ví (thêm/trừ/set số dư)
   - Xem tất cả giao dịch hệ thống
   - Thống kê tổng quan hệ thống

4. **Bảo mật:**
   - Mã hóa mật khẩu bằng SHA-256
   - Xác thực hai yếu tố (2FA) sử dụng TOTP
   - Rollback giao dịch khi thất bại
   - Lưu trữ dữ liệu an toàn

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

### Chức năng Admin:
- Xem tất cả người dùng
- Tạo/cập nhật/xóa tài khoản
- Reset mật khẩu người dùng
- Quản lý tất cả ví trong hệ thống
- Thêm/trừ điểm từ ví bất kỳ
- Xem tất cả giao dịch
- Thống kê tổng quan hệ thống

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

## Tài liệu tham khảo

- [C++ Reference](https://cppreference.com/)
- [CMake Documentation](https://cmake.org/documentation/)
- [SHA-256 Hashing](https://en.wikipedia.org/wiki/SHA-2)
- [TOTP Algorithm](https://tools.ietf.org/html/rfc6238)

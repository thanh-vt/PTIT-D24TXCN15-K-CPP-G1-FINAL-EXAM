# 🏦 Hệ thống đăng nhập và đăng ký tài khoản và quản lý ví điểm thưởng

**Bài tập lớn môn Ngôn ngữ C++ - Lớp D24TXCN15-K - Nhóm 1**

## 📋 Giới thiệu dự án

Hệ thống quản lý ví điện tử được phát triển bằng C++ với giao diện dòng lệnh, cung cấp đầy đủ các chức năng quản lý tài khoản người dùng và ví điểm thưởng. Dự án được thiết kế theo yêu cầu đề bài với các tính năng:

- 🔐 **Hệ thống đăng nhập và đăng ký**: Tạo tài khoản, xác thực người dùng
- 👤 **Quản lý tài khoản**: Thông tin cá nhân, thay đổi mật khẩu, OTP
- 💰 **Quản lý ví điểm thưởng**: Chuyển điểm, theo dõi giao dịch
- 🛡️ **Hệ thống Admin**: Quản trị toàn bộ hệ thống
- 💾 **Lưu trữ dữ liệu**: File database với checksum bảo mật

## 👥 Thành viên nhóm và phân công công việc

| STT | Họ và tên            | Mã sinh viên | Công việc được giao                                                             |
|-----|----------------------|--------------|---------------------------------------------------------------------------------|
| 1 | **Vũ Tất Thành**     | K24DTCN638 | Thiết kế UI, giao diện người dùng, đăng nhập, đăng ký, 2FA, kiểm thử chất lượng |
| 2 | **Nguyễn Quang Đạo** | K24DTCN584 | Thiết kế hệ thống ví điện tử, xử lý giao dịch, lịch sử giao dịch                |
| 3 | **Nguyễn Văn Việt**  | K24DTCN651 | Phát triển chức năng thêm sửa xóa user (admin)                                  |
| 4 | **Nguyễn Viết Tùng** | K24DTCN633 | Thiết kế database,  tích hợp tổng thể                                           |

### 📋 Chi tiết công việc thực hiện

**Vũ Tất Thành:**
- Thiết kế giao diện dòng lệnh (CLI) cho toàn bộ hệ thống
- Xử lý input/output và điều hướng menu
- Kiểm thử và đảm bảo chất lượng sản phẩm
- Phát triển module đăng nhập và đăng ký tài khoản

**Nguyễn Văn Việt:**
- Xử lý thêm sửa xóa user (admin)

**Nguyễn Quang Đạo:**
- Thiết kế và phát triển hệ thống ví điểm thưởng
- Xử lý giao dịch chuyển điểm giữa các ví
- Lưu trữ thông tin lịch sử giao dịch

**Nguyễn Viết Tùng:**
- Thiết kế kiến trúc database và hệ thống lưu trữ
- Tích hợp các module và đảm bảo hoạt động tổng thể
- Backup/restore


## 🚀 Đặc tả chức năng đầy đủ

### A. Tạo mới tài khoản khi đăng ký
- ✅ **Tạo tài khoản người dùng**: Hệ thống cho phép tạo tài khoản với thông tin đầy đủ
- ✅ **Nhân viên nhập liệu**: Admin có thể tạo tài khoản hộ người dùng
- ✅ **Cấu trúc dữ liệu**: Lớp User quản lý thông tin tài khoản hoàn chỉnh

### B. Lưu trữ dữ liệu
- ✅ **File database**: Sử dụng file binary với định dạng có cấu trúc
- ✅ **Mật khẩu bảo mật**: Lưu trữ mật khẩu với hash function
- ✅ **Backup/Restore**: Hệ thống sao lưu và phục hồi dữ liệu hoàn chỉnh

### C. Quản lý đăng nhập
- ✅ **Sinh mật khẩu tự động**: Tạo mật khẩu ngẫu nhiên khi admin tạo tài khoản
- ✅ **Thay đổi mật khẩu**: Người dùng có thể thay đổi mật khẩu
- ✅ **TOTP**: Xác thực hai lớp cho các thao tác quan trọng
- ✅ **Đăng nhập**: Xác thực người dùng

### D. Phân chia người dùng và chức năng
- ✅ **Người dùng thông thường**: Truy xuất và điều chỉnh thông tin cá nhân
- ✅ **Người dùng quản lý (Admin)**: Đầy đủ chức năng quản trị hệ thống

### E. Quản lý hoạt động ví
- ✅ **Ví điểm thưởng**: Mỗi người dùng có ví với mã định danh duy nhất
- ✅ **Giao dịch chuyển điểm**: Xử lý atomic giữa các ví
- ✅ **Lịch sử giao dịch**: Ghi nhận và theo dõi tất cả giao dịch
- ✅ **Báo cáo**: Theo dõi số dư và lịch sử giao dịch

## 🛠️ Yêu cầu hệ thống

- Trình biên dịch C++ hỗ trợ C++17
- CMake (tùy chọn, có thể dùng g++ trực tiếp)

## 📦 Cách tải, dịch và chạy chương trình

### Bước 1: Tải mã nguồn
```bash
git clone https://github.com/thanh-vt/PTIT-D24TXCN15-K-CPP-G1-FINAL-EXAM.git
cd PTIT-D24TXCN15-K-CPP-G1-FINAL-EXAM
```

### Bước 2: Biên dịch chương trình

**Cách 1: Sử dụng CMake (Khuyến nghị)**
#### Trên windows (Cài cmake và build tool MinGW)
```cmd
cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" -S . -B .\cmake-build-debug
cmake --build .\cmake-build-debug
```

#### Trên Linux (Cài cmake và bộ build tool cho distro tương ứng, vd build-essentials cho Debian-based distro, Development Tools cho RHEL-based distro, build-base cho Alpine)
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -S . -B ./cmake-build-debug
cmake --build ./cmake-build-debug
```

**Cách 2: Sử dụng build tool cho platform tương ứng (tự tìm hiểu)**
- Trên windows: Cài build tool ví dụ như Ninja và viết Ninja file để chỉ định cấu hình compile các file source code và header
- Trên linux: Cài GNU make (thường có sẵn trên các distro Linux) và viết makefile để chỉ định cấu hình compile các file source code và header

**Cách 3: Sử dụng compiler g++**
- Trên windows (Cài bộ toolchain build C/C++ vd như MinGW, SygWin, Visual Studio Build Tools hoặc dùng WSL rồi cài toolchain của linux)
```cmd
g++ -std=c++17 .\src\*.cpp .\deps\libcotp.a -Iinclude -Ideps\cpp-otp -static -o ewallet-manager.exe
```

- Trên Linux (Cài bộ toolchains build C/C+ cho Linux distro tương ứng, vd build-essentials cho Debian-based distro, Development Tools cho RHEL-based distro, build-base cho Alpine)
```bash
g++ -std=c++17 ./src/*.cpp ./deps/libcotp.a -Iinclude -Ideps/cpp-otp -static -o ewallet-manager
```

### Bước 3: Chạy chương trình
- Trên windows (Cài cmake và build tool MinGW)
```cmd
.\ewallet-manager.exe
```

- Trên Linux (Cài cmake và bộ build tool cho distro tương ứng, vd build-essentials cho Debian-based distro, Development Tools cho RHEL-based distro, build-base cho Alpine)
```bash
./ewallet-manager
```

## 🎮 Hướng dẫn sử dụng

### Giao diện chính
Khi chạy chương trình, bạn sẽ thấy menu chính với 3 lựa chọn:

```
=== E-Wallet Management System ===

1. Login
2. Register
3. Exit

Enter your choice:
```

### Đăng nhập Admin
1. Chạy chương trình
2. Chọn "1. Login"
3. Nhập username: `admin`
4. Nhập password: `admin123
5. Hệ thống sẽ tự động nhận diện và chuyển đến Admin Dashboard


## 📁 Cấu trúc dự án

```
├── deps/                  # Source code các thư viện
│   └── cpp-otp            # Source code thư viện otp
├── src/                   # Mã nguồn
│   ├── main.cpp           # Entry point
│   ├── database.cpp       # Database logic
│   ├── user.cpp           # User logic
│   ├── wallet.cpp         # Wallet logic
│   └── ui.cpp             # Giao diện CLI
├── include/               # Header files
│   ├── database.hpp       # header khai báo các function database
│   ├── hmac.hpp           # header chứa các hàm băm/mã hóa
│   ├── ui.hpp             # header khai báo các function UI
│   ├── user.hpp           # header khai báo class User
│   ├── user.hpp           # header khai báo class Wallet
│   └── wallet.hpp         # header khai báo class Transaction
├── README.md              # File này
└── CMakeLists.txt         # File cấu hình cmake
```

## 📚 Tài liệu tham khảo

### Nguồn tham khảo chính
1. **CPP_OTP**: https://github.com/patzol768/cpp-otp - Thư viện OTP cho C++
2. **COTP**: https://github.com/tilkinsc/COTP - Thư viện OTP tương thích C
3. **ACID Properties**: https://200lab.io/blog/acid-la-gi/ - Tính chất ACID trong database

### Tài liệu dự án
- **Mã nguồn**: Có chú thích đầy đủ, mô tả input/output và thủ tục xử lý

## 🎯 Đặc điểm kỹ thuật

### Kiến trúc hệ thống
- **Design Pattern**: Singleton cho Database, Factory cho UI components
- **File Format**: Binary-based format for fast read/write operations
- **Security**: Simple checksum validation, password hashing
- **Error Handling**: Exception handling đầy đủ với try-catch blocks

### Tính năng bảo mật
- **Password Hashing**: Mật khẩu được hash trước khi lưu trữ
- **OTP Validation**: Xác thực hai lớp cho các thao tác quan trọng
- **Backup/Restore**: Sao lưu định kỳ với timestamp

### Hiệu năng và tối ưu
- **Memory Management**: Sử dụng smart pointers (shared_ptr)
- **File I/O**: Optimized text format với simple checksum
- **Code Quality**: Clean code với comments tiếng Việt đầy đủ

---

## 📋 Tóm tắt dự án

**Hệ thống đăng nhập và đăng ký tài khoản và quản lý ví điểm thưởng** là một ứng dụng C++ hoàn chỉnh được phát triển theo yêu cầu đề bài. Dự án cung cấp đầy đủ các chức năng quản lý tài khoản, ví điện tử, và hệ thống admin với giao diện dòng lệnh thân thiện.

**Nhóm 1 - Lớp D24TXCN15-K**
- Vũ Tất Thành (K24DTCN638)
- Nguyễn Quang Đạo (K24DTCN584)
- Nguyễn Văn Việt (K24DTCN5651)
- Nguyễn Viết Tùng (K24DTCN633)

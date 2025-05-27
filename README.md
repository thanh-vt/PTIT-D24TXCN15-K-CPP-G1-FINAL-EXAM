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

| STT | Họ và tên | Mã sinh viên | Công việc được giao |
|-----|-----------|--------------|---------------------|
| 1 | **Vũ Tất Thành** | K24DTCN638 | Thiết kế UI, giao diện người dùng, kiểm thử chất lượng |
| 2 | **Nguyễn Quang Đạo** | K24DTCN--- | Phát triển chức năng đăng nhập/đăng ký, quản lý session |
| 3 | **Nguyễn Việt** | K24DTCN--- | Thiết kế hệ thống ví điện tử, xử lý giao dịch |
| 4 | **Nguyễn Viết Tùng** | K24DTCN633 | Thiết kế database, hệ thống admin, tích hợp tổng thể |

### 📋 Chi tiết công việc thực hiện

**Vũ Tất Thành:**
- Thiết kế giao diện dòng lệnh (CLI) cho toàn bộ hệ thống
- Xử lý input/output và điều hướng menu
- Kiểm thử và đảm bảo chất lượng sản phẩm
- Tối ưu hóa trải nghiệm người dùng

**Nguyễn Quang Đạo:**
- Phát triển module đăng nhập và đăng ký tài khoản
- Xử lý xác thực người dùng và quản lý session
- Tích hợp bảo mật cho hệ thống đăng nhập

**Nguyễn Việt:**
- Thiết kế và phát triển hệ thống ví điểm thưởng
- Xử lý giao dịch chuyển điểm giữa các ví
- Đảm bảo tính atomic của các giao dịch

**Nguyễn Viết Tùng:**
- Thiết kế kiến trúc database và hệ thống lưu trữ
- Phát triển đầy đủ chức năng admin
- Tích hợp các module và đảm bảo hoạt động tổng thể
- Backup/restore và bảo mật dữ liệu


## 🚀 Đặc tả chức năng đầy đủ

### A. Tạo mới tài khoản khi đăng ký
- ✅ **Tạo tài khoản người dùng**: Hệ thống cho phép tạo tài khoản với thông tin đầy đủ
- ✅ **Nhân viên nhập liệu**: Admin có thể tạo tài khoản hộ người dùng
- ✅ **Cấu trúc dữ liệu**: Lớp AdminUser quản lý thông tin tài khoản hoàn chỉnh

### B. Lưu trữ dữ liệu
- ✅ **File database**: Sử dụng file văn bản với định dạng có cấu trúc
- ✅ **Mật khẩu bảo mật**: Lưu trữ mật khẩu với hash function
- ✅ **Backup/Restore**: Hệ thống sao lưu và phục hồi dữ liệu hoàn chỉnh

### C. Quản lý đăng nhập
- ✅ **Sinh mật khẩu tự động**: Tạo mật khẩu ngẫu nhiên khi admin tạo tài khoản
- ✅ **Thay đổi mật khẩu**: Người dùng có thể thay đổi mật khẩu
- ✅ **OTP**: Xác thực hai lớp cho các thao tác quan trọng
- ✅ **Đăng nhập**: Xác thực người dùng và quản lý session

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

**Cách 1: Sử dụng g++ (Khuyến nghị)**
```bash
g++ -std=c++17 src/main.cpp src/ui.cpp src/database.cpp -Iinclude -o ewallet_manager
```

**Cách 2: Sử dụng CMake**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Bước 3: Chạy chương trình
```bash
./ewallet_manager
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
4. Nhập password: `admin`
5. Hệ thống sẽ tự động nhận diện và chuyển đến Admin Dashboard

### Admin Dashboard - Các chức năng đầy đủ
Sau khi đăng nhập admin thành công, bạn sẽ thấy menu admin với 11 chức năng:

1. **View All Users** - Xem danh sách tất cả người dùng
2. **Create User** - Tạo tài khoản người dùng mới (tự động sinh mật khẩu)
3. **Update User** - Cập nhật thông tin người dùng (có OTP validation)
4. **Delete User** - Xóa tài khoản người dùng
5. **View All Wallets** - Xem danh sách tất cả ví
6. **View Wallet Details** - Xem chi tiết ví cụ thể
7. **View System Statistics** - Thống kê tổng quan hệ thống
8. **View Transaction Log** - Xem lịch sử giao dịch
9. **Backup Database** - Sao lưu database
10. **Restore Database** - Phục hồi database từ backup
11. **Logout** - Đăng xuất và quay về menu chính

### Thông tin đăng nhập mặc định
- **Admin Username**: `admin`
- **Admin Password**: `admin`

## 📁 Cấu trúc dự án

```
├── src/                    # Mã nguồn
│   ├── main.cpp           # Entry point
│   ├── database.cpp       # Database logic (đã đơn giản hóa)
│   └── ui.cpp             # Giao diện CLI
├── include/               # Header files
│   ├── database.hpp
│   └── ui.h
├── README.md              # File này
├── DATABASE_README.md     # Tài liệu database chi tiết
├── SIMPLIFICATION_SUMMARY.md  # Tóm tắt đơn giản hóa
└── CMakeLists.txt         # Build configuration
```

## 📚 Tài liệu tham khảo

### Nguồn tham khảo chính
1. **CPP_OTP**: https://github.com/patzol768/cpp-otp - Thư viện OTP cho C++
2. **COTP**: https://github.com/tilkinsc/COTP - Thư viện OTP tương thích C
3. **ACID Properties**: https://200lab.io/blog/acid-la-gi/ - Tính chất ACID trong database

### Tài liệu dự án
- **[DATABASE_README.md](DATABASE_README.md)**: Chi tiết về hệ thống database
- **Mã nguồn**: Có chú thích đầy đủ, mô tả input/output và thủ tục xử lý

## 🎯 Đặc điểm kỹ thuật

### Kiến trúc hệ thống
- **Design Pattern**: Singleton cho Database, Factory cho UI components
- **File Format**: Text-based với delimiter `|` cho dễ đọc và debug
- **Security**: Simple checksum validation, password hashing
- **Error Handling**: Exception handling đầy đủ với try-catch blocks

### Tính năng bảo mật
- **Password Hashing**: Mật khẩu được hash trước khi lưu trữ
- **OTP Validation**: Xác thực hai lớp cho các thao tác quan trọng
- **Atomic Transactions**: Đảm bảo tính toàn vẹn dữ liệu khi chuyển điểm
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
- Nguyễn Quang Đạo
- Nguyễn Việt
- Nguyễn Viết Tùng (K24DTCN633)

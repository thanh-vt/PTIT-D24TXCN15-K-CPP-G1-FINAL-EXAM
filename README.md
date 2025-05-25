# 🏦 E-Wallet Management System

**Bài tập lớn môn Ngôn ngữ C++ - Lớp D24TXCN15-K - Nhóm 1**

## 📋 Giới thiệu

Hệ thống quản lý ví điện tử dòng lệnh được phát triển bằng C++, cung cấp đầy đủ các chức năng:

- 👤 Quản lý tài khoản người dùng (đăng ký, đăng nhập)
- 🔐 Xác thực OTP cho các thao tác quan trọng
- 💰 Quản lý ví và giao dịch chuyển điểm
- 🛡️ Hệ thống admin với đầy đủ chức năng quản trị
- 💾 Lưu trữ dữ liệu an toàn với checksum

## 👥 Thành viên nhóm

| Thành viên | Mã SV | Phụ trách |
|------------|-------|-----------|
| **Nguyễn Viết Tùng** | K24DTCN633 | 🔧 **Admin System & Database** |
| **Vũ Tất Thành** | K24DTCN638 | 🎨 **UI & Quality Assurance** |
| **Nguyễn Quang Đạo** | *(Cập nhật)* | 💳 **Wallet & Transaction** |
| **Nguyễn Việt** | *(Cập nhật)* | 👤 **User & Security** |

### 🔧 Phân công chi tiết

**Nguyễn Viết Tùng:**

- ✅ Hệ thống Admin hoàn chỉnh (tạo/xem/sửa/xóa user)
- ✅ Database logic với định dạng văn bản đơn giản
- ✅ Backup/restore dữ liệu
- ✅ Simple checksum

**Vũ Tất Thành:**

- ✅ Giao diện CLI đầy đủ cho admin
- ✅ Xử lý input/output và menu navigation
- ✅ Quality assurance và testing

**Nguyễn Quang Đạo:**

- 🔄 Wallet management và transaction logic
- 🔄 Atomic transaction processing

**Nguyễn Việt:**

- 🔄 User authentication và security
- 🔄 Password hashing và OTP integration


## 🚀 Chức năng hiện tại

### ✅ Đã hoàn thành (Admin System)

- **Quản lý người dùng**: Tạo, xem, sửa, xóa tài khoản
- **Database**: Lưu trữ dữ liệu định dạng văn bản với simple checksum
- **Backup/Restore**: Sao lưu và phục hồi dữ liệu đơn giản
- **UI Admin**: Giao diện dòng lệnh đầy đủ cho admin
- **OTP**: Xác thực đơn giản cho các thao tác quan trọng

### 🔄 Đang phát triển

- **User Authentication**: Đăng nhập, đăng ký, bảo mật
- **Wallet Management**: Quản lý ví và giao dịch
- **Transaction System**: Chuyển điểm atomic

## 🛠️ Yêu cầu hệ thống

- Trình biên dịch C++ hỗ trợ C++17
- CMake (tùy chọn, có thể dùng g++ trực tiếp)

## 📦 Cách build và chạy

### Cách 1: Sử dụng g++ trực tiếp (Đơn giản)

```bash
git clone https://github.com/thanh-vt/PTIT-D24TXCN15-K-CPP-G1-FINAL-EXAM.git
cd PTIT-D24TXCN15-K-CPP-G1-FINAL-EXAM
g++ -std=c++17 src/main.cpp src/ui.cpp src/database.cpp -Iinclude -o ewallet_admin
./ewallet_admin
```

### Cách 2: Sử dụng CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
./ewallet_admin
```

## 🔑 Tài khoản mặc định

- **Username**: `admin`
- **Password**: `admin`

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

## 📚 Tài liệu

- **[DATABASE_README.md](DATABASE_README.md)**: Chi tiết về hệ thống database
- **[SIMPLIFICATION_SUMMARY.md](SIMPLIFICATION_SUMMARY.md)**: Tóm tắt việc đơn giản hóa code

## 🎯 Đặc điểm nổi bật

- **Code đơn giản**: Giảm 23% số dòng code (933 → 721 dòng)
- **Dễ debug**: File database dạng văn bản có thể đọc trực tiếp
- **Dễ hiểu**: Logic rõ ràng, ít phức tạp không cần thiết
- **Đầy đủ chức năng**: Vẫn đáp ứng 100% yêu cầu đề bài

---

**Bài tập lớn môn Ngôn ngữ C++ - Lớp D24TXCN15-K - Nhóm 1**

### Thành viên

1. **Vũ Tất Thành** - K24DTCN638
2. **Nguyễn Quang Đạo** - *(Cập nhật)*
3. **Nguyễn Việt** - *(Cập nhật)*
4. **Nguyễn Viết Tùng** - K24DTCN633

# E-Wallet Management System - Build Guide

## Yêu cầu hệ thống

### Windows
- **Visual Studio 2019/2022** với C++ support
- **CMake 3.31+**
- **Git**

### Linux/macOS
- **GCC 7+ hoặc Clang 5+**
- **CMake 3.31+**
- **Git**

## Cách build

### Windows (Khuyến nghị)

1. **Sử dụng script tự động:**
   ```cmd
   build.bat
   ```

2. **Build thủ công:**
   ```cmd
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Debug
   ```

### Linux/macOS

1. **Sử dụng script tự động:**
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

2. **Build thủ công:**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   cmake --build .
   ```

## Chạy ứng dụng

### Windows
```cmd
cd build
Debug\ewallet-manager.exe
```

### Linux/macOS
```bash
cd build
./ewallet-manager
```

## Tùy chọn chạy

- **Với database path tùy chỉnh:**
  ```
  ewallet-manager.exe --db-path path/to/database
  ```

- **Hiển thị help:**
  ```
  ewallet-manager.exe --help
  ```

## Dọn dẹp build

### Windows
```cmd
clean.bat
```

### Linux/macOS
```bash
rm -rf build
```

## Troubleshooting

### Lỗi CMake không tìm thấy compiler
- **Windows:** Cài đặt Visual Studio với C++ workload
- **Linux:** `sudo apt-get install build-essential`
- **macOS:** `xcode-select --install`

### Lỗi CMake version
- Cập nhật CMake từ https://cmake.org/download/

### Lỗi build cpp-otp
- Đã được fix trong source code, đảm bảo sử dụng Visual Studio trên Windows

## Cấu trúc build output

```
build/
├── Debug/                  # Windows build output
│   └── ewallet-manager.exe
├── ewallet-manager         # Linux/macOS build output
└── deps/
    └── cpp-otp/
        └── Debug/
            └── cotp.lib    # Static library
```

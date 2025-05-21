# Dự án: Hệ thống Quản lý Ví điện tử (E-Wallet Management System) - Bài tập lớn môn Ngôn ngữ C++

## 1. Giới thiệu dự án

Dự án này là một hệ thống quản lý ví điện tử dòng lệnh, được phát triển bằng ngôn ngữ C++. Hệ thống cung cấp đầy đủ các chức năng quản lý tài khoản người dùng (đăng ký, đăng nhập), xác thực hai yếu tố (2FA), và các nghiệp vụ ví cốt lõi bao gồm xem số dư, lịch sử giao dịch và thực hiện chuyển điểm an toàn. Dự án được hoàn thành trong khuôn khổ bài tập lớn môn học Ngôn ngữ C++, thể hiện việc áp dụng thành thạo kiến thức về lập trình C++, cấu trúc dữ liệu, quản lý file và các nguyên tắc bảo mật quan trọng trong phát triển phần mềm.

## 2. Thành viên tham gia dự án và Phân công công việc

Dự án được hoàn thành với sự đóng góp và nỗ lực của các thành viên trong nhóm, mỗi người phụ trách các mảng công việc chuyên biệt, đồng thời phối hợp chặt chẽ để đảm bảo tính nhất quán và chất lượng của sản phẩm cuối cùng.

* **Nguyễn Viết Tùng - K24DTCN633 - Phụ trách chính: Admin Core Functions & Database Logic**
    * Phát triển và hoàn thiện các chức năng quản trị:
        * Tạo tài khoản người dùng mới, bao gồm chức năng admin tạo hộ và sinh mật khẩu tự động.
        * Theo dõi và quản lý danh sách người dùng.
        * Điều chỉnh thông tin tài khoản người dùng theo yêu cầu, có xác thực OTP đầy đủ.
    * Xây dựng và tối ưu hóa logic đọc/ghi dữ liệu, serialization/deserialization trong `database.cpp`.
    * Triển khai cơ chế sao lưu và phục hồi dữ liệu hiệu quả.
    * Đảm bảo tính toàn vẹn dữ liệu với cơ chế checksum mạnh mẽ cho `database.dat`.

* **Nguyễn Việt - *(Điền mã sinh viên)* - Phụ trách chính: User Core Functions & Security Implementation**
    * Phát triển và hoàn thiện các chức năng cốt lõi cho người dùng thông thường:
        * Đăng ký tài khoản an toàn.
        * Hệ thống đăng nhập bảo mật, bao gồm xử lý yêu cầu đổi mật khẩu tự sinh trong lần đầu.
        * Thay đổi mật khẩu cá nhân với các bước xác thực cần thiết.
        * Truy xuất và điều chỉnh thông tin cá nhân, được bảo vệ bằng OTP.
    * Chịu trách nhiệm về thiết kế và mã hóa lớp `User` trong `user.cpp`.
    * Triển khai cơ chế băm mật khẩu an toàn sử dụng SHA-256 (hoặc thuật toán mạnh hơn) kết hợp với salt ngẫu nhiên cho từng người dùng.
    * Tích hợp và quản lý xác thực hai yếu tố (2FA) bằng TOTP cho các thao tác quan trọng.

* **Nguyễn Quang Đạo - *(Điền mã sinh viên)* - Phụ trách chính: Wallet Core Functions & Transaction Integrity**
    * Phát triển và hoàn thiện các chức năng nghiệp vụ của ví điện tử:
        * Quản lý số dư ví chính xác.
        * Thực hiện giao dịch chuyển điểm an toàn và đáng tin cậy giữa các ví.
        * Đảm bảo tính không tách rời (atomic) cho các giao dịch chuyển điểm, với cơ chế rollback hoàn chỉnh.
        * Ghi nhận đầy đủ và chính xác lịch sử giao dịch.
        * Cung cấp chức năng cho người dùng xem số dư và chi tiết lịch sử giao dịch.
    * Chịu trách nhiệm về thiết kế và mã hóa lớp `Wallet` trong `wallet.cpp`.
    * Tích hợp thành công OTP vào quy trình xác nhận giao dịch chuyển điểm.

* **Vũ Tất Thành - K24DTCN638 - Phụ trách chính: UI (Giao diện người dùng CLI) & Quality Assurance**
    * Thiết kế và triển khai toàn bộ giao diện người dùng dòng lệnh (CLI) trực quan và dễ sử dụng.
    * Tối ưu hóa luồng tương tác người dùng, đảm bảo trải nghiệm người dùng tốt nhất có thể với giao diện CLI.
    * Xử lý hiệu quả việc nhập liệu từ người dùng và hiển thị thông tin phản hồi một cách rõ ràng.
    * Chịu trách nhiệm về thiết kế và mã hóa lớp `UI` trong `ui.cpp`.
    * Xây dựng và thực thi kế hoạch kiểm thử toàn diện (bao gồm unit test và kiểm thử tích hợp) cho tất cả các module của dự án.
    * Đảm bảo chất lượng tổng thể của sản phẩm thông qua việc kiểm thử chéo và phối hợp chặt chẽ với các thành viên.


## 3. Các chức năng chính (Features)

Hệ thống E-Wallet Management cung cấp đầy đủ các chức năng theo yêu cầu đề bài:

1.  **Quản lý Người dùng:**
    * Tạo tài khoản người dùng mới với mật khẩu được tạo tự động bởi quản trị viên hoặc do người dùng tự đặt.
    * Kích hoạt và vô hiệu hóa xác thực hai yếu tố (2FA) sử dụng TOTP.
    * Người dùng có thể tự thay đổi mật khẩu của mình; mật khẩu tự sinh bắt buộc phải đổi trong lần đăng nhập đầu tiên.
    * Tài khoản Quản trị (admin) có các quyền đặc thù như tạo tài khoản, xem danh sách người dùng, và hỗ trợ người dùng thay đổi thông tin (với OTP xác thực).
2.  **Quản lý Ví:**
    * Mỗi người dùng sở hữu một ví điện tử với mã định danh duy nhất.
    * Chuyển điểm an toàn và chính xác giữa các ví.
    * Xem lịch sử chi tiết các giao dịch đã thực hiện (bao gồm trạng thái thành công/thất bại).
    * Xem số dư hiện tại của ví.
3.  **Bảo mật:**
    * **Băm mật khẩu an toàn:** Mật khẩu người dùng được băm bằng thuật toán SHA-256 (hoặc tương đương) kết hợp với salt ngẫu nhiên cho mỗi tài khoản, đảm bảo an toàn chống lại các kỹ thuật tấn công phổ biến.
    * **Xác thực hai yếu tố (2FA):** Tích hợp TOTP (Time-based One-Time Password) sử dụng thư viện `cpp-otp` để tăng cường bảo mật cho việc đăng nhập và các thao tác quan trọng.
    * **Tính toàn vẹn giao dịch (Atomicity):** Các giao dịch chuyển tiền được xử lý như một đơn vị không thể tách rời; nếu có lỗi xảy ra, hệ thống đảm bảo khôi phục lại trạng thái ban đầu của các ví liên quan.
    * **Lưu trữ dữ liệu an toàn:** Dữ liệu được lưu trữ trong file nhị phân `database.dat`, có cơ chế ghi file tạm thời và checksum để đảm bảo tính toàn vẹn, được xác minh khi tải dữ liệu.
    * **OTP cho các thao tác quan trọng:** Việc sử dụng OTP đã được tích hợp để xác nhận các giao dịch chuyển điểm và các thay đổi thông tin tài khoản nhạy cảm.

## 4. Yêu cầu Hệ thống (Requirements)

* Trình biên dịch C++ tương thích C++17.
* CMake phiên bản 3.31 trở lên.
* Thư viện `cpp-otp` (đã bao gồm trong `deps/` và được liên kết tĩnh).
    * `cpp-otp` sử dụng triển khai HMAC-SHA1 tùy chỉnh từ `hmac.hpp` do tùy chọn `-DNO_OPENSSL` khi build.
* Các thư viện chuẩn C++17 (ví dụ: `<filesystem>`). Không yêu cầu thư viện bên ngoài lớn như Boost nếu trình biên dịch hỗ trợ đầy đủ C++17.

## 5. Hướng dẫn Build và Chạy chương trình

1.  **Clone repository:**
    ```bash
    git clone https://github.com/thanh-vt/PTIT-D24TXCN15-K-CPP-G1-FINAL-EXAM.git
    cd ewallet-manager
    ```
2.  **Tạo thư mục build và build dự án (sử dụng CMake):**
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```
    *(Chương trình được cấu hình để liên kết tĩnh, giảm thiểu phụ thuộc vào môi trường chạy.)*

## 6. Hướng dẫn Sử dụng

1.  **Chạy file thực thi:**
    * Trên Linux/macOS: `./ewallet_manager` (từ trong thư mục `build`)
    * Trên Windows: `ewallet_manager.exe` (từ trong thư mục `build\Debug` hoặc `build\Release`)
2.  **Tài khoản admin mặc định:**
    * Tên đăng nhập: `admin`
    * Mật khẩu: `admin123`
3.  **Thực hiện theo menu trên màn hình để tương tác với các chức năng.**
    *(Tham khảo tài liệu chi tiết về các lệnh và luồng sử dụng trong thư mục `docs/`, đặc biệt là `docs/06_USER_INTERFACE_CLI.md`.)*

## 7. Lưu trữ Dữ liệu

* Toàn bộ dữ liệu hệ thống (người dùng, ví, giao dịch) được lưu trữ tập trung và an toàn trong file nhị phân `database.dat`, đặt tại thư mục hiện hành khi chương trình được chạy.
* Dữ liệu được tự động lưu vào file khi chương trình kết thúc một cách hợp lệ.
* Tính toàn vẹn của file dữ liệu được đảm bảo thông qua magic number, version và cơ chế checksum được xác minh mỗi khi tải dữ liệu.
* Hệ thống cung cấp quy trình sao lưu và phục hồi dữ liệu, được mô tả chi tiết trong `docs/05_FILE_STORAGE_AND_BACKUP.md`.

## 8. Các Lưu ý về Bảo mật

* **Luôn thay đổi mật khẩu admin mặc định (`admin123`) ngay sau lần đăng nhập đầu tiên để đảm bảo an toàn hệ thống.**
* Người dùng nên kích hoạt và sử dụng xác thực hai yếu tố (2FA) cho tất cả các tài khoản để tăng cường lớp bảo vệ.
* Thông tin Wallet ID và khóa bí mật 2FA cần được người dùng bảo vệ cẩn thận.
* Người dùng nên thường xuyên kiểm tra lịch sử giao dịch của mình để phát hiện sớm các hoạt động bất thường.
* Các giao dịch chuyển điểm và thay đổi thông tin tài khoản quan trọng đều được bảo vệ bằng một lớp xác thực OTP.

## 9. Chất lượng và Hoàn thiện Dự án

Dự án đã được hoàn thiện với các điểm nổi bật sau:

* **Mã nguồn có cấu trúc tốt:** Phân chia rõ ràng thành các module (User, Wallet, Database, UI) với các lớp và tệp tiêu đề (.hpp) / tệp nguồn (.cpp) tương ứng, giúp dễ dàng quản lý và bảo trì.
* **Bảo mật được chú trọng:** Áp dụng các biện pháp bảo mật quan trọng như băm mật khẩu có salt, xác thực hai yếu tố, và cơ chế đảm bảo tính toàn vẹn dữ liệu.
* **Xử lý giao dịch đáng tin cậy:** Các giao dịch chuyển tiền được thiết kế để đảm bảo tính atomic.
* **Tài liệu đầy đủ:** Bên cạnh file `README.md` này, dự án có thư mục `docs/` chứa các tài liệu phân tích, thiết kế chi tiết (Mô hình dữ liệu, Thiết kế bảo mật, Các trường hợp sử dụng, Quy trình sao lưu/phục hồi, v.v.), đáp ứng đầy đủ yêu cầu mục (B) của đề bài.
* **Kiểm thử toàn diện:** Dự án đã được kiểm thử kỹ lưỡng thông qua unit test và kiểm thử tích hợp các chức năng chính để đảm bảo hoạt động ổn định và chính xác.
* **Quản lý mã nguồn chuyên nghiệp:** Sử dụng Git cho quản lý phiên bản, với các commit rõ ràng, thể hiện sự đóng góp của từng thành viên. Một file `.gitignore` được sử dụng để duy trì kho chứa sạch sẽ, không bao gồm các file build hay file cấu hình IDE không cần thiết.

## 10. Tài liệu tham khảo (References)

* Đề bài môn học Ngôn ngữ C++ (File: `Cplusplus-Project.docx`).
* Thư viện `cpp-otp` của `patzol768` (dựa trên `tilkinsc/COTP`): `https://github.com/patzol768/cpp-otp`.
* Thư viện QR Code generator của Nayuki: `https://www.nayuki.io/page/qr-code-generator-library` (được bao gồm trong `cpp-otp`).
* Tính chất ACID trong cơ sở dữ liệu: `https://200lab.io/blog/acid-la-gi/`.
* *(Các nguồn tham khảo, thư viện, hoặc bài viết hữu ích khác mà nhóm đã sử dụng trong quá trình thực hiện dự án được liệt kê trong thư mục `docs/`).*

---

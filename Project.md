Hệ thống đăng nhập và đăng ký tài khoản và quản lý ví điểm thưởng

Dự án Hệ thống đăng nhập và đăng ký chủ yếu liên quan đến quá trình đăng ký người dùng. Thông tin xác thực người dùng như tên người dùng và mật khẩu được yêu cầu từ người dùng. Nếu người dùng đăng ký thành công thì với thông tin xác thực được cung cấp, một bản ghi (có thể là tập tin) của người dùng cụ thể sẽ được tạo trong cơ sở dữ liệu (CSDL).
Ví điểm thưởng là cơ chế quản lý các lượng (đơn vị : điểm) được dùng để quy đổi ra hàng hóa (còn gọi là hoạt động mua - bán). Giữa các ví có giao dịch chuyển điểm.


A. Các chức năng:

1 - Tạo mới tài khoản khi đăng ký: tạo mới bộ dữ liệu liên quan đến tài khoản người dùng

     Người dùng nhập dữ liệu để tạo tài khoản. 

     Có thể có nhân viên nhập liệu (người quản lý) để tạo tài khoản hộ người dùng, với điều kiện người dùng cung cấp đầy đủ thông tin cá nhân theo yêu cầu tạo tài khoản.

     Hãy phân tích và đề xuất một cấu trúc dữ liệu (lớp) quản lý thông tin tài khoản người dùng.



2 - Lưu trữ dữ liệu: lưu lại dữ liệu tài khoản người dùng xuống tập tin.

    Hãy đề xuất việc lưu trữ dữ liệu người dùng dưới dạng tập tin riêng cho từng người dùng hay một tập tin cho tất cả người dùng? Nêu lý do lựa chọn giải pháp.

    Với mật khẩu, hãy tìm hiểu cách lưu mật khẩu của người dùng vào CSDL. Gợi ý: dùng hàm băm (hash function).

    Mọi lưu trữ phải có bản sao lưu (backup). Hãy tìm hiểu quy trình sao lưu - phục hồi dữ liệu. Đề xuất giải pháp sao lưu cho dữ liệu người dùng



3 - Quản lý đăng nhập

3.1 - Sinh mật khẩu tự động: sinh một mật khẩu tự động kèm theo thông tin đăng nhập của người dùng trong trường hợp người dùng không cung cấp mật khẩu. 

Trường hợp này thường được sử dụng khi nhân viên quản lý mở tài khoản mới và người dùng không thể trực tiếp nhập mật khẩu cho mình.



3.2 - Thay đổi mật khẩu: một người dùng có thể tự thay đổi mật khẩu đăng nhập của mình.

Hãy tìm hiểu quy trình xử lý thay đổi mật khẩu.

Với trường hợp mật khẩu tự sinh, hệ thống phải kiểm tra mật khẩu là tự sinh và yêu cầu người dùng thay đổi ngay mật khẩu mới trong lần đăng nhập đầu tiên của mình.



3.3 - OTP - One Time Password: bảo mật hai lớp để xác thực giao dịch.

OTP được sử dụng trong đồ án này để xác thực các hoạt động thay đổi thông tin quan trọng của người dùng.

Hãy tìm hiểu về OTP và đề xuất giải pháp sử dụng OTP cho đồ án (phần B). Xem nguồn tham khảo [1] và [2] và tìm thêm các tài liệu khác về OTP.



3.4 - Đăng nhập:      Khi người dùng đăng nhập bằng tài khoản của mình, chương trình phải tìm kiếm trong dữ liệu đã lưu trữ và xác định liệu thông tin đăng nhập đó có tồn tại và hợp lệ không. Nếu hợp lệ, cho phép người dùng vào hệ thống và sử dụng các chức năng tương ứng



B. Phân chia người dùng và chức năng

Nhóm người dùng gồm: 

a/ Người dùng thông thường chỉ được phép truy xuất thông tin cá nhân của mình. 

Được phép điều chỉnh tên, mật khẩu v.v. Hãy căn cứ vào quản lý thông tin tài khoản trên để đưa ra yêu cầu cho phép điều chỉnh, thay đổi trường dữ liệu cụ thể và viết các chức năng cập nhật tương ứng.

Khi điều chỉnh xong, hệ thống gửi một mã OTP đến chủ tài khoản cùng thông báo các nội dung sẽ thay đổi để chủ tài khoản xác nhận thao tác cập nhật.

Nếu mã OTP do chủ tài khoản nhập vào hợp lệ, hệ thống tiến hành cập nhật thông tin của người dùng.



b/ Người dùng quản lý: Ngoài chức năng quản lý thông tin cá nhân, người quản lý có thể:

  b.1 - Theo dõi danh sách nhóm

  b.2 - Tạo thêm tài khoản mới

  b.3 - Điều chỉnh thông tin của tài khoản khi có yêu cầu từ chủ tài khoản (làm hộ). Khi điều chỉnh xong, hệ thống gửi một mã OTP đến chủ tài khoản cùng thông báo các nội dung sẽ thay đổi để chủ tài khoản xác nhận thao tác cập nhật.

Nếu mã OTP do chủ tài khoản nhập vào hợp lệ, hệ thống tiến hành cập nhật thông tin của người dùng.



Chú ý: Không được phép thay đổi tên tài khoản đăng nhập.



C. Quản lý hoạt động ví:

Mỗi người dùng có một bộ dữ liệu về số điểm (ví - wallet) và dữ liệu các giao dịch trao đổi, chuyển điểm từ một ví sang một ví khác.

Một ví có mã số định danh duy nhất phân biệt với tất cả các ví còn lại.

Ví tổng: nguồn duy nhất sinh ra tổng số điểm sẽ lưu chuyển trong toàn bộ hệ thống.



Giao dịch chuyển d điểm từ ví A sang ví B sẽ gồm các thao tác thành phần sau

    1_ Tìm, mở ví A. Đây là ví chủ của giao dịch, là ví của chủ tài khoản muốn thực hiện chuyển điểm đi.

    2_ Tìm, mở ví B. Đây là ví đích của giao dịch. Nếu mã ví B tồn tại, tiếp tục đến 3_Giao dịch

    3_ Giao dịch: Hai tác vụ dưới đây là không tách rời (atomic). Bất kỳ có một bất thường (exception) nào xảy ra trong một tác vụ đều dẫn đến hủy toàn bộ giao dịch, phục hồi lại trạng thái số dư của hai ví A và B trước 3_ 

        3_1 if (A.balance >=d) 

                   A.balance = A.balance - d

               else

                   "low balance. Cannot proceed.”. Đến 4_

        3_2 B.balance = B.balance + d

    4_ Kết thúc 



Hãy đề xuất cách sử dụng OTP để xác nhận thực hiện giao dịch chuyển điểm giữa các ví.



Hệ thống ghi nhận lại giao dịch này trong lịch sử giao dịch (transaction log)

Báo cáo: hệ thống cho phép người dùng theo dõi số dư, lịch sử giao dịch (và trạng thái thực hiện giao dịch) 





Nguồn tham khảo

[1] CPP_OTP, https://github.com/patzol768/cpp-otp, truy cập lần cuối ngày 04/10/2024

[2] COTP, https://github.com/tilkinsc/COTP, truy cập lần cuối ngày 04/10/2024

[3] ACID, https://200lab.io/blog/acid-la-gi/, truy cập lần cuối ngày 04/10/2024

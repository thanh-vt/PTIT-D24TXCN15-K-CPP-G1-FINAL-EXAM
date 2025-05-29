cmake_minimum_required(VERSION 3.31)
project(ewallet_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fully static linking
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# Add source files
set(SOURCES
    src/main.cpp
    src/ui.cpp
    src/database.cpp
    src/user.cpp
    src/wallet.cpp
)

# Add header files
set(HEADERS
    include/ui.hpp
    include/database.hpp
    include/user.hpp
    include/wallet.hpp
    include/hmac.hpp
)

# Create executable
add_executable(ewallet_manager ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(ewallet_manager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Tạm comment vì chưa thấy file này
# Create test executable
# add_executable(test_admin test_admin.cpp src/database.cpp)
# target_include_directories(test_admin PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
# )


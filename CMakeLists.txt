cmake_minimum_required(VERSION 3.10)
project(ewallet-manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/user.cpp
    src/wallet.cpp
    src/database.cpp
    src/ui.cpp
    src/admin.cpp
)

# Add header files
set(HEADERS
    include/user.hpp
    include/wallet.hpp
    include/database.hpp
    include/ui.hpp
    include/admin.hpp
    include/transaction.hpp
    include/hmac.hpp
)

# Create executable
add_executable(ewallet-manager ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(ewallet-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

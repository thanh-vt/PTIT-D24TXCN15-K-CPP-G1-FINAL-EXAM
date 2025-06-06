cmake_minimum_required(VERSION 3.31)
project(ewallet-manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fully static linking
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

## Add cpp-otp library
#add_subdirectory(deps/cpp-otp)

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
add_executable(ewallet-manager ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(ewallet-manager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpp-otp
)

# Link libraries
target_link_libraries(ewallet-manager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/libcotp.a
)

# Copy test files
file(GLOB TEST_FILES
        "${CMAKE_SOURCE_DIR}/test/**"
)
file(COPY ${TEST_FILES} DESTINATION ${CMAKE_BINARY_DIR})


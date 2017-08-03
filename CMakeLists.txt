cmake_minimum_required (VERSION 3.0.2)
project (fake-smtp-server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set(BOOST_MIN_VERSION "1.39.0")
find_package(Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS system filesystem)

set (SOURCE_FILES
   EmailAnalyser.cpp
   TcpConnection.cpp
   TcpServer.cpp
   main.cpp
)

add_executable(fake-smtp-server ${SOURCE_FILES})

target_link_libraries(fake-smtp-server pthread boost_system boost_filesystem)
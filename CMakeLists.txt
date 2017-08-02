cmake_minimum_required (VERSION 2.8.11)
project (fake-smtp-server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set (SOURCE_FILES
   EmailAnalyser.cpp
   TcpConnection.cpp
   TcpServer.cpp
   main.cpp
)

add_executable(fake-smtp-server ${SOURCE_FILES})

target_link_libraries(fake-smtp-server pthread boost_system boost_filesystem)
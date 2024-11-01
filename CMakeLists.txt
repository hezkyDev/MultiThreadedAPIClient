cmake_minimum_required(VERSION 3.10)
project(MultiThreadedAPIClient)

set(CMAKE_CXX_STANDARD 17)

# Main executable
add_executable(MultiThreadedAPIClient src/main.cpp src/APIClient.cpp)

# Include directory for headers
include_directories(include)

# Link cURL
find_package(CURL REQUIRED)
target_include_directories(MultiThreadedAPIClient PRIVATE /opt/homebrew/opt/curl/include)
target_link_libraries(MultiThreadedAPIClient PRIVATE /opt/homebrew/opt/curl/lib/libcurl.dylib)

# Link SQLite
find_package(SQLite3 REQUIRED)
target_include_directories(MultiThreadedAPIClient PRIVATE /opt/homebrew/opt/sqlite/include)
target_link_libraries(MultiThreadedAPIClient PRIVATE /opt/homebrew/opt/sqlite/lib/libsqlite3.dylib)

# Link JSON library (header-only)
target_include_directories(MultiThreadedAPIClient PRIVATE /opt/homebrew/include)

# Test executable
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Define the test executable and link APIClient.cpp, cURL, SQLite, and GTest libraries
add_executable(APIClientTest tests/APIClientTest.cpp src/APIClient.cpp)
target_link_libraries(APIClientTest PRIVATE ${GTEST_BOTH_LIBRARIES} pthread ${CURL_LIBRARIES} ${SQLite3_LIBRARIES})
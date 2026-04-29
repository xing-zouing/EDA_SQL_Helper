cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE "~/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(EDA_SQL_Helper)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_TOOLCHAIN_FILE "~/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 头文件目录
include_directories(include)

# 查找依赖库
find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Matplot++ REQUIRED)
find_package(nlohmann_json REQUIRED)

# 源文件
set(SOURCES
    src/main.cpp
    src/VLLMClient.cpp
    src/SQLiteDB.cpp
    src/Visualizer.cpp
)

# 生成可执行文件
add_executable(eda_helper ${SOURCES})

# 链接库
target_link_libraries(eda_helper
    ${CURL_LIBRARIES}
    ${SQLite3_LIBRARIES}
    Matplot++::matplot
    nlohmann_json::nlohmann_json
)
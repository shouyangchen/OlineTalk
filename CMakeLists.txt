cmake_minimum_required(VERSION 3.30)
# 工具链文件应该在 project() 之前设置
set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg-2025.06.13/scripts/buildsystems/vcpkg.cmake")
project(Onlietallk)

if(WIN32)
    # 设置 linux 的头文件支持，使用正斜杠避免反斜杠转义问题
    set(Linux_Include "C:/Users/chenshouyang/AppData/Local/Microsoft/Linux/HeaderCache/1.0/-1854440610/usr/include")
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 相关设置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt 安装路径
set(CMAKE_PREFIX_PATH "D:\\Qt\\6.9.1\\msvc2022_64")
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# 可选：设置输出目录
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)



# 收集主程序源文件
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB UI_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)
file(GLOB RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.qrc)


#收集子进程源文件
file(GLOB_RECURSE SUB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/FileOperatorProcess/src/*.cpp)
file(GLOB SUB_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/FileOperatorProcess/include/*.h)


# 查找依赖包
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Sql
        Concurrent
        DBus
        REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

# 显式设置 Boost 和 jsoncpp 的路径以确保找到正确的配置文件
set(Boost_DIR "D:/vcpkg-2025.06.13/installed/x64-windows/share/boost")
set(jsoncpp_DIR "D:/vcpkg-2025.06.13/installed/x64-windows/share/jsoncpp")
set(jwt-cpp_DIR "D:/vcpkg-2025.06.13/installed/x64-windows/share/jwt-cpp")
set(OpenCV_DIR "D:/vcpkg-2025.06.13/installed/x64-windows/share/opencv")
set(quirc_DIR "D:/vcpkg-2025.06.13/installed/x64-windows/share/quirc")

find_package(Boost CONFIG REQUIRED COMPONENTS system filesystem)
find_package(jsoncpp CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

# 创建可执行文件
add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${RESOURCES}
        ${INCLUDES}
       )

#创建子进程可执行文件
add_executable(FileOperator
        ${SUB_SOURCES}
        ${SUB_INCLUDES}
       )

#主程序链接FFMPEG和Qt6::Core
target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)

#子进程链接Qt6::Core
target_link_libraries(FileOperator PRIVATE Qt6::Core)#
# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Sql
        Qt6::Concurrent
        Qt6::DBus
        Boost::system
        Boost::filesystem
        JsonCpp::JsonCpp
        jwt-cpp::jwt-cpp
        ${FFMPEG_LIBRARIES}
        opencv_core
        opencv_imgcodecs
        opencv_imgproc
)

#子进程链接库子进程子需要Qt::Core, Qt::Network, Qt::Sql, Qt6::Concurrent
target_link_libraries(FileOperator PRIVATE
        Qt::Core
        Qt::Network
        Qt::Sql
        Qt6::Concurrent
        Qt6::DBus
)


# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        ${FFMPEG_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
)

target_include_directories(FileOperator PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/FileOperator/include
        ${CMAKE_BINARY_DIR}
)




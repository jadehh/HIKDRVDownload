cmake_minimum_required(VERSION 3.10)
project(HikDVRDownload)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/HCNetSDKCom/Windows/include")
    link_directories("${CMAKE_SOURCE_DIR}/HCNetSDKCom/Windows/lib")
elseif (UNIX)
    include_directories("${CMAKE_SOURCE_DIR}/HCNetSDKCom/Linux/include")
    link_directories("${CMAKE_SOURCE_DIR}/HCNetSDKCom/Linux/lib")
endif()

# Add executable
add_executable(HikDVRDownload main.cpp)

# Link libraries based on the operating system
if (WIN32)
    target_link_libraries(HikDVRDownload HCNetSDK HCCore)
elseif (UNIX)
    target_link_libraries(HikDVRDownload hcnetsdk HCCore pthread)
endif()

# Find all DLL files in the lib directory (Windows only)
if (WIN32)
    file(GLOB DLL_FILES "${CMAKE_SOURCE_DIR}/HCNetSDKCom/Windows/lib/*.dll")
elseif (UNIX)
    file(GLOB DLL_FILES "${CMAKE_SOURCE_DIR}/HCNetSDKCom/Linux/lib/*.so")
endif()

add_custom_command(
        TARGET HikDVRDownload POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_FILES}
        "$<TARGET_FILE_DIR:HikDVRDownload>"
)
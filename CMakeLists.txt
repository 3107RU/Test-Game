cmake_minimum_required(VERSION 3.5.1)

project(test)
message("Building ${PROJECT_NAME}...") 

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY x64)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY x64)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY x64)
else()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY x86)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY x86)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY x86)
endif()

if(MSVC)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS_MINSIZEREL)
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()

add_subdirectory(3rdparty/glfw-3.3.5)
add_subdirectory(3rdparty/glew-2.2.0/build/cmake)
add_subdirectory(3rdparty/freetype)
add_subdirectory(bin2cpp)
add_subdirectory(packer)
add_subdirectory(game)

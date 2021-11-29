# Alex Light (dev@3107.ru)
# Makefile для игры
# version 0.1
# 2021-11-28
cmake_minimum_required(VERSION 3.5.1)

project(game)

set(RESOURCES font.ttf.cpp)
set (INCLUDES common.h utils.h geometry.h objects.h sprites.h scene.h text.h)
set (SOURCES ${RESOURCES} main.cpp text.cpp scene.cpp objects.cpp)

set (HTML main.html)

set (CFG main.cfg)

set (EMS emscripten)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "../3rdparty/freetype/include")

if (DEFINED EMSCRIPTEN)
# this called from generated ninja buld
# set emcc parameters
set_target_properties(${PROJECT_NAME}
        PROPERTIES SUFFIX ".js"
        LINK_FLAGS "-s ASSERTIONS=1 -s MIN_WEBGL_VERSION=2 -s USE_GLFW=3 -s USE_FREETYPE=1 -s WASM=1 -O3")
else(DEFINED EMSCRIPTEN)
# this called when we build executable
# setup libs
target_link_libraries(${PROJECT_NAME} glfw glew_s freetype opengl32)
# run bin2cpp to convert resources to cpp
foreach(file ${RESOURCES})
    string(REGEX REPLACE "\\.[^.]*$" "" name ${file})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${file} 
        COMMAND bin2cpp ${CMAKE_CURRENT_SOURCE_DIR}/${name} ${CMAKE_CURRENT_SOURCE_DIR}/${file}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${name})
endforeach()
add_custom_target(${PROJECT_NAME}Resources DEPENDS ${RESOURCES})
# generate ninja build for emcc
add_custom_command(OUTPUT ${EMS}/build.ninja
        COMMAND emcmake cmake ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${EMS}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
add_custom_target(${PROJECT_NAME}Emcmake DEPENDS ${EMS}/build.ninja)
add_dependencies(${PROJECT_NAME}Emcmake ${PROJECT_NAME}Resources)
# run ninja build
add_custom_command(OUTPUT ${EMS}/${PROJECT_NAME}.wasm
        COMMAND ninja
        WORKING_DIRECTORY ${EMS}
        DEPENDS ${INCLUDES} ${SOURCES})
add_custom_target(${PROJECT_NAME}Ninja DEPENDS ${EMS}/${PROJECT_NAME}.wasm ${EMS}/${PROJECT_NAME}.js)
add_dependencies(${PROJECT_NAME}Ninja ${PROJECT_NAME}Emcmake)
# run packer to combine js & wasm to custom html
add_custom_command(OUTPUT ${PROJECT_NAME}.html
        COMMAND packer ${CMAKE_CURRENT_SOURCE_DIR}/${HTML} ${CMAKE_CURRENT_SOURCE_DIR}/${CFG}
        ${EMS}/${PROJECT_NAME}.wasm ${EMS}/${PROJECT_NAME}.js ${PROJECT_NAME}.html
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${HTML} ${EMS}/${PROJECT_NAME}.wasm ${EMS}/${PROJECT_NAME}.js)
add_custom_target(${PROJECT_NAME}Html DEPENDS ${PROJECT_NAME}.html)
add_dependencies(${PROJECT_NAME}Html ${PROJECT_NAME}Ninja)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}Html)
endif (DEFINED EMSCRIPTEN)

cmake_minimum_required (VERSION 3.10)
project(arkanoid)

set(CPP main.cpp block.cpp)
file(GLOB HEADERS *.h)

#set(SDL_INCLUDE_DIRS C:/dev/vcpkg/vcpkg/installed/x64-windows/include/SDL2)
set(SDL2 "C:/dev/vcpkg/vcpkg/installed/x64-windows")   
#set(SDL_LIBRARIES C:/dev/vcpkg/vcpkg/installed/x64-windows/lib)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SDL2_INCLUDE_DIR ${SDL2}/SDL.h)
get_filename_component(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIR} DIRECTORY) 
get_filename_component(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIR} DIRECTORY) 

file(GLOB_RECURSE SDL2_LIBRARY ${SDL2}/SDL*.lib)

file(GLOB_RECURSE SDL2_BINARY ${SDL2}/SDL2.dll)
get_filename_component(SDL2_BINARY_DIR ${SDL2_BINARY} DIRECTORY) 
file(GLOB SDL2_BINARIES ${SDL2_BINARY_DIR}/*.dll)

file(GLOB_RECURSE SDL2_BINARY_D ${SDL2}/SDL2d.dll)
get_filename_component(SDL2_BINARY_DIR ${SDL2_BINARY_D} DIRECTORY) 
file(GLOB SDL2_BINARIES_D ${SDL2_BINARY_DIR}/*.dll)


file(COPY ${SDL2_BINARY_D} ${SDL2_BINARIES}   DESTINATION ${CMAKE_BINARY_DIR}/RelWithDebInfo)
file(COPY ${SDL2_BINARY}   ${SDL2_BINARIES_D} DESTINATION ${CMAKE_BINARY_DIR}/Debug)

add_executable(${PROJECT_NAME}
   ${CPP}
   ${HEADERS}
)
source_group("" FILES ${CPP})

message(STATUS "SDL2" ${SDL2} ${SDL2_INCLUDE_DIR} ${SDL2_BINARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup ")
set_property(TARGET ${PROJECT_NAME}   PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32.lib ${SDL2_LIBRARY})

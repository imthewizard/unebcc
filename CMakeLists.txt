cmake_minimum_required(VERSION 3.10)

project(unebcc LANGUAGES C)

option(ADDRESS_SANITIZER "use -fsanitize=address and -fno-omit-frame-pointer" OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "./src/*.c")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	if (ADDRESS_SANITIZER)
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -fsanitize=address -fno-omit-frame-pointer)
		target_link_options(unebcc PRIVATE -fsanitize=address)
	else()
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
	endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ./include)

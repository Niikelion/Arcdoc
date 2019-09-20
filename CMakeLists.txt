cmake_minimum_required(VERSION 3.13)
project(Arcdoc)

set(DEP_ENVS "")
list(APPEND DEP_ENVS "PATH_TO_NULLSCRIPT" "PATH_TO_NLIB" "PATH_TO_PARSELIB")

foreach(E IN LISTS DEP_ENVS)
	if (NOT DEFINED ENV{${E}})
		message(SEND_ERROR "Enviroment variable ${E} not found")
	endif()
endforeach()

#Include main headers directory
include_directories(${PROJECT_SOURCE_DIR}/include)

#Include library header folders
include_directories($ENV{PATH_TO_NULLSCRIPT}/include)
include_directories($ENV{PATH_TO_NLIB})
include_directories($ENV{PATH_TO_PARSELIB})

#Include linker folders
link_directories($ENV{PATH_TO_NULLSCRIPT}/bin)
link_directories($ENV{PATH_TO_NLIB}/bin)
link_directories($ENV{PATH_TO_PARSELIB}/bin)

#Add all .cpp files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/main.cpp")

add_executable(Release ${SOURCES})
add_executable(Debug ${SOURCES})
add_library(GenBase STATIC ${PROJECT_SOURCE_DIR}/src/module.cpp)

set_target_properties(Release PROPERTIES
				OUTPUT_NAME "Arcdoc"
				RUNTIME_OUTPUT_DIRECTORY "bin/Release")
set_target_properties(Debug PROPERTIES
				OUTPUT_NAME "Arcdoc"
				RUNTIME_OUTPUT_DIRECTORY "bin/Debug")
set_target_properties(GenBase PROPERTIES
				ARCHIVE_OUTPUT_DIRECTORY "bin")

target_link_libraries(Debug --static -g)
target_link_libraries(Debug -lnullscript-d -lNslib-d -ljson -lxml)
target_link_libraries(Release --static -O3)
target_link_libraries(Release -lnullscript -lNslib -ljson -lxml)
target_link_libraries(GenBase --static -O3)
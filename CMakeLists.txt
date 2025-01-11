#https://github.com/meemknight/cmakeSetup
#https://github.com/meemknight/openglConfiguration
#https://github.com/meemknight/game-in-cpp-full-course/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

#! ! ! ! ! ! !
# To ship the game:
# 1. Set PRODUCTION_BUILD to `ON`.
# 2. Switch to x64-Release configuration.
# 3. DELETE the `out` folder (VS doesn't recognize this change and rebuild).
# 4. Click inside CMakeLists.txt file and save to initiate CMake generation.
# 5. Run game.exe to complete build.
# 6. Use the game.exe file from ./out/build/x64-Release/game.exe and the ./resources folder together in a directory.
#    - Note, all the other stuff in the x64-Release folder is not needed, just the exe.
#
# You can now run game.exe and the resources path will use the resources folder you copied.
option(PRODUCTION_BUILD "Make this a production build" OFF)


# We want to use the static runtime so we don't have to ship with a .dll
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(MSVC) 
add_compile_options(/arch:AVX2) #make sure SIMD optimizations take place
endif()

project(mygame)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw-3.3.2)			# Window opener
add_subdirectory(thirdparty/glad)				# Opengl loader
add_subdirectory(thirdparty/stb_image)			# Loading images
add_subdirectory(thirdparty/stb_truetype)		# Loading ttf font files
add_subdirectory(thirdparty/raudio)				# Audio
add_subdirectory(thirdparty/glm)				# Math
add_subdirectory(thirdparty/imgui-docking)		# UI
add_subdirectory(thirdparty/freetype)			# Fonts
add_subdirectory(thirdparty/perlin_noise)		# Perlin Noise Generator
add_subdirectory(thirdparty/thread_pool)		# Thread pool


# Define MY_SOURCES to be a list of all the source files for my game 
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Define MY_INCLUDES to be a list of all the include files for my game 
file(GLOB_RECURSE MY_INCLUDES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable("${CMAKE_PROJECT_NAME}")

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 20)

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC GLFW_INCLUDE_NONE=1) 


if(PRODUCTION_BUILD)
	# setup the ASSETS_PATH macro to be in the root folder of your exe
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/") 

	# remove the option to debug asserts.
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 

	#no console
	set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

else()
	# This is useful to get an ASSETS_PATH in your IDE during development
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 

endif()


target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )


if(MSVC) # If using the VS compiler...

	target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "/ZI")
	target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "/INCREMENTAL")

	#target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	#remove console
	#set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	
	#set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug<$<CONFIG:Debug>:Debug>")
	#set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

	# Debug stuff: uncomment to get verbose output (ChatGPT helps a lot to decypher it)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zi /diagnostics:caret")
	#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

endif()

# Get unique directories from the list of header files
set(MY_INCLUDE_DIRS "")
foreach(file_path ${MY_INCLUDES})
    get_filename_component(dir ${file_path} PATH)
    list(APPEND MY_INCLUDE_DIRS ${dir})
endforeach()
list(REMOVE_DUPLICATES MY_INCLUDE_DIRS)

# Add directories to target_include_directories
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC ${MY_INCLUDE_DIRS})

target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw 
	glad stb_image stb_truetype raudio imgui freetype perlin_noise thread_pool)


# Minimum CMake version required, we'll just use the latest version.
cmake_minimum_required(VERSION 3.25)

# Project name, version and description
project(discord-bot VERSION 1.0 DESCRIPTION "combinestupid")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Create an executable
add_executable(${PROJECT_NAME}
	src/main.cpp
	src/types/shared.h 
	src/types/tracker_types.h 
	src/commands/set_info.cpp 
	src/commands/set_info.h
	src/commands/get_info.cpp 
	src/commands/get_info.h 
	src/commands/owes_me.cpp 
	src/commands/owes_me.h 
	src/commands/iou.cpp 
	src/commands/iou.h 
	src/commands/pay.cpp
	src/commands/pay.h
	src/commands/help.cpp 
	src/commands/help.h 
	src/states/state_manager.cpp 
	src/states/state_manager.h 
	src/states/user_state.cpp 
	src/states/user_state.h
	src/states/tracker.cpp 
	src/states/tracker.h 
	src/utils/io_utils.cpp 
	src/utils/io_utils.h 
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Find packages
find_package(DPP REQUIRED)
# Link packages.\
target_link_libraries(${PROJECT_NAME} 
	${DPP_LIBRARIES} 
	nlohmann_json::nlohmann_json
)

# Include the DPP directories.
target_include_directories(${PROJECT_NAME} PRIVATE
	${DPP_INCLUDE_DIR}
)
	
# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)
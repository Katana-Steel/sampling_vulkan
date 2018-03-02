project(devEnum)

add_executable(${PROJECT_NAME}
	dev_enum/main.cpp)

target_link_libraries(${PROJECT_NAME}
	vulkan)

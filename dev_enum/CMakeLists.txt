project(devEnum)

include(FindPkgConfig)

pkg_check_modules(VK REQUIRED vulkan)

add_executable(${PROJECT_NAME}
	main.cpp)

target_link_libraries(${PROJECT_NAME}
	${VK_LIBRARIES})

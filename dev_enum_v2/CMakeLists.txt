project(devEnum2)

include(FindPkgConfig)

pkg_check_modules(VK REQUIRED vulkan)
pkg_check_modules(X REQUIRED xcb)

add_executable(${PROJECT_NAME}
        xcb.cpp
	main.cpp)

target_link_libraries(${PROJECT_NAME}
        ${X_LIBRARIES}
	${VK_LIBRARIES})

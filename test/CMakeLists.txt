include_directories(../src)

set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB TEST_SOURCES ${TEST_DIR}/*.cpp)

find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})

foreach(TEST_SOURCE ${TEST_SOURCES})
    # 获取测试文件名（不包含路径和扩展名）
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    # 为每个测试文件创建一个可执行文件
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} src ${YAML_CPP_LIBRARIES})
endforeach()
    

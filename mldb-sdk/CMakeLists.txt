cmake_minimum_required(VERSION 3.13)

project(mldb_sdk
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")
set(CURL_LIBRARY "-lcurl")
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 11)

find_package(CURL REQUIRED)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/nlohmann_json)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/googletest)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

include_directories(BEFORE ${CMAKE_CURRENT_LIST_DIR}/src ${CURL_INCLUDE_DIR})
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/common CM_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/crane CRN_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/log LOG_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/uri URI_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/http HTTP_SRC)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/test TEST_SRC)

add_library(mldb_sdk SHARED ${CMAKE_CURRENT_LIST_DIR}/src/sdk.cc ${CM_SRC} ${CRN_SRC} ${LOG_SRC} ${URI_SRC} ${HTTP_SRC})

add_executable(mldb_test ${TEST_SRC} ${LOG_SRC} ${HTTP_SRC} ${CM_SRC})

target_link_libraries(mldb_sdk
    ${CURL_LIBRARIES}
    ${PROTOBUF_LIBRARY}
    nlohmann_json::nlohmann_json)
target_link_libraries(mldb_test
    ${CURL_LIBRARIES}
    ${PROTOBUF_LIBRARY}
    gtest
    gtest_main
    nlohmann_json::nlohmann_json)
target_link_options(mldb_sdk
    INTERFACE "-pthread -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl")

add_custom_target(py_setup_install
    python3 ${CMAKE_CURRENT_LIST_DIR}/src/setup.py install)

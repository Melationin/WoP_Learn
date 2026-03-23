

add_subdirectory(Ex_WosDemo)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
target_include_directories(Ex_WosDemo
        PUBLIC ${INCLUDE_DIR}
        PUBLIC ${THIRD_PARTY_DIR}
)
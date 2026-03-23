project(Ex_WosDemo)
file(GLOB source
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
  "${CMAKE_CURRENT_SOURCE_DIR}/shapes/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/shapes/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/solvers/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/solvers/*.cpp"
)
add_executable(${PROJECT_NAME} ${source}
        shapes/rect.cpp
        shapes/rect.h
        util.h
        solvers/HC_solver.cpp
        solvers/HC_solver.h
        solvers/WoP.cpp
        solvers/WoP.h
        main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES 
  DEBUG_POSTFIX "_d"
  RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_DIR}") 
#target_link_libraries(${PROJECT_NAME} PUBLIC common)

find_package(OpenMP REQUIRED)

target_link_libraries(Ex_WosDemo PRIVATE OpenMP::OpenMP_CXX)


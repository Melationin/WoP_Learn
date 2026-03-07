project(1_MiniDraw)
file(GLOB source
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
  "${CMAKE_CURRENT_SOURCE_DIR}/shapes/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/shapes/*.h" 
)
set(CMAKE_CXX_STANDARD 20)
add_executable(${PROJECT_NAME} ${source}
        shapes/ellipse.cpp
        shapes/ellipse.h
        shapes/polygon.cpp
        shapes/polygon.h
        shapes/freehand.cpp
        shapes/freehand.h
        tools/tool.h
        tools/selector.cpp
        tools/selector.h
        tools/shape_creater.cpp
        tools/shape_creater.h
        ShapeType.h
        tools/shape_setter.cpp
        tools/shape_setter.h
        tools/shape_mover.cpp
        tools/shape_mover.h
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES 
  DEBUG_POSTFIX "_d"
  RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_DIR}") 
target_link_libraries(${PROJECT_NAME} PUBLIC common) 
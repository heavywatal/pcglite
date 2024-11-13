set(source_files
  example.cpp
  static.cpp
)

find_package(pcg)
if(pcg_FOUND)
  set(source_files ${source_files} identical.cpp)
  link_libraries(pcg::pcg)
endif()

foreach(src IN LISTS source_files)
  get_filename_component(name_we ${src} NAME_WE)
  add_executable(test-${name_we} ${src})
  set_target_properties(test-${name_we} PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(test-${name_we} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endforeach()

set(source_files
  example.cpp
  identical.cpp
  static.cpp
  std.cpp
)

foreach(src IN LISTS source_files)
  cmake_path(GET src STEM name_we)
  add_executable(test-${name_we} ${src})
  set_target_properties(test-${name_we} PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(test-${name_we} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
  target_compile_options(test-${name_we} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_compile_options(test-${name_we} PRIVATE -Wconversion -Wsign-conversion -Wshadow)
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endforeach()

include(FetchContent)
FetchContent_Declare(
  pcg
  URL https://github.com/imneme/pcg-cpp/archive/428802d1a5634f96bcd0705fab379ff0113bcf13.tar.gz
  URL_HASH SHA256=79f23706ed0cbc1bb57ea35d50d0abed66898b8f7b0bcf2aaecdfd234863b060
  DOWNLOAD_NO_PROGRESS TRUE
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(pcg)

target_include_directories(test-identical PRIVATE ${pcg_SOURCE_DIR}/include)

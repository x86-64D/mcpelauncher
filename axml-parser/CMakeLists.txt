cmake_minimum_required(VERSION 3.0)
project(axml_parser)

set(CMAKE_CXX_STANDARD 11)

add_library(axml_parser include/axml/axml_file.h include/axml/endian_aware_int.h lib/axml_file.cpp include/axml/chunk_header.h include/axml/string_pool.h lib/string_pool.cpp include/axml/chunk_header_range.h lib/chunk_header_range.cpp include/axml/axml_parser.h lib/axml_parser.cpp include/axml/chunk_xml.h)
target_include_directories(axml_parser PUBLIC include)

add_executable(axml_parser_demo main.cpp)
target_link_libraries(axml_parser_demo axml_parser)
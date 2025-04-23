add_subdirectory(nlohmann_json)

add_library(search_engine
        ConverterJSON.cpp InvertedIndex.cpp SearchServer.cpp
        ${MY_INCLUDE_DIR}/ConverterJSON.h
        ${MY_INCLUDE_DIR}/InvertedIndex.h
        ${MY_INCLUDE_DIR}/SearchServer.h)

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(search_engine PUBLIC ${MY_INCLUDE_DIR})
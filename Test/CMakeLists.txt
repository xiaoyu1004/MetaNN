set(module_name Test)

file(GLOB_RECURSE TEST_SRCS "data/*.h"
                            "data/*.cpp"
                            "evaluate/*.h"
                            "evaluate/*.cpp"
                            "facilities/*.h"
                            "facilities/*.cpp"
                            "layers/*.h"
                            "layers/*.cpp"
                            "model_rel/*.h"
                            "model_rel/*.cpp"
                            "operators/*.h"
                            "operators/*.cpp"
                            "policies/*.h"
                            "policies/*.cpp"
                            "main.cpp")

add_executable(${module_name} ${TEST_SRCS})
target_include_directories(${module_name} PRIVATE .)
target_include_directories(${module_name} PRIVATE ${PROJECT_SOURCE_DIR})
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
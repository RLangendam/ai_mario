cmake_minimum_required (VERSION 3.14)
project (sandbox)

add_executable(sandbox
    main.cpp
)

target_compile_features(sandbox PRIVATE cxx_std_20)
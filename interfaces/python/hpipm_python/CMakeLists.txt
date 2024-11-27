cmake_minimum_required(VERSION 3.15)
project(hpipm-python C)

find_package(hpipm REQUIRED)

install(FILES $<TARGET_PROPERTY:hpipm,IMPORTED_LOCATION_$<UPPER_CASE:$<CONFIG>>>
        EXCLUDE_FROM_ALL
        COMPONENT python_binaries
        DESTINATION DESTINATION ${PY_BUILD_CMAKE_MODULE_NAME})

project(rime-calculator)
cmake_minimum_required(VERSION 3.10)

aux_source_directory(src calculator_src)

add_library(rime-calculator-objs OBJECT ${calculator_src})
if(BUILD_SHARED_LIBS)
  set_target_properties(rime-calculator-objs
    PROPERTIES
    POSITION_INDEPENDENT_CODE ON)
endif()

set(plugin_name rime-calculator PARENT_SCOPE)
set(plugin_objs $<TARGET_OBJECTS:rime-calculator-objs> PARENT_SCOPE)
set(plugin_deps ${rime_library} ${rime_gears_library} PARENT_SCOPE)
set(plugin_modules "calculator" PARENT_SCOPE)

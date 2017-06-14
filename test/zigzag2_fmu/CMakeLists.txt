# --------------------------------------------------------------
# Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(zigzag2_fmu)

add_library(zigzag2 SHARED zigzag2.c)
target_link_libraries( zigzag2 -lm )
set_target_properties(zigzag2 PROPERTIES PREFIX "")
pack_fmu(zigzag2 ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zigzag2)

# Reduced version which does not export co-simulation functions
add_library(zigzag2_me_only SHARED zigzag2.c)
target_link_libraries( zigzag2_me_only -lm )
set_target_properties(zigzag2_me_only PROPERTIES PREFIX "")
set_target_properties(zigzag2_me_only PROPERTIES COMPILE_FLAGS "-DFMI2_NO_CO_SIMULATION_FUNCTION")
pack_fmu(zigzag2_me_only ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription_me_only.xml zigzag2_me_only)

# --------------------------------------------------------------
# Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(zigzag2_fmu)

# find_package(Java REQUIRED)
# include(UseJava)

add_library(zigzag2 SHARED zigzag2.c)

target_link_libraries( zigzag2 -lm )

set_target_properties(zigzag2 PROPERTIES PREFIX "")

pack_fmu(zigzag2 ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zigzag2)

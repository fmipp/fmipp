# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(zigzag_fmu)

add_library(zigzag SHARED zigzag.c)
add_library(givezero SHARED givezero.c)
target_link_libraries(zigzag givezero)

set_target_properties(zigzag PROPERTIES PREFIX "")

pack_fmu(zigzag ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zigzag)
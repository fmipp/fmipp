# -------------------------------------------------------------------
# Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(v2_0_fmu)

include(FindJava)

add_library(v2_0 SHARED v2_0.c)

set_target_properties(v2_0 PROPERTIES PREFIX "")

pack_fmu(v2_0 ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml v2_0)

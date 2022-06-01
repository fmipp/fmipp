# -------------------------------------------------------------------
# Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(dxiskx_fmu)

include(FindJava)

add_library(dxiskx SHARED dxiskx.c)

set_target_properties(dxiskx PROPERTIES PREFIX "")

pack_fmu(dxiskx ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml dxiskx)

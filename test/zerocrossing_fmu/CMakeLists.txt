# -------------------------------------------------------------------
# Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(zerocrossing_fmu)

include(FindJava)

add_library(zerocrossing SHARED zerocrossing.c)
target_link_libraries(zerocrossing)

set_target_properties(zerocrossing PROPERTIES PREFIX "")

pack_fmu(zerocrossing ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zerocrossing)

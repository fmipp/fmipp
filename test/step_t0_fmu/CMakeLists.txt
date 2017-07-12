# -------------------------------------------------------------------
# Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(step_t0_fmu)

add_library(step_t0 SHARED step_t0.c)

set_target_properties(step_t0 PROPERTIES PREFIX "")

pack_fmu(step_t0 ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml step_t0)

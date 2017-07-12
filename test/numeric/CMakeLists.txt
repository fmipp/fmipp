# -------------------------------------------------------------------
# Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required( VERSION 2.8.12 )

project( numeric_fmus )

add_subdirectory( stiff_fmu )
add_subdirectory( linear_stiff_fmu )
add_subdirectory( stiff2_fmu )
add_subdirectory( polynomial_fmu )
add_subdirectory( asymptotic_sine_fmu )
add_subdirectory( robertson_fmu )